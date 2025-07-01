/**
 * @file
 * The <C>FluidSynthFileConverter</C> module implements a conversion
 * from a MIDI file into a wave file using the FluidSynth library.
 *
 * @author Dr. Thomas Tensi
 * @date   2022-08
 */

/*=========*/
/* IMPORTS */
/*=========*/

#include "FluidSynthFileConverter.h"

#include "Assertion.h"
#include "CommandLineArguments.h"
#include "DynamicLibrary.h"
#include "File.h"
#include "Logging.h"
#include "MidiEventConverter.h"
#include "MidiFile.h"
#include "OperatingSystem.h"
#include "WaveFile.h"

/*--------------------*/

using Audio::AudioSample;
using Audio::AudioSampleList;
using Audio::WaveFile;
using BaseModules::CommandLineArgument;
using BaseModules::CommandLineArgumentList;
using BaseModules::CommandLineArgumentHandler;
using BaseModules::File;
using BaseModules::OperatingSystem;
using Libraries::DynamicLibrary;
using MIDI::MidiEventConverter;
using MIDI::MidiEventKind;
using MIDI::MidiMetaEventKind;
using MIDI::MidiFile;
using Main::FluidSynthFileConverter::ConverterProgram;

/** abbreviation for StringUtil */
using STR = BaseModules::StringUtil;

/*====================*/

/**
 * A simple structure for storing an event time in different formats
 * (midiTime, timeInSeconds, timeInSamples)
 */
struct _EventTime {

    /** the event time in midi ticks */
    Integer midiTime;

    /** the event time in seconds */
    Real timeInSeconds;

    /** the event time in samples */
    Integer timeInSamples;

    /*--------------------*/

    /**
     * Constructs a new event time object from <C>newMidiTime</C>,
     * <C>newTimeInSeconds</C>, and <C>newTimeInSamples</C>.
     *
     * @param[in] newMidiTime         event time as MIDI ticks
     * @param[in] newTimeInSeconds    event time as seconds
     * @param[in] newTimeInSamples    event time as sample count
     */
    _EventTime (Integer newMidiTime = 0,
                Real newTimeInSeconds = 0.0,
                Integer newTimeInSamples = 0)
        : midiTime{newMidiTime},
          timeInSeconds{newTimeInSeconds},
          timeInSamples{newTimeInSamples}
    {
    }
    
    /*--------------------*/

    /**
     * Returns string representation of current event time.
     *
     * @return string representation of time
     */     
    String toString () const
    {
        String st =
            STR::expand("_EventTime"
                        "(midiTime = %1, timeInSeconds = %2,"
                        " timeInSamples = %3)",
                        TOSTRING(midiTime), TOSTRING(timeInSeconds),
                        TOSTRING(timeInSamples));
        return st;
    }

    /*--------------------*/

    /**
     * Converts <C>midiEventTime</C> in MIDI ticks to event time using
     * reference time <C>previousEventTime</C> using factors
     * <C>midiTimeToSecondsFactor</C> and <C>sampleRate</C>.
     *
     * @param[in]  midiEventTime            event time in MIDI ticks
     * @param[in]  previousTime             the previous event time
     * @param[in]  midiTimeToSecondsFactor  conversion factor from
     *                                      ticks to seconds
     * @param[in]  sampleRate               current sample rate (in Hz)
     * @return  event time structure for given 
     */
    static _EventTime makeRelative (IN Integer midiEventTime,
                                    IN _EventTime& previousTime,
                                    IN Real midiTimeToSecondsFactor,
                                    IN Real sampleRate)
    {
        Logging_trace4(">>: midiEventTime = %1, otherTime = %2,"
                       " midiTimeToSecondsFactor = %3, sampleRate = %4",
                       TOSTRING(midiEventTime),
                       previousTime.toString(),
                       TOSTRING(midiTimeToSecondsFactor),
                       TOSTRING(sampleRate));

        Real eventTimeInSeconds = 
            (previousTime.timeInSeconds +
             (Real{midiEventTime - previousTime.midiTime}
              * midiTimeToSecondsFactor));
        Integer eventTimeInSamples =
            (Integer) Real::round(eventTimeInSeconds * sampleRate);
        _EventTime result{midiEventTime, eventTimeInSeconds,
                          eventTimeInSamples};

        Logging_trace1("<<: %1", result.toString());
        return result;
    }    

};

/*====================*/
/* PRIVATE FEATURES   */
/*====================*/

/** name of this program (e.g. for error messages) */
static const String _programName = "fluidSynthFileConverter";

/** version of this program */
const String _version = "0.2";

/** count of channels supported by this program */
const Natural _channelCount = 2;

/** default sample rate of fluidsynth */
const Real _defaultSampleRate = 44100.0;

/*--------------------*/

/**
 * mapping from file format name to type code and byte width
 */
static const Dictionary _fileFormatToTypeCodeAndWidthMap =
    Dictionary::makeFromString(
        "double -> R/8,"
        "float  -> R/4,"
        "s16    -> I/2,"
        "s24    -> I/3,"
        "s32    -> I/4,"
        "s8     -> I/1,"
        "u8     -> N/1"
    );

/*--------------------*/

/**
 * mapping from command line argument name to encoded option
 * information (including abstract name, number of parameters and
 * parameter kind)
 */
static const Dictionary _nameToAbstractArgumentDataMap =
    Dictionary::makeFromString(
        "NAME:*.mid             -> MIDIFILE/0/-,"
        "NAME:*.sf2             -> SOUNDFONTFILE/0/-,"
        "NAME:*.sf3             -> SOUNDFONTFILE/0/-,"
        "-a                     -> UNSUPPORTED/1/S,"
        "--audio-driver=        -> UNSUPPORTED/0/S,"
        "-C                     -> CHORUS/1/B,"
        "--chorus               -> CHORUS/1/B,"
        "-c                     -> UNSUPPORTED/1/N,"
        "--audio-bufcount=      -> UNSUPPORTED/0/N,"
        "-d                     -> UNSUPPORTED/0/-,"
        "-dump                  -> UNSUPPORTED/0/-,"
        "-E                     -> UNSUPPORTED/0/-,"
        "-f                     -> CONFIGFILE/1/S,"
        "--load-config          -> CONFIGFILE/1/S,"
        "-F                     -> DESTINATIONFILE/1/S,"
        "--fast-render=         -> DESTINATIONFILE/1/S,"
        "-G                     -> UNSUPPORTED/1/N,"
        "--audio-groups         -> UNSUPPORTED/1/N,"
        "-g                     -> GAIN/1/R,"
        "--gain                 -> GAIN/1/R,"
        "-h                     -> HELP/0/-,"
        "--help                 -> HELP/0/-,"
        "-i                     -> NOSHELL/0/-,"
        "--no-shell             -> NOSHELL/0/-,"
        "-j                     -> UNSUPPORTED/0/-,"
        "-connect-jack-outputs  -> UNSUPPORTED/0/-,"
        "-K                     -> UNSUPPORTED/1/N,"
        "--midi-channels=       -> UNSUPPORTED/0/N,"
        "-L                     -> UNSUPPORTED/1/N,"
        "--audio-channels=      -> UNSUPPORTED/1/N,"
        "-l                     -> UNSUPPORTED/0/-,"
        "--disable-lash         -> UNSUPPORTED/0/-,"
        "-m                     -> UNSUPPORTED/1/S,"
        "--midi-driver=         -> UNSUPPORTED/0/S,"
        "-n                     -> NOMIDIIN/0/-,"
        "--no-midi-in           -> NOMIDIIN/0/-,"
        "-O                     -> AUDIOFORMAT/1/S,"
        "-audio-file-format     -> AUDIOFORMAT/1/S,"
        "-o                     -> SETTING/1/S,"
        "-p                     -> UNSUPPORTED/1/S,"
        "--portname=            -> UNSUPPORTED/1/S,"
        "-q                     -> QUIET/0/-,"
        "--quiet                -> QUIET/0/-,"
        "-R                     -> REVERB/1/B,"
        "--reverb               -> REVERB/1/B,"
        "-r                     -> SAMPLERATE/1/R,"
        "--sample-rate          -> SAMPLERATE/1/R,"
        "-s                     -> UNSUPPORTED/0/-,"
        "--server               -> UNSUPPORTED/0/-,"
        "-T                     -> AUDIOFILETYPE/1/S,"
        "--audio-file-type      -> AUDIOFILETYPE/1/S,"
        "-v                     -> VERBOSE/0/-,"
        "--verbose              -> VERBOSE/0/-,"
        "-V                     -> VERSION/0/-,"
        "--version              -> VERSION/0/-,"
        "-z                     -> UNSUPPORTED/1/N,"
        "--audio-bufsize=       -> UNSUPPORTED/1/N"
    );

/*--------------------*/

/** the usage text for this program */
static const String _usageText =
    String{
        "usage: %1 [options] filenames\n"
        "-----\n"
        "options:\n"
        "  -a, --audio-driver=[label]\n"
        "    audio driver to use (IGNORED)\n"
        "  -C, --chorus\n"
        "    turn chorus on or off [0|1|yes|no, default = on]\n"
        "  -c, --audio-bufcount=[count]\n"
        "    number of audio buffers (IGNORED)\n"
        "  -d, --dump\n"
        "    dump incoming and outgoing MIDI events to stdout (IGNORED)\n"
        "  -E, --audio-file-endian\n"
        "    audio file endian (IGNORED: always little endian)\n"
        "  -f, --load-config\n"
        "    right upon starting, load and execute a configuration\n"
        "    file containing fluidsynth related shell commands\n"
        "  -F, --fast-render=[file]\n"
        "    render MIDI file to raw audio data and store in [file]\n"
        "  -G, --audio-groups\n"
        "    define the number of LADSPA audio nodes (IGNORED)\n"
        "  -g, --gain\n"
        "    set the master gain [0 < gain < 10, default = 0.2]\n"
        "  -h, --help\n"
        "    print out this help summary\n"
        "  -i, --no-shell\n"
        "    don't read commands from the shell (IGNORED)\n"
        "  -j, --connect-jack-outputs\n"
        "    connect jack outputs to the physical ports (IGNORED)\n"
        "  -K, --midi-channels=[num]\n"
        "    number of midi channels [default = 16] (IGNORED)\n"
        "  -L, --audio-channels=[num]\n"
        "    number of stereo audio channels [default = 1] (IGNORED)\n"
        "  -l, --disable-lash\n"
        "    don't connect to LASH server (IGNORED)\n"
        "  -m, --midi-driver=[label]\n"
        "    name of the midi driver to use (IGNORED)\n"
        "  -n, --no-midi-in\n"
        "    don't create midi driver to read MIDI input events (IGNORED)\n"
        "  -O, --audio-file-format\n"
        "    audio file format for fast rendering (IGNORED)\n"
        "  -o\n"
        "    define a setting, -o name=value; see FluidSynth for details\n"
        "  -p, --portname=[label]\n"
        "    set MIDI port name (IGNORED)\n"
        "  -q, --quiet\n"
        "    do not print informational output (synth.verbose=false)\n"
        "  -R, --reverb\n"
        "    turn reverb on or off [0|1|yes|no, default = on]\n"
        "  -r, --sample-rate\n"
        "    set the sample rate\n"
        "  -s, --server\n"
        "    start FluidSynth as a server process (IGNORED)\n"
        "  -T, --audio-file-type\n"
        "    audio file type for fast rendering (IGNORED: always WAV)\n"
        "  -v, --verbose\n"
        "    print out verbose info about midi events (synth.verbose=true)\n"
        "  -V, --version\n"
        "    show version of program\n"
        "  -z, --audio-bufsize=[size]\n"
        "    size of each audio buffer (IGNORED)"
    };

/*----------------------*/
/* FORWARD DECLARATIONS */
/*----------------------*/

static void _renderEvents (IN Dictionary& settings,
                           IN Real sampleRate,
                           IN Natural midiTicksPerQuarterNote,
                           IN MidiEventList& midiEventList,
                           OUT AudioSampleListVector& sampleBuffer);

static void
_renderSynchronousEvents (INOUT MidiEventConverter& midiEventConverter,
                          IN MidiEventList& synchronousEventList,
                          IN Natural trailingSampleCount,
                          INOUT AudioSampleListVector& temporaryBuffer,
                          INOUT AudioSampleListVector& destinationBuffer);

static Real _updatedTimeFactor (IN MidiEvent& event,
                                IN Natural midiTicksPerQuarterNote);

static void _writeMessage (IN String& message,
                           IN Boolean isError = true);

/*--------------------*/
/*--------------------*/

/**
 * Finalizes all internal data of the main program.
 */
static void _finalize ()
{
    Logging_finalize();
}

/*--------------------*/

/**
 * Initializes all internal data of the main program.
 */
static void _initialize ()
{
    Logging_initializeWithDefaults("FluidSynthFileConverter",
                                   "FluidSynthFileConverter.");
}

/*--------------------*/
/*--------------------*/

/**
 * Checks whether <C>fileName</C> is readable or writeable
 * (depending on <C>isCheckedForReading</C> and returns success.
 *
 * @param[in] fileName             name of file to be checked
 * @param[in] isCheckedForReading  flag to indicate read or write
 *                                 access
 * @return  information about success
 */
static Boolean _checkFileAccess (IN String& fileName,
                                 IN Boolean isCheckedForReading)
{
    Logging_trace2(">>: file = '%1', isCheckedForReading = %2",
                   fileName, TOSTRING(isCheckedForReading));

    Boolean isOkay;

    if (isCheckedForReading) {
        isOkay = OperatingSystem::fileExists(fileName);
    } else {
        File outputFile;
        outputFile.open(fileName, "w");
        isOkay = outputFile.isOpen();
        outputFile.close();
    }

    if (!isOkay) {
        String format = (isCheckedForReading
                         ? "cannot read %1" : "cannot write %1");
        String message = STR::expand(format, fileName);
        _writeMessage(message);
    }

    Logging_trace1("<<: %1", TOSTRING(isOkay));
    return isOkay;
}

/*--------------------*/

/**
 * Handles command line options and updates <C>midiFileName</C>,
 * <C>waveFileName</C>, <C>sampleRate</C>, <C>audioFileFormat</C> and
 * <C>renderSettings</C>.
 *
 * @param[in]  argumentCount    number of arguments in command line
 * @param[in]  argv             array of argument strings
 * @param[out] midiFileName     midi source file name
 * @param[out] waveFileName     destination wave file name
 * @param[out] sampleRate       selected sample rate
 * @param[out] audioFileFormat  string code for the wave audio file
 *                              format
 * @param[out] renderSettings   settings for fluidsynth player
 * @return  information whether argument scanning was successful
 */
static Boolean
_handleCommandLineArguments (IN Natural argumentCount,
                             char* argv[],
                             OUT String& midiFileName,
                             OUT String& waveFileName,
                             OUT Real& sampleRate,
                             OUT String& audioFileFormat,
                             OUT Dictionary& renderSettings)
{
    Logging_trace1(">>: argumentCount = %1", TOSTRING(argumentCount));

    Boolean isOkay = true;
    Boolean fileIsOkay;
    StringList argumentList;

    for (Natural i = 0;  i < argumentCount;  i++) {
        argumentList.append(argv[(size_t) i]);
    }

    CommandLineArgumentHandler optionHandler;
    CommandLineArgumentList optionList =
        optionHandler.convert(argumentList,
                              _nameToAbstractArgumentDataMap);

    /* set to default values */
    sampleRate      = _defaultSampleRate;
    audioFileFormat = "s16";

    for (const CommandLineArgument& option : optionList) {
        Logging_trace1("--: processing %1", option.toString());
        const String& optionName        = option.originalName;
        const String& abstractName      = option.abstractName;
        const StringList& parameterList = option.parameterList;

        if (abstractName == "AUDIOFILETYPE") {
            String message =
                STR::expand("audio file type option '%1' ignored,"
                            " only WAV supported",
                            optionName);
            Logging_trace1("--: %1", message);
        } else if (abstractName == "AUDIOFORMAT") {
            const String& parameter =
                StringUtil::toLowercase(parameterList[0]);
            
            if (_fileFormatToTypeCodeAndWidthMap.contains(parameter)) {
                audioFileFormat = parameter;
            } else {
                String message =
                    STR::expand("unknown audio format option '%1'",
                                optionName);
                Logging_trace1("--: %1", message);
                isOkay = false;
            }
        } else if (abstractName == "CHORUS") {
            const String& parameter =
                StringUtil::toLowercase(parameterList[0]);
            Boolean isActive = (parameter == "1" || parameter == "yes");
            renderSettings.set("synth.chorus.active",
                               (isActive ? "1" : "0"));
        } else if (abstractName == "CONFIGFILE") {
            Logging_traceError("CONFIGFILE not yet supported");
        } else if (abstractName == "ERR_BADFILE") {
            String message =
                STR::expand("unknown file type %1", optionName);
            _writeMessage(message);
            isOkay = false;
        } else if (abstractName == "ERR_BADPARAMS") {
            String message =
                STR::expand("bad parameter(s) for %1", optionName);
            _writeMessage(message);
            isOkay = false;
        } else if (abstractName == "ERR_NOPARAMS") {
            String message =
                STR::expand("not enough parameters left for %1", optionName);
            _writeMessage(message);
            isOkay = false;
        } else if (abstractName == "ERR_UNKNOWN_OPTION") {
            String message = STR::expand("unknown option %1", optionName);
            _writeMessage(message);
            isOkay = false;
        } else if (abstractName == "GAIN") {
            const String& parameter = parameterList[0];
            renderSettings.set("synth.gain", parameter);
        } else if (abstractName == "HELP") {
            OperatingSystem::writeMessageToConsole(_usageText);
        } else if (abstractName == "MIDIFILE") {
            midiFileName = optionName;
            fileIsOkay = _checkFileAccess(midiFileName, true);
            isOkay = isOkay && fileIsOkay;
        } else if (abstractName == "NOMIDIIN") {
            String message =
                STR::expand("nomidiin option '%1' ignored", optionName);
            Logging_trace1("--: %1", message);
        } else if (abstractName == "NOSHELL") {
            String message =
                STR::expand("noshell option '%1' ignored", optionName);
            Logging_trace1("--: %1", message);
        } else if (abstractName == "QUIET") {
            renderSettings.set("synth.verbose", "0");
        } else if (abstractName == "REVERB") {
            const String& parameter =
                StringUtil::toLowercase(parameterList[0]);
            Boolean isActive = (parameter == "1" || parameter == "yes");
            renderSettings.set("synth.reverb.active",
                               (isActive ? "1" : "0"));
        } else if (abstractName == "SAMPLERATE") {
            const String& parameter = parameterList[0];
            sampleRate = StringUtil::toReal(parameter);
            renderSettings.set("synth.sample-rate", parameter);
        } else if (abstractName == "SETTING") {
            const String& parameter = parameterList[0];
            String prefix;
            String suffix;
            Boolean isSplit = StringUtil::splitAt(parameter, "=",
                                                  prefix, suffix);

            if (isSplit) {
                renderSettings.set(prefix, suffix);
            } else {
                String message =
                    STR::expand("bad parameter '%1' for -o", parameter);
                _writeMessage(message);
                isOkay = false;
            }
        } else if (abstractName == "SOUNDFONTFILE") {
            String soundFontFileName = optionName;
            renderSettings.set("soundfont", soundFontFileName);
            fileIsOkay = _checkFileAccess(soundFontFileName, true);
            isOkay = isOkay && fileIsOkay;
        } else if (abstractName == "DESTINATIONFILE") {
            waveFileName = parameterList[0];
            fileIsOkay = _checkFileAccess(waveFileName, false);
            isOkay = isOkay && fileIsOkay;
        } else if (abstractName == "UNSUPPORTED") {
            String message =
                STR::expand("unsupported option %1", optionName);
            _writeMessage(message);
            isOkay = false;
        } else if (abstractName == "VERBOSE") {
            renderSettings.set("synth.verbose", "1");
        } else if (abstractName == "VERSION") {
            String message = STR::expand("%1 %2", _programName, _version);
            _writeMessage(message, false);
        }
    }

    Logging_trace6("<<: isOkay = %1, midiFileName = %2,"
                   " waveFileName = %3, sampleRate = %4,"
                   " audioFileFormat = %5, renderSettings = %6",
                   TOSTRING(isOkay), midiFileName,
                   waveFileName, TOSTRING(sampleRate),
                   audioFileFormat, renderSettings.toString());
    return isOkay;
}

/*--------------------*/

/**
 * Constructs midi event converter with render settings defined in
 * <C>settings</C>.
 *
 * @param[in] settings   dictionary of render settings (including e.g.
 *                       the sound font name)
 * @return  newly constructed midi event converter or NULL on failure
 */
static MidiEventConverter* 
_makeMidiEventConverter (IN Dictionary& settings)
{
    Logging_trace1(">>: settings = %1", settings.toString());

    MidiEventConverter* midiEventConverter =
        new MidiEventConverter(false);
    Boolean isOkay = midiEventConverter->isCorrectlyInitialized();

    if (!isOkay) {
        _writeMessage("cannot open FluidSynth library");
        delete midiEventConverter;
        midiEventConverter = NULL;
    } else {
        for (auto& [key, value] : settings) {
            Boolean settingIsOkay =
                midiEventConverter->setSetting(key, value);

            if (!settingIsOkay) {
                String errorMessage =
                    STR::expand("cannot set '%1' to '%2'", key, value);
                _writeMessage(errorMessage);
                isOkay = false;
            }
        }
    }

    Logging_trace1("<<: isOkay = %1", TOSTRING(isOkay));
    return midiEventConverter;
}

/*--------------------*/

/**
 * Converts MIDI file named <C>midiFileName</C> into a wave file named
 * <C>waveFileName</C> using the FluidSynth library with settings
 * given by <C>renderSettings</C> and sample rate <C>sampleRate</C>
 * with format <C>audioFileFormat</C>.
 *
 * @param[in] renderSettings   the fluidsynth player settings
 * @param[in] midiFileName     source midi file name
 * @param[in] sampleRate       the sample rate for the wave file
 * @param[in] audioFileFormat  string code for the wave audio file
 *                             format
 * @param[in] waveFileName     destination wave file name
 */
static void _process (IN Dictionary& renderSettings,
                      IN String& midiFileName,
                      IN Real sampleRate,
                      IN String& audioFileFormat,
                      IN String& waveFileName)
{
    Logging_trace5(">>: renderSettings = %1, midiFileName = '%2',"
                   " sampleRate = '%3', audioFileFormat = '%4',"
                   " waveFileName = '%5'",
                   renderSettings.toString(), midiFileName,
                   TOSTRING(sampleRate), audioFileFormat,
                   waveFileName);

    Assertion_pre(_fileFormatToTypeCodeAndWidthMap.contains(audioFileFormat),
                  STR::expand("unknown audio file format '%1'",
                              audioFileFormat));

    /* read MIDI file into event list */
    MidiEventList midiEventList;
    Natural fileType;
    Natural midiTicksPerQuarterNote;
    MidiFile midiFile{midiFileName};

    midiFile.read(fileType, midiTicksPerQuarterNote, midiEventList);

    /* render MIDI events into sample buffer via fluidsynth */
    AudioSampleListVector buffer{_channelCount};
    _renderEvents(renderSettings, sampleRate, midiTicksPerQuarterNote,
                  midiEventList, buffer);

    /* write samples as wave file */
    Logging_trace1("--: sample buffer prefix = %1", buffer.toString(100));
    WaveFile destinationFile{waveFileName};
    const Natural audioFrameCount = buffer.frameCount();

    String typeCode;
    String sampleWidth;
    const String typeCodeAndWidth =
        _fileFormatToTypeCodeAndWidthMap.at(audioFileFormat);
    const Boolean isSplit = StringUtil::splitAt(typeCodeAndWidth, "/",
                                                typeCode, sampleWidth);
    Assertion_check(isSplit,
                    STR::expand("bad type code for '%1' - '%2'",
                                audioFileFormat, typeCodeAndWidth));
    const Natural sampleWidthInBytes = StringUtil::toNatural(sampleWidth);
    destinationFile.write((Natural) Real::round(sampleRate), _channelCount, 
                          audioFrameCount, typeCode, sampleWidthInBytes,
                          buffer);

    Logging_trace("<<");
}

/*--------------------*/

/**
 * Render events from <C>midiEventList</C> with
 * <C>midiTicksPerQuarterNote</C> time resolution via fluidsynth
 * to <C>sampleBuffer</C> using render settings defined in
 * <C>settings</C>.
 *
 * @param[in]  settings                 dictionary of render
 *                                      settings (including e.g.
 *                                      the sound font name)
 * @param[in]  sampleRate               the sample rate to be used
 *                                      (in Hz)
 * @param[in]  midiTicksPerQuarterNote  midi ticks per quarter note
 *                                      as time resolution
 * @param[in]  midiEventList            list of midi events to be
 *                                      played
 * @param[out] sampleBuffer             stereo sample array
 */
static void _renderEvents (IN Dictionary& settings,
                           IN Real sampleRate,
                           IN Natural midiTicksPerQuarterNote,
                           IN MidiEventList& midiEventList,
                           OUT AudioSampleListVector& sampleBuffer)
{
    const Natural midiEventCount = midiEventList.length();
    Logging_trace4(">>: sampleRate = %1, midiTicksPerQuarterNote = %2,"
                   " eventCount = %3, settings = %4",
                   TOSTRING(sampleRate),
                   TOSTRING(midiTicksPerQuarterNote),
                   TOSTRING(midiEventCount),
                   settings.toString());

    MidiEventConverter* midiEventConverter =
        _makeMidiEventConverter(settings);

    if (midiEventConverter != NULL) {
        const Natural tempBufferSize = 2048;
        const Real defaultBpmRate = 120.0;

        AudioSampleListVector tempBuffer{_channelCount};
        tempBuffer.setFrameCount(tempBufferSize);
        midiEventConverter->prepareToPlay(sampleRate, tempBufferSize);
        Real midiTimeToSecondsFactor =
            Real{60.0} / (defaultBpmRate * midiTicksPerQuarterNote);
        Logging_trace1("--: midiTimeToSecondsFactor = %1",
                       TOSTRING(midiTimeToSecondsFactor));
        MidiEventList synchronousEventList;
        _EventTime previousEventTime;

        for (Natural i = 0;  i < midiEventCount;  i++) {
            const MidiEvent& event = midiEventList.at(i);
            Logging_trace1("--: event = %1", event.toString());
            const Integer midiEventTime = event.time();

            _EventTime currentEventTime =
                _EventTime::makeRelative(midiEventTime,
                                         previousEventTime,
                                         midiTimeToSecondsFactor,
                                         sampleRate);

            const Integer currentTimeInSamples =
                currentEventTime.timeInSamples;
            const Natural offsetTimeInSamples
                {currentTimeInSamples - previousEventTime.timeInSamples};
            const Boolean isTempoChange =
                (event.kind() == MidiEventKind::meta
                 && event.metaKind() == MidiMetaEventKind::tempo);

            /* modify event to one with time offset 0, since it will
             * be rendered at the beginning of some audio
             * processing */
            const MidiEvent effectiveEvent{0, event.rawData()};

            /* if offset time is 0, append event to current list of
             * events, otherwise put out synchronous events and
             * generate following samples accordingly; if a tempo
             * change happens, pending events have to be flushed,
             * because the time base is changed */
            if (offsetTimeInSamples == 0 && !isTempoChange) {
                synchronousEventList.append(effectiveEvent);
            } else {
                Logging_trace1("currentTime = %1",
                               currentEventTime.toString());
                _renderSynchronousEvents(*midiEventConverter,
                                         synchronousEventList,
                                         offsetTimeInSamples,
                                         tempBuffer, sampleBuffer);

                synchronousEventList.clear();
                synchronousEventList.append(effectiveEvent);

                if (isTempoChange) {
                    /* adapt the time conversion factor because the
                     * tempo has changed */
                    midiTimeToSecondsFactor =
                        _updatedTimeFactor(event,
                                           midiTicksPerQuarterNote);
                }
            }

            previousEventTime = currentEventTime;
        }

        if (!synchronousEventList.isEmpty()) {
            _renderSynchronousEvents(*midiEventConverter,
                                     synchronousEventList, 0,
                                     tempBuffer, sampleBuffer);
        }

        delete midiEventConverter;
    }

    Logging_trace("<<");
}

/*--------------------*/

/**
 * Render synchronous events from <C>midiEventList</C> trailed by
 * <C>trailingSampleCount</C> samples via fluidsynth synthesizer
 * from <C>midiEventConverter</C> using <C>temporaryBuffer</C> and
 * appending result to <C>destinationBuffer</C>.
 *
 * @param[in]  midiEventConverter    FluidSynth processor for events
 * @param[in]  synchronousEventList  list of MIDI events happening
 *                                   simultaneously and now
 * @param[in]  trailingSampleCount   samples to be rendered after
 *                                   MIDI events
 * @param[in]  temporaryBuffer       auxiliary buffer for incremental
 *                                   rendering
 * @param[out] destinationBuffer     destination stereo sample array
 */
static void
_renderSynchronousEvents (INOUT MidiEventConverter& midiEventConverter,
                          IN MidiEventList& synchronousEventList,
                          IN Natural trailingSampleCount,
                          INOUT AudioSampleListVector& temporaryBuffer,
                          INOUT AudioSampleListVector& destinationBuffer)
{
    Logging_trace2(">>: eventListCount = %1,"
                   " sampleCount = %2",
                   TOSTRING(synchronousEventList.length()),
                   TOSTRING(trailingSampleCount));

    Natural remainingCount = trailingSampleCount;
    Natural bufferLength = temporaryBuffer.frameCount();
    MidiEventList eventList = synchronousEventList;

    do {
        Natural renderCount = (remainingCount >= bufferLength
                               ? bufferLength : remainingCount);
        Logging_trace2("--: destinationPosition = %1, count = %2",
                       TOSTRING(destinationBuffer.frameCount()),
                       TOSTRING(renderCount));
        midiEventConverter.processBlock(eventList, temporaryBuffer,
                                        renderCount);
        Logging_trace1("--: currentSamples = %1",
                       temporaryBuffer.toString(renderCount, true));
        destinationBuffer.extend(temporaryBuffer, renderCount);
        remainingCount -= renderCount;
        eventList.clear();
    } while (remainingCount > 0);

    Logging_trace1("<<: sampleBufferCount = %1",
                   TOSTRING(destinationBuffer.frameCount()));
}

/*--------------------*/

/**
 * Returns MIDI ticks to seconds factor for given tempo event
 * <C>event</C> based on <C>midiTicksPerQuarterNote</C>.
 *
 * @param[in] event                    tempo event changing factor
 * @param[in] midiTicksPerQuarterNote  midi ticks per quarter note
 *                                     as time resolution
 * @return  factor for conversion from MIDI ticks to seconds
 */
static
Real _updatedTimeFactor (IN MidiEvent& event,
                         IN Natural midiTicksPerQuarterNote)
{
    Logging_trace2(">>: event = %1, midiTicksPerQuarterNote = %2",
                   event.toString(), TOSTRING(midiTicksPerQuarterNote));

    Assertion_check(event.kind() == MidiEventKind::meta
                    && event.metaKind() == MidiMetaEventKind::tempo,
                    "event must be tempo event");
    ByteList eventData = event.rawData();
    Assertion_check(eventData.length() == 5,
                    "tempo event must have length 5");
    Natural microsecondsPerQuarterNote =
        (((Natural) eventData[2] * 256 + (Natural) eventData[3]) * 256
         + (Natural) eventData[4]);
    Real result = (Real{microsecondsPerQuarterNote}
                         / (1.0E6 * (size_t) midiTicksPerQuarterNote));

    Logging_trace1("<<: %1", TOSTRING(result));
    return result;
}

/*--------------------*/

/**
 * Writes <C>message</C> to console terminated by a newline and
 * also to log file; if <C>isError</C> is set, message is
 * qualified as an error.
 *
 * @param[in] message  message to be written as error to console
 *                     and log
 * @param[in] isError  information whether message is considered
 *                     an error message
 */
static void _writeMessage (IN String& message,
                           IN Boolean isError)
{
    String st =
        STR::expand("%1:%2%3%4",
                    _programName, (message == "" ? "" : " "),
                    (isError ? "error - " : ""), message);

    if (isError) {
        Logging_traceError(message);
    } else {
        Logging_trace(message);
    }

    OperatingSystem::writeMessageToConsole(st);
}

/*====================*/
/* PUBLIC FEATURES    */
/*====================*/

int ConverterProgram::main (int argc, char* argv[])
{
    String audioFileFormat;
    Boolean isOkay;
    String midiFileName;
    Dictionary renderSettings;
    Real sampleRate;
    String waveFileName;

    _initialize();
    Logging_trace(">>");

    /* add executable directory path to library path */
    const String executableDirectoryPath =
        OperatingSystem::executableDirectoryPath(true);
    DynamicLibrary::addToSearchPath(executableDirectoryPath);
    
    /* no need to use a mutex for fluidsynth synthesizer, because it
     * is a single thread (hopefully) */
    renderSettings.set("synth.threadsafe-api", "0");

    /* read parameters from command line */
    isOkay = _handleCommandLineArguments(argc, argv,
                                         midiFileName, waveFileName,
                                         sampleRate, audioFileFormat,
                                         renderSettings);

    if (midiFileName == "") {
        _writeMessage("midi file name not specified", true);
        isOkay = false;
    }

    if (waveFileName == "") {
        _writeMessage("wave file name not specified", true);
        isOkay = false;
    }

    if (!isOkay) {
        _writeMessage(STR::expand(_usageText, _programName), false);
    } else {
        _process(renderSettings, midiFileName, sampleRate,
                 audioFileFormat, waveFileName);
    }

    Logging_trace("<<");
    _finalize();
    return (isOkay ? 0 : 1);
}
