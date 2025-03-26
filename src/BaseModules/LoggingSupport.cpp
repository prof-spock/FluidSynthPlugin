/**
 * @file
 * The <C>LoggingSupport</C> body implements a class for simple
 * entry/exit logging to a file or via a callback function; this
 * logging relies on trace calls at the beginning or end of a function
 * with prefices ">>" and "<<" as well as intermediate log lines with
 * prefix "--"; the name of the function is also logged to give a
 * fully bracketed log.
 *
 * @author Dr. Thomas Tensi
 * @date   2021-01
 */

/*=========*/
/* IMPORTS */
/*=========*/

#include <chrono>
#include <cstdint>
#include <stdlib.h>
    /** qualified version of atexit from stdlib */
    #define StdLib_atexit atexit

#include "Assertion.h"
#include "File.h"
#include "LoggingSupport.h"
#include "OperatingSystem.h"

/*--------------------*/

using BaseModules::LoggingCallbackFunction;
using namespace std::chrono;

using BaseModules::File;
using BaseModules::Logging;
using BaseModules::OperatingSystem;
using BaseTypes::GenericTypes::GenericList;

/** abbreviation for StringUtil */
using STR = BaseModules::StringUtil;

/** the timestamp type as provided by the system call */
using Timestamp = Natural;

/*====================*/

/** number of milliseconds per day */
static const Natural _millisecondsPerDay = 86400000;

/*--------------------*/

/** time zone offset in milliseconds */
static const Natural _timeOffsetInMilliseconds{3600000};

/*--------------------*/

/** offset for DST in milliseconds */
static const Natural _dstOffsetInMilliseconds{3600000};

/*--------------------*/

/** dictionary from already known signatures */
static Dictionary _signatureToFunctionNameMap;

/*====================*/
/* PROTOTYPES         */
/*====================*/

static void _writeBufferToFile ();


/*====================*/

namespace BaseModules {

    /**
     * A <C>_LoggingBufferEntry</C> object is a list of entries
     * consisting of function signature, system time and a logging
     * message.
     */
    struct _LoggingBufferEntry {

        /** the function signature for this logging entry */
        String functionSignature;

        /** the timestamp for this logging entry */
        Timestamp systemTime;

        /** the associated message this logging entry */
        String message;

    };

    /*====================*/

    /**
     * The <C>_LoggingState</C> gives the state of the logger
     */
    enum _LoggingState {

        /** state where logging is not active or finalized */
        inLimbo,

        /** state where output is buffered and postponed to
         * finalization */
        inBufferingMode,

        /** state where output is directly written to file */
        inWriteThroughMode,

        /** state where logging is finalized */
        isDone
    };

    /*====================*/

    /**
     * A <C>_LoggingTime</C> object provides simple time related
     * services like caching the current time string to speed up
     * tracing output.
     */
    class _LoggingTime {

        private:

            /** the number of digits after decimal point for time
             * indications */
            Natural _timeFractionalDigitCount;

            /*--------------------*/

            /** factor 10^<C>timeFractionalDigitCount</C> */
            Natural _timeFactor;

            /*--------------------*/

            /** the effective offset in milliseconds for DST (either 0 or
             * 1h) */
            Natural _effectiveDSTOffsetInMilliseconds;

            /*--------------------*/

            /** the previous time stamp returned by the system */
            Timestamp _previousSystemTime;

            /*--------------------*/

            /** the time stamp of the previous log line as time of day. */
            Timestamp _previousTimeOfDay;

            /*--------------------*/

            /** the previous time of day formatted as a string. */
            String _previousTimeOfDayString;

            /*--------------------*/


            /**
             * Tells whether DST is applicable (very imprecisely)
             *
             * @return information about being in DST
             */
            Boolean _isDST () {
                Natural secondsSinceEpoch = systemTime() / 1000;
                Natural secondsPerYear    = 86400 * 1461 / 4;
                Natural secondsPerQuarter = secondsPerYear / 4;
                Natural secondsInCurrentYear =
                    secondsSinceEpoch % secondsPerYear;
                Boolean result =
                    (secondsInCurrentYear >= secondsPerQuarter
                     && secondsInCurrentYear <= secondsPerQuarter *3);
                return result;
            }

        /*--------------------*/
        /*--------------------*/

        public:

            /**
             * Initializes logging time object.
             */
            _LoggingTime ()
                : _timeFractionalDigitCount{0},
                  _timeFactor{1},
                  _effectiveDSTOffsetInMilliseconds{
                      (_isDST() ? _dstOffsetInMilliseconds : Natural{0})},
                  _previousSystemTime{0},
                  _previousTimeOfDay{0},
                  _previousTimeOfDayString{""}
            {
            }

            /*--------------------*/

            /**
             * Converts system time <C>systemTime</C> to time within a
             * day.
             *
             * @param[in] systemTime  time in milliseconds since epoch
             * @return  adapted time for a single day scaled by
             *          _timeFactor
             */
            Timestamp adaptToTimeOfDay (IN Timestamp systemTime)
            {
                Timestamp result;

                if (systemTime == _previousSystemTime) {
                    result = _previousTimeOfDay;
                } else {
                    _previousSystemTime = systemTime;
                    result = systemTime;
                    result %= _millisecondsPerDay;
                    result += _timeOffsetInMilliseconds;
                    result += _effectiveDSTOffsetInMilliseconds;
                    result /= (Natural{1000} / _timeFactor);
                    _previousTimeOfDay = result;
                    _previousTimeOfDayString = "";
                }

                return result;
            }

            /*--------------------*/

            /**
             * Returns <C>timeOfDay</C> as string with millisecond
             * resolution (using caching).
             *
             * @param[in] timeOfDay  time within day in milliseconds
             *                       resolution
             * @return time of day as string
             */
            String asDayString (IN Timestamp timeOfDay)
            {
                String result;

                if (timeOfDay == _previousTimeOfDay
                    && _previousTimeOfDayString > "") {
                    result =_previousTimeOfDayString;
                } else {
                    _previousTimeOfDay = timeOfDay;
                    Timestamp time = timeOfDay;
                    Natural fractionalPart = time % _timeFactor;
                    time /= _timeFactor;
                    const Natural seconds = time % 60;
                    const Natural minutes = (time / 60) % 60;
                    const Natural hours   = (time / 3600) % 24;
                    result = (TOSTRING(hours, 2)
                              + TOSTRING(minutes, 2)
                              + TOSTRING(seconds, 2));

                    if (_timeFractionalDigitCount > 0) {
                        result +=
                            ("." +
                             TOSTRING(fractionalPart,
                                      _timeFractionalDigitCount));
                    }

                    _previousTimeOfDayString = result;
                }

                return result;
            }

            /*--------------------*/

            /**
             * Returns current system time with millisecond resolution
             *
             * @return  current system time in milliseconds
             */
            Timestamp systemTime ()
            {
                const system_clock::duration d =
                    system_clock::now().time_since_epoch();
                Timestamp result =
                    (size_t) duration_cast<milliseconds>(d).count();
                return result;
            }
               
            /*--------------------*/

            /**
             * Sets the number of digits after decimal point for
             * time indications to <C>fractionalDigitCount</C>.
             *
             * @param[in] fractionalDigitCount number of digits
             *                                 after decimal point
             *                                 in time indications
             */
            void setFractionalDigitCount (IN Natural fractionalDigitCount)
            {
                _timeFractionalDigitCount =
                    Natural::minimum(3,
                                     Natural::maximum(0,
                                                      fractionalDigitCount));
                _timeFactor = Natural::power(10, _timeFractionalDigitCount);
            }

    };

}

/*====================*/

using BaseModules::_LoggingBufferEntry;
using BaseModules::_LoggingState;
using BaseModules::_LoggingTime;

/*--------------------*/
/* PRIVATE FEATURES   */
/*--------------------*/

/** the name prefix to be ignored in function names */
static String _ignoredFunctionNamePrefix = "";

/*--------------------*/

/** the function to be directly called when logging (e.g. for embedded
 * systems) */
static LoggingCallbackFunction _callbackFunction = NULL;

/*--------------------*/

/** length of identification prefix for trace messages */
static const Natural _prefixLength = 2;

/*--------------------*/

/** list of allowed two-character prefices in trace template */
static StringList _standardPrefixList =
    StringList::makeBySplit(">>,<<,--", ",");

/*--------------------*/

/** the buffer stores log data either before the log file is opened or
 * when no write-through to target file is done */
static GenericList<_LoggingBufferEntry> _buffer;

/*--------------------*/

/** flag to tell whether time is logged or omitted */
static Boolean _timeIsLogged{false};

/*--------------------*/

/** flag to tell whether logging is active or suppressed */
static Boolean _isActive{true};

/*--------------------*/

/** the file name of the logging file */
static String _fileName = "";

/*--------------------*/

/** the logging target file */
static File _file;

/*--------------------*/

/** the logging state telling whether logging has already a file
 * associated and this file is updated continually or at finalization
 * time */
static _LoggingState _loggingState{_LoggingState::inLimbo};

/*--------------------*/

/** the current logging time handler */
static _LoggingTime _loggingTime = _LoggingTime();

/*--------------------*/
/* Prototypes         */
/*--------------------*/

static String _bufferEntryToString (IN _LoggingBufferEntry& bufferEntry);

static String _functionNameFromSignature (IN String& functionSignature,
                                          IN String& ignoredFunctionNamePrefix);

/*--------------------*/
/*--------------------*/

/**
 * Adds a new entry consisting of <C>functionSignature</C>,
 * <C>time</C> and <C>message</C> to buffer.
 *
 * @param[in] functionSignature  signature of current function
 * @param[in] time               time of entry (0 signifies an entry
 *                               without associated time, e.g. for
 *                               titles)
 * @param[in] message            the logging message
 */
void _appendEntryToBuffer (IN String& functionSignature,
                           IN Timestamp time,
                           IN String& message)
{
    if (_isActive) {
        _LoggingBufferEntry bufferEntry = {functionSignature, time, message};

        if (_callbackFunction != NULL) {
            String st = _bufferEntryToString(bufferEntry);
            _callbackFunction(st);
        } else {
            _buffer.append(bufferEntry);
            
            if (_loggingState == _LoggingState::inWriteThroughMode) {
                _writeBufferToFile();
            }
        }
    }
}

/*--------------------*/

/**
 * Converts <C>bufferEntry</C> to string for logging file or callback
 * function.
 *
 * @param[in] bufferEntry  logging buffer entry to be converted
 * @return  string representation of logging buffer entry
 */
static String _bufferEntryToString (IN _LoggingBufferEntry& bufferEntry)
{
    const String& functionSignature = bufferEntry.functionSignature;
    const String& message           = bufferEntry.message;
    const Timestamp systemTime      = bufferEntry.systemTime;
    String result = message;

    if (systemTime != 0) {
        const String functionName =
            _functionNameFromSignature(functionSignature,
                                       _ignoredFunctionNamePrefix);
        const Natural messageLength{message.size()};

        String timeString;

        if (_timeIsLogged) {
            Timestamp timeOfDay =
                _loggingTime.adaptToTimeOfDay(systemTime);
            timeString = STR::expand(" (%1)",
                                     _loggingTime.asDayString(timeOfDay));
        }

        /* ensure that message starts with a known prefix */
        String messagePrefix =
            STR::prefix(message, _prefixLength);
        if (messageLength < _prefixLength
            || !_standardPrefixList.contains(messagePrefix)) {
            result = ((messageLength > 0 ? "--:" : "--") + message);
        }

        result = (STR::prefix(result, _prefixLength)
                  + functionName
                  + timeString
                  + STR::substring(result, _prefixLength));
    }

    return result;
}

/*--------------------*/

/**
 * Calculates function name from <C>functionSignature</C>.
 *
 * @param[in] functionSignature          signature of function as string
 * @param[in] ignoredFunctionNamePrefix  prefix to be stripped from
 *                                       function name
 * @return  sanitized function name
 */
static
String _functionNameFromSignature (IN String& functionSignature,
                                   IN String& ignoredFunctionNamePrefix)
{
    String functionName;
    
    if (_signatureToFunctionNameMap.contains(functionSignature)) {
        /* great, use already known entry */
        functionName = _signatureToFunctionNameMap.at(functionSignature);
    } else {
        /* calculate name from signature and store it in cache */
        functionName = functionSignature;

        /* sanitize function name */
        Natural position = STR::find(functionName, "(");
        functionName = STR::prefix(functionName, position);
        STR::replace(functionName, "__cdecl", "");
        STR::replace(functionName, "*", "");
        position = STR::findFromEnd(functionName, " ");
        functionName = STR::substring(functionName, position + 1);
        STR::replace(functionName, "::", ".");

        const Boolean hasPrefix =
            (ignoredFunctionNamePrefix > ""
             && STR::startsWith(functionName,
                                       ignoredFunctionNamePrefix));

        if (hasPrefix) {
            const Natural prefixLength = ignoredFunctionNamePrefix.size();
            functionName = STR::substring(functionName,
                                                 prefixLength);
        }

        _signatureToFunctionNameMap.set(functionSignature, functionName);
    }

    return functionName;
}

/*--------------------*/

/**
 * Creates or reopens logging file depending on value of
 * <C>isNew</C>.
 *
 * @param[in] isNew  flag to tell whether file must be created
 */
static void _openOrCreateFile (IN Boolean isNew)
{
    if (_fileName == "" || _fileName == "STDERR") {
        _file.closeConditionally();
    } else {
        String mode = (isNew ? "w" : "a");
        Boolean isOkay = _file.open(_fileName, mode);
        Assertion_check(isOkay,
                        STR::expand("file must be writable: %1",
                                    _fileName));
    }
}

/*--------------------*/

/**
 * Writes all entries in buffer to logging file and clears it.
 */
static void _writeBufferToFile ()
{
    for (const _LoggingBufferEntry& bufferEntry : _buffer) {
        String line = _bufferEntryToString(bufferEntry) + "\n";
        _file.writeString(line);
    }

    _buffer.clear();
}

/*--------------------*/
/* EXPORTED FEATURES  */
/*--------------------*/

void Logging::initialize ()
{
    StdLib_atexit(finalize);
    _buffer.clear();
    _isActive = true;
    _appendEntryToBuffer("", 0, "START LOGGING -*- coding: utf-8 -*-");
}

/*--------------------*/

void
Logging::initializeWithDefaults (IN String& fileNameStem,
                                 IN String& ignoredFunctionNamePrefix)
{
    initialize();
    String fileName = fileNameStem + ".log";
    STR::replace(fileName, " ", "");
    const String loggingFilePath =
        STR::expand("%1/%2",
                    OperatingSystem::temporaryDirectoryPath(),
                    fileName);
    setFileName(loggingFilePath, false);
    setIgnoredFunctionNamePrefix(ignoredFunctionNamePrefix);
    setTracingWithTime(true, 2);
}

/*--------------------*/

void Logging::finalize ()
{
    if (_loggingState != _LoggingState::isDone) {
        _appendEntryToBuffer("", 0, "END LOGGING");

        if (_loggingState == _LoggingState::inBufferingMode) {
            _openOrCreateFile(true);
        }

        _writeBufferToFile();
        _file.closeConditionally();
        _loggingState = _LoggingState::isDone;
    }
}

/*--------------------*/
/*--------------------*/

Boolean Logging::isActive ()
{
    return _isActive;
}

/*--------------------*/

void Logging::setActive (IN Boolean isActive)
{
    _isActive = isActive;
}

/*--------------------*/

void
Logging::setCallbackFunction (IN LoggingCallbackFunction callbackFunction)
{
    _callbackFunction = callbackFunction;
}

/*--------------------*/

void Logging::setFileName (IN String& fileName,
                           IN Boolean writeThroughIsActive)
{
    if (_fileName == fileName) {
        const String message =
            STR::expand("logging file %1 already open => skip", fileName);
        _appendEntryToBuffer("", 0, message);
    } else {
        _loggingState = (writeThroughIsActive
                         ? _LoggingState::inWriteThroughMode
                         : _LoggingState::inBufferingMode);
        Boolean fileIsAvailable = _file.open(fileName, "a");
        
        if (!fileIsAvailable) {
            /* file cannot be used => reset logging state */
            _fileName = "";
            _loggingState = _LoggingState::inLimbo;
        } else {
            _fileName = fileName;

            if (writeThroughIsActive) {
                _file.close();
                _openOrCreateFile(true);
                _writeBufferToFile();
            }
        }
    }
}

/*--------------------*/

void Logging::setIgnoredFunctionNamePrefix (IN String& namePrefix)
{
    _ignoredFunctionNamePrefix = namePrefix;
}

/*--------------------*/

void Logging::setTracingWithTime (IN Boolean timeIsLogged,
                                  IN Natural fractionalDigitCount)
{
    _timeIsLogged = timeIsLogged;
    _loggingTime.setFractionalDigitCount(fractionalDigitCount);
}

/*--------------------*/

void Logging::trace (IN String& functionSignature,
                     IN String& message)
{
    _appendEntryToBuffer(functionSignature,
                         _loggingTime.systemTime(),
                         message);
}

/*--------------------*/

void Logging::traceError (IN String& functionSignature,
                          IN String& message)
{
    trace(functionSignature, "--: ERROR - " + message);
}
