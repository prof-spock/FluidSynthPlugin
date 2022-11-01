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

/** the library name for the fluidsynth DLL */
static String _libraryName = "libfluidsynth-3.dll";

/*====================*/
/* PUBLIC FEATURES    */
/*====================*/

FluidSynth::FluidSynth ()
    : _descriptor{NULL}
{
    Logging_trace(">>");

    _descriptor = new DynamicLibrary(_libraryName);

    String message = (_descriptor == NULL
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

Object FluidSynth::fsLibrary () const
{
    Logging_trace(">>");
    Logging_trace("<<");
    return _descriptor;
}
