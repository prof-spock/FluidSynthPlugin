/**
 * @file
 * The <C>AudioSampleFifoQueueVector</C> specification defines a list
 * of fifo queues for audio samples.
 *
 * @author Dr. Thomas Tensi
 * @date   2022-09
 */

/*====================*/

#pragma once

/*=========*/
/* IMPORTS */
/*=========*/

#include "AudioSampleFifoQueue.h"
#include "AudioSampleListVector.h"

/*--------------------*/

using Audio::AudioSampleFifoQueue;
using Audio::AudioSampleListVector;

/*====================*/

namespace Audio {

    /**
     * A <C>AudioSampleFifoQueueVector</C> object is a list of fifo queues
     * for audio samples with selection by index and pop and append
     * operations extended to sample list vectors.
     */
    struct AudioSampleFifoQueueVector
               : public GenericList<AudioSampleFifoQueue> {

        /**
         * Makes a list of fifo sample queues for <C>channelCount</C>
         * channels with arbitrary length; initial queue capacity is
         * <C>sampleQueueCapacity</C>
         *
         * @param[in] channelCount         number of channels in vector
         * @param[in] sampleQueueCapacity  initial capacity of queues
         *                                 (in samples)
         */
        AudioSampleFifoQueueVector
            (IN Natural channelCount = 0,
             IN Natural sampleQueueCapacity = 1000);

        /*--------------------*/

        /**
         * Returns string representation of fifo sample queue list;;
         * if <C>audioFrameCount</C> is set, only that number of audio
         * frames with sample data will be returned; if
         * <C>isGroupedByFrames</C> is set, then frames are grouped,
         * otherwise the list is by channels.
         *
         * @param[in] audioFrameCount    count of sample frames to be
         *                               returned
         * @param[in] isGroupedByFrames  information, whether grouping
         *                               is by frames instead of channels
         * @return string representation of fifo sample queue vector
         */
        String
        toString (IN Natural audioFrameCount = Natural::maximumValue(),
                  IN Boolean isGroupedByFrames = false) const;

        /*--------------------*/

        /**
         * Returns count of all queues
         *
         * @return count of queues
         */
        Natural queueCount () const;

        /*--------------------*/

        /**
         * Returns length of each fifo queue.
         *
         * @return  length of queues
         */
        Natural queueLength () const;

        /*--------------------*/

        /**
         * Sets count of all queues to <C>count</C>
         *
         * @param[in] count  the new count of queues
         */
        void setQueueCount (IN Natural count);

        /*--------------------*/

        /**
         * Ensures that capacity of all queues is at least
         * <C>capacity</C>
         *
         * @param[in] capacity  the new minimum capacity of each queue
         *                      in vector
         */
        void ensureQueueCapacity (IN Natural capacity);

        /*--------------------*/

        /**
         * Pops off <C>count</C> samples from front of corresponding
         * queues into each of the lists in <C>sampleListVector</C>
         * starting at <C>position</C>.
         *
         * @param[in] sampleListVector  vector of sample lists to be
         *                              filled from front of
         *                              corresponding fifo queues
         * @param[in] position          first position in each list
         *                              to be filled
         * @param[in] count             count of samples per channel
         *                              to be copied
         */
        void popFromQueues (INOUT AudioSampleListVector& sampleListVector,
                            IN Natural position,
                            IN Natural count);

        /*--------------------*/

        /**
         * Appends data from <C>sampleListVector</C> to corresponding
         * queues.
         *
         * @param[in] sampleListVector  vector of sample lists to be
         *                              appended to corresponding
         *                              queues
         * @param[in] position          first position in each list
         *                              to be copied
         * @param[in] count             count of samples per channel
         *                              to be copied
         */
        void appendToQueues (IN AudioSampleListVector& sampleListVector,
                             IN Natural position,
                             IN Natural count);

    };

}

/*============================================================*/

#ifndef DEBUG
    /* production code is inlined */
    #include "AudioSampleFifoQueueVector.cpp-inc"
#endif
