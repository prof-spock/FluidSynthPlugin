/**
 * @file
 * The <C>FluidSynthPlugin_Editor</C> specification implements the
 * editor for the processor of MIDI events from the DAW via JUCE to
 * the FluidSynth library.
 *
 * @author Dr. Thomas Tensi
 * @date   2022-04
 */

/*====================*/

#pragma once

/*=========*/
/* IMPORTS */
/*=========*/

#include "FluidSynthPlugin_EventProcessor.h"

/*--------------------*/

using Main::FluidSynthPlugin::FluidSynthPlugin_EventProcessor;

/*====================*/

namespace Main::FluidSynthPlugin {

    /**
     * A very simple editor for the FluidSynth plugin.
     */
    struct FluidSynthPlugin_Editor :
        public juce::AudioProcessorEditor,
               juce::FileDragAndDropTarget {

        /*-----------------------*/
        /* setup and destruction */
        /*-----------------------*/

        /**
         * Creates a very simple editor for the FluidSynth plugin.
         *
         * @param[inout] processor  the associated event processor
         */
        FluidSynthPlugin_Editor
            (INOUT FluidSynthPlugin_EventProcessor& processor);

        /*--------------------*/

        /** Destroys a FluidSynth plugin editor */
        ~FluidSynthPlugin_Editor() override;

        /*--------------------*/
        /* queries            */
        /*--------------------*/

        /**
         * Tells whether file names in <C>fileNameList</C> are
         * interesting for this plugin editor
         */
        bool isInterestedInFileDrag (const juce::StringArray &fileNameList)
            override;

        /*--------------------*/
        /* event handling     */
        /*--------------------*/

        /**
         * Tells that files with names in <C>fileNameList</C> are
         * dropped onto this plugin editor at (<C>x</C>,<C>y</C>)
         *
         * @param fileNameList  list of files dropped
         * @param x             x-position of drop
         * @param y             y-position of drop
         */
        void filesDropped (const juce::StringArray &fileNameList,
                           int x,
                           int y)
            override;

        /*--------------------*/

        /**
         * Paints the editor based on a graphics context.
         *
         * @param[inout] context  graphics context to be used for
         *                        painting this editor data
         */
        void paint (INOUT juce::Graphics& context) override;

        /*--------------------*/

        /**
         * Tells the editor that it has been resized.
         */
        void resized() override;

        /*--------------------*/

        /**
         * Rereads data of underlying processor.
         */
        void update ();

        /*--------------------*/

        private:

            /** the reference to an internal editor descriptor */
            Object _descriptor;

            JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(FluidSynthPlugin_Editor)
    };
}
