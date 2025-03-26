/**
 * @file
 * The <C>FluidSynthSettings</C> class implements an object-oriented
 * wrapper around an underlying fluidsynth settings object with
 * dynamic library access.
 *
 * @author Dr. Thomas Tensi
 * @date   2022-04
 */

/*=========*/
/* IMPORTS */
/*=========*/

#include "Logging.h"
#include "Dictionary.h"

#include "DynamicLibrary.h"
#include "FluidSynthSettings.h"

/*--------------------*/

using BaseTypes::Containers::Dictionary;
using Libraries::DynamicLibrary;
using Libraries::FluidSynth::FluidSynthSettings;

/** abbreviation for StringUtil */
using STR = BaseModules::StringUtil;

/*-----------------------------------------*/
/* TYPES AND FUNCTIONS FOR DYNAMIC BINDING */
/*-----------------------------------------*/

/** settings creation function type in library */
typedef Object (*FSSettings_CreationProc)(void);
 
/** settings destruction function type in library */
typedef void (*FSSettings_DestructionProc)(Object);
 
/** setting function type for int value in library */
typedef int (*FSSettings_SetIntProc)(Object, const char*, int);
 
/** setting function type for float value in library */
typedef int (*FSSettings_SetNumProc)(Object, const char*, double);
 
/** setting function type for string value in library */
typedef int (*FSSettings_SetStrProc)(Object, const char*, const char*);

/** settings creation function in library */
static FSSettings_CreationProc FSSettings_make = NULL;

/** settings destruction function in library */
static FSSettings_DestructionProc FSSettings_destroy = NULL;

/** setting function for int value in library */
static FSSettings_SetIntProc FSSettings_setInt = NULL;

/** setting function for float value in library */
static FSSettings_SetNumProc FSSettings_setNum = NULL;

/** setting function for string value in library */
static FSSettings_SetStrProc FSSettings_setStr = NULL;

/** flag to tell whether function pointers have been initialized */
static Boolean _functionsAreInitialized = false;

/*--------*/
/* MACROS */
/*--------*/

/** reports that function with <C>name</C> is not dynamically defined */
#define reportBadFunction(name) \
    Logging_traceError("settings '" name "' function undefined")

/** simple macro for dynamic binding of a function */
#define GPA(signature, name) \
    (signature)((DynamicLibrary*) fsLibrary)->getFunctionByName(name)

/*====================*/
/* PRIVATE FEATURES    */
/*====================*/

/**
 * a mapping from a setting name from FluidSynth to the associated
 * type (encoded as a single letter)
*/
static Dictionary _settingsKeyToKindMap =
    Dictionary::makeFromString(
        "synth.chorus.active               -> B,"
        "synth.chorus.depth                -> F,"
        "synth.chorus.level                -> F,"
        "synth.chorus.nr                   -> I,"
        "synth.chorus.speed                -> F,"
        "synth.default-soundfont           -> S,"
        "synth.dynamic-sample-loading      -> B,"
        "synth.gain                        -> F,"
        "synth.interpolation-method        -> I,"
        "synth.midi-bank-select            -> S,"
        "synth.min-note-length             -> I,"
        "synth.overflow.age                -> F,"
        "synth.overflow.important          -> F,"
        "synth.overflow.important-channels -> S,"
        "synth.overflow.percussion         -> F,"
        "synth.overflow.released           -> F,"
        "synth.overflow.sustained          -> F,"
        "synth.overflow.volume             -> F,"
        "synth.polyphony                   -> I,"
        "synth.reverb.active               -> B,"
        "synth.reverb.damp                 -> F,"
        "synth.reverb.level                -> F,"
        "synth.reverb.room-size            -> F,"
        "synth.reverb.width                -> F,"
        "synth.sample-rate                 -> F,"
        "synth.threadsafe-api              -> B,"
        "synth.verbose                     -> B"
        );

/*--------------------*/

/**
 * Connects static functions dynamically to functions from DLL
 * <C>fsLibrary</C>
 *
 * @param[in] fsLibrary  underlying fluidsynth library
 */
static void _initializeFunctionsForLibrary (IN Object fsLibrary)
{
    Logging_trace(">>");

    if (fsLibrary == NULL) {
        Logging_traceError("fluidsynth library not loaded");
    } else if (!_functionsAreInitialized) {
        FSSettings_make =
            GPA(FSSettings_CreationProc, "new_fluid_settings");
        FSSettings_destroy =
            GPA(FSSettings_DestructionProc, "delete_fluid_settings");
        FSSettings_setInt =
            GPA(FSSettings_SetIntProc, "fluid_settings_setint");
        FSSettings_setNum =
            GPA(FSSettings_SetNumProc, "fluid_settings_setnum");
        FSSettings_setStr =
            GPA(FSSettings_SetStrProc, "fluid_settings_setstr");

        _functionsAreInitialized = true;
    }
    
    Logging_trace("<<");
}

/*====================*/
/* PUBLIC FEATURES    */
/*====================*/

FluidSynthSettings::FluidSynthSettings (IN FluidSynth* library)
    : _descriptor{NULL}
{
    Logging_trace(">>");

    Logging_trace1("--: settingsToKindMap = %1",
                   _settingsKeyToKindMap.toString());

    if (!library->isLoaded()) {
        Logging_traceError("library is undefined");
    } else {
        _initializeFunctionsForLibrary(library->underlyingObject());

        if (FSSettings_make == NULL) {
            reportBadFunction("make");
        } else {
            _descriptor = FSSettings_make();
        }
    }

    Logging_trace("<<");
}

/*--------------------*/

FluidSynthSettings::~FluidSynthSettings ()
{
    Logging_trace(">>");

    if (_descriptor != NULL) {
        if (FSSettings_destroy == NULL) {
            reportBadFunction("destroy");
        } else {
            FSSettings_destroy(_descriptor);
        }

        _descriptor = NULL;
    }

    Logging_trace("<<");
}

/*--------------------*/

Boolean FluidSynthSettings::set (IN String& key, IN String& value)
{
    Logging_trace2(">>: key = %1, value = %2", key, value);

    Boolean isOkay = false;
    Integer operationResult;

    if (!_settingsKeyToKindMap.contains(key)) {
        Logging_traceError("key not found --> skipped");
    } else {
        const String kind = _settingsKeyToKindMap.at(key);
        const char* keyAsCharArray = key.c_str();
        Object settingsObject = _descriptor;

        if (settingsObject == NULL) {
            Logging_traceError("settings object must be defined");
        } else {
            if (key == "synth.interpolation-method") {
                /* special handling of interpolation method (which is
                   not an official parameter */
                Integer intValue = STR::toInteger(value);
                Logging_trace2("--: kind = %1, value = %2",
                               kind, TOSTRING(intValue));
                isOkay = (intValue == 0 || intValue == 1
                          || intValue == 4 || intValue == 7);
            } else if (kind == "B") {
                Integer boolValue = (value == "true" || value == "1"
                                     ? 1 : 0);
                Logging_trace2("--: kind = %1, value = %2",
                               kind, TOSTRING(boolValue));

                if (FSSettings_setInt == NULL) {
                    reportBadFunction("setInt");
                } else {
                    operationResult = FSSettings_setInt(settingsObject,
                                                        keyAsCharArray,
                                                        (int) boolValue);
                    isOkay = (operationResult == 0);
                }
            } else if (kind == "I") {
                Integer intValue = STR::toInteger(value);
                Logging_trace2("--: kind = %1, value = %2",
                               kind, TOSTRING(intValue));

                if (FSSettings_setInt == NULL) {
                    reportBadFunction("setInt");
                } else {
                    operationResult = FSSettings_setInt(settingsObject,
                                                        keyAsCharArray,
                                                        (int) intValue);
                    isOkay = (operationResult == 0);
                }
            } if (kind == "F") {
                Real numValue = STR::toReal(value);
                Logging_trace2("--: kind = %1, value = %2",
                               kind, TOSTRING(numValue));

                if (FSSettings_setNum == NULL) {
                    reportBadFunction("setNum");
                } else {
                    operationResult = FSSettings_setNum(settingsObject,
                                                        keyAsCharArray,
                                                        (float) numValue);
                    isOkay = (operationResult == 0);
                }
            } if (kind == "S") {
                Logging_trace2("--: kind = %1, value = %2",
                               kind, value);

                if (FSSettings_setStr == NULL) {
                    reportBadFunction("setStr");
                } else {
                    operationResult = FSSettings_setStr(settingsObject,
                                                        keyAsCharArray,
                                                        value.c_str());
                    isOkay = (operationResult == 0);
                }
            }
        } 
    }
    
    Logging_trace1("<<: %1", TOSTRING(isOkay));
    return isOkay;
}

/*--------------------*/

Object FluidSynthSettings::underlyingObject () const
{
    return _descriptor;
}
