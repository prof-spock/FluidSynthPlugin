/**
 * @file
 * The <C>FluidSynthPlugin_EditorSupport</C> specification defines
 * support services for the editor of the FluidSynthPlugin audio
 * processor.
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

/*====================*/

namespace Main::FluidSynthPlugin {

    /**
     * The <C>FluidSynthPlugin_EditorSupport</C> type defines support
     * services for the editor of the FluidSynthPlugin audio
     * processor.
     */
    struct FluidSynthPlugin_EditorSupport {

        /**
         * Shows dialog for file selection for SoundFont files
         * starting in <C>startDirectory</C> and updates
         * <C>fileName<C> if selected
         *
         * @param[in]  startDirectory  directory where selection is
         *                             started
         * @param[out] fileName        file name when selected
         * @return  information whether selected file name is valid
         */
        static
        Boolean selectFileByDialog (IN String& startDirectory,
                                    OUT String& fileName);

        /*--------------------*/

        /**
         * Shows dialog for preset selection for current SoundFont
         * with presets given by <C>presetList</C> and updates
         * <C>presetNumber<C> if selected
         *
         * @param[in]     presetList            list of programs with
         *                                      entries consisting of
         *                                      bank, program and name
         *                                      separated by tabulators
         * @param[inout]  presetIdentification  bank and program (when
         *                                      selected)
         * @param[inout]  eventProcessor        the underlying event
         *                                      processor
         * @return  information whether selected value is valid
         */
        static
        Boolean selectPresetByDialog
                    (IN StringList& presetList,
                     INOUT MidiPresetIdentification& presetIdentification,
                     INOUT FluidSynthPlugin_EventProcessor& eventProcessor);
        
        /*--------------------*/

        /**
         * Shows information dialog with build information and library
         * version of the underlying fluidsynth library
         *
         * @param[in] fluidSynthLibraryVersion  version of FluidSynth library
         */
        static
        void showInformationDialog (IN String& fluidSynthLibraryVersion);

    };
}
