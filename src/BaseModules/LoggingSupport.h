/**
 * @file
 * The <C>LoggingSupport</C> specification defines a class for simple
 * entry/exit logging to a file or via an arbitrary callback function;
 * this logging relies on trace calls at the beginning or end of a
 * function with prefices ">>" and "<<" as well as intermediate log
 * lines with prefix "--"; the name of the function is also logged to
 * give a fully bracketed log.
 *
 * @author Dr. Thomas Tensi
 * @date   2021-02
 */

/*====================*/

#pragma once

/*=========*/
/* IMPORTS */
/*=========*/

#include "Boolean.h"
#include "Natural.h"

/*--------------------*/

using BaseTypes::Primitives::Boolean;
using BaseTypes::Primitives::Natural;
using BaseTypes::Primitives::String;

/*====================*/

namespace BaseModules {

    /** a callback function for logging (does direct calls for each
     * logging entry and excludes file output) */
    typedef void (*LoggingCallbackFunction)(IN String& st);

    /*====================*/
    
    /**
     * A <C>Logging</C> object defines methods for entry/exit logging
     * to a file or via a callback function; this logging relies on
     * trace calls at the beginning or end of a function with prefices
     * ">>" and "<<" as well as intermediate log lines with prefix
     * "--"; the name of the function is also logged to give a fully
     * bracketed log.
     */
    struct Logging {

        /**
         * Sets up logging configuration and starts logging.
         */
        static void initialize ();

        /*--------------------*/

        /**
         * Sets up logging configuration and starts logging with
         * hundredth seconds time collection onto a logging file
         * in the temp directory with <C>fileNameStem</C> as part
         * of the log file name; <C>ignoredFunctionNamePrefix</C>
         * tells what prefix shall be skipped from the full
         * function names; this is a convenience function
         * combining several elementary functions
         *
         * @param[in] fileNameStem               identifying part of
         *                                       logging file name
         * @param[in] ignoredFunctionNamePrefix  string to be stripped
         *                                       from function names
         */
        static void initializeWithDefaults
                        (IN String& fileNameStem,
                         IN String& ignoredFunctionNamePrefix);

        /*--------------------*/

        /**
         * Cleans up logging configuration and ends logging.
         */
        static void finalize ();

        /*--------------------*/

        /**
         * Tells whether logging is currently activated.
         *
         * @return information whether logging is active or suppressed
         */
        static Boolean isActive ();

        /*--------------------*/

        /**
         * Sets logging to active or inactive due to <C>isActive</C>.
         *
         * @param[in] isActive  tells whether logging shall be
         *                      suppressed
         */
        static void setActive (IN Boolean isActive);

        /*--------------------*/

        /**
         * Sets callback function for logging to <C>callback
         * function</C>. Using NULL resets the callback mechanism.
         *
         * @param[in] callbackFunction  function to be called for
         *                              logging
         */
        static void
        setCallbackFunction (IN LoggingCallbackFunction callbackFunction);

        /*--------------------*/

        /**
         * Sets file name for logging to <C>fileName</C>; if
         * <C>writeThroughIsActive</C> is set, the log data goes
         * directly to the logging file (at the expense of
         * performance).
         *
         * @param[in] fileName              name of logging file from
         *                                  now on
         * @param[in] writeThroughIsActive  flag to tell that logging
         *                                  is done directly to file
         */
        static void setFileName (IN String& fileName,
                                 IN Boolean writeThroughIsActive=false);

        /*--------------------*/

        /**
         * Sets function name prefix to be ignored to {@code namePrefix}.
         *
         * @param[in] namePrefix  string to be stripped from function
         *                        names
         */
        static void setIgnoredFunctionNamePrefix (IN String& namePrefix);

        /*--------------------*/

        /**
         * Sets logging of thread information in logs to active or
         * inactive.
         *
         * @param[in] threadIDIsLogged  sets or disables thread ID output
         *                              in log
         */
        static void setTracingOfThreadID (IN Boolean threadIDIsLogged);

        /*--------------------*/

        /**
         * Sets logging of time when tracing to active or inactive.
         *
         * @param[in] timeIsLogged          sets or disables time output
         *                                  in log
         * @param[in] fractionalDigitCount  number of digits after decimal
         *                                  point in timing data
         */
        static void setTracingWithTime (IN Boolean timeIsLogged,
                                        IN Natural fractionalDigitCount=0);

        /*--------------------*/

        /**
         * Writes <C>message</C> together with function name
         * derived from <C>functionSignature</C> to log file
         *
         * @param[in] functionSignature  signature of enclosing
         *                               function
         * @param[in] message            string to be logged
         */
        static void trace (IN String& functionSignature,
                           IN String& message);

        /*--------------------*/

        /**
         * Writes <C>message</C> together with function name
         * derived from <C>functionSignature</C> to log file as an
         * error entry.
         *
         * @param[in] functionSignature  signature of enclosing
         *                               function
         * @param[in] message            string to be logged
         */
        static void traceError (IN String& functionSignature,
                                IN String& message);

    };

}
