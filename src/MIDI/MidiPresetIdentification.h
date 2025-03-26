/**
 * @file
 * The <C>MidiPresetIdentification</C> specification defines the
 * identification of a MIDI preset as a tuple of bank and program
 * number encoded as a string.
 *
 * @author Dr. Thomas Tensi
 * @date   2024-04
 */

/*====================*/

#pragma once

/*=========*/
/* IMPORTS */
/*=========*/

#include "Boolean.h"
#include "Integer.h"

/*--------------------*/

using BaseTypes::Primitives::Boolean;
using BaseTypes::Primitives::Natural;
using BaseTypes::Primitives::String;

/*====================*/

namespace MIDI {

    /**
     * Represents the tuple of bank number and program number
     * characterizing a preset
     */
    struct MidiPresetIdentification {

        /*--------------------*/
        /* construction       */
        /*--------------------*/

        /**
         * Constructs a preset number from a string
         */
        MidiPresetIdentification (IN String& st);

        /*--------------------*/

        /**
         * Constructs a preset number from a bank and program number
         *
         * @param[in] bankNumber     bank number of preset
         * @param[in] programNumber  program number of preset
         */
        MidiPresetIdentification (IN Natural bankNumber = 0,
                                  IN Natural programNumber = 0);

        /*--------------------*/
        /* type conversion    */
        /*--------------------*/

        /**
         * Returns string representation of preset
         *
         * @return  string representation
         */
        String toString () const;

        /*--------------------*/
        /* measurement        */
        /*--------------------*/

        /**
         * Tells whether this preset has no contained information.
         *
         * @return  information whether this identification is empty
         */
        Boolean isEmpty () const;
        
        /*--------------------*/

        /**
         * Tells whether this preset has a valid structure
         *
         * @return  validity of preset number
         */
        Boolean isValid () const;
        
        /*--------------------*/
        /* change             */
        /*--------------------*/

        /**
         * Resets identification to undefined
         */
        void clear ();

        /*--------------------*/
        /* complex calculation */
        /*---------------------*/

        /**
         * Splits preset number string into <C>bankNumber</C> and
         * <C>programNumber</C>
         *
         * @param[out] bankNumber     resulting bank number
         * @param[out] programNumber  resulting program number
         * @return  information whether preset number is valid
         */
        Boolean split (OUT Natural& bankNumber,
                       OUT Natural& programNumber) const;

        /*--------------------*/
        /*--------------------*/

        private:

            /** the preset number string */
            String _data;
        
    };

}
