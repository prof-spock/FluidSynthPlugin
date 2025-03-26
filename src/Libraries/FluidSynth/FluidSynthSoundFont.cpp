/**
 * @file
 * The <C>FluidSynthSoundFont</C> class implements an object-oriented
 * wrapper around an underlying fluidsynth soundfont object with
 * dynamic library access.
 *
 * @author Dr. Thomas Tensi
 * @date   2024-03
 */

/*=========*/
/* IMPORTS */
/*=========*/

#include "DynamicLibrary.h"
#include "FluidSynthSoundFont.h"
#include "Logging.h"
#include "OperatingSystem.h"

/*--------------------*/

using BaseModules::OperatingSystem;
using Libraries::DynamicLibrary;
using Libraries::FluidSynth::FluidSynthSoundFont;

/** abbreviation for StringUtil */
using STR = BaseModules::StringUtil;

/*-----------------------------------------*/
/* TYPES AND FUNCTIONS FOR DYNAMIC BINDING */
/*-----------------------------------------*/

/** soundfont construction function type in library  */
typedef Object (*FSSoundFont_ConstructionProc)(Object, int);

/** sound font preset enumeration reset function type in library */
typedef void (*FSSoundFont_EnumerationResetProc)(Object);

/** sound font preset enumeration successor function type in library */
typedef Object (*FSSoundFont_EnumerationSuccessorProc)(Object);

/** sound font object name query type in library */
typedef const char* (*FSSoundFont_ObjectNameQueryProc)(Object);

/** sound font preset bank number query type in library */
typedef int (*FSSoundFont_PresetBankNumQueryProc)(Object);

/** sound font preset program number query type in library */
typedef int (*FSSoundFont_PresetProgNumQueryProc)(Object);

/** sound font preset query by bank and program type in library */
typedef Object (*FSSoundFont_PresetQueryProc)(Object, int, int);

/*--------------------*/

/** soundfont creation function in library  */
static FSSoundFont_ConstructionProc FSSoundFont_make;

/** sound font name query in library */
static FSSoundFont_ObjectNameQueryProc FSSoundFont_name;

/** sound font preset enumeration reset function in library */
static FSSoundFont_EnumerationSuccessorProc
    FSSoundFont_nextPresetInEnumeration;

/** sound font preset bank number query in library */
static FSSoundFont_PresetBankNumQueryProc FSSoundFont_presetBankNum;

/** sound font preset name query in library */
static FSSoundFont_ObjectNameQueryProc FSSoundFont_presetName;

/** sound font preset program number query in library */
static FSSoundFont_PresetProgNumQueryProc FSSoundFont_presetProgNum;

/** sound font preset query in library */
static FSSoundFont_PresetQueryProc FSSoundFont_presetQuery;

/** sound font preset enumeration reset function in library */
static FSSoundFont_EnumerationResetProc FSSoundFont_resetPresetEnumeration;

/** flag to tell whether function pointers have been initialized */
static Boolean _functionsAreInitialized = false;

/*--------*/
/* MACROS */
/*--------*/

/** error message for an undefined descriptor */
static const String _errorMessageForUndefinedDescriptor =
    "soundfont object must be defined";

/** reports that function with <C>name</C> is not dynamically defined */
#define _reportBadFunction(name) \
    Logging_traceError("soundfont '" name "' function undefined")

/** simple macro for dynamic binding of a function */
#define GPA(signature, name) \
    (signature)((DynamicLibrary*) fsLibrary)->getFunctionByName(name)

/*====================*/
/* PRIVATE FEATURES   */
/*====================*/

/**
 * Checks and returns whether all soundfont-related functions are
 * available
 *
 * @return  information about full function availability
 */
static Boolean _allFSFunctionsAreAvailable ()
{
    Logging_trace(">>");

    Boolean result = true;
    
    if (FSSoundFont_resetPresetEnumeration == NULL) {
        _reportBadFunction("resetPresetEnumeration");
        result = false;
    }

    if (FSSoundFont_nextPresetInEnumeration ==  NULL) {
        _reportBadFunction("nextPresetInEnumeration");
        result = false;
    }

    if (FSSoundFont_presetBankNum == NULL) {
        _reportBadFunction("presetBankNum");
        result = false;
    }

    if (FSSoundFont_presetName == NULL) {
        _reportBadFunction("presetName");
        result = false;
    }

    if (FSSoundFont_presetProgNum == NULL) {
        _reportBadFunction("presetProgNum");
        result = false;
    }

    Logging_trace1("<<: %1", TOSTRING(result));
    return result;
}

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
        FSSoundFont_make =
            GPA(FSSoundFont_ConstructionProc, "fluid_synth_get_sfont");

        FSSoundFont_name =
            GPA(FSSoundFont_ObjectNameQueryProc, "fluid_sfont_get_name");
        FSSoundFont_nextPresetInEnumeration = 
            GPA(FSSoundFont_EnumerationSuccessorProc,
                "fluid_sfont_iteration_next");

        FSSoundFont_presetBankNum =
            GPA(FSSoundFont_PresetBankNumQueryProc,
                "fluid_preset_get_banknum");
        FSSoundFont_presetName =
            GPA(FSSoundFont_ObjectNameQueryProc, "fluid_preset_get_name");
        FSSoundFont_presetProgNum =
            GPA(FSSoundFont_PresetProgNumQueryProc, "fluid_preset_get_num");
        FSSoundFont_presetQuery =
            GPA(FSSoundFont_PresetQueryProc, "fluid_sfont_get_preset");

        FSSoundFont_resetPresetEnumeration =
            GPA(FSSoundFont_EnumerationResetProc,
                "fluid_sfont_iteration_start");

        _functionsAreInitialized = true;
    }
    
    Logging_trace("<<");
}

/*====================*/
/* PUBLIC FEATURES    */
/*====================*/

/*--------------------*/
/* con-/destruction   */
/*--------------------*/

FluidSynthSoundFont::FluidSynthSoundFont
                         (IN FluidSynthSynthesizer* synthesizer,
                          IN Natural levelFromTop)
    : _descriptor{NULL}
{
    Logging_trace(">>");

    const FluidSynth* library =
        (synthesizer == NULL ? NULL : synthesizer->library());

    if (synthesizer == NULL || library == NULL || !library->isLoaded()) {
        Logging_traceError("library or synthesizer object is undefined");
    } else {
        _initializeFunctionsForLibrary(library->underlyingObject());

        if (FSSoundFont_make == NULL) {
            _reportBadFunction("make");
        } else {
            _descriptor = FSSoundFont_make(synthesizer->underlyingObject(),
                                           (int) levelFromTop);
        }
    }
    
    Logging_trace("<<");
}

/*--------------------*/

FluidSynthSoundFont::~FluidSynthSoundFont ()
{
    Logging_trace(">>");
    Logging_trace("<<");
}

/*--------------------*/
/* queries            */
/*--------------------*/

Boolean FluidSynthSoundFont::hasPreset (IN Natural bankNumber,
                                        IN Natural programNumber) const
{
    Logging_trace2(">>: bank = %1, program = %2",
                   TOSTRING(bankNumber), TOSTRING(programNumber));

    Boolean result = false;

    if (_descriptor == NULL) {
        Logging_traceError(_errorMessageForUndefinedDescriptor);
    } else if (_allFSFunctionsAreAvailable()) {
        Object self = _descriptor;
        const Object preset =
            FSSoundFont_presetQuery(self,
                                    (int) bankNumber,
                                    (int) programNumber);
        result = (preset != NULL);
    }

    Logging_trace1("<<: %1", TOSTRING(result));
    return result;
}
        
/*--------------------*/

String FluidSynthSoundFont::name () const
{
    Logging_trace(">>");

    String result;

    if (_descriptor == NULL) {
        Logging_traceError(_errorMessageForUndefinedDescriptor);
    } else if (_allFSFunctionsAreAvailable()) {
        Object self = _descriptor;
        result = String{FSSoundFont_name(self)};
        result = OperatingSystem::basename(result, false);
    }

    Logging_trace1("<<: %1", result.toString());
    return result;
}

/*--------------------*/

StringList FluidSynthSoundFont::presetList () const
{
    Logging_trace(">>");

    StringList result;
    
    if (_descriptor == NULL) {
        Logging_traceError(_errorMessageForUndefinedDescriptor);
    } else if (_allFSFunctionsAreAvailable()) {
        Object self = _descriptor;
        FSSoundFont_resetPresetEnumeration(self);

        while (true) {
            Object preset = FSSoundFont_nextPresetInEnumeration(self);

            if (preset == NULL) {
                break;
            }

            Natural bankNumber    = FSSoundFont_presetBankNum(preset);
            Natural programNumber = FSSoundFont_presetProgNum(preset);
            String name           = FSSoundFont_presetName(preset);
            String line = STR::expand("%1\t%2\t%3",
                                      TOSTRING(bankNumber),
                                      TOSTRING(programNumber),
                                      name);
            result.append(line);
        }
    }

    Logging_trace1("<<: %1", result.toString());
    return result;
}

/*--------------------*/

String FluidSynthSoundFont::presetName (IN Natural bankNumber,
                                        IN Natural programNumber) const
{
    Logging_trace2(">>: bank = %1, program = %2",
                   TOSTRING(bankNumber), TOSTRING(programNumber));

    Object fsSoundfont = _descriptor;
    Object preset = FSSoundFont_presetQuery(fsSoundfont,
                                            (int) bankNumber,
                                            (int) programNumber);
    String result =
        (preset == NULL ? "???" : FSSoundFont_presetName(preset));

    Logging_trace1("<<: %1", result);
    return result;
}
