/**
 * @file
 * The <C>FluidSynthSoundFont</C> class specifies an object-oriented
 * wrapper around an underlying fluidsynth soundfont object.
 *
 * @author Dr. Thomas Tensi
 * @date   2024-03
 */

/*====================*/

#pragma once

/*=========*/
/* IMPORTS */
/*=========*/

#include "FluidSynthSynthesizer.h"
#include "StringList.h"

/*--------------------*/

using BaseTypes::Containers::StringList;
using Libraries::FluidSynth::FluidSynthSynthesizer;

/*====================*/

namespace Libraries::FluidSynth {

    /**
     * An object-oriented wrapper around an underlying fluidsynth
     * soundfont object.
     */
    struct FluidSynthSoundFont {

        /*--------------------*/
        /* con-/destruction   */
        /*--------------------*/

        /**
         * Constructs soundfont from <C>synthesizer</C> object at
         * level <C>levelFromTop</C> on soundfont stack
         *
         * @param[in] synthesizer   the fluidsynth synthesizer object
         * @param[in] levelFromTop  the distance to the top of stack
         *                          for this soundfont
         * @return 
         */
        FluidSynthSoundFont (IN FluidSynthSynthesizer* synthesizer,
                             IN Natural levelFromTop = 0);

        /*--------------------*/

        /**
         * Constructs new fluidsynth soundfont from
         * <C>otherSoundFont</C> (NOT AVAILABLE!)
         *
         * @param[in] otherSoundFont  soundfont to be copied
         */
        FluidSynthSoundFont (IN FluidSynthSoundFont& otherSoundFont)
            = delete;

        /*--------------------*/

        /**
         * Destructor for a fluidsynth soundfont
         */
        ~FluidSynthSoundFont ();

        /*--------------------*/
        /* assignment         */
        /*--------------------*/

        /**
         * Assigns current soundfont from <C>otherSoundFont</C>
         * (NOT AVAILABLE!)
         *
         * @param[in] otherSoundFont  soundfont to be assigned
         */
        FluidSynthSoundFont&
        operator= (IN FluidSynthSoundFont& otherSoundFont)
            = delete;

        /*--------------------*/
        /* queries            */
        /*--------------------*/

        /**
         * Tells whether soundfont has a preset at <C>bankNumber</C>
         * and <C>programNumber</C>.
         *
         * @param[in] bankNumber     bank number of preset
         * @param[in] programNumber  program number of preset
         * @return  information whether preset exists at position
         */
        Boolean hasProgram (IN Natural bankNumber,
                            IN Natural programNumber) const;
        
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

        protected:

            /** descriptor object with internal data */
            Object _descriptor;

    };

}
