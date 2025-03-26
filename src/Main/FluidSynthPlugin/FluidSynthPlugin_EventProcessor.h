/**
 * @file
 * The <C>FluidSynthPlugin_Processor</C> specification defines the
 * handling of MIDI events from the DAW via JUCE having them processed
 * by the FluidSynth library.
 *
 * @author Dr. Thomas Tensi
 * @date   2022-04
 */

/*====================*/

#pragma once

/*=========*/
/* IMPORTS */
/*=========*/

#include "JuceHeaders.h"
#include "Object.h"
#include "MidiPresetIdentification.h"

/*--------------------*/

using BaseTypes::Primitives::Object;
using MIDI::MidiPresetIdentification;

/*====================*/

namespace Main::FluidSynthPlugin {

    /**
     * A simple plugin for processing MIDI events via the FluidSynth
     * library and return the resulting audio as a stereo stream.
     */
    struct FluidSynthPlugin_EventProcessor : public juce::AudioProcessor {

        /** Creates a fluidsynth MIDI processor */
        FluidSynthPlugin_EventProcessor ();

        /*--------------------*/

        /** Destroys a fluidsynth MIDI processor */
        ~FluidSynthPlugin_EventProcessor ();

        /*--------------------*/
        /* property queries   */
        /*--------------------*/

        /**
         * Tells whether this processor has an editor.
         *
         * @return  information whether this processor has an editor 
         */
        bool hasEditor () const override;

        /*--------------------*/

        /**
         * Tells whether this processor can handle double sample values.
         *
         * @return  information whether this processor can handle
         *          sample values with type double
         */
        bool supportsDoublePrecisionProcessing () const override;

        /*--------------------*/

        /**
         * Tells whether this processor accepts MIDI data (should be
         * <C>true</C>).
         *
         * @return  information whether this processor accepts MIDI
         *          data
         */
        bool acceptsMidi () const override;

        /*--------------------*/

        /**
         * Tells whether this processor produces MIDI data (should be
         * <C>false</C>).
         *
         * @return  information whether this processor produces MIDI
         *          data
         */
        bool producesMidi () const override;

        /*--------------------*/

        /**
         * Tells whether this processor is a MIDI effect i.e. it
         * transforms MIDI data (should be <C>false</C>)
         *
         * @return  information whether this processor is a MIDI effect
         */
        bool isMidiEffect () const override;

        /*--------------------*/

        /**
         * Returns the tail length of this effect (in seconds).
         *
         * @return  the length in seconds of a tail of this effect 
         */
        double getTailLengthSeconds () const override;

        /*--------------------*/

        /**
         * Returns the number of programs this effect supports; if a
         * host like VST3 sets the programs not by the MIDI stream,
         * but via setCurrentProgram this just returns 128 to support
         * that mechanism.
         *
         * @return the number of programs of this effect 
         */
        int getNumPrograms () override;

        /*--------------------*/

        /**
         * Returns the current program of this effect.
         *
         * @return  the current program of this effect 
         */
        int getCurrentProgram () override;

        /*--------------------*/

        /**
         * Returns the program name for this processor given by
         * <C>index</C>.
         *
         * @param[in] index  the index of the program of this processor
         * @return  name of program 
         */
        const juce::String getProgramName (int index) override;

        /*--------------------*/

        /**
         * Returns the name of this plugin for JUCE.
         *
         * @return  name of processor as a JUCE string 
         */
        const juce::String getName () const override;

        /*--------------------*/

        /**
         * Returns list of presets together with their bank and
         * program numbers with bank, program and name separated by
         * tabulators
         *
         * @return  list of strings each with bank, program and name
         *          separated by tabulators
         */
        StringList presetList () const;
        
        /*--------------------*/
        /* property change    */
        /*--------------------*/

        /**
         * Creates the associated editor for this processor (a very
         * simplistic editor with a simple UI).
         *
         * @return  processor editor 
         */
        juce::AudioProcessorEditor* createEditor() override;

        /*--------------------*/

        /**
         * Sets the current program of this effect by <C>index</C>.
         *
         * @param[in] index  the index of the current program to be set
         */
        void setCurrentProgram (int index) override;

        /*--------------------*/

        /**
         * Changes the program name for this processor given by
         * <C>index</C>.
         *
         * @param[in] index    the index of the program of this
         *                     processor
         * @param[in] newName  new name of program
         */
        void changeProgramName (int index,
                                const juce::String& newName) override;

        /*--------------------*/
        /* property access    */
        /*--------------------*/

        /**
         * Tells whether processor is in erroneous state.
         *
         * @return  information whether processor cannot work
         */
        Boolean isInErrorState () const;

        /*--------------------*/

        /**
         * Returns the version of the underlying fluidsynth library.
         *
         * @return  associated FluidSynth version
         */
        String fsLibraryVersion () const;

        /*--------------------*/

        /**
         * Returns the message string for current settings: either an
         * error information or an information about soundfont and
         * preset.
         *
         * @return current message string
         */
        String messageString () const;

        /*--------------------*/

        /**
         * Returns the active preset.
         *
         * @return  current preset in processor
         */
        MidiPresetIdentification preset () const;

        /*--------------------*/

        /**
         * Returns the current value of the settings string.
         *
         * @return current settings string
         */
        String settings () const;

        /*--------------------*/
        /* property change    */
        /*--------------------*/

        /**
         * Sets the active preset to <C>preset</C>.
         *
         * @param[in] preset  new value for preset
         */
        void setPreset (IN MidiPresetIdentification& preset);

        /*--------------------*/

        /**
         * Sets the settings string to <C>st</C>.
         *
         * @param[in] st  new value for settings string
         */
        void setSettings (IN String& st);

        /*--------------------*/
        /* event handling     */
        /*--------------------*/

        /**
         * Informs the processor to be prepared for playback.
         *
         * @param[in] sampleRate                      the sample rate to
         *                                            be used for playback
         * @param[in] maximumExpectedSamplesPerBlock  the number of samples
         *                                            per processing call 
         */
        void prepareToPlay (IN double sampleRate,
                            IN int maximumExpectedSamplesPerBlock)
            override;

        /*--------------------*/

        /**
         * Tells processor to release resources after playback.
         */
        void releaseResources () override;

        /*--------------------*/

        /**
         * Processes a block of double samples and MIDI information
         * with this audio processor.
         *
         * @param[inout] audioBuffer    combination of input (ignored
         *                              here) and output sample lists
         *                              in double format
         * @param[in]    midiEventList  list of midi messages to be
         *                              processed 
         */
        void processBlock (juce::AudioBuffer<double>& audioBuffer,
                           juce::MidiBuffer& midiEventList)
            override;

        /*--------------------*/

        /**
         * Processes a block of float samples and MIDI information
         * with this audio processor.
         *
         * @param[inout] audioBuffer    combination of input (ignored
         *                              here) and output sample lists
         *                              in float format
         * @param[in]    midiEventList  list of midi messages to be
         *                              processed 
         */
        void processBlock (juce::AudioBuffer<float>& audioBuffer,
                           juce::MidiBuffer& midiEventList)
            override;

        /*--------------------*/
        /* persistence        */
        /*--------------------*/

        /**
         * Gets data from processor and stores it in serialized form
         * in <C>destData</C>.
         *
         * @param[out] destData  JUCE memory block to be filled with
         *                       serialized form of plugin data
         */
        void getStateInformation (juce::MemoryBlock& destData)
            override;

        /*--------------------*/

        /**
         * Sets data for processor from serialized form in <C>data</C>
         * with length <C>sizeInBytes</C>.
         *
         * @param[in] data         byte list with serialized form of
         *                         plugin information
         * @param[in] sizeInBytes  length of serialized data 
         */
        void setStateInformation (const void* data, int sizeInBytes)
            override;

        /*--------------------*/

        private:

            /** the internal descriptor object storing all the data */
            Object _descriptor;

            /*--------------------*/

            JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(FluidSynthPlugin_EventProcessor)
    };
}
