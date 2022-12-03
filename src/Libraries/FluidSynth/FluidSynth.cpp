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

/*====================*/
/* PUBLIC FEATURES    */
/*====================*/

FluidSynth::FluidSynth ()
    : _descriptor{NULL}
{
    Logging_trace(">>");

    DynamicLibrary* library = new DynamicLibrary(_libraryName);
    _descriptor = library;

    String message = (library->underlyingTechnicalLibrary() == NULL
                      ? "could not load library"
                      : "library loaded");

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
    DynamicLibrary* library = (DynamicLibrary*) _descriptor;
    Boolean result = library->isLoaded();
    Logging_trace1("<<: %1", TOSTRING(result));
    return result;
}

/*--------------------*/

Object FluidSynth::dynamicLibrary () const
{
    Logging_trace(">>");
    Logging_trace("<<");
    return _descriptor;
}
