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
#include "Logging.h"
#include "FluidSynthSoundFont.h"
#include "MidiEventConverter.h"
#include "MidiPresetIdentification.h"

/*--------------------*/

using Audio::AudioSample;
using Audio::AudioSampleList;
using BaseTypes::Primitives::Boolean;
using Libraries::FluidSynth::FluidSynthSoundFont;
using MIDI::MidiEventConverter;
using MIDI::MidiEventKind;
using MIDI::MidiPresetIdentification;

/** abbreviation for StringUtil */
using STR = BaseModules::StringUtil;

/*--------------------*/
/* prototypes         */
/*--------------------*/

static String
_findSuitablePreset (IN FluidSynthSoundFont& soundFont);

static Boolean _setPreset (INOUT FluidSynthSynthesizer& synthesizer,
                           IN Natural bankNumber,
                           IN Natural programNumber);

/*====================*/
/* PRIVATE FEATURES    */
/*====================*/

/** the number of audio channels provided by this plugin */
#define _channelCount 2

/** the name of the key for the MIDI preset with bank and program */
static const String _presetKey = "preset";

/*====================*/

namespace MIDI {

    /*--------------------*/

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

        /** a fluidsynth synthesizer object */
        FluidSynthSynthesizer* synthesizer;

        /** a flag to tell whether sound font is loaded */
        Boolean soundFontIsOkay;

        /** a flag to tell whether synthesizer is available for
         * playback */
        Boolean synthesizerIsAvailable;

        /*--------------------*/
        /*--------------------*/

        /** Sets up the processor descriptor.
         *
         * @param[in] audioIsSuppressedForBadSettings  tells that no audio
         *                                         is produced when settings
         *                                         are inconsistent
         */
        _MidiEventConverterDescriptor
             (IN Boolean audioIsSuppressedForBadSettings);

        /*--------------------*/

        /** Destroys the processor descriptor */
        ~_MidiEventConverterDescriptor ();

        /*--------------------*/

        /**
         * Changes settings for <C>key</C> to <C>value</C>.
         *
         * @param[in] key    fluidsynth settings key
         * @param[in] value  associated string value
         * @return  information whether set operation has been
         *          successful
         */
        Boolean setSetting (IN String& key, IN String& value);

        /*--------------------*/

        /**
         * Handles bank and program change depending on for given
         * string <C>value</C> on synthesizer. <C>value</C> contains
         * bank and program number separated by a colon.  The bank
         * number comes first followed by a colon and finally the
         * program number.  All MIDI channels are affected, if the
         * bank is missing, bank number 0 is assumed; an empty bank is
         * ignored and an empty program is erroneous.
         *
         * @param[in] value           string value for bank and/or
         *                             program
         * @param[in] changeIsForced  tells whether program change
         *                            must be done unconditionally
         * @return  information whether program change handling has
         *          been successful
         */
        Boolean handleProgramChange (IN String& value,
                                     IN Boolean changeIsForced = false);

        /*--------------------*/

        /**
         * Handles change of soundfont for given string <C>value</C>
         * on synthesizer where value contains soundfont path.
         * 
         * @param[in] value  string value for sound font path
         * @return  information whether sound font change handling has
         *          been successful
         */
        Boolean handleSoundFontChange (IN String& value);

        /*--------------------*/

        /**
         * Handles MIDI event <C>event</C> for synthesizer
         *
         * @param[in] event  midi event to be processed
         * @return  information whether midi event handling has been
         *          successful
         */
        Boolean processMidiEvent (IN MidiEvent& event);

        /*--------------------*/

        /**
         * Processes MIDI events in MIDI event list <C>eventList</C> at or
         * after <C>eventIndex</C> scheduled at <C>referenceTimeInSamples</C>
         * using Fluidsynth synthesizer
         *
         * @param[in]    referenceTimeInSamples  reference time for midi events
         * @param[in]    eventList               list of midi events
         * @param[inout] eventIndex              pointer to current position
         *                                       in MIDI event list
         */
        void processMidiEventList (IN Natural referenceTimeInSamples,
                                  IN MidiEventList& eventList,
                                  INOUT Natural& eventIndex);

        /*--------------------*/

        /**
         * Resets all settings to default.
         */
        void resetSettings ();

        /*--------------------*/

        private:

            /** flag for suppressing audio output for bad
                settings */
            Boolean _audioIsSuppressedForBadSettings;
        
            /** tells that current program number is valid */
            Boolean _hasValidPreset;
        
            /** a fluidsynth settings object */
            FluidSynthSettings* _settings;

    };

}

/*====================*/

using MIDI::_MidiEventConverterDescriptor;

/*===============================*/
/* _MidiEventConverterDescriptor */
/*===============================*/

_MidiEventConverterDescriptor
::_MidiEventConverterDescriptor (IN Boolean audioIsSuppressedForBadSettings)
{
    Logging_trace(">>");

    FluidSynth* library = new FluidSynth();
    _settings = new FluidSynthSettings(library);
    _audioIsSuppressedForBadSettings = audioIsSuppressedForBadSettings;
    synthesizer = new FluidSynthSynthesizer(library, _settings);
    synthesizerBufferSize = (!library->isLoaded() ? 0
                             : synthesizer->internalBufferSize());
    synthesizerIsAvailable = true;
    _hasValidPreset = false;

    Logging_trace("<<");
}

/*--------------------*/
    
_MidiEventConverterDescriptor::~_MidiEventConverterDescriptor ()
{
    Logging_trace(">>");
    synthesizerIsAvailable = false;
    delete synthesizer;
    delete _settings;
    Logging_trace("<<");
}

/*--------------------*/

Boolean
_MidiEventConverterDescriptor::setSetting (IN String& key,
                                           IN String& value)
{
    Logging_trace2(">>: key = %1, value = %2", key, value);

    const String acceptedKeyPrefix = "synth.";
    const String alternativePresetKey = "program";
    const String interpolationMethodKey = "synth.interpolation-method";
    const StringList restartingKeysList =
        StringList::makeBySplit("synth.sample-rate,"
                                "synth.interpolation-method,"
                                "synth.verbose",
                                ",");
    const StringList specialKeysList =
        StringList::makeBySplit("synth.interpolation-method", ",");
    const String soundFontKey = "soundfont";
    String effectiveKey = key;

    enum class UpdateLevel
        { restart = 0, soundFont = 1, program = 2, std = 3, error = 99 };
    UpdateLevel updateLevel = UpdateLevel::error;
    
    Boolean isOkay = true;
    synthesizerIsAvailable = false;

    /* replace 'program' by 'preset' as key */
    if (key == alternativePresetKey) {
        effectiveKey = _presetKey;
    }
    
    if (effectiveKey == soundFontKey) {
        updateLevel = UpdateLevel::soundFont;
    } else if (effectiveKey == _presetKey) {
        updateLevel = UpdateLevel::program;
    } else if (!STR::startsWith(effectiveKey, acceptedKeyPrefix)) {
        updateLevel = UpdateLevel::error;
        isOkay = false;
    } else {
        updateLevel = UpdateLevel::std;

        if (!specialKeysList.contains(effectiveKey)) {
            /* the synthesizer settings can handle that*/
            isOkay = _settings->set(effectiveKey, value);
        }

        if (isOkay && (restartingKeysList.contains(effectiveKey))) {
            updateLevel = UpdateLevel::restart;
        }
    }

    if (updateLevel != UpdateLevel::error) {
        settingsDictionary.set(effectiveKey, value);
    }

    Logging_trace1("--: updateLevel = %1",
                   TOSTRING(Natural{(size_t) updateLevel}));
    
    if (isOkay && updateLevel == UpdateLevel::restart) {
        /* restart synthesizer when synthesizer related setting is
         * activated */
        FluidSynth* library = synthesizer->library();
        delete synthesizer;
        synthesizer = new FluidSynthSynthesizer(library, _settings);
        synthesizerBufferSize = (!library->isLoaded() ? 0
                                 : synthesizer->internalBufferSize());
    }

    if (isOkay && updateLevel <= UpdateLevel::soundFont) {
        if (settingsDictionary.contains(soundFontKey)) {
            String soundFontValue = settingsDictionary.at(soundFontKey);
            isOkay = handleSoundFontChange(soundFontValue);
        }
    }
    
    if (isOkay
        && (updateLevel == UpdateLevel::restart
            || updateLevel == UpdateLevel::program)) {
        if (settingsDictionary.contains(_presetKey)) {
            String presetIdentification{settingsDictionary.at(_presetKey)};
            isOkay = handleProgramChange(presetIdentification, true);
            _hasValidPreset = true;
        }

        if (!isOkay) {
            /* remove invalid entry for program to signal error */
            settingsDictionary.remove(_presetKey);
        }
    }
    
    if (isOkay && updateLevel < UpdateLevel::std) {
        /* restore specific settings for synthesizer */
        if (settingsDictionary.contains(interpolationMethodKey)) {
            String methodCodeAsString =
                settingsDictionary.at(interpolationMethodKey);
            Natural methodCode = STR::toNatural(methodCodeAsString);
            isOkay = synthesizer->setInterpolationMethod(methodCode);
        }
    }

    synthesizerIsAvailable =
        (!_audioIsSuppressedForBadSettings || soundFontIsOkay);
    
    Logging_trace1("<<: %1", TOSTRING(isOkay));
    return isOkay;
}

/*--------------------*/

Boolean _MidiEventConverterDescriptor::handleProgramChange
                      (IN String& value,
                       IN Boolean changeIsForced)
{
    Logging_trace2(">>: value = %1, changeIsForced = %2",
                   value, TOSTRING(changeIsForced));

    Boolean isOkay;

    if (_hasValidPreset && !changeIsForced) {
        Logging_trace("--: skip program change when preset is valid");
        isOkay = true;
    } else {
        Natural bankNumber;
        Natural programNumber;
        Boolean bankIsEmpty;
        MidiPresetIdentification presetIdentification{value};
        isOkay = presetIdentification.split(bankNumber, programNumber);

        const String errorMessageTemplate =
            "'%1' is not a valid program number";

        if (!isOkay) {
            Logging_traceError1(errorMessageTemplate, value);
        } else {
            FluidSynthSoundFont soundFont{synthesizer};

            if (soundFont.hasProgram(bankNumber, programNumber)) {
                isOkay = _setPreset(*synthesizer, bankNumber, programNumber);
            } else {
                /* program does not exist in current soundfont => skip */
                Logging_traceError1(errorMessageTemplate, value);
                isOkay = false;
            }
        }

        _hasValidPreset = isOkay;
    }

    Logging_trace1("<<: %1", TOSTRING(isOkay));
    return isOkay;
}

/*--------------------*/

Boolean _MidiEventConverterDescriptor::handleSoundFontChange
            (IN String& value)
{
    Logging_trace1(">>: %1", value);

    soundFontIsOkay = synthesizer->loadSoundFont(value);
    /* reset bank and program to 0 */
    Boolean isOkay =
        (soundFontIsOkay && _setPreset(*synthesizer, 0, 0));
    _hasValidPreset = false;
    
    Logging_trace1("<<: %1", TOSTRING(isOkay));
    return isOkay;
}

/*--------------------*/

Boolean _MidiEventConverterDescriptor::processMidiEvent (IN MidiEvent& event)
{
    Logging_trace1(">>: %1", event.toString());

    const MidiEventKind eventKind = event.kind();
    Boolean isOkay = false;

    if (eventKind != MidiEventKind::meta
        && eventKind != MidiEventKind::systemExclusive) {
        const Natural midiChannel = event.channel();

        if (eventKind == MidiEventKind::controlChange) {
            const Natural controller = (Natural) event.getDataByte(1);
            const Natural value      = (Natural) event.getDataByte(2);
            const Boolean isBankChange =
                (controller == 0 && controller != 32);

            if (_hasValidPreset && isBankChange) {
                Logging_trace("--: skipped bank change when preset is valid");
                isOkay = true;
            } else {
                /* transmit control change when there is no valid
                 * preset or this is just another control change*/
                isOkay = synthesizer->handleControlChange(midiChannel,
                                                          controller, value);
            }
        } else if (eventKind == MidiEventKind::monoTouch) {
            const Natural key   = (Natural) event.getDataByte(1);
            const Natural value = (Natural) event.getDataByte(2);
            isOkay = synthesizer->handleMonoTouch(midiChannel, key, value);
        } else if (eventKind == MidiEventKind::noteOff) {
            const Natural key = (Natural) event.getDataByte(1);
            isOkay = synthesizer->handleNoteOff(midiChannel, key);
        } else if (eventKind == MidiEventKind::noteOn) {
            const Natural key          = (Natural) event.getDataByte(1);
            const Natural midiVelocity = (Natural) event.getDataByte(2);
            isOkay = synthesizer->handleNoteOn(midiChannel,
                                               key, midiVelocity);
        } else if (eventKind == MidiEventKind::pitchBend) {
            const Natural value = ((Natural) event.getDataByte(1)
                                   + (Natural) event.getDataByte(2) * 128);
            isOkay = synthesizer->handlePitchBend(midiChannel, value);
        } else if (eventKind == MidiEventKind::polyTouch) {
            const Natural value = (Natural) event.getDataByte(1);
            isOkay = synthesizer->handlePolyTouch(midiChannel, value);
        } else if (eventKind == MidiEventKind::programChange) {
            const Natural programNumber = (Natural) event.getDataByte(1);

            if (_hasValidPreset) {
                /* ignore */
                isOkay = true;
            } else {
                isOkay = synthesizer->handleProgramChange(midiChannel,
                                                          programNumber);
                synthesizerIsAvailable = isOkay && soundFontIsOkay;
            }
        }
    }

    Logging_trace1("<<: %1", TOSTRING(isOkay));
    return isOkay;
}
        
/*--------------------*/

void _MidiEventConverterDescriptor::processMidiEventList
         (IN Natural referenceTimeInSamples,
          IN MidiEventList& eventList,
          INOUT Natural& eventIndex)
{
    Logging_trace2(">>: referenceTimeInSamples = %1, eventIndex = %2",
                   TOSTRING(referenceTimeInSamples), TOSTRING(eventIndex));

    const Natural eventListLength = eventList.size();

    while (eventIndex < eventListLength) {
        const MidiEvent& event = eventList[eventIndex];
        const Integer eventTimeInSamples = event.time();

        if (eventTimeInSamples > referenceTimeInSamples) {
            /* this event and the following are not yet relevant */
            break;
        } else {
            /* this event has to be processed */
            processMidiEvent(event);
            eventIndex++;
        }
    }

    Logging_trace1("<<: new event index = %1",
                   TOSTRING(eventIndex));
}

/*--------------------*/

void _MidiEventConverterDescriptor::resetSettings ()
{
    Logging_trace(">>");

    settingsDictionary.clear();
    _hasValidPreset = false;
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
 * Finds and returns smallest preset number in <C>soundFont</C>.
 *
 * @param[in] soundFont  sound font to be analyzed
 * @return bank and program number of smallest numbered preset
 */
static String _findSuitablePreset (IN FluidSynthSoundFont& soundFont)
{
    Logging_trace(">>");

    Natural bankNumber = 0;
    Natural programNumber = 0;
    Boolean isFound = false;

    while (!isFound && bankNumber < 16384) {
        while (!isFound && programNumber < 128) {
            if (soundFont.hasProgram(bankNumber, programNumber)) {
                isFound = true;
                break;
            }

            programNumber++;
        }

        if (isFound) {
            break;
        } else {
            bankNumber++;
        }
    }

    if (!isFound) {
        bankNumber    = 0;
        programNumber = 0;
    }

    String result =
        STR::expand("%1:%2", TOSTRING(bankNumber), TOSTRING(programNumber));

    Logging_trace1("<<: %1", result);
    return result;
}

/*--------------------*/

/**
 * Sets preset on all channels of <C>synthesizer</C> to
 * <C>bankNumber</C> and <C>programNumber</C>
 *
 * @param[inout] synthesizer    current synthesizer
 * @param[in]    bankNumber     bank number to be set
 * @param[in]    programNumber  program number to be set
 * @return  information whether preset change has been successful
 */
static Boolean _setPreset (INOUT FluidSynthSynthesizer& synthesizer,
                           IN Natural bankNumber,
                           IN Natural programNumber)
{
    Logging_trace2(">>: bankNumber = %1, programNumber = %2",
                   TOSTRING(bankNumber), TOSTRING(programNumber));

    Boolean isOkay = true;

    for (Natural midiChannel = 0;  midiChannel < 16;  midiChannel++) {
        isOkay = synthesizer.handleBankChange(midiChannel, bankNumber);
        isOkay = (isOkay
                  && synthesizer.handleProgramChange(midiChannel,
                                                     programNumber));

        if (!isOkay) {
            break;
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

MidiEventConverter::MidiEventConverter
                        (IN Boolean audioIsSuppressedForBadSettings)
    : _descriptor{NULL}
{
    Logging_trace(">>");
    _descriptor =
        new _MidiEventConverterDescriptor(audioIsSuppressedForBadSettings);
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

Boolean MidiEventConverter::getSetting (IN String& key,
                                        OUT String& value)
{
    Logging_trace1(">>: %1", key);

    _MidiEventConverterDescriptor& descriptor =
        TOREFERENCE<_MidiEventConverterDescriptor>(_descriptor);
    Dictionary& settings = descriptor.settingsDictionary;
    Boolean isOkay = settings.contains(key);

    if (isOkay) {
        value = settings.at(key);
    }

    Logging_trace2("<<: isOkay = %1, value = %2",
                   TOSTRING(isOkay), value);
    return isOkay;
}

/*--------------------*/

Boolean MidiEventConverter::isCorrectlyInitialized () const
{
    Logging_trace(">>");
    _MidiEventConverterDescriptor& descriptor =
        TOREFERENCE<_MidiEventConverterDescriptor>(_descriptor);
    FluidSynth* library = descriptor.synthesizer->library();
    Boolean result = (library->isLoaded());
    Logging_trace1("<<: %1", TOSTRING(result));
    return result;
}

/*--------------------*/

String MidiEventConverter::fsLibraryVersion () const
{
    Logging_trace(">>");

    _MidiEventConverterDescriptor& descriptor =
        TOREFERENCE<_MidiEventConverterDescriptor>(_descriptor);
    FluidSynth* library = descriptor.synthesizer->library();
    String result = library->version();

    Logging_trace1("<< %1", result);
    return result;
}

/*--------------------*/

StringList MidiEventConverter::presetList () const
{
    Logging_trace(">>");
    _MidiEventConverterDescriptor& descriptor =
        TOREFERENCE<_MidiEventConverterDescriptor>(_descriptor);

    FluidSynthSoundFont soundFont{descriptor.synthesizer};
    StringList result = soundFont.presetList(); 
    
    Logging_trace1("<< %1", result.toString());
    return result;
}

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
/* property change    */
/*--------------------*/

void MidiEventConverter::resetSettings ()
{
    Logging_trace(">>");

    _MidiEventConverterDescriptor& descriptor =
        TOREFERENCE<_MidiEventConverterDescriptor>(_descriptor);
    descriptor.resetSettings();
    
    Logging_trace("<<");
}

/*--------------------*/

Boolean MidiEventConverter::setSetting (IN String& key,
                                        IN String& value)
{
    Logging_trace2(">>: key = %1, value = %2", key, value);
    
    _MidiEventConverterDescriptor& descriptor =
        TOREFERENCE<_MidiEventConverterDescriptor>(_descriptor);
    Boolean isOkay = descriptor.setSetting(key, value);
    
    Logging_trace1("<<: %1", TOSTRING(isOkay));
    return isOkay;
}

/*--------------------*/

Boolean MidiEventConverter::setSettings (IN Dictionary& dictionary)
{
    Logging_trace1(">>: %1", dictionary.toString());

    _MidiEventConverterDescriptor& descriptor =
        TOREFERENCE<_MidiEventConverterDescriptor>(_descriptor);

    Boolean isOkay = true;

    for (const auto& [key, value]: dictionary) {
        Boolean settingIsOkay = descriptor.setSetting(key, value);
        isOkay = isOkay && settingIsOkay;
    }

    Logging_trace1("<<: %1", TOSTRING(isOkay));
    return isOkay;
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
    descriptor.setSetting("synth.sample-rate", TOSTRING(sampleRate));

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

    FluidSynthSynthesizer* synthesizer = descriptor.synthesizer;

    /* calculate the times (in samples) */
    Natural currentTimeInSamples = 0;
    const Natural endTimeInSamples = sampleCountInChannel;
    Natural eventIndex = 0;

    while (currentTimeInSamples < endTimeInSamples) {
        /* set count of samples to be processed to remaining buffer
         * length */
        Natural intervalDurationInSamples =
            endTimeInSamples - currentTimeInSamples;

        descriptor.processMidiEventList(currentTimeInSamples,
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

        if (!descriptor.synthesizerIsAvailable) {
            audioBuffer.setToZero();
        } else {
            synthesizer->process(audioBuffer,
                                 currentTimeInSamples,
                                 intervalDurationInSamples);
        }

        currentTimeInSamples += intervalDurationInSamples;
    }

    Logging_trace("<<");
}

/*--------------------*/

Boolean MidiEventConverter::processMidiEvent (IN MidiEvent& event)
{
    Logging_trace1(">>: %1", event.toString());

    _MidiEventConverterDescriptor& descriptor =
        TOREFERENCE<_MidiEventConverterDescriptor>(_descriptor);
    Boolean result = descriptor.processMidiEvent(event);
    
    Logging_trace1("<<: %1", TOSTRING(result));
    return result;
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
    StringList keyList = Dictionary::makeKeyListFromString(st);

    for (String key : keyList) {
        String value = d.at(key);
        descriptor.setSetting(key, value);
    }

    Logging_trace("<<");
}
