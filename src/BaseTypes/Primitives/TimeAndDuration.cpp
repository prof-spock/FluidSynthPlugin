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

#include <time.h>
    #define SystemTime_ClockType clock_t
    #define SystemTime_clockTicksPerSecond CLOCKS_PER_SEC
    #define SystemTime_currentClockTime clock

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
    const String digitCharacters = "0123456789";
    RealList partList;
    Real partValue;
    Boolean isNegative = false;
    Boolean isOkay = true;
    String fractionalPart;

    const Character ParseState_inLimbo          = 'L';
    const Character ParseState_inPart           = 'P';
    const Character ParseState_inFractionalPart = 'F';
    Character parseState = ParseState_inLimbo;
    String fsaTrace;

    for (Character ch : adaptedSt) {
        if (Logging_isActive) {
            fsaTrace +=
                STR::expand("[%1]%2", TOSTRING(parseState), TOSTRING(ch));
        }
        
        if (parseState == ParseState_inLimbo) {
            if (ch == ' ') {
                /* do nothing */
            } else if (ch == '+' || ch == '-') {
                isNegative = (ch == '-');
                partValue = 0.0;
                parseState = ParseState_inPart;
            } else if (STR::contains(digitCharacters, ch)) {
                Real value{STR::find(digitCharacters, ch)};
                partValue = value;
                parseState = ParseState_inPart;
            } else {
                isOkay = false;
                break;
            }
        } else if (parseState == ParseState_inPart) {
            if (STR::contains(digitCharacters, ch)) {
                Real value{STR::find(digitCharacters, ch)};
                partValue = partValue * 10.0 + value;
            } else if (ch == '.' || ch == ',') {
                partList.append(partValue);
                parseState = ParseState_inFractionalPart;
                fractionalPart = "0.";
            } else if (ch == ':') {
                partList.append(partValue);
                partValue = 0.0;

                if (partList.length() == 3) {
                    isOkay = false;
                    break;
                }
            } else {
                isOkay = false;
                break;
            }
        } else if (parseState == ParseState_inFractionalPart) {
            if (STR::contains(digitCharacters, ch)) {
                STR::append(fractionalPart, ch);
            } else {
                isOkay = false;
                break;
            }
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
    SystemTime_ClockType clockTicks = SystemTime_currentClockTime();
    Time result{(float) clockTicks / SystemTime_clockTicksPerSecond};
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
