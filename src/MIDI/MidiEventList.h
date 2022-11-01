/**
 * @file
 * The <C>MidiEventList</C> specification defines sequences of MIDI
 * events with zero-based arbitrary indexed access to positions in the
 * sequence.
 *
 * @author Dr. Thomas Tensi
 * @date   2022-04
 */

/*====================*/

#pragma once

/*=========*/
/* IMPORTS */
/*=========*/

#include <initializer_list>
#include "GenericSequence.h"
#include "MidiEvent.h"

/*--------------------*/

using std::initializer_list;
using BaseTypes::Containers::GenericSequence;
using MIDI::MidiEvent;

/*====================*/

namespace MIDI {

    /**
     * An <C>MidiEventList</C> object is a sequence of MIDI events
     * with arbitrary indexed access to positions in the sequence.
     * Indexing starts at zero and is consecutive.  Lists also allow
     * duplicate elements.
     */
    struct MidiEventList : public GenericSequence<MidiEvent,
                                                  MidiEvent::toString>
    {

        /*--------------------*/
        /* construction       */
        /*--------------------*/

        /**
         * Initializes sequence of MIDI events from an initializer list
         * of values.
         *
         * @param list  initializer list of MIDI events
         * @return  sequence with values in order given
         */
        static
        MidiEventList fromList (IN initializer_list<MidiEvent> list);

        /*--------------------*/
        /* conversion         */
        /*--------------------*/

        /**
         * Returns printable representation of list.
         *
         * @return string representation of list
         */
        String toString () const;

        /*--------------------*/
        /* change             */
        /*--------------------*/

        /**
         * Combines current list with <C>other</C> and keeps event
         * sorting order.
         *
         * @param[in] other  other MIDI event list to be combined
         */
        void merge (IN MidiEventList& other);

        /*--------------------*/

        /**
         * Shifts event times in list by <C>offset</C>.
         *
         * @param[in] offset  offset to be applied to event times in
         *                    list
         */
        void shiftEventTimes (IN Integer offset);

    };

}
