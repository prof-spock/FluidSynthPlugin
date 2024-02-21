/**
 * @file
 * The <C>MidiFile</C> specification defines the services of a
 * MIDI file; currently only file reading is supported.
 *
 * @author Dr. Thomas Tensi
 * @date   2022-08
 */

/*====================*/

#pragma once

/*=========*/
/* IMPORTS */
/*=========*/

#include "Natural.h"
#include "MidiEventList.h"

/*--------------------*/

using BaseTypes::Primitives::Natural;
using MIDI::MidiEventList;

/*====================*/

namespace MIDI {

    /**
     * A <C>MidiFile</C> object represents an external file with MIDI
     * events according to the MIDI specification; currently only
     * reading of such a file is supported
     */
    struct MidiFile {

        /*--------------------*/
        /* con-/destruction   */
        /*--------------------*/

        /**
         * Constructs a MIDI file named <C>name</C>
         *
         * @param[in] name  name of MIDI file
         */
        MidiFile (IN String& name);
        
        /*--------------------*/

        /**
         * Constructs new file from <C>otherFile</C>
         * (NOT AVAILABLE).
         *
         * @param[in] otherFile  midi file to be copied
         */
        MidiFile (IN MidiFile& otherFile) = delete;

        /*--------------------*/

        /**
         * Destroys MIDI file
         */
        ~MidiFile ();
        
        /*--------------------*/
        /* assignment         */
        /*--------------------*/

        /**
         * Assigns <C>otherFile</C> to current
         * (NOT AVAILABLE).
         *
         * @param[in] otherFile  midi file to be assigned
         */
        MidiFile& operator= (IN MidiFile& otherFile) = delete;

        /*--------------------*/
        /* access             */
        /*--------------------*/

        /**
         * Reads data from MIDI file and returns characteristics in
         * <C>fileType</C>, <C>timeDivision</C> and flattened list of
         * MIDI events in <C>midiEventList</C>.
         *
         * @param[out] fileType       type of MIDI file (0, 1, 2)
         * @param[out] timeDivision   time division (in milliseconds
         *                            per quarter)
         * @param[out] midiEventList  ordered list of MIDI events from
         *                            all tracks
         */
        void read (OUT Natural& fileType,
                   OUT Natural& timeDivision,
                   OUT MidiEventList& midiEventList);

        /*--------------------*/

        private:

            /** the internal data for the event */
            Object _descriptor;

    };

}
