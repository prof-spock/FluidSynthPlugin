/**
 * @file
 * The <C>FluidSynthPlugin_Processor</C> module implements the
 * handling of MIDI events from the DAW via JUCE having them processed
 * by the FluidSynth library.
 *
 * @author Dr. Thomas Tensi
 * @date   2022-04
 */

/*=========*/
/* IMPORTS */
/*=========*/

#include "AudioSampleFifoQueueVector.h"
#include "DynamicLibrary.h"
#include "Environment.h"
#include "FluidSynthPlugin_Editor.h"
#include "MidiEventConverter.h"
#include "MyArray.h"
#include "Logging.h"
#include "Integer.h"
#include "OperatingSystem.h"

/*--------------------*/

using Audio::AudioSampleList;
using Audio::AudioSampleFifoQueueVector;
using BaseModules::Environment;
using BaseModules::OperatingSystem;
using BaseTypes::Containers::convertArray;
using BaseTypes::Primitives::Integer;
using Libraries::DynamicLibrary;
using MIDI::MidiEventConverter;
using Main::FluidSynthPlugin::FluidSynthPlugin_Editor;

/** abbreviation for StringUtil */
using STR = BaseModules::StringUtil;

/*====================*/
/* PRIVATE FEATURES   */
/*====================*/

/** the number MIDI programs allowed */
#define _midiProgramCount 128

/** settings key used for preset change */
static const String _settingsKeyForPreset = "preset";

/** error message for missing library initialization */
static const String _errorMessageForBadLibraryInitialization =
    "fluidsynth library could not be loaded";

/** the key string for flagging a slight compensation of sample
 * buffering by the fluidsynth synthesizer */
static const String _fsBufferingCompensationKey =
    "fsBufferCompensationIsDone";

/*====================*/
/* PROTOTYPES         */
/*====================*/

static Integer _readTimeInSamples (juce::AudioPlayHead* playHead);

static void
_resetBlockProcessing (INOUT MidiEventConverter* eventConverter,
                       IN Natural unprocessedSampleCount,
                       IN Natural newOffsetInSamples,
                       OUT AudioSampleListVector& sampleBuffer);

static void _splitListAtTime (INOUT MidiEventList& midiEventList,
                              IN Natural splitTime,
                              INOUT MidiEventList& otherEventList);

/*====================*/

namespace Main::FluidSynthPlugin {

    struct _EventProcessorDescriptor;

    /*====================*/

    /**
     * Type for processing of MIDI events into sample list vectors
     * with the possibility of delaying events for later processing
     */
    struct _RingBuffer {

        /** list of midi events to be processed */
        MidiEventList scheduledEventList;

        /** vector of sample fifo queues */
        AudioSampleFifoQueueVector sampleQueueList;

        /** the last time [in samples] that has been processed by
         * processBlock */
        Integer lastProcessedTime = Integer::maximumValue();

        /** the delay (in samples) between the MIDI events and the
         * rendered samples */
        Natural delayDurationInSamples{0};

        /*--------------------*/

        /**
         * Constructs a ring buffer
         */
        _RingBuffer ();

        /*--------------------*/

        /**
         * Destroys a ring buffer
         */
        ~_RingBuffer ();

        /*--------------------*/

        /**
         * Processes events in <C>midiEventList</C> happening relative
         * to <C>currentTime</C> using <C>processor</C> and its
         * <C>descriptor</C> and returns block of samples in
         * <C>sampleListVector</C>;  note that the processing might be
         * delayed to adapt to the raster size of the underlying
         * fluidsynth synthesizer
         *
         * @param[inout] processor         event processor object
         * @param[inout] descriptor        associated processor descriptor
         * @param[in]    currentTime       time in samples
         * @param[in]    midiEventList     list of midi events
         *                                 starting at given time
         * @param[out]   sampleListVector  vector of audio sample lists
         *                                 returned
         * @param[in]    audioFrameCount   number of samples to be
         *                                 returned
         */
        void processBlock
                 (INOUT FluidSynthPlugin_EventProcessor* processor,
                  INOUT _EventProcessorDescriptor& descriptor,
                  IN Integer currentTime,
                  IN MidiEventList& midiEventList,
                  OUT AudioSampleListVector& sampleListVector,
                  IN Natural audioFrameCount);

    };

    /*====================*/

    /**
     * Internal type representation for event processor containing a
     * MIDI event converter and the string containing all fluidsynth
     * settings
    */
    struct _EventProcessorDescriptor {

        /** the associated event processor object */
        FluidSynthPlugin_EventProcessor* eventProcessor;

        /** the associated midi event converter for this processor */
        MidiEventConverter* midiEventConverter;

        /** the settings string for this processor */
        String settingsString;

        /** the error message list for this processor */
        StringList errorMessageList;

        /** the current MIDI program for this processor (from 0 to
         * 127) */
        Integer currentProgramIndex;

        /** tells whether the buffering of the FluidSynth library is
         * compensated (somehow) */
        Boolean fluidSynthBufferingIsCompensated;

        /** a ring buffer for setting the processing window of this
         * processor onto the raster of the underlying midi event
         * converter */
        _RingBuffer ringBuffer;

        /*--------------------*/

        /**
         * Creates a new descriptor
         *
         * @param[in] parent  the processor where this descriptor
         *                    belongs to
         */
        _EventProcessorDescriptor
            (FluidSynthPlugin_EventProcessor* parent);

        /*--------------------*/

        /**
         * Tells whether the underlying converter has been
         * correctly set up (e.g. by loading the dynamic libraries).
         *
         * @return  information about successful initialization of
         *          underlying converter
         */
        Boolean isCorrectlyInitialized () const;
        
        /*--------------------*/

        /**
         * Destroy a descriptor
         */
        ~_EventProcessorDescriptor ();

        /*--------------------*/

        /**
         * Update settings string by <C>st</C> and also updates midi
         * event converter
         *
         * @param[in] st  new settings string
         */
        void setSettings (IN String& st);

    };

}

/*====================*/

using Main::FluidSynthPlugin::_EventProcessorDescriptor;
using Main::FluidSynthPlugin::_RingBuffer;

/*====================*/
/* _RingBuffer        */
/*====================*/

_RingBuffer::_RingBuffer ()
{
}

/*--------------------*/

_RingBuffer::~_RingBuffer ()
{
}

/*--------------------*/

void
_RingBuffer::processBlock
    (INOUT FluidSynthPlugin_EventProcessor* processor,
     INOUT _EventProcessorDescriptor& descriptor,
     IN Integer currentTime,
     IN MidiEventList& midiEventList,
     OUT AudioSampleListVector& sampleListVector,
     IN Natural audioFrameCount)
{
    Logging_trace3(">>: currentTime = %1, midiEventList = %2,"
                   " audioFrameCount = %3",
                   TOSTRING(currentTime), midiEventList.toString(),
                   TOSTRING(audioFrameCount));

    MidiEventConverter* eventConverter = descriptor.midiEventConverter;
    const Natural channelCount = sampleListVector.length();
    const Natural synthesizerBufferSize =
        eventConverter->synthesizerBufferSize();
    AudioSampleListVector localSampleBuffer{channelCount};
    localSampleBuffer.setFrameCount(synthesizerBufferSize);

    if (currentTime != lastProcessedTime) {
        const Natural relativeTime =
            Natural{(size_t) currentTime} % synthesizerBufferSize;
        Logging_trace1("--: relativeTime = %1", TOSTRING(relativeTime));
        sampleQueueList.setQueueCount(channelCount);
        sampleQueueList.ensureQueueCapacity(synthesizerBufferSize);

        if (relativeTime != delayDurationInSamples) {
            Logging_trace1("--: relative time differs from"
                           " delay duration %1",
                           TOSTRING(delayDurationInSamples));
            const Natural unprocessedSampleCount =
                (delayDurationInSamples == 0 ? 0
                 : synthesizerBufferSize - delayDurationInSamples);
            _resetBlockProcessing(eventConverter,
                                  unprocessedSampleCount,
                                  relativeTime, localSampleBuffer);
            delayDurationInSamples = relativeTime;
            scheduledEventList.clear();
            processor->setLatencySamples((int) delayDurationInSamples);
            sampleQueueList.appendToQueues(localSampleBuffer,
                                           0, delayDurationInSamples);
        }
    }

    /* shift events in list by an offset, merge them with the
     * unprocessed elements and split them at buffer size time
     * in samples */
    const Natural offset =
        (delayDurationInSamples == 0 ? 0
         : synthesizerBufferSize - delayDurationInSamples);

    MidiEventList currentEventList{midiEventList};
    currentEventList.shiftEventTimes(offset);
    scheduledEventList.merge(currentEventList);

    /* process samples until the available sample count is at least
     * the number of samples required */
    while (sampleQueueList.queueLength() < audioFrameCount) {
        const Natural durationInSamples = synthesizerBufferSize;
        currentEventList = scheduledEventList;
        _splitListAtTime(currentEventList,
                         durationInSamples, scheduledEventList);
        eventConverter->processBlock(currentEventList,
                                     localSampleBuffer,
                                     durationInSamples);
        Logging_trace1("--: localSampleBufferC = %1",
                       localSampleBuffer.toString(durationInSamples,
                                                  true));
        sampleQueueList.appendToQueues(localSampleBuffer,
                                       0, durationInSamples);
        const Integer backShift = -Integer{durationInSamples};
        scheduledEventList.shiftEventTimes(backShift);
    };

    sampleQueueList.popFromQueues(sampleListVector, 0, audioFrameCount);
    lastProcessedTime += audioFrameCount;

    Logging_trace1("<<: %1",
                   sampleListVector.toString(audioFrameCount, true));
}

/*===========================*/
/* _EventProcessorDescriptor */
/*===========================*/

_EventProcessorDescriptor::_EventProcessorDescriptor
    (FluidSynthPlugin_EventProcessor* parent)
        : eventProcessor(parent),
          midiEventConverter{new MidiEventConverter(true)},
          currentProgramIndex{0},
          fluidSynthBufferingIsCompensated{false}
{
    if (!isCorrectlyInitialized()) {
        errorMessageList.append(_errorMessageForBadLibraryInitialization);
    } else {
        midiEventConverter->setSetting(_settingsKeyForPreset,
                                       TOSTRING(currentProgramIndex));
    }
}

/*--------------------*/

_EventProcessorDescriptor::~_EventProcessorDescriptor ()
{
    delete midiEventConverter;
}

/*--------------------*/

Boolean _EventProcessorDescriptor::isCorrectlyInitialized () const
{
    Logging_trace(">>");
    Boolean result = (midiEventConverter->isCorrectlyInitialized());
    Logging_trace1("<<: %1", TOSTRING(result));
    return result;
}

/*--------------------*/

void _EventProcessorDescriptor::setSettings (IN String& st)
{
    Logging_trace1(">>: %1", st);

    Boolean libraryIsAvailable = isCorrectlyInitialized();
    errorMessageList.clear();

    if (!libraryIsAvailable) {
        errorMessageList.append(_errorMessageForBadLibraryInitialization);
    }

    midiEventConverter->resetSettings();
    settingsString = st;

    const String entrySeparator{"#"};
    const String nlSt = STR::newlineReplacedString(st, entrySeparator);
    Dictionary d =
        Dictionary::makeFromString(nlSt, entrySeparator, "=");
    StringList keyList = 
        Dictionary::makeKeyListFromString(nlSt, entrySeparator, "=");

    for (String key : keyList) {
        String value = Environment::expand(d.at(key));

        if (key == _fsBufferingCompensationKey) {
            fluidSynthBufferingIsCompensated =
                (STR::toLowercase(value) == "true");
        } else {
            Boolean isOkay = midiEventConverter->setSetting(key, value);

            if (libraryIsAvailable && !isOkay) {
                String errorMessage =
                    STR::expand("cannot set '%1' to '%2'", key, value);
                errorMessageList.append(errorMessage);
            }
        }
    }

    Logging_trace("<<");
}

/*====================*/

/*--------------------*/
/* internal routines  */
/*--------------------*/

/**
 * Converts JUCE MIDI event list to local MIDI event list.
 *
 * @param[in] juceMidiEventList  midi event list to be converted
 * @return  MIDI event list in local format
 */
static MidiEventList
_convertFromJuceEventList(IN juce::MidiBuffer& juceMidiEventList)
{
    Logging_trace(">>");

    MidiEventList result;

    /* traverse all the events */
    for (juce::MidiMessageMetadata metadata : juceMidiEventList) {
        Natural eventTime{(size_t) metadata.samplePosition};
        const juce::uint8* byteList = metadata.data;
        ByteList midiDataList;

        /* copy all the midi bytes to the MIDI data list */
        for (int i = 0;  i < metadata.numBytes;  i++) {
            midiDataList.append(Byte{byteList[i]});
        }

        MidiEvent midiEvent{eventTime, midiDataList};
        result.append(midiEvent);
    }

    Logging_trace1("<<: %1", result.toString());
    return result;
}

/*--------------------*/

/**
 * Copies data from <C>sampleListVector</C> to JUCE
 * <C>audioBuffer</C>.
 *
 * @param[in]  sampleListVector  list of rendered samples
 * @param[out] audioBuffer       JUCE audio buffer to be changed
 */
template <typename T>
static void
_copyToJuceBuffer (IN AudioSampleListVector sampleListVector,
                   OUT juce::AudioBuffer<T>& audioBuffer)
{
    Logging_trace(">>");

    const Natural channelCount = sampleListVector.length();
    const Natural frameCount = sampleListVector.frameCount();

    for (Natural channel = 0;  channel < channelCount;  channel++) {
        const AudioSample* sampleList =
            sampleListVector[channel].asArray();
        T* otherSampleList =
            (T*) audioBuffer.getWritePointer((int) channel);
        convertArray(otherSampleList, sampleList, frameCount);
    }
}

/*--------------------*/

/**
 * Processes events in <C>juceMidiEventList</C> using <C>processor</C>
 * and its <C>descriptor</C> and returns block of samples in
 * <C>audioBuffer</C>; note that the processing might be delayed to
 * adapt to the raster size of the underlying fluidsynth synthesizer
 *
 * @param[inout] processor          event processor object
 * @param[inout] descriptor         associated processor descriptor
 * @param[in]    juceMidiEventList  list of midi events starting at
 *                                  given time
 * @param[out]   audioBuffer        vector of audio sample lists
 *                                  returned
 */
template <typename T>
static void
_processBlock (INOUT FluidSynthPlugin_EventProcessor* processor,
               INOUT _EventProcessorDescriptor& descriptor,
               IN juce::MidiBuffer& juceMidiEventList,
               OUT juce::AudioBuffer<T>& audioBuffer)
{
    Logging_trace(">>");

    Integer currentTime = _readTimeInSamples(processor->getPlayHead());

    /* provide a sample list vector from audioBuffer */
    MidiEventList midiEventList =
        _convertFromJuceEventList(juceMidiEventList);
    const Natural channelCount =
        Natural{(size_t) audioBuffer.getNumChannels()};
    const Natural audioFrameCount =
        Natural{(size_t) audioBuffer.getNumSamples()};
    AudioSampleListVector sampleListVector{channelCount};
    sampleListVector.setFrameCount(audioFrameCount);

    if (!descriptor.fluidSynthBufferingIsCompensated) {
        /* do a direct processing of the events to a sample list
         * vector */
        MidiEventConverter* eventConverter =
            descriptor.midiEventConverter;
        eventConverter->processBlock(midiEventList,
                                     sampleListVector, audioFrameCount);
    } else {
        _RingBuffer& ringBuffer = descriptor.ringBuffer;
        ringBuffer.processBlock(processor, descriptor,
                                currentTime, midiEventList,
                                sampleListVector, audioFrameCount);
    }

    Logging_trace1("--: currentSamples = %1",
                   sampleListVector.toString(audioFrameCount, true));
    _copyToJuceBuffer<T>(sampleListVector, audioBuffer);

    Logging_trace("<<");
}

/*--------------------*/

/**
 * Reads current time in samples and returns its value.
 *
 * @param[in] playHead  the JUCE audio play head
 * @return  current time (as a real value)
 */
static Integer _readTimeInSamples (juce::AudioPlayHead* playHead)
{
    Integer currentTimeInSamples  = Integer::maximumValue();;

    if (playHead != nullptr) {
        currentTimeInSamples =
            (int) *(playHead->getPosition()->getTimeInSamples());
    }

    return currentTimeInSamples;
}

/*--------------------*/

/**
 * Processes samples to bring sample count to a multiple of
 * synthesizer buffer size.
 *
 * @param[inout]  eventConverter          underlying fluidsynth event
 *                                        converter
 * @param[in]     unprocessedSampleCount  offset in samples to bring the
 *                                        buffer to underlying synthesizer
 *                                        raster
 * @param[in]     newOffsetInSamples      the new offset to bring the buffer
 *                                        to underlying synthesizer raster
 * @param[out]    sampleBuffer            the new offset to bring the buffer
 *                                        to underlying synthesizer raster
 */
static void
_resetBlockProcessing (INOUT MidiEventConverter* eventConverter,
                       IN Natural unprocessedSampleCount,
                       IN Natural newOffsetInSamples,
                       OUT AudioSampleListVector& sampleBuffer)
{
    Logging_trace2(">>: unprocessedSampleCount = %1, newOffset = %2",
                   TOSTRING(unprocessedSampleCount),
                   TOSTRING(newOffsetInSamples));

    const MidiEventList& emptyEventList{};

    /* provide samples to fill buffer up to buffer */
    if (unprocessedSampleCount > 0) {
        eventConverter->processBlock(emptyEventList, sampleBuffer,
                                     unprocessedSampleCount);
        Logging_trace1("--: samples filled = %1",
                       sampleBuffer.toString(unprocessedSampleCount, true));
    }

    eventConverter->processBlock(emptyEventList, sampleBuffer,
                                 newOffsetInSamples);
    Logging_trace1("<<: sampleBuffer = %1",
                   sampleBuffer.toString(newOffsetInSamples, true));
}

/*--------------------*/

/**
 * Scans events in <C>midiEventList</C>, keeps all elements less than
 * <C>splitTime</C> in that list and puts all elements greater or
 * equal into <C>otherEventList</C>.
 *
 * @param[inout] midiEventList   event list to be split
 * @param[in]    splitTime       time where element list is split
 * @param[inout] otherEventList  event list where elements greater
 *                               or equal to split time are put
 */
static void _splitListAtTime (INOUT MidiEventList& midiEventList,
                              IN Natural splitTime,
                              INOUT MidiEventList& otherEventList)
{
    otherEventList.clear();
    Natural i;
    const Natural eventCount = midiEventList.length();

    /* find split position */
    for (i = 0;  i < eventCount;  i++) {
        const MidiEvent& event = midiEventList[i];
        if (event.time() >= splitTime) {
            break;
        }
    }

    const Natural splitPosition = i;

    /* copy the events larger than split time to other list */
    for (;  i < eventCount;  i++) {
        otherEventList.append(midiEventList[i]);
    }

    midiEventList.setLength(splitPosition);
}

/*====================*/
/* PUBLIC FEATURES    */
/*====================*/

/*--------------------*/
/* setup              */
/*--------------------*/

FluidSynthPlugin_EventProcessor::FluidSynthPlugin_EventProcessor ()
    : juce::AudioProcessor(juce::AudioProcessor::BusesProperties()
                           .withOutput("Output",
                                       juce::AudioChannelSet::stereo(),
                                       true)),
      _descriptor{NULL}
{
    Logging_initializeWithDefaults("FluidSynthPlugin",
                                   "FluidSynthPlugin.");
    Logging_trace(">>");

    /* add executable directory path to library path */
    const String executableDirectoryPath =
        OperatingSystem::executableDirectoryPath(false);
    DynamicLibrary::addToSearchPath(executableDirectoryPath);
    
    _descriptor = new _EventProcessorDescriptor(this);

    Logging_trace("<<");
}

/*--------------------*/

FluidSynthPlugin_EventProcessor::~FluidSynthPlugin_EventProcessor ()
{
    Logging_trace(">>");

    _EventProcessorDescriptor& descriptor =
        TOREFERENCE<_EventProcessorDescriptor>(_descriptor);
    delete &descriptor;

    Logging_trace("<<");
    Logging_finalize();
}

/*--------------------*/
/* property queries   */
/*--------------------*/

bool
FluidSynthPlugin_EventProcessor::supportsDoublePrecisionProcessing () const
{
    return true;
}


/*--------------------*/

bool FluidSynthPlugin_EventProcessor::acceptsMidi () const
{
    return true;
}

/*--------------------*/

bool FluidSynthPlugin_EventProcessor::producesMidi () const
{
    return false;
}

/*--------------------*/

bool FluidSynthPlugin_EventProcessor::isMidiEffect () const
{
    return false;
}

/*--------------------*/

double FluidSynthPlugin_EventProcessor::getTailLengthSeconds () const
{
    return 0.0;
}

/*--------------------*/

int FluidSynthPlugin_EventProcessor::getNumPrograms ()
{
    return _midiProgramCount;
}

/*--------------------*/

int FluidSynthPlugin_EventProcessor::getCurrentProgram ()
{
    _EventProcessorDescriptor& descriptor =
        TOREFERENCE<_EventProcessorDescriptor>(_descriptor);
    return (int) descriptor.currentProgramIndex;
}

/*--------------------*/

const juce::String
FluidSynthPlugin_EventProcessor::getProgramName (int index)
{
    String indexAsString = TOSTRING(Integer{index});
    String programName = STR::expand("program-%1", indexAsString);
    return programName.c_str();
}

/*--------------------*/

bool FluidSynthPlugin_EventProcessor::hasEditor () const
{
    return true;
}

/*--------------------*/

const juce::String FluidSynthPlugin_EventProcessor::getName () const
{
    return juce::String("FluidSynthPlugin");
}

/*--------------------*/

StringList FluidSynthPlugin_EventProcessor::presetList () const
{
    Logging_trace(">>");
    _EventProcessorDescriptor& descriptor =
        TOREFERENCE<_EventProcessorDescriptor>(_descriptor);
    StringList result = descriptor.midiEventConverter->presetList();
    Logging_trace1("<<: %1", result.toString());
    return result;
}
        
/*--------------------*/
/* property change    */
/*--------------------*/

juce::AudioProcessorEditor*
FluidSynthPlugin_EventProcessor::createEditor ()
{
    Logging_trace(">>");
    Logging_trace("<<");
    return new FluidSynthPlugin_Editor(*this);
}

/*--------------------*/

void FluidSynthPlugin_EventProcessor::setCurrentProgram (int index)
{
    String indexAsString = TOSTRING(Integer{index});
    Logging_trace1(">>: %1", indexAsString);
    Assertion_pre(0 <= index && index < _midiProgramCount,
                  STR::expand("must be a MIDI program number: %1",
                              indexAsString));

    _EventProcessorDescriptor& descriptor =
        TOREFERENCE<_EventProcessorDescriptor>(_descriptor);
    descriptor.currentProgramIndex = index;
    MidiEventConverter& midiEventConverter =
        TOREFERENCE<MidiEventConverter>(descriptor.midiEventConverter);
    ByteList byteList;
    byteList.setLength(2);
    byteList.set(1, Byte{index});

    for (Byte midiChannel = 0;  midiChannel < 16;  midiChannel++) {
        Byte statusByte = Byte{0xC0} + midiChannel;
        byteList.set(0, statusByte);
        MidiEvent programChangeEvent{0, byteList};
        midiEventConverter.processMidiEvent(programChangeEvent);
    }

    Logging_trace("<<");
}

/*--------------------*/

void
FluidSynthPlugin_EventProcessor::changeProgramName (int, const juce::String&)
{
    /* does not apply */
}

/*--------------------*/
/* property access    */
/*--------------------*/

Boolean FluidSynthPlugin_EventProcessor::isInErrorState () const
{
    Logging_trace(">>");

    const _EventProcessorDescriptor& descriptor =
        TOREFERENCE<_EventProcessorDescriptor>(_descriptor);
    const Boolean result = !descriptor.errorMessageList.isEmpty();

    Logging_trace1("<< %1", TOSTRING(result));
    return result;
}

/*--------------------*/

String FluidSynthPlugin_EventProcessor::fsLibraryVersion () const
{
    Logging_trace(">>");

    const _EventProcessorDescriptor& descriptor =
        TOREFERENCE<_EventProcessorDescriptor>(_descriptor);
    String result = descriptor.midiEventConverter->fsLibraryVersion();

    Logging_trace1("<< %1", result);
    return result;
}

/*--------------------*/

String FluidSynthPlugin_EventProcessor::messageString () const
{
    Logging_trace(">>");

    _EventProcessorDescriptor& descriptor =
        TOREFERENCE<_EventProcessorDescriptor>(_descriptor);

    StringList& errorMessageList = descriptor.errorMessageList;
    String result;

    if (!errorMessageList.isEmpty()) {
        result = errorMessageList[0];
    } else {
        /* no error messages, construct message from SoundFont name
         * and preset name */
        const MidiEventConverter* midiEventConverter =
            descriptor.midiEventConverter;
        result = STR::expand("preset: %1 - %2",
                             midiEventConverter->soundFontName(),
                             midiEventConverter->presetName());
    }

    Logging_trace1("<<: %1", result);
    return result;
}

/*--------------------*/

MidiPresetIdentification
FluidSynthPlugin_EventProcessor::preset () const
{
    Logging_trace(">>");

    const _EventProcessorDescriptor& descriptor =
        TOREFERENCE<_EventProcessorDescriptor>(_descriptor);
    const MidiEventConverter* midiEventConverter =
        descriptor.midiEventConverter;
    String value;
    const Boolean isOkay =
        midiEventConverter->getSetting(_settingsKeyForPreset, value);
    value = (!isOkay ? "0:0" : value);
    const MidiPresetIdentification result{value};

    Logging_trace1("<<: %1", result.toString());
    return result;
}

/*--------------------*/

String FluidSynthPlugin_EventProcessor::settings () const
{
    Logging_trace(">>");

    const _EventProcessorDescriptor& descriptor =
        TOREFERENCE<_EventProcessorDescriptor>(_descriptor);
    String result = descriptor.settingsString;

    Logging_trace1("<<: %1", STR::newlineReplacedString(result));
    return result;
}

/*--------------------*/
/* property change    */
/*--------------------*/

void FluidSynthPlugin_EventProcessor
::setPreset (IN MidiPresetIdentification& preset)
{
    Logging_trace(">>");

    _EventProcessorDescriptor& descriptor =
        TOREFERENCE<_EventProcessorDescriptor>(_descriptor);
    MidiEventConverter* midiEventConverter =
        descriptor.midiEventConverter;
    midiEventConverter->setSetting(_settingsKeyForPreset, preset.toString());
    
    Logging_trace("<<");
}

/*--------------------*/

void FluidSynthPlugin_EventProcessor::setSettings (IN String& st)
{
    Logging_trace1(">>: %1", STR::newlineReplacedString(st));

    _EventProcessorDescriptor& descriptor =
        TOREFERENCE<_EventProcessorDescriptor>(_descriptor);
    descriptor.setSettings(st);

    FluidSynthPlugin_Editor* editor =
        (FluidSynthPlugin_Editor*) getActiveEditor();

    if (editor != nullptr) {
        editor->update();
    }

    Logging_trace("<<");
}

/*--------------------*/
/* persistence        */
/*--------------------*/

void
FluidSynthPlugin_EventProcessor::getStateInformation
    (OUT juce::MemoryBlock& destData)
{
    Logging_trace(">>");

    /* stores state of processor in <C>destData</C> */
    const String st = settings();
    const Natural sizeInBytes = st.size();
    destData.setSize((int) sizeInBytes);
    destData.copyFrom(st.c_str(), 0, (int) sizeInBytes);

    Logging_trace1("<<: settings = %1", STR::newlineReplacedString(st));
}

/*--------------------*/

void
FluidSynthPlugin_EventProcessor::setStateInformation (IN void* data,
                                                      int sizeInBytes)
{
    Logging_trace(">>");

    /* restores state of audio processor from <C>data</C> */
    String st((char *) data, sizeInBytes);
    setSettings(st);

    Logging_trace1("<<: settings = %1", STR::newlineReplacedString(st));
}

/*--------------------*/
/* event handling     */
/*--------------------*/

void
FluidSynthPlugin_EventProcessor::prepareToPlay
    (IN double sampleRate, IN int maximumExpectedSamplesPerBlock)
{
    const Real sRate{sampleRate};
    Natural sampleCount{maximumExpectedSamplesPerBlock};
    Logging_trace2(">>: sampleRate = %1, samplesPerBlock = %2",
                   TOSTRING(sRate), TOSTRING(sampleCount));

    Integer currentTime = _readTimeInSamples(getPlayHead());
    Logging_trace1("--: currentTime = %1 [samples]",
                   TOSTRING(currentTime));

    _EventProcessorDescriptor& descriptor =
        TOREFERENCE<_EventProcessorDescriptor>(_descriptor);
    descriptor.midiEventConverter->prepareToPlay(sRate, sampleCount);

    Logging_trace("<<");
}

/*--------------------*/

void FluidSynthPlugin_EventProcessor::releaseResources ()
{
    Logging_trace(">>");

    _EventProcessorDescriptor& descriptor =
        TOREFERENCE<_EventProcessorDescriptor>(_descriptor);
    descriptor.midiEventConverter->releaseResources();

    Logging_trace("<<");
}

/*--------------------*/

void
FluidSynthPlugin_EventProcessor::processBlock
    (juce::AudioBuffer<double>& audioBuffer,
     juce::MidiBuffer& juceMidiEventList)
{
    Logging_trace(">>");
    _EventProcessorDescriptor& descriptor =
        TOREFERENCE<_EventProcessorDescriptor>(_descriptor);
    _processBlock<double>(this, descriptor, juceMidiEventList, audioBuffer);
    Logging_trace("<<");
}

/*--------------------*/

void
FluidSynthPlugin_EventProcessor::processBlock
    (juce::AudioBuffer<float>& audioBuffer,
     juce::MidiBuffer& juceMidiEventList)
{
    Logging_trace(">>");
    _EventProcessorDescriptor& descriptor =
        TOREFERENCE<_EventProcessorDescriptor>(_descriptor);
    _processBlock<float>(this, descriptor, juceMidiEventList, audioBuffer);
    Logging_trace("<<");
}

/*--------------------*/

/**
 * Provides a callback for JUCE to create a FluidSynth audio processor.
 *
 * @return JUCE audio processor
 */
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter ()
{
    return new FluidSynthPlugin_EventProcessor();
}
