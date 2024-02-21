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
         * Constructs new fluidsynth library from <C>otherLibrary</C>
         * (NOT AVAILABLE!)
         *
         * @param[in] otherLibrary  library to be copied
         */
        FluidSynth (IN FluidSynth& otherLibrary) = delete;

        /*--------------------*/

        /**
         * Destroys fluidsynth object and unloads the underlying
         * library
         */
        ~FluidSynth ();
  
        /*--------------------*/
        /* assignment         */
        /*--------------------*/

        /**
         * Assigns current library from <C>otherLibrary</C>
         * (NOT AVAILABLE!)
         *
         * @param[in] otherLibrary  library to be assigned
         */
        FluidSynth& operator= (IN FluidSynth& otherLibrary) = delete;

        /*--------------------*/
        /* queries            */
        /*--------------------*/

        /**
         * Returns whether library has been correctly loaded.
         *
         * @return  information whether library is okay
         */
        Boolean isLoaded () const;
  
        /*--------------------*/

        /**
         * Returns the underlying dynamic library object.
         *
         * @return  underlying dynamic library object
         */
        Object dynamicLibrary () const;
  
        /*--------------------*/
  
        private:

            /** descriptor object with internal data */
            Object _descriptor;
  
    };
    
}
