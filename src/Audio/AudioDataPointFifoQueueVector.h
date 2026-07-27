/**
 * @file
 * The <C>AudioDataPointFifoQueueVector</C> specification defines a
 * list of fifo queues for audio data points.
 *
 * @author Dr. Thomas Tensi
 * @date   2022-09
 */

/*====================*/

#pragma once

/*=========*/
/* IMPORTS */
/*=========*/

#include "AudioDataPointFifoQueue.h"
#include "AudioDataPointListVector.h"

/*--------------------*/

using Audio::AudioDataPointFifoQueue;
using Audio::AudioDataPointListVector;

/*====================*/

namespace Audio {

    /**
     * A <C>AudioDataPointFifoQueueVector</C> object is a list of fifo
     * queues for audio data points with selection by index and pop
     * and append operations extended to data point list vectors.
     */
    struct AudioDataPointFifoQueueVector
               : public GenericList<AudioDataPointFifoQueue> {

        /**
         * Makes a list of fifo data point queues for
         * <C>channelCount</C> channels with arbitrary length; initial
         * queue capacity is <C>dataPointQueueCapacity</C>
         *
         * @param[in] channelCount            number of channels in vector
         * @param[in] dataPointQueueCapacity  initial capacity of queues
         *                                    (in data points)
         */
        AudioDataPointFifoQueueVector
            (IN Natural channelCount = 0,
             IN Natural dataPointQueueCapacity = 1000);

        /*--------------------*/

        /**
         * Returns string representation of fifo data point queue list;;
         * if <C>audioFrameCount</C> is set, only that number of audio
         * frames with data points will be returned; if
         * <C>isGroupedByFrames</C> is set, then frames are grouped,
         * otherwise the list is by channels.
         *
         * @param[in] audioFrameCount    count of data point frames to be
         *                               returned
         * @param[in] isGroupedByFrames  information, whether grouping
         *                               is by frames instead of channels
         * @return string representation of fifo data point queue vector
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
         * Pops off <C>count</C> data points from front of
         * corresponding queues into each of the lists in
         * <C>sampleListVector</C> starting at <C>position</C>.
         *
         * @param[in] dataPointListVector  vector of data point lists
         *                                 to be filled from front of
         *                                 corresponding fifo queues
         * @param[in] position             first position in each list
         *                                 to be filled
         * @param[in] count                count of data points per
         *                                 channel to be copied
         */
        void
        popFromQueues (INOUT AudioDataPointListVector& dataPointListVector,
                       IN Natural position,
                       IN Natural count);

        /*--------------------*/

        /**
         * Appends data from <C>dataPointListVector</C> to corresponding
         * queues.
         *
         * @param[in] dataPointListVector  vector of data point lists
         *                                 to be appended to
         *                                 corresponding queues
         * @param[in] position             first position in each list
         *                                 to be copied
         * @param[in] count                count of data points per
         *                                 channel to be copied
         */
        void
        appendToQueues (IN AudioDataPointListVector& dataPointListVector,
                        IN Natural position,
                        IN Natural count);

    };

}

/*============================================================*/

#ifndef DEBUG
    /* production code is inlined */
    #include "AudioDataPointFifoQueueVector.cpp-inc"
#endif
