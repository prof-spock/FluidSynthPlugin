/**
 * @file
 * The <C>FluidSynthSettings</C> class specifies an object-oriented
 * wrapper around an underlying fluidsynth settings object.
 *
 * @author Dr. Thomas Tensi
 * @date   2022-04
 */

/*====================*/

#pragma once

/*=========*/
/* IMPORTS */
/*=========*/

#include "FluidSynth.h"

/*--------------------*/

using Libraries::FluidSynth::FluidSynth;

/*====================*/

namespace Libraries::FluidSynth {

    /**
     * An object-oriented wrapper around an underlying fluidsynth
     * settings object
     */
    struct FluidSynthSettings {

        /**
         * Constructor for settings using an underlying fluidsynth
         * library object.
         *
         * @param [in] library  the fluidsynth library object 
         */
        FluidSynthSettings (IN FluidSynth* library);

        /*--------------------*/

        /** Settings destructor */
        ~FluidSynthSettings ();

        /*--------------------*/

        /**
         * Sets setting given by <C>key</C> to new value given by
         * <C>value</C>; the value type is adjusted internally.
         *
         * @param[in] key    settings name
         * @param[in] value  value for key as string
         * @return  information whether setting was successful
         */
        Boolean set (IN String& key, IN String& value);

        /*--------------------*/

        /**
         * Returns the underlying settings object.
         *
         * @return  underlying fluidsynth settings object
         */
        Object fsSettings () const;

        /*--------------------*/

        private:

            /** descriptor object with internal data */
            Object _descriptor;

    };

}
