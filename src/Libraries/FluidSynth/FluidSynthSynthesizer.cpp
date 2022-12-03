/**
 * @file
 * The <C>FluidSynthSynthesizer</C> class implements an object-oriented
 * wrapper around an underlying fluidsynth synthesizer object with
 * dynamic library access.
 *
 * @author Dr. Thomas Tensi
 * @date   2022-04
 */

/*=========*/
/* IMPORTS */
/*=========*/

#include "DynamicLibrary.h"
#include "FluidSynthSynthesizer.h"
#include "Logging.h"
#include "MyArray.h"

/*--------------------*/

using BaseTypes::Containers::convertArray;
using Libraries::DynamicLibrary;
using Libraries::FluidSynth::FluidSynthSynthesizer;

/*-----------------------------------------*/
/* TYPES AND FUNCTIONS FOR DYNAMIC BINDING */
/*-----------------------------------------*/

/** synthesizer creation function type in library  */
typedef Object (*FSSynthesizer_CreationProc)(Object);

/** synthesizer destruction function type in library */
typedef void (*FS_DestructionProc)(Object);

/** MIDI bank selection function type in library */
typedef int (*FSSynthesizer_BankChangeProc)(Object, int, int);

/** internal buffer size function type in library */
typedef int (*FSSynthesizer_BufferSizeProc)(Object);

/** MIDI control change function type in library */
typedef int (*FSSynthesizer_ControlChangeProc)(Object, int, int, int);

/** mono touch function type in library */
typedef int (*FSSynthesizer_MonoTouchProc)(Object, int, int, int);

/** MIDI note off function type in library */
typedef int (*FSSynthesizer_NoteOffProc)(Object, int, int);

/** MIDI note on function type in library */
typedef int (*FSSynthesizer_NoteOnProc)(Object, int, int, int);

/** pitch bend function type in library */
typedef int (*FSSynthesizer_PitchBendProc)(Object, int, int);

/** poly touch function type in library */
typedef int (*FSSynthesizer_PolyTouchProc)(Object, int, int);

/** MIDI program change function type in library */
typedef int (*FSSynthesizer_ProgChangeProc)(Object, int, int);

/** sound font load function type in library */
typedef int (*FSSynthesizer_SFLoadProc)(Object, const char*, int);

/** audio buffer processing function type in library */
typedef int (*FSSynthesizer_ProcessProc)(Object, int, int,
                                         Object, int, Object);

/*--------------------*/

/** synthesizer creation function in library  */
static FSSynthesizer_CreationProc FSSynthesizer_make;

/** synthesizer destruction function in library */
static FS_DestructionProc FSSynthesizer_destroy;

/** the internal buffer size (raster) in library */
static FSSynthesizer_BufferSizeProc FSSynthesizer_internalBufferSize;

/** MIDI bank select function type in library */
static FSSynthesizer_BankChangeProc FSSynthesizer_handleBankChange;

/** MIDI control change function type in library */
static FSSynthesizer_ControlChangeProc FSSynthesizer_handleControlChange;

/** mono touch function in library */
static FSSynthesizer_MonoTouchProc FSSynthesizer_handleMonoTouch;

/** MIDI note off function in library */
static FSSynthesizer_NoteOffProc FSSynthesizer_handleNoteOff;

/** MIDI note on function in library */
static FSSynthesizer_NoteOnProc FSSynthesizer_handleNoteOn;

/** pitch bend processing function in library */
static FSSynthesizer_PitchBendProc FSSynthesizer_handlePitchBend;

/** poly touch function in library */
static FSSynthesizer_PolyTouchProc FSSynthesizer_handlePolyTouch;

/** MIDI program change function in library */
static FSSynthesizer_ProgChangeProc FSSynthesizer_handleProgramChange;

/** sound font load function in library */
static FSSynthesizer_SFLoadProc FSSynthesizer_loadSoundFont;

/** audio buffer processing function in library */
static FSSynthesizer_ProcessProc FSSynthesizer_process;

/*--------*/
/* MACROS */
/*--------*/

/** error message for an undefined descriptor */
static const String _errorMessageForUndefinedDescriptor =
    "synthesizer object must be defined";

/** reports that function with <C>name</C> is not dynamically defined */
#define _reportBadFunction(name) \
    Logging_traceError("synthesizer '" name "' function undefined")

/** simple macro for dynamic binding of a function */
#define GPA(signature, name) \
    (signature) ((DynamicLibrary*) fsLibrary)->getFunctionByName(name)

/*====================*/
/* PRIVATE FEATURES   */
/*====================*/

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
    } else {
        FSSynthesizer_make =
            GPA(FSSynthesizer_CreationProc, "new_fluid_synth");
        FSSynthesizer_destroy =
            GPA(FS_DestructionProc, "delete_fluid_synth");

        FSSynthesizer_internalBufferSize =
            GPA(FSSynthesizer_BufferSizeProc,
                "fluid_synth_get_internal_bufsize");
        FSSynthesizer_handleBankChange =
            GPA(FSSynthesizer_BankChangeProc, "fluid_synth_bank_select");
        FSSynthesizer_handleControlChange = 
            GPA(FSSynthesizer_ControlChangeProc, "fluid_synth_cc");
        FSSynthesizer_handleMonoTouch = 
            GPA(FSSynthesizer_MonoTouchProc, "fluid_synth_key_pressure");
        FSSynthesizer_handleNoteOff =
            GPA(FSSynthesizer_NoteOffProc, "fluid_synth_noteoff");
        FSSynthesizer_handleNoteOn =
            GPA(FSSynthesizer_NoteOnProc, "fluid_synth_noteon");
        FSSynthesizer_handlePitchBend =
            GPA(FSSynthesizer_PitchBendProc, "fluid_synth_pitch_bend");
        FSSynthesizer_handlePolyTouch = 
            GPA(FSSynthesizer_PolyTouchProc,
                "fluid_synth_channel_pressure");
        FSSynthesizer_handleProgramChange =
            GPA(FSSynthesizer_ProgChangeProc,
                "fluid_synth_program_change");
        FSSynthesizer_loadSoundFont =
            GPA(FSSynthesizer_SFLoadProc, "fluid_synth_sfload");
        FSSynthesizer_process =
            GPA(FSSynthesizer_ProcessProc, "fluid_synth_process");
    }
    
    Logging_trace("<<");
}

/*====================*/
/* PUBLIC FEATURES    */
/*====================*/

FluidSynthSynthesizer::FluidSynthSynthesizer (IN FluidSynth* library,
                                              IN FluidSynthSettings* settings)
    : _descriptor{NULL}
{
    Logging_trace(">>");

    if (!library->isLoaded()) {
        Logging_traceError("library or settings object is undefined");
    } else {
        _initializeFunctionsForLibrary(library->dynamicLibrary());

        if (FSSynthesizer_make == NULL) {
            _reportBadFunction("make");
        } else {
            _descriptor = FSSynthesizer_make(settings->fsSettings());
        }
    }
    
    Logging_trace("<<");
}

/*--------------------*/

FluidSynthSynthesizer::~FluidSynthSynthesizer ()
{
    Logging_trace(">>");

    if (_descriptor != NULL) {
        if (FSSynthesizer_destroy == NULL) {
            _reportBadFunction("destroy");
        } else {
            FSSynthesizer_destroy(_descriptor);
        }
        
        _descriptor = NULL;
    }

    Logging_trace("<<");
}

/*--------------------*/

Natural FluidSynthSynthesizer::internalBufferSize () const
{
    Logging_trace(">>");
    Natural result = 0;

    if (_descriptor == NULL) {
        Logging_traceError(_errorMessageForUndefinedDescriptor);
    } else if (FSSynthesizer_internalBufferSize == NULL) {
        _reportBadFunction("internalBufferSize");
    } else {
        result = FSSynthesizer_internalBufferSize(_descriptor);
    }
    
    Logging_trace1("<<: %1", TOSTRING(result));
    return result;
}
        
/*--------------------*/

Boolean
FluidSynthSynthesizer::handleBankChange (IN Natural channel,
                                         IN Natural bankNumber)
{
    Logging_trace2(">>: channel = %1, bankNumber = %2",
                   TOSTRING(channel), TOSTRING(bankNumber));

    Boolean isOkay = false;

    if (_descriptor == NULL) {
        Logging_traceError(_errorMessageForUndefinedDescriptor);
    } else if (FSSynthesizer_handleBankChange == NULL) {
        _reportBadFunction("bankChange");
    } else {
        Integer operationResult =
            FSSynthesizer_handleBankChange(_descriptor,
                                           (int) channel,
                                           (int) bankNumber);
        isOkay = (operationResult == 0);
    }

    Logging_trace1("<<: %1", TOSTRING(isOkay));
    return isOkay;
}

/*--------------------*/

Boolean FluidSynthSynthesizer::handleControlChange (IN Natural channel,
                                                 IN Natural controller,
                                                 IN Natural value)
{
    Logging_trace3(">>: channel = %1, controller = %2, value = %3",
                   TOSTRING(channel), TOSTRING(controller), TOSTRING(value));

    Boolean isOkay = false;

    if (_descriptor == NULL) {
        Logging_traceError(_errorMessageForUndefinedDescriptor);
    } else if (FSSynthesizer_handleControlChange == NULL) {
        _reportBadFunction("controlChange");
    } else {
        Integer operationResult =
            FSSynthesizer_handleControlChange(_descriptor,
                                              (int) channel,
                                              (int) controller,
                                              (int) value);
        isOkay = (operationResult == 0);
    }

    Logging_trace1("<<: %1", TOSTRING(isOkay));
    return isOkay;
}

/*--------------------*/

Boolean FluidSynthSynthesizer::handleMonoTouch (IN Natural channel,
                                                IN Natural key,
                                                IN Natural value)
{
    Logging_trace3(">>: channel = %1, key = %2, value = %3",
                   TOSTRING(channel), TOSTRING(key), TOSTRING(value));

    Boolean isOkay = false;

    if (_descriptor == NULL) {
        Logging_traceError(_errorMessageForUndefinedDescriptor);
    } else if (FSSynthesizer_handleMonoTouch == NULL) {
        _reportBadFunction("monoTouch");
    } else {
        Integer operationResult =
            FSSynthesizer_handleMonoTouch(_descriptor,
                                          (int) channel,
                                          (int) key,
                                          (int) value);
        isOkay = (operationResult == 0);
    }

    Logging_trace1("<<: %1", TOSTRING(isOkay));
    return isOkay;
}

/*--------------------*/

Boolean FluidSynthSynthesizer::handleNoteOff (IN Natural channel,
                                              IN Natural note)
{
    Logging_trace2(">>: channel = %1, note = %2",
                   TOSTRING(channel), TOSTRING(note));

    Boolean isOkay = false;

    if (_descriptor == NULL) {
        Logging_traceError(_errorMessageForUndefinedDescriptor);
    } else if (FSSynthesizer_handleNoteOff == NULL) {
        _reportBadFunction("noteoff");
    } else {
        Integer operationResult =
            FSSynthesizer_handleNoteOff(_descriptor,
                                        (int) channel, (int) note);
        isOkay = (operationResult == 0);
    }

    Logging_trace1("<<: %1", TOSTRING(isOkay));
    return isOkay;
}

/*--------------------*/

Boolean FluidSynthSynthesizer::handleNoteOn (IN Natural channel,
                                             IN Natural note,
                                             IN Natural velocity)
{
    Logging_trace3(">>: channel = %1, note = %2, velocity = %3",
                   TOSTRING(channel), TOSTRING(note),
                   TOSTRING(velocity));

    Boolean isOkay = false;

    if (_descriptor == NULL) {
        Logging_traceError(_errorMessageForUndefinedDescriptor);
    } else if (FSSynthesizer_handleNoteOn == NULL) {
        _reportBadFunction("noteon");
    } else {
        Integer operationResult =
            FSSynthesizer_handleNoteOn(_descriptor,
                                       (int) channel, (int) note,
                                       (int) velocity);
        isOkay = (operationResult == 0);
    }

    Logging_trace1("<<: %1", TOSTRING(isOkay));
    return isOkay;
}

/*--------------------*/

Boolean FluidSynthSynthesizer::handlePitchBend (IN Natural channel,
                                                IN Natural value)
{
    Logging_trace2(">>: channel = %1, value = %2",
                   TOSTRING(channel), TOSTRING(value));

    Boolean isOkay = false;

    if (_descriptor == NULL) {
        Logging_traceError(_errorMessageForUndefinedDescriptor);
    } else if (FSSynthesizer_handlePitchBend == NULL) {
        _reportBadFunction("pitchBend");
    } else {
        Integer operationResult =
            FSSynthesizer_handlePitchBend(_descriptor,
                                          (int) channel,
                                          (int) value);
        isOkay = (operationResult == 0);
    }

    Logging_trace1("<<: %1", TOSTRING(isOkay));
    return isOkay;
}

/*--------------------*/

Boolean FluidSynthSynthesizer::handlePolyTouch (IN Natural channel,
                                                IN Natural value)
{
    Logging_trace2(">>: channel = %1, value = %2",
                   TOSTRING(channel), TOSTRING(value));

    Boolean isOkay = false;

    if (_descriptor == NULL) {
        Logging_traceError(_errorMessageForUndefinedDescriptor);
    } else if (FSSynthesizer_handlePolyTouch == NULL) {
        _reportBadFunction("polyTouch");
    } else {
        Integer operationResult =
            FSSynthesizer_handlePolyTouch(_descriptor,
                                          (int) channel,
                                          (int) value);
        isOkay = (operationResult == 0);
    }

    Logging_trace1("<<: %1", TOSTRING(isOkay));
    return isOkay;
}

/*--------------------*/

Boolean
FluidSynthSynthesizer::handleProgramChange (IN Natural channel,
                                            IN Natural programNumber)
{
    Logging_trace2(">>: channel = %1, programNumber = %2",
                   TOSTRING(channel), TOSTRING(programNumber));

    Boolean isOkay = false;

    if (_descriptor == NULL) {
        Logging_traceError(_errorMessageForUndefinedDescriptor);
    } else if (FSSynthesizer_handleProgramChange == NULL) {
        _reportBadFunction("programChange");
    } else {
        Integer operationResult =
            FSSynthesizer_handleProgramChange(_descriptor,
                                              (int) channel,
                                              (int) programNumber);
        isOkay = (operationResult == 0);
    }

    Logging_trace1("<<: %1", TOSTRING(isOkay));
    return isOkay;
}

/*--------------------*/

Boolean FluidSynthSynthesizer::loadSoundFont (IN String& soundFontPath)
{
    Logging_trace1(">>: %1", soundFontPath);

    Boolean isOkay = false;

    if (_descriptor == NULL) {
        Logging_traceError(_errorMessageForUndefinedDescriptor);
    } else if (FSSynthesizer_loadSoundFont == NULL) {
        _reportBadFunction("loadSoundFont");
    } else {
        int soundFontId =
            FSSynthesizer_loadSoundFont(_descriptor,
                                        soundFontPath.c_str(), 1);

        if (soundFontId < 0) {
            Logging_trace("--: loading of the sound font failed!");
        } else {
            Logging_trace("--: sound font loaded");
            isOkay = true;
        }
    }

    Logging_trace1("<<", TOSTRING(isOkay));
    return isOkay;
}

/*--------------------*/

Boolean
FluidSynthSynthesizer::process (INOUT AudioSampleListVector& sampleBuffer,
                                IN Natural position,
                                IN Natural sampleCount)
{
    Natural channelCount = sampleBuffer.length();
    Logging_trace3(">>: channelCount = %1, position = %2,"
                   " sampleCount = %3",
                   TOSTRING(channelCount),
                   TOSTRING(position),
                   TOSTRING(sampleCount));

    Boolean isOkay = false;

    if (_descriptor == NULL) {
        Logging_traceError(_errorMessageForUndefinedDescriptor);
    } else if (FSSynthesizer_process == NULL) {
        _reportBadFunction("process");
    } else {
        /* provide a stereo buffer with sample count frames */
        float* floatSampleBuffer[] = {
            (float*) makeLocalArray(float, sampleCount),
            (float*) makeLocalArray(float, sampleCount)
        };

        clearArray(floatSampleBuffer[0], float, sampleCount);
        clearArray(floatSampleBuffer[1], float, sampleCount);
        
        /* mix dry audio and effects into sample buffer */
        Integer operationResult =
            FSSynthesizer_process(_descriptor,
                                  (int) sampleCount,
                                  (int) channelCount, floatSampleBuffer,
                                  (int) channelCount, floatSampleBuffer);

        AudioSample* sampleListA = sampleBuffer[0].asArray(position);
        AudioSample* sampleListB = sampleBuffer[1].asArray(position);
        convertArray(sampleListA, floatSampleBuffer[0], sampleCount);
        convertArray(sampleListB, floatSampleBuffer[1], sampleCount);

        isOkay = (operationResult == 0);
    }

    Logging_trace1("<<: %1", TOSTRING(isOkay));
    return isOkay;
}
