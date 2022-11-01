/**
 * @file
 * The <C>AudioSampleFifoQueue</C> specification defines a fifo queue
 * for audio samples with popping off access to the front and
 * appending to the back of the queue.
 *
 * @author Dr. Thomas Tensi
 * @date   2022-09
 */

/*====================*/

#pragma once

/*=========*/
/* IMPORTS */
/*=========*/

#include "Natural.h"
#include "AudioSampleList.h"

/*--------------------*/

using Audio::AudioSampleList;

/*====================*/

namespace Audio {

    /**
     * A <C>AudioSampleFifoQueue</C> object is a fifo queue for audio
     * samples with popping off access to the front and appending to
     * the back of the queue.
     */
    struct AudioSampleFifoQueue {

        /**
         * Defines new fifo queue with arbitrary allocated
         * length.
         */
        AudioSampleFifoQueue ();

        /*--------------------*/

        /**
         * Defines new fifo queue with given <C>capacity</C>.
         *
         * @param[in] capacity  initial allocated length of fifo queue
         */
        AudioSampleFifoQueue (IN Natural capacity);

        /*--------------------*/

        /**
         * Destroys fifo queue
         */
        ~AudioSampleFifoQueue ();

        /*--------------------*/

        /**
         * Returns string representation of fifo sample queue.
         *
         * @return string representation
         */
        String toString() const;

        /*--------------------*/

        /**
         * Ensurce that capacity of fifo queue is at least
         * <C>capacity</C> entries; note that capacity is
         * automatically extended when it is too low, but each
         * prolongation might be costly.
         *
         * @param[in] capacity   new minimum capacity of samples in fifo
         *                       queue
         */
        void ensureCapacity (IN Natural capacity);

        /*--------------------*/

        /**
         * Returns current capacity of fifo queue
         *
         * @return  maximum count of samples in fifo queue
         */
        Natural capacity () const;

        /*--------------------*/

        /**
         * Returns effective length of fifo queue
         *
         * @return count of samples in fifo queue
         */
        Natural length () const;

        /*--------------------*/

        /**
         * Returns sample from fifo queue at <C>position</C>.
         *
         * @param[in] position  position in queue
         * @return  sample in fifo queue at position
         */
        const AudioSample& at (IN Natural position) const;

        /*--------------------*/

        /**
         * Returns sample from fifo queue at <C>position</C>.
         *
         * @param[in] position  position in queue
         * @return  sample in fifo queue at position
         */
        const AudioSample& operator[] (IN Natural position) const;

        /*--------------------*/

        /**
         * Returns first sample from fifo queue and removes it
         * from fifo queue decreasing length by one.
         *
         * @return  first sample in fifo queue to be removed
         */
        AudioSample pop ();

        /*--------------------*/

        /**
         * Gets <C>count</C> samples from front of fifo queue
         * into list <C>sampleList</C> starting at <C>position</C>.
         *
         * @param[in] sampleList  the target list for samples to be
         *                        taken from front of fifo queue
         * @param[in] position    position of first element in target
         * @param[in] count       number of samples to be copied from
         *                        fifo queue
         */
        void pop (INOUT AudioSampleList& sampleList,
                  IN Natural position,
                  IN Natural count);

        /*--------------------*/

        /**
         * Appends <C>sample</C> to fifo queue after last
         * position increasing its length by one.
         *
         * @param[in] sample  the samples to be appended as new last
         *                    elements of fifo queue
         */
        void append (IN AudioSample& sample);

        /*--------------------*/

        /**
         * Appends <C>count</C> samples from list <C>sampleList</C>
         * to end of fifo queue starting at <C>position</C>.
         *
         * @param[in] sampleList  the list of samples to be inserted
         *                        as new last elements of fifo queue
         * @param[in] position    position of first element to be
         *                        copied
         * @param[in] count       number of samples to transfer from
         *                        list
         */
        void append (IN AudioSampleList& sampleList,
                     IN Natural position,
                     IN Natural count);

        /*--------------------*/

        /**
         * Gets all elements in fifo queue in ordered form into
         * <C>elementArray</C>.  Assumes that capacity of target is
         * large enough.
         *
         * @param[out] elementArray  array of samples
         */
        void toArray (OUT AudioSample* elementArray) const;

        /*--------------------*/
        /*--------------------*/

        private:

            /** the allocated capacity of the fifo queue */
            Natural _capacity;

            /** the effective length of the fifo queue */
            Natural _length;

            /** the index of the first element in the fifo queue */
            Natural _firstIndex;

            /** the elements of the fifo queue as a sequence */
            GenericSequence<AudioSample> _data;

    };
}

/*============================================================*/

#ifndef DEBUG
    /* production code is inlined */
    #include "AudioSampleFifoQueue.cpp-inc"
#endif
