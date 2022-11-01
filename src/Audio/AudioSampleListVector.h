/**
 * @file
 * The <C>AudioSampleListVector</C> specification defines the
 * communication buffer structure for the block processing of audio
 * effects; it is a list of audio sample lists containing the input
 * samples and finally the (processed) output samples
 *
 * @author Dr. Thomas Tensi
 * @date   2020-08
 */

/*====================*/

#pragma once

/*=========*/
/* IMPORTS */
/*=========*/

#include "AudioSampleList.h"

/*====================*/

namespace Audio {

    /**
     * A <C>AudioSampleListVector</C> object is a buffer of audio
     * sample lists containing the input samples and the (processed)
     * output samples serving as the communication buffer structure
     * for the block processing of audio effects.
     */
    struct AudioSampleListVector
        : public GenericSequence<AudioSampleList,
                                 AudioSampleList::toString> {

        /**
         * Returns printable representation of buffer.
         *
         * @param[in] frameCount         the number of frames to be
         *                               shown in representation
         * @param[in] isGroupedByFrames  information whether grouping
         *                               should be done by frames
         *                               instead of channels
         * @return string representation of buffer
         */
        String toString (IN Natural frameCount = Natural::maximumValue(),
                         IN Boolean isGroupedByFrames = true) const;

        /*--------------------*/

        /**
         * Returns number of frames (the samples in each channel).
         *
         * @return  number of frames in sample lists
         */
        Natural frameCount () const;

        /*--------------------*/

        /**
         * Sets number of frames (the samples in each channel) to
         * <C>newFrameCount</C>.
         *
         * @param[in] newFrameCount  new number of frames in sample
         *                           lists
         */
        void setFrameCount (IN Natural newFrameCount);

        /*--------------------*/

        /**
         * Sets all entries in sample buffer to zero starting at
         * <C>position</C> with a count of <C>count</C>.
         *
         * @param[in]  position  the index of the first element in
         *                       each list to be reset
         * @param[in]  count     the number of elements in each list
         *                       to be reset
         */
        void setToZero (IN Natural position = 0,
                        IN Natural count = Natural::maximumValue());

        /*--------------------*/

        /**
         * Appends frames from <C>other</C> with a count of
         * <C>frameCount</C> to current buffer.
         *
         * @param[in] other       buffer to be appended to current buffer
         * @param[in] frameCount  the number of frames to be appended;
         *                        if missing, all frames from other will
         *                        be used
         */
        void extend (IN AudioSampleListVector& other,
                     IN Natural frameCount = Natural::maximumValue());

    };

}

/*============================================================*/

#ifndef DEBUG
    /* production code is inlined */
    #include "AudioSampleListVector.cpp-inc"
#endif
