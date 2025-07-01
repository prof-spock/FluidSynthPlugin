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

using BaseTypes::Containers::clearArray;
using BaseTypes::Containers::convertArray;
using Libraries::DynamicLibrary;
using Libraries::FluidSynth::FluidSynthSynthesizer;

/*-----------------------------------------*/
/* TYPES AND FUNCTIONS FOR DYNAMIC BINDING */
/*-----------------------------------------*/

/** synthesizer creation function type in library  */
typedef Object (*FSSynthesizer_CreationProc)(Object);

/** synthesizer destruction function type in library */
typedef void (*FSSynthesizer_DestructionProc)(Object);

/** MIDI bank selection function type in library */
typedef int (*FSSynthesizer_BankChangeProc)(Object, int, int);

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

/** audio buffer processing function type in library */
typedef int (*FSSynthesizer_ProcessProc)(Object, int, int,
                                         Object, int, Object);

/** MIDI program change function type in library */
typedef int (*FSSynthesizer_ProgChangeProc)(Object, int, int);

/** sound font load function type in library */
typedef int (*FSSynthesizer_SFLoadProc)(Object, const char*, int);

/** internal size function type in library */
typedef int (*FSSynthesizer_SizeProc)(Object);


/** SPECIAL FUNCTION: setting function type for interpolation method
  * in library */
typedef int (*FSSynthesizer_SetInterpolationMethodProc)(Object, int, int);
 

/*--------------------*/

/** synthesizer creation function in library  */
static FSSynthesizer_CreationProc FSSynthesizer_make;

/** synthesizer destruction function in library */
static FSSynthesizer_DestructionProc FSSynthesizer_destroy;

/** the internal buffer size (raster) in library */
static FSSynthesizer_SizeProc FSSynthesizer_internalBufferSize;

/** the number of audio channel pairs in library */
static FSSynthesizer_SizeProc FSSynthesizer_audioChannelPairCount;

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

/** interpolation method set function in library */
static FSSynthesizer_SetInterpolationMethodProc
            FSSynthesizer_setInterpolationMethod;

/** flag to tell whether function pointers have been initialized */
static Boolean _functionsAreInitialized = false;

/*--------*/
/* MACROS */
/*--------*/

/** error message for an undefined descriptor */
static const String _errorMessageForUndefinedDescriptor =
    "synthesizer object must be defined";

/** error message for a bad interpolation method */
static const String _errorMessageForBadInterpolationMethodCode =
    "interpolation method code must be 0, 1, 4 or 7";

/** reports that function with <C>name</C> is not dynamically defined */
#define _reportBadFunction(name) \
    Logging_traceError("synthesizer '" name "' function undefined")

/** simple macro for dynamic binding of a function */
#define GPA(signature, name) \
    (signature)((DynamicLibrary*) fsLibrary)->getFunctionByName(name)

/*====================*/
/* PRIVATE FEATURES   */
/*====================*/

namespace Libraries::FluidSynth {

    struct _SynthesizerDescriptor {

        /** the underlying synthesizer object */
        Object synthesizer = NULL;

        /** the associated fluidsynth library */
        FluidSynth* associatedLibrary = NULL;

    };
}

/*====================*/

/*--------------------*/
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
        FSSynthesizer_make =
            GPA(FSSynthesizer_CreationProc, "new_fluid_synth");
        FSSynthesizer_destroy =
            GPA(FSSynthesizer_DestructionProc, "delete_fluid_synth");

        FSSynthesizer_internalBufferSize =
            GPA(FSSynthesizer_SizeProc,
                "fluid_synth_get_internal_bufsize");
        FSSynthesizer_audioChannelPairCount =
            GPA(FSSynthesizer_SizeProc,
                "fluid_synth_count_audio_channels");
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
        FSSynthesizer_setInterpolationMethod =
            GPA(FSSynthesizer_SetInterpolationMethodProc,
                "fluid_synth_set_interp_method");

        _functionsAreInitialized = true;
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

    _descriptor = new _SynthesizerDescriptor();
    _SynthesizerDescriptor& descriptor =
        TOREFERENCE<_SynthesizerDescriptor>(_descriptor);
    
    if (!library->isLoaded()) {
        Logging_traceError("library or settings object is undefined");
    } else {
        descriptor.associatedLibrary =
            (FluidSynth*) library;
        _initializeFunctionsForLibrary(library->underlyingObject());

        if (FSSynthesizer_make == NULL) {
            _reportBadFunction("make");
        } else {
            descriptor.synthesizer =
                FSSynthesizer_make(settings->underlyingObject());
        }
    }
    
    Logging_trace("<<");
}

/*--------------------*/

FluidSynthSynthesizer::~FluidSynthSynthesizer ()
{
    Logging_trace(">>");

    if (_descriptor != NULL) {
        _SynthesizerDescriptor& descriptor =
            TOREFERENCE<_SynthesizerDescriptor>(_descriptor);

        if (descriptor.synthesizer != NULL) {
            if (FSSynthesizer_destroy == NULL) {
                _reportBadFunction("destroy");
            } else {
                FSSynthesizer_destroy(descriptor.synthesizer);
            }
        }

        delete &descriptor;
        _descriptor = NULL;
    }

    Logging_trace("<<");
}

/*--------------------*/
/* property access    */
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
        _SynthesizerDescriptor& descriptor =
            TOREFERENCE<_SynthesizerDescriptor>(_descriptor);
        result = FSSynthesizer_internalBufferSize(descriptor.synthesizer);
    }
    
    Logging_trace1("<<: %1", TOSTRING(result));
    return result;
}



/*--------------------*/

FluidSynth* FluidSynthSynthesizer::library () const
{
    FluidSynth* result = NULL;

    if (_descriptor == NULL) {
        Logging_traceError(_errorMessageForUndefinedDescriptor);
    } else {
        const _SynthesizerDescriptor& descriptor =
            TOREFERENCE<_SynthesizerDescriptor>(_descriptor);
        result = descriptor.associatedLibrary;
    }

    return result;
}

/*--------------------*/

Object FluidSynthSynthesizer::underlyingObject () const
{
    Object result = NULL;

    if (_descriptor == NULL) {
        Logging_traceError(_errorMessageForUndefinedDescriptor);
    } else {
        const _SynthesizerDescriptor& descriptor =
            TOREFERENCE<_SynthesizerDescriptor>(_descriptor);
        result = descriptor.synthesizer;
    }

    return result;
}
        
/*--------------------*/
/* change             */
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
        _SynthesizerDescriptor& descriptor =
            TOREFERENCE<_SynthesizerDescriptor>(_descriptor);
        Integer operationResult =
            FSSynthesizer_handleBankChange(descriptor.synthesizer,
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
        _SynthesizerDescriptor& descriptor =
            TOREFERENCE<_SynthesizerDescriptor>(_descriptor);
        Integer operationResult =
            FSSynthesizer_handleControlChange(descriptor.synthesizer,
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
        _SynthesizerDescriptor& descriptor =
            TOREFERENCE<_SynthesizerDescriptor>(_descriptor);
        Integer operationResult =
            FSSynthesizer_handleMonoTouch(descriptor.synthesizer,
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
        _SynthesizerDescriptor& descriptor =
            TOREFERENCE<_SynthesizerDescriptor>(_descriptor);
        Integer operationResult =
            FSSynthesizer_handleNoteOff(descriptor.synthesizer,
                                        (int) channel,
                                        (int) note);
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
        _SynthesizerDescriptor& descriptor =
            TOREFERENCE<_SynthesizerDescriptor>(_descriptor);
        Integer operationResult =
            FSSynthesizer_handleNoteOn(descriptor.synthesizer,
                                       (int) channel,
                                       (int) note,
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
        _SynthesizerDescriptor& descriptor =
            TOREFERENCE<_SynthesizerDescriptor>(_descriptor);
        Integer operationResult =
            FSSynthesizer_handlePitchBend(descriptor.synthesizer,
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
        _SynthesizerDescriptor& descriptor =
            TOREFERENCE<_SynthesizerDescriptor>(_descriptor);
        Integer operationResult =
            FSSynthesizer_handlePolyTouch(descriptor.synthesizer,
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
        _SynthesizerDescriptor& descriptor =
            TOREFERENCE<_SynthesizerDescriptor>(_descriptor);
        Integer operationResult =
            FSSynthesizer_handleProgramChange(descriptor.synthesizer,
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
        _SynthesizerDescriptor& descriptor =
            TOREFERENCE<_SynthesizerDescriptor>(_descriptor);
        int soundFontId =
            FSSynthesizer_loadSoundFont(descriptor.synthesizer,
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
    } else if (FSSynthesizer_audioChannelPairCount == NULL) {
        _reportBadFunction("audioChannelPairCount");
    } else {
        int i;
        _SynthesizerDescriptor& descriptor =
            TOREFERENCE<_SynthesizerDescriptor>(_descriptor);

        /* count of channels that FluidSynth can handle */
        const Natural fluidSynthChannelCount =
            2 * FSSynthesizer_audioChannelPairCount(descriptor.synthesizer);
        int effectiveChannelCount =
            (size_t) Natural::minimum(channelCount + channelCount % 2,
                                      fluidSynthChannelCount);
            
        /* provide a float buffer for FluidSynth with <sampleCount> frames */
        float** floatSampleBuffer =
            static_cast<float**>(makeLocalArray(float*,
                                                effectiveChannelCount));

        for (i = 0;  i < effectiveChannelCount;  i++) {
            floatSampleBuffer[i] = 
                static_cast<float*>(makeLocalArray(float, sampleCount));
            clearArray(floatSampleBuffer[i], sampleCount, 0.0f);
        };

        /* mix dry audio and effects into sample buffer */
        Integer operationResult =
            FSSynthesizer_process(descriptor.synthesizer,
                                  (int) sampleCount,
                                  effectiveChannelCount, floatSampleBuffer,
                                  effectiveChannelCount, floatSampleBuffer);

        /* provide an audio sample buffer with <sampleCount> frames */
        AudioSample** sampleList =
            static_cast<AudioSample**>(makeLocalArray(AudioSample*,
                                                      channelCount));

        effectiveChannelCount =
            (size_t) Natural::minimum(channelCount, effectiveChannelCount);

        /* copy the channel data from FluidSynth into <sampleBuffer> */
        for (i = 0;  i < effectiveChannelCount;  i++) {
            convertArray(sampleBuffer[i].asArray(position),
                         floatSampleBuffer[i], sampleCount);
        }

        /* clear remaining channels (if any) */
        while (i < (int) channelCount) {
            clearArray(sampleBuffer[i++].asArray(position),
                       sampleCount, AudioSample{0.0});
        }

        isOkay = (operationResult == 0);
    }

    Logging_trace1("<<: %1", TOSTRING(isOkay));
    return isOkay;
}

/*--------------------*/

Boolean
FluidSynthSynthesizer::setInterpolationMethod (IN Natural methodCode)
{
    Logging_trace1(">>: %1", TOSTRING(methodCode));
    
    Boolean isOkay = false;

    if (_descriptor == NULL) {
        Logging_traceError(_errorMessageForUndefinedDescriptor);
    } else if (FSSynthesizer_setInterpolationMethod == NULL) {
        _reportBadFunction("setInterpolationMethod");
    } else if (methodCode != 0 && methodCode != 1
               && methodCode != 4 && methodCode != 7) {
        Logging_traceError(_errorMessageForBadInterpolationMethodCode);
    } else {
        _SynthesizerDescriptor& descriptor =
            TOREFERENCE<_SynthesizerDescriptor>(_descriptor);
        Object synthesizer = descriptor.synthesizer;
        isOkay = true;

        for (Natural channel = 0;  channel < 16;  channel++) {
            Integer operationResult =
                FSSynthesizer_setInterpolationMethod(synthesizer,
                                                     (int) channel,
                                                     (int) methodCode);
            isOkay = isOkay && (operationResult == 0);
        }
    }

    Logging_trace1("<<: %1", TOSTRING(isOkay));
    return isOkay;
}
