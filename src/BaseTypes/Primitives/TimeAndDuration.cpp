/**
 * @file
 * The <C>TimeAndDuration</C> body implements simple functions for the
 * types for time and duration.
 *
 * @author Dr. Thomas Tensi
 * @date   2025-06
 */

/*====================*/

/*=========*/
/* IMPORTS */
/*=========*/

#include <MySystemTime.h>

#include "Logging.h"
#include "RealList.h"
#include "TimeAndDuration.h"

/*--------------------*/

using BaseTypes::Containers::RealList;
using BaseTypes::Primitives::Duration;
using BaseTypes::Primitives::Time;

/** abbreviation for StringUtil */
using STR = BaseModules::StringUtil;

/*====================*/
/* PRIVATE FUNCTIONS  */
/*====================*/

/**
* Converts string <C>hours</C>, <C>minutes</C>, <C>seconds</C> and
* <C>milliseconds</C> to real.
*
* @param[in] hours         hours value
* @param[in] minutes       minutes value
* @param[in] seconds       seconds value
* @param[in] milliseconds  milliseconds value
* @return  real value
*/
static Real _fromParts (IN Real hours,
                        IN Real minutes,
                        IN Real seconds,
                        IN Real milliseconds)
{
    Logging_trace4(">>: hours = %1, minutes = %2,"
                   " seconds = %3, milliseconds = %4",
                   TOSTRING(hours), TOSTRING(minutes),
                   TOSTRING(seconds), TOSTRING(milliseconds));

    Real result = (hours * 3600.0
                   + minutes * 60.0
                   + seconds
                   + milliseconds / 1000.0);
    
    Logging_trace1("<<: %1", TOSTRING(result));
    return result;
}

/*--------------------*/

using _ParseState = Character;
static const Character _ParseState_inLimbo          = 'L';
static const Character _ParseState_inPart           = 'P';
static const Character _ParseState_inFractionalPart = 'F';
static const String _digitCharacters = "0123456789";

/*--------------------*/

/**
 * Supports <C>_fromString</C> function by handling the <C>inLimbo</C>
 * state.
 *
 * @param[in]    ch          character to be processed
 * @param[inout] isNegative  flag to tell whether result is negative
 * @param[inout] partValue   partial value collected
 * @param[inout] parseState  successor parse state
 * @param[inout] isOkay      flag whether parsing was successful
 */
static Boolean _fromString_process_inLimbo (IN Character ch,
                                            INOUT Boolean isNegative,
                                            INOUT Real partValue,
                                            INOUT _ParseState parseState)
{
    Boolean isOkay = true;

    if (ch == ' ') {
        /* do nothing */
    } else if (ch == '+' || ch == '-') {
        isNegative = (ch == '-');
        partValue = 0.0;
        parseState = _ParseState_inPart;
    } else if (STR::contains(_digitCharacters, ch)) {
        Real value{STR::find(_digitCharacters, ch)};
        partValue = value;
        parseState = _ParseState_inPart;
    } else {
        isOkay = false;
    }

    return isOkay;
}

/*--------------------*/

/**
 * Supports <C>_fromString</C> function by handling the <C>inPart</C>
 * state.
 *
 * @param[in]    ch          character to be processed
 * @param[inout] partList    list of partial values
 * @param[inout] partValue   partial value collected
 * @param[inout] parseState  successor parse state
 * @param[inout] isOkay      flag whether parsing was successful
 */
static Boolean _fromString_process_inPart (IN Character ch,
                                           INOUT RealList partList,
                                           INOUT Real partValue,
                                           INOUT String fractionalPart,
                                           INOUT _ParseState parseState)
{
    Boolean isOkay = true;

    if (STR::contains(_digitCharacters, ch)) {
        Real value{STR::find(_digitCharacters, ch)};
        partValue = partValue * 10.0 + value;
    } else if (ch == '.' || ch == ',') {
        partList.append(partValue);
        parseState = _ParseState_inFractionalPart;
        fractionalPart = "0.";
    } else if (ch == ':') {
        partList.append(partValue);
        partValue = 0.0;
        isOkay = (partList.length() < 3);
    } else {
        isOkay = false;
    }

    return isOkay;
 }

/*--------------------*/

/**
* Converts string <C>st</C> in canonical time format to real.
*
* @param[in] st  string containing time or duration information
* @return  real value
*/
static Real _fromString (IN String& st)
{
    Logging_trace1(">>: %1", st);

    const String adaptedSt = STR::strip(st);
    RealList partList;
    Real partValue;
    Boolean isNegative = false;
    Boolean isOkay = true;
    String fractionalPart;

    _ParseState parseState = _ParseState_inLimbo;
    String fsaTrace;

    for (Character ch : adaptedSt) {
        if (Logging_isActive) {
            fsaTrace +=
                STR::expand("[%1]%2", TOSTRING(parseState), TOSTRING(ch));
        }
        
        if (parseState == _ParseState_inLimbo) {
            isOkay = _fromString_process_inLimbo(ch, isNegative,
                                                 partValue, parseState);

        } else if (parseState == _ParseState_inPart) {
            isOkay = _fromString_process_inPart(ch, partList,
                                                partValue, fractionalPart,
                                                parseState);
        } else if (parseState == _ParseState_inFractionalPart) {
            if (STR::contains(_digitCharacters, ch)) {
                STR::append(fractionalPart, ch);
            } else {
                isOkay = false;
            }
        }

        if (!isOkay) {
            break;
        }
    }

    Logging_trace2("--: isOkay = %1, trace = %2",
                   TOSTRING(isOkay), fsaTrace);
    Real result = Real::infinity;

    if (isOkay) {
        while (partList.length() < 3) {
            partList.prepend(0.0);
        }

        Real milliseconds = STR::toReal(fractionalPart) * 1000.0;
        result = _fromParts(partList[0], partList[1],
                            partList[2], milliseconds);
        result = (isNegative ? -result : result);
    }
    
    Logging_trace1("<<: %1", TOSTRING(result));
    return result;
}

/*--------------------*/

/**
 * Returns the clock time of the system in seconds as a real number.
 *
 * @return  system time in seconds
 */
static Real _systemTime ()
{
    Logging_trace(">>");

    C_SystemTime::TimeSpec timeSpec;
    C_SystemTime::currentClockTime(timeSpec);
    Real result{double(timeSpec.tv_sec)
                + double(timeSpec.tv_nsec) * 1.0E-9};

    Logging_trace1("<<: %1", TOSTRING(result));
    return result;
}

/*--------------------*/

/** the start time of process as an offset duration */
static const Duration _processStartOffset = -_systemTime();

/*====================*/
/* Duration           */
/*====================*/

Duration BaseTypes::Primitives::Duration_fromParts (IN Real hours,
                                                    IN Real minutes,
                                                    IN Real seconds,
                                                    IN Real milliseconds)
{
    Logging_trace4(">>: hours = %1, minutes = %2,"
                   " seconds = %3, milliseconds = %4",
                   TOSTRING(hours), TOSTRING(minutes),
                   TOSTRING(seconds), TOSTRING(milliseconds));
    Duration result = _fromParts(hours, minutes, seconds, milliseconds);
    Logging_trace1("<<: %1", TOSTRING(result));
    return result;
}

/*--------------------*/

Duration BaseTypes::Primitives::Duration_fromString (IN String& st)
{
    Logging_trace1(">>: %1", st);
    Duration result = _fromString(st);
    Logging_trace1("<<: %1", TOSTRING(result));
    return result;
}

/*====================*/
/* Time               */
/*====================*/

Time BaseTypes::Primitives::Time_withinProcess ()
{
    Logging_trace(">>");

    Real sysTime = _systemTime();
    Time result = sysTime + _processStartOffset;
    Logging_trace1("<<: %1", TOSTRING(result));
    return result;
}

/*--------------------*/

Time BaseTypes::Primitives::Time_fromParts (IN Real hours,
                                            IN Real minutes,
                                            IN Real seconds,
                                            IN Real milliseconds)
{
    Logging_trace4(">>: hours = %1, minutes = %2,"
                   " seconds = %3, milliseconds = %4",
                   TOSTRING(hours), TOSTRING(minutes),
                   TOSTRING(seconds), TOSTRING(milliseconds));
    Time result = _fromParts(hours, minutes, seconds, milliseconds);
    Logging_trace1("<<: %1", TOSTRING(result));
    return result;
}

/*--------------------*/

Time BaseTypes::Primitives::Time_fromString (IN String& st)
{
    Logging_trace1(">>: %1", st);
    Time result = _fromString(st);
    Logging_trace1("<<: %1", TOSTRING(result));
    return result;
}

/*====================*/
/* MIXED FUNCTIONS    */
/*====================*/

Time BaseTypes::Primitives::Time_addDuration (IN Time time,
                                              IN Duration duration)
{
    Logging_trace2(">>: time = %1, duration = %2",
                   TOSTRING(time), TOSTRING(duration));
    Time result = time + duration;
    Logging_trace1("<<: %1", TOSTRING(result));
    return result;
}

/*--------------------*/

Duration BaseTypes::Primitives::Time_difference (IN Time endTime,
                                                 IN Time startTime)
{
    Logging_trace2(">>: endTime = %1, startTime = %2",
                   TOSTRING(endTime), TOSTRING(startTime));
    Duration result = endTime - startTime;
    Logging_trace1("<<: %1", TOSTRING(result));
    return result;
}
