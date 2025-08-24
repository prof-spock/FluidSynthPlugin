/**
 * @file
 * The <C>MidiEvent</C> specification defines a single MIDI event and
 * the MIDI event kind enumeration.
 *
 * @author Dr. Thomas Tensi
 * @date   2022-04
 */

/*====================*/

#pragma once

/*=========*/
/* IMPORTS */
/*=========*/

#include "Boolean.h"
#include "Integer.h"
#include "ByteList.h"
#include "Object.h"

/*--------------------*/

using BaseTypes::Containers::ByteList;
using BaseTypes::Primitives::Boolean;
using BaseTypes::Primitives::Natural;
using BaseTypes::Primitives::Integer;
using BaseTypes::Primitives::Object;
using BaseTypes::Primitives::String;

/*====================*/

namespace MIDI {

    /**
     * the kind of a MIDI event
     */
    struct MidiEventKind {

        /*--------------------*/
        /* constant values    */
        /*--------------------*/

        /** the MIDI note off event kind */
        static MidiEventKind noteOff;

        /** the MIDI note on event kind */
        static MidiEventKind noteOn;

        /** the MIDI polyphonic touch event kind */
        static MidiEventKind polyphonicKeyPressure;

        /** the MIDI control change event kind */
        static MidiEventKind controlChange;

        /** the MIDI program change event kind */
        static MidiEventKind programChange;

        /** the MIDI monophonic touch event kind */
        static MidiEventKind channelPressure;

        /** the MIDI pitch bend event kind */
        static MidiEventKind pitchBend;

        /** the MIDI system exclusive event kind */
        static MidiEventKind systemExclusive;

        /** the MIDI meta event kind (bundling several variants) */
        static MidiEventKind meta;

        /*--------------------*/
        /* con-/destruction   */
        /*--------------------*/

        /**
         * Makes event kind from byte <C>b</C>
         *
         * @param[in] b  byte value to be converted into event kind
         */
        MidiEventKind (IN Byte b = '\xFF');

        /*--------------------*/

        /**
         * Returns whether <C>n</C> is a valid midi event byte
         *
         * @param[in] b  value to be checked for event kind range
         * @return  information whether <C>b</C> is acceptable
         */
        static Boolean isValid (IN Byte b);

        /*--------------------*/
        /* conversion         */
        /*--------------------*/

        /**
         * Returns string representation of event kind
         *
         * @return string representation
         */
        String toString () const;

        /*--------------------*/

        /**
         * Returns count of bytes for this event kind (including the
         * event byte itself); 0 tells that this event has variable
         * length data
         *
         * @return byte count for this event kind
         */
        Natural byteCount () const;
        
        /*--------------------*/
        /* comparison         */
        /*--------------------*/

        /**
         * Compares <C>self</C> with <C>other</C> for equality.
         *
         * @param[in] self   first event kind to be compared
         * @param[in] other  second event kind to be compared
         * @return  information, whether two objects are equal
         */
        friend Boolean operator == (IN MidiEventKind& self,
                                    IN MidiEventKind& other);
        
        /*--------------------*/

        /**
         * Compares <C>self</C> with <C>other</C> for inequality.
         *
         * @param[in] self   first event kind to be compared
         * @param[in] other  second event kind to be compared
         * @return information, whether two objects are not equal
         */
        friend Boolean operator != (IN MidiEventKind& self,
                                    IN MidiEventKind& other);
        
        /*--------------------*/

        private:

            /** internal representation */
            Byte _value;

    };

    /*====================*/

    /**
     * the kind of a MIDI meta event
     */
    struct MidiMetaEventKind {

        /*--------------------*/
        /* constant values    */
        /*--------------------*/

        /** the MIDI meta event kind for a sequence number */
        static MidiMetaEventKind sequenceNumber;

        /** the MIDI meta event kind for an embedded text */
        static MidiMetaEventKind text;

        /** the MIDI meta event kind for a copyright notive */
        static MidiMetaEventKind copyright;

        /** the MIDI meta event kind for a track name */
        static MidiMetaEventKind trackName;

        /** the MIDI meta event kind for an instrument name */
        static MidiMetaEventKind instrumentName;

        /** the MIDI meta event kind for an embedded lyrics */
        static MidiMetaEventKind lyric;

        /** the MIDI meta event kind for a marker in a sequence */
        static MidiMetaEventKind marker;

        /** the MIDI meta event kind for a cue point */
        static MidiMetaEventKind cuePoint;

        /** the MIDI meta event kind for the channel prefix */
        static MidiMetaEventKind channelPrefix;

        /** the MIDI meta event kind for the end of a track */
        static MidiMetaEventKind trackEnd;

        /** the MIDI meta event kind for a tempo change */
        static MidiMetaEventKind tempo;

        /** the MIDI meta event kind for SMPTE data */
        static MidiMetaEventKind smpteOffset;

        /** the MIDI meta event kind for the time signature */
        static MidiMetaEventKind timeSignature;

        /** the MIDI meta event kind for the key signature */
        static MidiMetaEventKind keySignature;

        /** the MIDI meta event kind for some sequencer meta data */
        static MidiMetaEventKind sequencerMeta;

        /*--------------------*/
        /* con-/destruction   */
        /*--------------------*/

        /**
         * Makes meta event kind from byte <C>b</C>
         *
         * @param[in] b  byte value to be converted into meta event kind
         */
        MidiMetaEventKind (IN Byte b);

        /*--------------------*/

        /**
         * Returns whether <C>b</C> is a valid midi meta event byte
         *
         * @param[in] b  byte value to be checked for meta event
         *               kind range
         * @return  information whether <C>b</C> is acceptable
         */
        static Boolean isValid (IN Byte b);

        /*--------------------*/
        /* conversion         */
        /*--------------------*/

        /**
         * Returns string representation of meta event kind
         *
         * @return string representation
         */
        String toString () const;

        /*--------------------*/

        /**
         * Returns count of bytes for this meta event kind (including
         * the event byte itself); 0 tells that this meta event has
         * variable length data
         *
         * @return byte count for this meta event kind
         */
        Natural byteCount () const;
        
        /*--------------------*/
        /* comparison         */
        /*--------------------*/

        /**
         * Compares current meta kind with <C>other</C>.
         *
         * @param[in] other  partner to be compared
         * @return  information, whether two objects are equal
         */
        Boolean operator == (IN MidiMetaEventKind& other) const;
        
        /*--------------------*/

        /**
         * Compares current meta kind with <C>other</C>.
         *
         * @param[in] other  partner to be compared
         * @return  information, whether two objects are not equal
         */
        Boolean operator != (IN MidiMetaEventKind& other) const;
        
        /*--------------------*/

        private:

            /** internal representation */
            Byte _value;

    };

    /*====================*/

    /**
     * An abstract MIDI event
     */
    struct MidiEvent {

        /*--------------------*/
        /* con-/destruction   */
        /*--------------------*/

        /**
         * Makes a new midi event from time and byte list of data
         *
         * @param[in] eventTime  the time of the event (in an
         *                       arbitrary unit, typically MIDI ticks)
         * @param[in] midiData   the list of midi data including
         *                       the event kind and meta event kind
         *                       bytes
         */
        MidiEvent (IN Integer eventTime = 0,
                   IN ByteList midiData = ByteList{});

        /*--------------------*/

        /**
         * Makes a new midi event from an existing one
         *
         * @param[in] event  event to clone
         */
        MidiEvent (IN MidiEvent& event);

        /*--------------------*/

        /**
         * Destroys midi event
         */
        ~MidiEvent ();
        
        /*--------------------*/
        /* assignment         */
        /*--------------------*/

        /**
         * Assigns current from <C>event</C>
         *
         * @param[in] event  midi event to assign
         */
        MidiEvent& operator= (IN MidiEvent& event);

        /*--------------------*/
        /* conversion         */
        /*--------------------*/

        /**
         * Returns string representation of event
         *
         * @return string representation
         */
        String toString () const;

        /*--------------------*/

        /**
         * Returns string representation of MIDI event <C>event</C>.
         *
         * @param[in] event  MIDI event to be converted to a string
         * @return  string representation
         */
        static String toString (IN MidiEvent& event)
        {
            return event.toString();
        }

        /*--------------------*/
        /* comparison         */
        /*--------------------*/

        /**
         * Compares current kind with <C>other</C>.
         *
         * @param[in] other  partner to be compared
         * @return  information, whether this objects is less
         */
        Boolean operator < (IN MidiEvent& other) const;
        
        /*--------------------*/
        /* property access    */
        /*--------------------*/

        /**
         * Return time of midi event.
         *
         * @return  time (in the unit defined)
         */
        Integer time () const;
        
        /*--------------------*/

        /**
         * Return list of raw data of the event.
         *
         * @return  byte list
         */
        ByteList rawData () const;

        /*--------------------*/

        /**
         * Gets byte indexed by <C>index</C> from MIDI event data;
         * exits when index is not allowed.
         *
         * @param[in] index    index into byte data (starting with 0)
         * @return  value at index
         */
        Byte getDataByte (IN Natural index) const;

        /*--------------------*/

        /**
         * Tells the event kind of this event.
         *
         * @return  event kind
         */
        MidiEventKind kind () const;

        /*--------------------*/

        /**
         * Tells the meta event kind of this event; exits when kind of
         * event is not "meta".
         *
         * @pre kind() == MidiEventKind::meta
         * @return  meta event kind
         */
        MidiMetaEventKind metaKind () const;

        /*--------------------*/

        /**
         * Returns channel of the event.
         *
         * @return  channel number (starting with 0)
         */
        Natural channel () const;
        
        /*--------------------*/
        /* property change    */
        /*--------------------*/

        /**
         * Sets time of event to <C>eventTime</C>.
         *
         * @param[in] eventTime  new time of event
         */
        void setTime (IN Integer eventTime);

        /*--------------------*/
        /*--------------------*/

        protected:

            /**
             * Gets byte indexed by <C>index</C> from MIDI event data
             * (without logging); exits when index is not allowed.
             *
             * @param[in] index    index into byte data (starting with 0)
             * @return  value at index
             */
            Byte _getDataByteNOLOG (IN Natural index) const;

        /*--------------------*/
        /*--------------------*/

        private:

            /** the internal data for the event */
            Object _descriptor;

    };

}
