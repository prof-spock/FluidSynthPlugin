/**
 * @file
 * The <C>AudioDataPointFifoQueue</C> specification defines a fifo
 * queue for audio data points with popping off access to the front
 * and appending to the back of the queue.
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
#include "AudioDataPointList.h"

/*--------------------*/

using Audio::AudioDataPointList;

/*====================*/

namespace Audio {

    /**
     * A <C>AudioDataPointFifoQueue</C> object is a fifo queue for
     * audio data points with popping off access to the front and
     * appending to the back of the queue.
     */
    struct AudioDataPointFifoQueue {

        /**
         * Defines new fifo queue with arbitrary allocated
         * length.
         */
        AudioDataPointFifoQueue ();

        /*--------------------*/

        /**
         * Defines new fifo queue with given <C>capacity</C>.
         *
         * @param[in] capacity  initial allocated length of fifo queue
         */
        AudioDataPointFifoQueue (IN Natural capacity);

        /*--------------------*/

        /**
         * Destroys fifo queue
         */
        ~AudioDataPointFifoQueue ();

        /*--------------------*/

        /**
         * Returns string representation of fifo data point queue.
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
         * @param[in] capacity   new minimum capacity of data points
         *                       in fifo queue
         */
        void ensureCapacity (IN Natural capacity);

        /*--------------------*/

        /**
         * Returns current capacity of fifo queue
         *
         * @return  maximum count of data points in fifo queue
         */
        Natural capacity () const;

        /*--------------------*/

        /**
         * Returns effective length of fifo queue
         *
         * @return count of data points in fifo queue
         */
        Natural length () const;

        /*--------------------*/

        /**
         * Returns data points from fifo queue at <C>position</C>.
         *
         * @param[in] position  position in queue
         * @return  data point in fifo queue at position
         */
        const AudioDataPoint& at (IN Natural position) const;

        /*--------------------*/

        /**
         * Returns data point from fifo queue at <C>position</C>.
         *
         * @param[in] position  position in queue
         * @return  data point in fifo queue at position
         */
        const AudioDataPoint& operator[] (IN Natural position) const;

        /*--------------------*/

        /**
         * Returns first data point from fifo queue and removes it
         * from fifo queue decreasing length by one.
         *
         * @return  first data point in fifo queue to be removed
         */
        AudioDataPoint pop ();

        /*--------------------*/

        /**
         * Gets <C>count</C> data points from front of fifo queue
         * into list <C>dataPointList</C> starting at <C>position</C>.
         *
         * @param[in] dataPointList  the destination list for data
         *                           points to be taken from front of
         *                           fifo queue
         * @param[in] position       position of first element in
         *                           destination
         * @param[in] count          number of data points to be copied
         *                           from fifo queue
         */
        void pop (INOUT AudioDataPointList& dataPointList,
                  IN Natural position,
                  IN Natural count);

        /*--------------------*/

        /**
         * Appends <C>dataPoint</C> to fifo queue after last
         * position increasing its length by one.
         *
         * @param[in] dataPoint  the data point to be appended as new
         *                       last element of fifo queue
         */
        void append (IN AudioDataPoint& dataPoint);

        /*--------------------*/

        /**
         * Appends <C>count</C> data points from list
         * <C>dataPointList</C> to end of fifo queue starting at
         * <C>position</C>.
         *
         * @param[in] dataPointList  the list of data points to be
         *                           inserted as new last elements of
         *                           fifo queue
         * @param[in] position       position of first element to be
         *                           copied
         * @param[in] count          number of data points to be
         *                           transferred from list
         */
        void append (IN AudioDataPointList& dataPointList,
                     IN Natural position,
                     IN Natural count);

        /*--------------------*/

        /**
         * Gets all elements in fifo queue in ordered form into
         * <C>elementArray</C>.  Assumes that capacity of destination
         * is large enough.
         *
         * @param[out] elementArray  array of data points
         */
        void toArray (OUT AudioDataPoint* elementArray) const;

        /*--------------------*/
        /*--------------------*/

        private:

            /** the allocated capacity of the fifo queue */
            Natural _capacity;

            /** the effective length of the fifo queue */
            Natural _length;

            /** the index of the first element in the fifo queue */
            Natural _firstIndex;

            /** the elements of the fifo queue as a list */
            GenericList<AudioDataPoint, AudioDataPoint::toString> _data;

    };
}

/*============================================================*/

#ifndef DEBUG
    /* production code is inlined */
    #include "AudioDataPointFifoQueue.cpp-inc"
#endif
