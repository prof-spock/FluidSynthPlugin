/**
 * @file
 * The <C>Logging</C> specification defines macros for entry-, exit-
 * and intermediate logging of functions into a file; this file
 * provides a conditional facade that expands all logging functions to
 * empty for production code and expands to concrete logging
 * implementations for debugging code.
 *
 * @author Dr. Thomas Tensi
 * @date   2021-02
 */

#pragma once

/*=========*/
/* IMPORTS */
/*=========*/

#include <cstdarg>

#include "LoggingSupport.h"
#include "StringUtil.h"

/*--------------------*/

using BaseModules::Logging;
using BaseModules::StringUtil;

/*====================*/

#ifdef __FUNCSIG__
    /** the macro for the signature of a function within
      * the preprocessor */
    #define signatureOfFunction __FUNCSIG__
#else
    /** the macro for the signature of a function within
      * the preprocessor */
    #define signatureOfFunction __PRETTY_FUNCTION__
#endif

/*--------------------*/

#ifdef LOGGING_IS_ACTIVE
    /* for active logging routines are redirected to corresponding
     * routines in logging class */

    /** information whether logging is active */
    #define Logging_isActive Logging::isActive()

    /**
     * Initializes logging
     */
    #define Logging_initialize()  Logging::initialize()

    /**
     * Initializes logging with defaults
     */
    #define Logging_initializeWithDefaults(fileNameStem, \
                                           ignoredFunctionNamePrefix) \
            Logging::initializeWithDefaults(fileNameStem, \
                                            ignoredFunctionNamePrefix)

    /**
     * Finalizes logging
     */
    #define Logging_finalize()    Logging::finalize()

    /** Sets logging to active or inactive */
    #define Logging_setActive(isActive)  Logging::setActive(isActive)

    /**
     * Sets the name of the logging callback function (if applicable)
     */
    #define Logging_setCallbackFunction(callbackFunction)  \
                Logging::setCallbackFunction(callbackFunction)

    /**
     * Sets the name of the log file and information about
     * write-through (empty)
     */
    #define Logging_setFileName(fileName, writeThroughIsActive)  \
                Logging::setFileName(fileName, writeThroughIsActive)

    /**
     * Sets the function name prefix to be removed from the names
     */
    #define Logging_setIgnoredFunctionNamePrefix(prefix) \
                Logging::setIgnoredFunctionNamePrefix(prefix)

    /**
     * Enables or disables time information in logs with some
     * precision beyond seconds
     */
    #define Logging_setTracingWithTime(timeIsLogged, \
                                       fractionalDigitCount) \
                Logging::setTracingWithTime(timeIsLogged, \
                                            fractionalDigitCount)

    /**
     * Writes a message to log file
     */
    #define _Logging_trace(message) \
                Logging::trace(signatureOfFunction, message)

    /**
     * Writes an error message to log file
     */
    #define _Logging_traceError(message) \
                Logging::traceError(signatureOfFunction, message)

#else
    /* for inactive logging routines are simply empty */

    /** boolean information whether logging is active (no!) */
    #define Logging_isActive false

    /**
     * Initializes logging (empty)
     */
    #define Logging_initialize()

    /**
     * Initializes logging with defaults (empty)
     */
    #define Logging_initializeWithDefaults(fileNameStem, \
                                           ignoredFunctionNamePrefix)

    /**
     * Finalizes logging (empty)
     */
    #define Logging_finalize()

    /** Sets logging to active or inactive */
    #define Logging_setActive(isActive) 

    /**
     * Sets the name of the logging callback function (if applicable)
     */
    #define Logging_setCallbackFunction(callbackFunction)

    /**
     * Sets the name of the log file and information about
     * write-through (empty)
     */
    #define Logging_setFileName(fileName, writeThroughIsActive)

    /**
     * Sets the function name prefix to be removed from the names
     * (empty)
     */
    #define Logging_setIgnoredFunctionNamePrefix(prefix)

    /**
     * Enables or disables time information in logs with some
     * precision beyond seconds (empty)
     */
    #define Logging_setTracingWithTime(timeIsLogged, \
                                       fractionalDigitCount)

    /**
     * Writes a message to log file (empty)
     */
    #define _Logging_trace(message)

    /**
     * Writes an error message to log file (empty)
     */
    #define _Logging_traceError(message)

#endif

/*--------------------*/

/** abbreviated name for string expansion routine */
#define _expand  StringUtil::expand

/**
 * Writes trace message with formatting template only
 */
#define Logging_trace(formattingTemplate)   \
            _Logging_trace(formattingTemplate)

/**
 * Writes trace message with formatting template and one parameter
 */
#define Logging_trace1(formattingTemplate, st1) \
             _Logging_trace(_expand(formattingTemplate, \
                                    st1))

/**
 * Writes trace message with formatting template and two parameters
 */
#define Logging_trace2(formattingTemplate, st1, st2) \
            _Logging_trace(_expand(formattingTemplate, \
                                   st1, st2))

/**
 * Writes trace message with formatting template and three parameters
 */
#define Logging_trace3(formattingTemplate, st1, st2, st3) \
            _Logging_trace(_expand(formattingTemplate, \
                                   st1, st2, st3))

/**
 * Writes trace message with formatting template and four parameters
 */
#define Logging_trace4(formattingTemplate, st1, st2, st3, st4)  \
            _Logging_trace(_expand(formattingTemplate, \
                                   st1, st2, st3, st4))

/**
 * Writes trace message with formatting template and five parameters
 */
#define Logging_trace5(formattingTemplate, st1, st2, st3, st4, \
                       st5) \
            _Logging_trace(_expand(formattingTemplate, \
                                   st1, st2, st3, st4, st5))

/**
 * Writes trace message with formatting template and six parameters
 */
#define Logging_trace6(formattingTemplate, st1, st2, st3, st4, \
                       st5, st6) \
            _Logging_trace(_expand(formattingTemplate, \
                                   st1, st2, st3, st4, st5, \
                                   st6))

/**
 * Writes trace message with formatting template and seven parameters
 */
#define Logging_trace7(formattingTemplate, st1, st2, st3, st4, \
                       st5, st6, st7) \
            _Logging_trace(_expand(formattingTemplate, \
                                   st1, st2, st3, st4, st5, \
                                   st6, st7))

/**
 * Writes trace message with formatting template and eight parameters
 */
#define Logging_trace8(formattingTemplate, st1, st2, st3, st4, \
                       st5, st6, st7, st8) \
            _Logging_trace(_expand(formattingTemplate, \
                                   st1, st2, st3, st4, st5, \
                                   st6, st7, st8))

/**
 * Writes trace message with formatting template and nine parameters
 */
#define Logging_trace9(formattingTemplate, st1, st2, st3, st4, \
                       st5, st6, st7, st8, st9) \
            _Logging_trace(_expand(formattingTemplate, \
                                   st1, st2, st3, st4, st5,     \
                                   st6, st7, st8, st9))
/**
 * Writes trace message with formatting template and ten parameters
 */
#define Logging_trace10(formattingTemplate, st1, st2, st3, st4, \
                        st5, st6, st7, st8, st9, st10) \
            _Logging_trace(_expand(formattingTemplate, \
                                   st1, st2, st3, st4, st5,     \
                                   st6, st7, st8, st9, st10))

/**
 * Writes trace error message with formatting template only
 */
#define Logging_traceError(formattingTemplate)   \
            _Logging_traceError(formattingTemplate)

/**
 * Writes trace message with formatting template and one parameter
 */
#define Logging_traceError1(formattingTemplate, st1) \
            _Logging_traceError(_expand(formattingTemplate, \
                                        st1))

/**
 * Writes trace message with formatting template and two parameters
 */
#define Logging_traceError2(formattingTemplate, st1, st2)   \
            _Logging_traceError(_expand(formattingTemplate, \
                                        st1, st2))

/**
 * Writes trace message with formatting template and three parameters
 */
#define Logging_traceError3(formattingTemplate, st1, st2, st3)  \
            _Logging_traceError(_expand(formattingTemplate, \
                                        st1, st2, st3))

/**
 * Writes trace message with formatting template and four parameters
 */
#define Logging_traceError4(formattingTemplate, st1, st2, st3, st4)     \
            _Logging_traceError(_expand(formattingTemplate, \
                                        st1, st2, st3, st4))

/**
 * Writes trace message with formatting template and five parameters
 */
#define Logging_traceError5(formattingTemplate, st1, st2, st3, st4,     \
                            st5) \
            _Logging_traceError(_expand(formattingTemplate, \
                                        st1, st2, st3, st4, st5))

/**
 * Writes trace message with formatting template and six parameters
 */
#define Logging_traceError6(formattingTemplate, st1, st2, st3, st4,     \
                            st5, st6) \
            _Logging_traceError(_expand(formattingTemplate, \
                                        st1, st2, st3, st4, st5, \
                                        st6))

/**
 * Writes trace message with formatting template and seven parameters
 */
#define Logging_traceError7(formattingTemplate, st1, st2, st3, st4,     \
                            st5, st6, st7) \
            _Logging_traceError(_expand(formattingTemplate, \
                                        st1, st2, st3, st4, st5, \
                                        st6, st7))

/**
 * Writes trace message with formatting template and eight parameters
 */
#define Logging_traceError8(formattingTemplate, st1, st2, st3, st4,     \
                            st5, st6, st7, st8) \
            _Logging_traceError(_expand(formattingTemplate, \
                                        st1, st2, st3, st4, st5, \
                                        st6, st7, st8))

/**
 * Writes trace message with formatting template and nine parameters
 */
#define Logging_traceError9(formattingTemplate, st1, st2, st3, st4,     \
                            st5, st6, st7, st8, st9) \
            _Logging_traceError(_expand(formattingTemplate, \
                                        st1, st2, st3, st4, st5, \
                                        st6, st7, st8, st9))

/**
 * Writes trace message with formatting template and ten parameters
 */
#define Logging_traceError10(formattingTemplate, st1, st2, st3, st4,    \
                             st5, st6, st7, st8, st9, st10) \
            _Logging_traceError(_expand(formattingTemplate, \
                                        st1, st2, st3, st4, st5, \
                                        st6, st7, st8, st9, st10))
