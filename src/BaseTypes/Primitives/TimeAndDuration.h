/**
 * @file
 * The <C>TimeAndDuration</C> specification and body provides types for
 * time and duration.
 *
 * @author Dr. Thomas Tensi
 * @date   2025-06
 */

/*====================*/

#pragma once

/*=========*/
/* IMPORTS */
/*=========*/

#include "Real.h"

/*====================*/

namespace BaseTypes::Primitives {

    /**
     * A <C>Time</C> object describes a point in time as a seconds
     * value.
     */
    typedef Real Time;

    /*--------------------*/

    /**
    * Converts string <C>hours</C>, <C>minutes</C>, <C>seconds</C> and
    * <C>milliseconds</C> to time.
    *
    * @param[in] hours         hours value
    * @param[in] minutes       minutes value
    * @param[in] seconds       seconds value
    * @param[in] milliseconds  milliseconds value
    * @return  timevalue
    */
    Time Time_fromParts (IN Real hours,
                         IN Real minutes,
                         IN Real seconds,
                         IN Real milliseconds);

    /*--------------------*/

    /**
     * Converts string <C>st</C> in canonical time format to time.
     *
     * @param[in] st  string containing time information
     * @return  time value
     */
    Time Time_fromString (IN String& st);
    
    /*--------------------*/

    /**
     * Returns the current time within the process
     *
     * @return current process time in seconds
     */
    Time Time_withinProcess ();

    /*====================*/

    /**
     * A <C>Duration</C> object describes a time difference as a
     * seconds value.
     */
    typedef Real Duration;

    /*--------------------*/

    /**
    * Converts string <C>hours</C>, <C>minutes</C>, <C>seconds</C> and
    * <C>milliseconds</C> to duration.
    *
    * @param[in] hours         hours value
    * @param[in] minutes       minutes value
    * @param[in] seconds       seconds value
    * @param[in] milliseconds  milliseconds value
    * @return  duration value
    */
    Duration Duration_fromParts (IN Real hours,
                                 IN Real minutes,
                                 IN Real seconds,
                                 IN Real milliseconds);

    /*--------------------*/

    /**
     * Converts string <C>st</C> in canonical time format to duration.
     *
     * @param[in] st  string containing duration information
     * @return  duration value
     */
    Duration Duration_fromString (IN String& st);
    
    /*====================*/

    /**
     * Returns the time that is <C>duration</C> after <C>time</C>.
     *
     * @param[in] time      start time in seconds
     * @param[in] duration  duration between start and end time
     * @return end time in seconds
     */
    Time Time_addDuration (IN Time time,
                           IN Duration duration);

    /*--------------------*/

    /**
     * Returns the time that is <C>duration</C> after <C>time</C>.
     *
     * @param[in] time      start time in seconds
     * @param[in] duration  duration between start and end time
     * @return end time in seconds
     */
    Duration Time_difference (IN Time endTime,
                              IN Time startTime);

}
