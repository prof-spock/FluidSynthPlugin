/**
 * @file
 * The <C>FluidSynth</C> class implements the access to the fluidsynth
 * library via a dynamic loading of it.
 *
 * @author Dr. Thomas Tensi
 * @date   2022-04
 */

/*=========*/
/* IMPORTS */
/*=========*/

#include "Logging.h"
#include "Dictionary.h"

#include "FluidSynth.h"
#include "DynamicLibrary.h"

/*--------------------*/

using Libraries::DynamicLibrary;
using Libraries::FluidSynth::FluidSynth;

/*-----------------------------------------*/
/* TYPES AND FUNCTIONS FOR DYNAMIC BINDING */
/*-----------------------------------------*/

/** version function type in library */
typedef char* (*FSynth_VersionProc)(void);

/** version function in library */
static FSynth_VersionProc FSynth_version = NULL;

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

/** the library name for the fluidsynth dynamic library */
#if defined(_WINDOWS)
    #define _libraryName "libfluidsynth-3.dll"
#elif defined(APPLE)
    #define _libraryName "@loader_path/libfluidsynth.3.dylib"
#else
    #define _libraryName "libfluidsynth.so.3"
#endif

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
        FSynth_version =
            GPA(FSynth_VersionProc, "fluid_version_str");
        _functionsAreInitialized = true;
    }
    
    Logging_trace("<<");
}

/*====================*/
/* PUBLIC FEATURES    */
/*====================*/

FluidSynth::FluidSynth ()
    : _descriptor{NULL}
{
    Logging_trace(">>");

    DynamicLibrary* library = new DynamicLibrary(_libraryName);
    _descriptor = library;

    Boolean libraryIsLoaded = library->isLoaded();

    if (libraryIsLoaded) {
        _initializeFunctionsForLibrary(library);
    }

    const String message = (libraryIsLoaded
                            ? "library loaded"
                            : "could not load library");

    Logging_trace1("<<: %1", message);
}

/*--------------------*/

FluidSynth::~FluidSynth ()
{
    Logging_trace(">>");

    if (_descriptor != NULL) {
        DynamicLibrary* library = (DynamicLibrary*) _descriptor;
        delete library;
    }

    Logging_trace("<<");
}

/*--------------------*/

Boolean FluidSynth::isLoaded () const
{
    Logging_trace(">>");
    const DynamicLibrary* library =
        static_cast<DynamicLibrary*>(_descriptor);
    Boolean result = library->isLoaded();
    Logging_trace1("<<: %1", TOSTRING(result));
    return result;
}

/*--------------------*/

String FluidSynth::version () const
{
    Logging_trace(">>");

    String result = "???";

    if (_descriptor != NULL) {
        if (FSynth_version == NULL) {
            reportBadFunction("version");
        } else {
            result = String{FSynth_version()};
        }
    }

    Logging_trace1("<<: %1", result);
    return result;
}

/*--------------------*/

Object FluidSynth::underlyingObject () const
{
    Logging_trace(">>");
    Logging_trace("<<");
    return _descriptor;
}
