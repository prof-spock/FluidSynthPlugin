/**
 * @file
 * The <C>AudioSampleList</C> specification defines sequences of
 * audio samples with zero-based arbitrary indexed access to positions
 * in the sequence.
 *
 * @author Dr. Thomas Tensi
 * @date   2020-08
 */

/*====================*/

#pragma once

/*=========*/
/* IMPORTS */
/*=========*/

#include "GenericSequence.h"
#include "AudioSample.h"

/*--------------------*/

using Audio::AudioSample;
using BaseTypes::Containers::GenericSequence;

/*====================*/

namespace Audio {

    /**
     * An <C>AudioSampleList</C> object is a sequence of audio
     * samples with arbitrary indexed access to positions in the
     * sequence.  Indexing starts at zero and is consecutive.  Lists
     * also allow duplicate elements.
     */
    struct AudioSampleList
        : public GenericSequence<AudioSample, AudioSample::toString>
    {

        /**
         * Returns printable representation of list.
         *
         * @return string representation of list
         */
        String toString () const;

        /*--------------------*/

        /**
         * Returns printable representation of <C>list</C>.
         *
         * @param[in] list  list to be converted to string
         * @return string representation of list
         */
        static String toString (IN AudioSampleList& list);

        /*--------------------*/

        /**
         * Sets all entries in sample list to zero starting at
         * <C>position</C> with a count of <C>count</C>.
         *
         * @param[in]  position  the index of the first element in
         *                       list to be reset
         * @param[in]  count     the number of elements in list to
         *                       be reset
         */
        void setToZero (IN Natural position = 0,
                        IN Natural count = Natural::maximumValue());

        /*--------------------*/

        /**
         * Appends samples from <C>other</C> with a count of
         * <C>count</C> to current sample list.
         *
         * @param[in] other  list to be appended to current list
         * @param[in] count  the number of samples to be appended;
         *                   if missing, all samples from other will
         *                   be used
         */
        void extend (IN AudioSampleList& other,
                     IN Natural count = Natural::maximumValue());

    };

}

/*============================================================*/

#ifndef DEBUG
    /* production code is inlined */
    #include "AudioSampleList.cpp-inc"
#endif
