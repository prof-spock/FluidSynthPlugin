/**
 * @file
 * The <C>FluidSynth</C> class specifies an object-oriented wrapper
 * around the fluidsynth library and also allows its dynamic loading.
 *
 * @author Dr. Thomas Tensi
 * @date   2022-04
 */

/*====================*/
  
#pragma once
  
/*=========*/
/* IMPORTS */
/*=========*/

#include "Natural.h"
#include "Object.h"
  
/*--------------------*/
  
using BaseTypes::Primitives::Object;
using BaseTypes::Primitives::String;
  
/*====================*/

namespace Libraries::FluidSynth {
  
    /**
     * An object-oriented wrapper around the fluidsynth library and
     * also allows its dynamic loading.
     */
    struct FluidSynth {

        /**
         * Constructs fluidsynth library dynamically
         */
        FluidSynth ();
  
        /*--------------------*/

        /**
         * Destroys fluidsynth object and unloads the underlying
         * library
         */
        ~FluidSynth ();
  
        /*--------------------*/

        /**
         * Returns the underlying library.
         *
         * @return  underlying fluidsynth library object
         */
        Object fsLibrary () const;
  
        /*--------------------*/
  
        private:

            /** descriptor object with internal data */
            Object _descriptor;
  
    };
    
}
