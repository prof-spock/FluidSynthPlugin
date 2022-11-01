/**
 * @file
 * The <C>MidiEventConverter</C> module implements the handling of
 * MIDI events by feeding them to an underlying FluidSynth library.
 *
 * @author Dr. Thomas Tensi
 * @date   2022-04
 */

/*=========*/
/* IMPORTS */
/*=========*/

#include "Boolean.h"
#include "Byte.h"
#include "Logging.h"

#include "FluidSynthSynthesizer.h"
#include "MidiEventConverter.h"

/*--------------------*/

using Audio::AudioSample;
using Audio::AudioSampleList;
using BaseTypes::Primitives::Boolean;
using BaseTypes::Primitives::Byte;
using Libraries::FluidSynth::FluidSynthSynthesizer;
using MIDI::MidiEventConverter;
using MIDI::MidiEventKind;

/*--------------------*/
/* prototypes         */
/*--------------------*/

static Boolean
_handleMidiEvent (INOUT FluidSynthSynthesizer& synthesizer,
                  IN MidiEvent& event);

static Boolean
_handleProgramChange (IN String& value,
                      INOUT FluidSynthSynthesizer& synthesizer);

/*====================*/
/* PRIVATE FEATURES    */
/*====================*/

/** the number of audio channels provided by this plugin */
#define _channelCount 2

/*====================*/

namespace MIDI {

    /**
     * the internal data of a fluidsynth MIDI processor: a fluidsynth
     * library, settings and synthesizer object and the dictionary of
     * settings and some preallocated internal audio sample buffer
     */
    struct _MidiEventConverterDescriptor {

        /** the dictionary of settings with fluidsynth keys */
        Dictionary settingsDictionary;

        /** the underlying buffer size of the fluidsynth
         * synthesizer */
        Natural synthesizerBufferSize;

        /*--------------------*/
        /*--------------------*/

        /** sets up the processor descriptor */
        _MidiEventConverterDescriptor ();

        /*--------------------*/

        /** destroys the processor descriptor */
        ~_MidiEventConverterDescriptor ();

        /*--------------------*/

        /**
         * Changes settings for <C>key</C> to <C>value</C>.
         *
         * @param[in] key    fluidsynth settings key
         * @param[in] value  associated string value
         * @return  information whether set operation has been successful
         */
        Boolean changeSettings (IN String& key, IN String& value);

        /*--------------------*/

        /**
         * Resets all settings to default.
         */
        void resetSettings ();

        /*--------------------*/

        private:

            /** a fluidsynth library object */
            FluidSynth& _library;

            /*--------------------*/

            /** a fluidsynth settings object */
            FluidSynthSettings& _settings;

        public:

        /** a fluidsynth synthesizer object */
        FluidSynthSynthesizer& synthesizer;

    };

}

/*====================*/

using MIDI::_MidiEventConverterDescriptor;

/*====================*/

_MidiEventConverterDescriptor::_MidiEventConverterDescriptor ()
    : _library{*(new FluidSynth())},
      _settings{*(new FluidSynthSettings(_library))},
      synthesizer{*(new FluidSynthSynthesizer(_library, _settings))}
{
    Logging_trace(">>");
    synthesizerBufferSize = synthesizer.internalBufferSize();
    Logging_trace("<<");
}

/*--------------------*/
    
_MidiEventConverterDescriptor::~_MidiEventConverterDescriptor ()
{
    Logging_trace(">>");
    delete &synthesizer;
    delete &_settings;
    delete &_library;
    Logging_trace("<<");
}

/*--------------------*/

Boolean
_MidiEventConverterDescriptor::changeSettings (IN String& key,
                                               IN String& value)
{
    Logging_trace2(">>: key = %1, value = %2", key, value);

    Boolean isOkay = false;
    settingsDictionary.set(key, value);

    if (StringUtil::startsWith(key, "synth.")) {
        isOkay = _settings.set(key, value);
    } else if (key == "program") {
        isOkay = _handleProgramChange(value, synthesizer);
    } else if (key == "soundfont") {
        isOkay = synthesizer.loadSoundFont(value);
    }
    
    Logging_trace1("<<: %1", TOSTRING(isOkay));
    return isOkay;
}

/*--------------------*/

void _MidiEventConverterDescriptor::resetSettings ()
{
    Logging_trace(">>");

    settingsDictionary.clear();
    //TT settings.reset();
    
    Logging_trace("<<");
}

/*====================*/

/*--------------------*/
/* internal routines  */
/*--------------------*/

/**
 * Clear <C>sampleListVector</C>.
 *
 * @param[in] sampleListVector      audio buffer to be set to 0
 * @param[in] sampleCountInChannel  number of samples to set
 */
static void
_clearSampleListVector (INOUT AudioSampleListVector& sampleListVector,
                        IN Natural sampleCountInChannel)
{
    const Natural channelCount = sampleListVector.length();

    for (Natural channel = 0;  channel < channelCount;  channel++) {
        AudioSampleList& sampleList = sampleListVector[channel];
        
        for (Natural j = 0;  j < sampleCountInChannel;  j++) {
            sampleList[j] = 0.0;
        }
    }
}
    
/*--------------------*/

/**
 * Processes MIDI events in MIDI event list <C>eventList</C> at or
 * after <C>eventIndex</C> scheduled at <C>referenceTimeInSamples</C>
 * using Fluidsynth <C>synthesizer</C>
 *
 * @param[inout] synthesizer             fluidsynth synthesizer object
 * @param[in]    referenceTimeInSamples  reference time for midi events
 * @param[in]    eventList               list of midi events
 * @param[inout] eventIndex              pointer to current position
 *                                       in MIDI event list
 */
static void
_handleCurrentMidiEvents (INOUT FluidSynthSynthesizer& synthesizer,
                          IN Natural referenceTimeInSamples,
                          IN MidiEventList& eventList,
                          INOUT Natural& eventIndex)
{
    Logging_trace2(">>: referenceTimeInSamples = %1, eventIndex = %2",
                   TOSTRING(referenceTimeInSamples),
                   TOSTRING(eventIndex));

    const Natural eventListLength = eventList.size();

    while (eventIndex < eventListLength) {
        const MidiEvent& event = eventList[eventIndex];
        const Integer eventTimeInSamples = event.time();

        if (eventTimeInSamples > referenceTimeInSamples) {
            /* this event and the following are not yet relevant */
            break;
        } else {
            /* this event has to be processed */
            _handleMidiEvent(synthesizer, event);
            eventIndex++;
        }
    }

    Logging_trace1("<<: new event index = %1", TOSTRING(eventIndex));
}

/*--------------------*/

/**
 * Handles MIDI event <C>event</C> for <C>synthesizer</C>
 *
 * @param[inout] synthesizer  fluidsynth synthesizer object
 * @param[in]    event        JUCE midi event
 * @return  information whether midi event handling has been successful
 */
static Boolean
_handleMidiEvent (INOUT FluidSynthSynthesizer& synthesizer,
                  IN MidiEvent& event)
{
    Logging_trace1(">>: event = %1", event.toString());

    const MidiEventKind eventKind = event.kind();
    Boolean isOkay = false;

    if (eventKind != MidiEventKind::meta
        && eventKind != MidiEventKind::systemExclusive) {
        const Natural midiChannel  = event.channel();

        if (eventKind == MidiEventKind::controlChange) {
            const Natural controller = event.getDataByte(1);
            const Natural value      = event.getDataByte(2);
            isOkay = synthesizer.handleControlChange(midiChannel,
                                                     controller, value);
        } else if (eventKind == MidiEventKind::monoTouch) {
            const Natural key = event.getDataByte(1);
            const Natural value      = event.getDataByte(2);
            isOkay = synthesizer.handleMonoTouch(midiChannel,
                                                 key, value);
        } else if (eventKind == MidiEventKind::noteOff) {
            const Natural key = event.getDataByte(1);
            isOkay = synthesizer.handleNoteOff(midiChannel, key);
        } else if (eventKind == MidiEventKind::noteOn) {
            const Natural key          = event.getDataByte(1);
            const Natural midiVelocity = event.getDataByte(2);
            isOkay = synthesizer.handleNoteOn(midiChannel,
                                              key, midiVelocity);
        } else if (eventKind == MidiEventKind::pitchBend) {
            const Natural value = (event.getDataByte(1)
                                   + event.getDataByte(2) * 128);
            isOkay = synthesizer.handlePitchBend(midiChannel, value);
        } else if (eventKind == MidiEventKind::polyTouch) {
            const Natural value = event.getDataByte(1);
            isOkay = synthesizer.handlePolyTouch(midiChannel, value);
        } else if (eventKind == MidiEventKind::programChange) {
            const Natural programNumber = event.getDataByte(1);
            isOkay = synthesizer.handleProgramChange(midiChannel,
                                                     programNumber);
        }
    }

    Logging_trace1("<<: %1", TOSTRING(isOkay));
    return isOkay;
}
        
/*--------------------*/

/**
 * Handles bank and program change depending on for given string
 * <C>value</C> on <C>synthesizer</C>.
 *
 * @param[in]    value         string value for bank and/or program
 * @param[inout] synthesizer   fluidsynth synthesizer object
 * @return  information whether program change handling has been
 *          successful
 */
static Boolean
_handleProgramChange (IN String& value,
                      INOUT FluidSynthSynthesizer& synthesizer)
{
    Logging_trace1(">>: %1", value);

    const Natural undefined = Natural::maximumValue();
    const String bankAndProgramSeparator = ":";
    const Natural separatorPosition =
        StringUtil::find(value, bankAndProgramSeparator);

    String bank;
    String program;

    if (separatorPosition == undefined) {
        bank = "0";
        program = value;
    } else {
        StringUtil::splitAt(value, bankAndProgramSeparator,
                            bank, program);
    }
    
    Boolean isOkay = (StringUtil::isNatural(bank)
                      && StringUtil::isNatural(program));

    if (!isOkay) {
        Logging_trace1("--: '%1' is not a valid program number - ",
                       value);
    } else {
        Natural bankNumber = StringUtil::toNatural(bank);
        Natural programNumber = StringUtil::toNatural(program);

        for (Natural midiChannel = 0;  midiChannel < 16;  midiChannel++) {
            isOkay = synthesizer.handleBankChange(midiChannel,
                                                  bankNumber);

            if (isOkay) {
                isOkay = synthesizer.handleProgramChange(midiChannel,
                                                         programNumber);
            }
        }
    }

    Logging_trace1("<<: %1", TOSTRING(isOkay));
    return isOkay;
}

/*====================*/
/* PUBLIC FEATURES    */
/*====================*/

/*--------------------*/
/* setup              */
/*--------------------*/

MidiEventConverter::MidiEventConverter ()
    : _descriptor{NULL}
{
    Logging_trace(">>");
    _descriptor = new _MidiEventConverterDescriptor();
    Logging_trace("<<");
}

/*--------------------*/

MidiEventConverter::~MidiEventConverter ()
{
    Logging_trace(">>");

    _MidiEventConverterDescriptor& descriptor =
        TOREFERENCE<_MidiEventConverterDescriptor>(_descriptor);
    delete &descriptor;

    Logging_trace("<<");
}

/*--------------------*/
/* property queries   */
/*--------------------*/

Natural
MidiEventConverter::synthesizerBufferSize () const
{
    Logging_trace(">>");

    _MidiEventConverterDescriptor& descriptor =
        TOREFERENCE<_MidiEventConverterDescriptor>(_descriptor);
    Natural result = descriptor.synthesizerBufferSize;

    Logging_trace1("<<: %1", TOSTRING(result));
    return result;
}

/*--------------------*/
/* event handling     */
/*--------------------*/

void
MidiEventConverter::prepareToPlay
                        (IN Real sampleRate,
                         IN Natural maximumExpectedSamplesPerBlock)
{
    Logging_trace2(">>: sampleRate = %1, samplesPerBlock = %2",
                   TOSTRING(sampleRate),
                   TOSTRING(maximumExpectedSamplesPerBlock));

    _MidiEventConverterDescriptor& descriptor =
        TOREFERENCE<_MidiEventConverterDescriptor>(_descriptor);
    descriptor.changeSettings("synth.sample-rate", TOSTRING(sampleRate));
    Logging_trace("<<");
}

/*--------------------*/

void MidiEventConverter::releaseResources ()
{
    Logging_trace(">>");
    Logging_trace("<<");
}

/*--------------------*/

void
MidiEventConverter::processBlock
                        (IN MidiEventList& midiEventList,
                         INOUT AudioSampleListVector& audioBuffer,
                         IN Natural sampleCountInChannel)
{
    Logging_trace2(">>: midiEventList = %1, sampleCountInChannel = %2",
                   midiEventList.toString(),
                   TOSTRING(sampleCountInChannel));

    _MidiEventConverterDescriptor& descriptor =
        TOREFERENCE<_MidiEventConverterDescriptor>(_descriptor);
    FluidSynthSynthesizer& synthesizer = descriptor.synthesizer;

    /* calculate the times (in samples) */
    Natural currentTimeInSamples = 0;
    const Natural endTimeInSamples = sampleCountInChannel;
    Natural eventIndex = 0;

    while (currentTimeInSamples < endTimeInSamples) {
        /* set count of samples to be processed to remaining buffer
         * length */
        Natural intervalDurationInSamples =
            endTimeInSamples - currentTimeInSamples;

        _handleCurrentMidiEvents(synthesizer,
                                 currentTimeInSamples,
                                 midiEventList,
                                 eventIndex);

        if (eventIndex < midiEventList.size()) {
            /* there are still events open */
            const MidiEvent& event = midiEventList[eventIndex];
            const Natural durationToNextEventInSamples =
                Integer::toNatural(event.time() - currentTimeInSamples);
            intervalDurationInSamples =
                Natural::minimum(intervalDurationInSamples,
                                 durationToNextEventInSamples);
        }

        synthesizer.process(audioBuffer,
                            currentTimeInSamples,
                            intervalDurationInSamples);
        currentTimeInSamples += intervalDurationInSamples;
    }
    
    Logging_trace("<<");
}

/*--------------------*/
/* property change    */
/*--------------------*/

Boolean MidiEventConverter::set (IN String& key, IN String& value)
{
    Logging_trace2(">>: key = %1, value = %2", key, value);
    
    _MidiEventConverterDescriptor& descriptor =
        TOREFERENCE<_MidiEventConverterDescriptor>(_descriptor);
    Boolean isOkay = descriptor.changeSettings(key, value);
    
    Logging_trace1("<<: %1", TOSTRING(isOkay));
    return isOkay;
}

/*--------------------*/

Boolean MidiEventConverter::set (IN Dictionary& dictionary)
{
    Logging_trace1(">>: %1", dictionary.toString());

    _MidiEventConverterDescriptor& descriptor =
        TOREFERENCE<_MidiEventConverterDescriptor>(_descriptor);

    Boolean isOkay = true;

    for (const auto& [key, value]: dictionary) {
        Boolean settingIsOkay = descriptor.changeSettings(key, value);
        isOkay = isOkay && settingIsOkay;
    }

    Logging_trace1("<<: %1", TOSTRING(isOkay));
    return isOkay;
}

/*--------------------*/
/* persistence        */
/*--------------------*/

String MidiEventConverter::getStateInformation ()
{
    Logging_trace(">>");

    /* the state of the Midi instrument is just the serialized
       settings dictionary */
    _MidiEventConverterDescriptor& descriptor =
        TOREFERENCE<_MidiEventConverterDescriptor>(_descriptor);
    const String result = descriptor.settingsDictionary.toString();

    Logging_trace1("<<: %1", result);
    return result;
}

/*--------------------*/

void MidiEventConverter::setStateInformation (IN String& st)
{
    Logging_trace(">>");

    /* restores state of audio processor from <C>st</C> */
    _MidiEventConverterDescriptor& descriptor =
        TOREFERENCE<_MidiEventConverterDescriptor>(_descriptor);
    descriptor.resetSettings();

    Dictionary d = Dictionary::makeFromString(st);

    for (const auto& [key, value]: d) {
        descriptor.changeSettings(key, value);
    }

    Logging_trace("<<");
}
