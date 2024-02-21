/**
 * @file
 * The <C>MidiEventList</C> specification defines lists of MIDI
 * events with zero-based arbitrary indexed access to positions in the
 * list.
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
#include "GenericList.h"
#include "MidiEvent.h"

/*--------------------*/

using std::initializer_list;
using BaseTypes::GenericTypes::GenericList;
using MIDI::MidiEvent;

/*====================*/

namespace MIDI {

    /**
     * An <C>MidiEventList</C> object is a list of MIDI events
     * with arbitrary indexed access to positions in the list.
     * Indexing starts at zero and is consecutive.  Lists also allow
     * duplicate elements.
     */
    struct MidiEventList
        : public GenericList<MidiEvent,
                             MidiEvent::toString,
                             StringLiteral{"MidiEventList"}> {

        /*--------------------*/
        /* construction       */
        /*--------------------*/

        /**
         * Initializes list of MIDI events from an initializer list
         * of values.
         *
         * @param list  initializer list of MIDI events
         * @return  list with values in order given
         */
        static
        MidiEventList fromList (IN initializer_list<MidiEvent> list);

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
