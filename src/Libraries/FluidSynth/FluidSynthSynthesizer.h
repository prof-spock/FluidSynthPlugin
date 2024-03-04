/**
 * @file
 * The <C>FluidSynthSynthesizer</C> class specifies an object-oriented
 * wrapper around an underlying fluidsynth synthesizer object.
 *
 * @author Dr. Thomas Tensi
 * @date   2022-04
 */

/*====================*/

#pragma once

/*=========*/
/* IMPORTS */
/*=========*/

#include "AudioSampleListVector.h"
#include "Natural.h"
#include "Object.h"
#include "FluidSynthSettings.h"

/*--------------------*/

using Audio::AudioSampleListVector;
using BaseTypes::Primitives::Natural;
using Libraries::FluidSynth::FluidSynthSettings;

/*====================*/

namespace Libraries::FluidSynth {

    /**
     * An object-oriented wrapper around an underlying fluidsynth
     * synthesizer object.
     */
    struct FluidSynthSynthesizer {

        /**
         * Constructor for synthesizer using an underlying fluidsynth
         * <C>library</C> object and <C>settings</C> with
         * key-value-pairs.
         *
         * @param[in] library   the fluidsynth library object
         * @param[in] settings  the current settings from fluidsynth 
         */
        FluidSynthSynthesizer (IN FluidSynth* library,
                               IN FluidSynthSettings* settings);

        /*--------------------*/

        /**
         * Constructs new fluidsynth synthesizer from
         * <C>otherSynthesizer</C> (NOT AVAILABLE!)
         *
         * @param[in] otherSynthesizer  synthesizer to be copied
         */
        FluidSynthSynthesizer (IN FluidSynthSynthesizer& otherSynthesizer)
            = delete;

       /*--------------------*/

        /**
         * Destructor for a fluidsynth synthesizer
         */
        ~FluidSynthSynthesizer ();

        /*--------------------*/
        /* assignment         */
        /*--------------------*/

        /**
         * Assigns current synthesizer from <C>otherSynthesizer</C>
         * (NOT AVAILABLE!)
         *
         * @param[in] otherSynthesizer  synthesizer to be assigned
         */
        FluidSynthSynthesizer&
        operator= (IN FluidSynthSynthesizer& otherSynthesizer)
            = delete;

        /*--------------------*/
        /* queries            */
        /*--------------------*/

        /**
         * Returns internal buffer size of the synthesizer in samples.
         *
         * @return  internal buffer size [in samples]
         */
        Natural internalBufferSize () const;
        
        /*--------------------*/

        /**
         * Handles a bank-select event on <C>channel</C> setting
         * bank to <C>bankNumber</C>
         *
         * @param[in] channel     MIDI channel for this bank change
         * @param[in] bankNumber  new MIDI bank on channel 
         * @return  information whether bank change has been handled
         */
        Boolean handleBankChange (IN Natural channel,
                                  IN Natural bankNumber);

        /*--------------------*/

        /**
         * Handles a control-change event on <C>channel</C> for
         * <C>controller</C> with <C>value</C>
         *
         * @param[in] channel     MIDI channel of this control change
         * @param[in] controller  MIDI controller to be changed
         * @param[in] value       new value for controller
         * @return  information whether control change has been handled
         *
         */
        Boolean handleControlChange (IN Natural channel,
                                     IN Natural controller,
                                     IN Natural value);

        /*--------------------*/

        /**
         * Handles a mono-touch event on <C>channel</C> setting
         * touch intensity for <C>key</C> to <C>value</C>
         *
         * @param[in] channel  MIDI channel for this touch event
         * @param[in] key      key pressed
         * @param[in] value    new value for touch of key
         * @return  information whether mono touch has been handled
         */
        Boolean handleMonoTouch (IN Natural channel,
                                 IN Natural key,
                                 IN Natural value);

        /*--------------------*/

        /**
         * Handles a note-off event on <C>channel</C> for <C>note</C>
         *
         * @param[in] channel  MIDI channel of this note
         * @param[in] note     MIDI note to be stopped 
         * @return  information whether note off has been handled
         */
        Boolean handleNoteOff (IN Natural channel,
                               IN Natural note);

        /*--------------------*/

        /**
         * Handles a note-on event on <C>channel</C> for <C>note</C>
         * with <C>velocity</C>
         *
         * @param[in] channel   MIDI channel of this note
         * @param[in] note      MIDI note to be started 
         * @param[in] velocity  velocity of MIDI note
         * @return  information whether note on has been handled
         */
        Boolean handleNoteOn (IN Natural channel,
                              IN Natural note,
                              IN Natural velocity);

        /*--------------------*/

        /**
         * Handles a pitch-bend event on <C>channel</C> setting
         * pitch controller to <C>value</C>
         *
         * @param[in] channel  MIDI channel for this pitch bend
         * @param[in] value    new value for pitch bender
         * @return  information whether pitch bend has been handled
         */
        Boolean handlePitchBend (IN Natural channel,
                                 IN Natural value);

        /*--------------------*/

        /**
         * Handles a poly-touch event on <C>channel</C> setting global
         * intensity to <C>value</C>
         *
         * @param[in] channel  MIDI channel for this poly touch event
         * @param[in] value    new value for touch of key
         * @return  information whether poly touch has been handled
         */
        Boolean handlePolyTouch (IN Natural channel,
                                 IN Natural value);

        /*--------------------*/

        /**
         * Handles a program-change event on <C>channel</C> setting
         * program to <C>programNumber</C>
         *
         * @param[in] channel        MIDI channel for this program change
         * @param[in] programNumber  new MIDI program on channel 
         * @return  information whether program change has been handled
         */
        Boolean handleProgramChange (IN Natural channel,
                                     IN Natural programNumber);

        /*--------------------*/

        /**
         * Loads sound font given by <C>soundFontPath</C>.
         *
         * @param[in] soundFontPath  path to sound font
         * @return  information whether sound font has been loaded
         */
        Boolean loadSoundFont (IN String& soundFontPath);

        /*--------------------*/

        /**
         * Processes <C>sampleCount</C> samples and stores them in
         * buffer <C>sampleBuffer</C> starting at <C>position</C>.
         *
         * @param[inout] sampleBuffer  vector of audio sample lists
         * @param[in]    position      offset in buffer for the samples
         *                             returned
         * @param[in]    sampleCount   number of samples to return
         * @return  information whether processing has been done
         */
        Boolean process (INOUT AudioSampleListVector& sampleBuffer,
                         IN Natural position,
                         IN Natural sampleCount);

        /*--------------------*/

        protected:

            /** descriptor object with internal data */
            Object _descriptor;

    };

}
