/**
 * @file
 * The <C>MidiEventConverter</C> specification defines the handling of
 * MIDI events by feeding them to an underlying FluidSynth library.
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
#include "Dictionary.h"
#include "Real.h"
#include "Natural.h"
#include "MidiEventList.h"

/*--------------------*/

using Audio::AudioSampleListVector;

using BaseTypes::Containers::Dictionary;
using BaseTypes::Primitives::Natural;
using BaseTypes::Primitives::Object;
using BaseTypes::Primitives::Real;
using BaseTypes::Primitives::String;

using MIDI::MidiEventList;

/*====================*/

namespace MIDI {

    /**
     * A simple plugin for processing MIDI events via the FluidSynth
     * library and return the resulting audio as a stereo stream.
     */
    struct MidiEventConverter {

        /*--------------------*/
        /* con-/destruction   */
        /*--------------------*/

        /**
         * Creates a fluidsynth MIDI event converter
         */
        MidiEventConverter ();

        /*--------------------*/

        /**
         * Constructs new converter from <C>otherConverter</C>
         * (NOT AVAILABLE).
         *
         * @param[in] otherConverter  midi event converter to be copied
         */
        MidiEventConverter (IN MidiEventConverter& otherConverter)
            = delete;

        /*--------------------*/

        /** Destroys a fluidsynth MIDI event converter */
        ~MidiEventConverter ();

        /*--------------------*/
        /* assignment         */
        /*--------------------*/

        /**
         * Assigns <C>otherConverter</C> to current
         * (NOT AVAILABLE).
         *
         * @param[in] otherConverter  midi event converter to be
         *                            assigned
         */
        MidiEventConverter&
        operator= (IN MidiEventConverter& otherConverter)
            = delete;

        /*--------------------*/
        /* event handling     */
        /*--------------------*/

        /**
         * Informs the handler to be prepared for playback.
         *
         * @param[in] sampleRate                      the sample rate
         *                                            to be used for
         *                                            playback
         * @param[in] maximumExpectedSamplesPerBlock  the number of
         *                                            samples per
         *                                            processing call 
         */
        void prepareToPlay (IN Real sampleRate,
                            IN Natural maximumExpectedSamplesPerBlock);

        /*--------------------*/

        /**
         * Tells instrument to release resources after playback.
         */
        void releaseResources ();

        /*--------------------*/

        /**
         * Processes a list of MIDI events with this converter and
         * updates audio sample buffer assuming a stereo setup.
         *
         * @param[in]    midiEventList         list of MIDI messages
         *                                     to be processed
         * @param[inout] audioBuffer           vector of sample lists
         * @param[in]    sampleCountInChannel  relevant count of samples
         *                                     per channel
         */
        void processBlock (IN MidiEventList& midiEventList,
                           INOUT AudioSampleListVector& audioBuffer,
                           IN Natural sampleCountInChannel);

        /*--------------------*/
        /* property queries   */
        /*--------------------*/

        /**
         * Tells whether the underlying fluidsynth library has been
         * correctly loaded.
         *
         * @return  information about successful loading of FluidSynth
         */
        Boolean isCorrectlyInitialized () const;
        
        /*--------------------*/

        /**
         * Returns the underlying buffer size of the fluidsynth
         * synthesizer.
         *
         * @return  synthesizer buffer size (in samples)
         */
        Natural synthesizerBufferSize () const;

        /*--------------------*/
        /* property change    */
        /*--------------------*/

        /**
         * Sets setting of event converter given by <C>key</C> to new
         * value given by <C>value</C>
         *
         * @param[in] key    settings name
         * @param[in] value  value for key as string 
         * @return  information whether set operation has been successful
         */
        Boolean set (IN String& key, IN String& value);

        /*--------------------*/

        /**
         * Sets several settings of event converter from
         * <C>dictionary</C>
         *
         * @param[in] dictionary    list of key-value pairs defining
         *                          new setting
         * @return  information whether set operation has been successful
         */
        Boolean set (IN Dictionary& dictionary);

        /*--------------------*/
        /* persistence        */
        /*--------------------*/

        /**
         * Gets settings data from event converter.
         *
         * @return  string of settings data
         */
        String getStateInformation ();

        /*--------------------*/

        /**
         * Sets settings data for event converter from <C>st</C>.
         *
         * @param[in] st           serialized settings data
         */
        void setStateInformation (IN String& st);

        /*--------------------*/

        private:

            /** the internal descriptor object storing all the data */
            Object _descriptor;

    };
}
