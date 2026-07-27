/**
 * @file
 * The <C>AudioDataPointList</C> specification defines lists of
 * audio samples with zero-based arbitrary indexed access to positions
 * in the list.
 *
 * @author Dr. Thomas Tensi
 * @date   2020-08
 */

/*====================*/

#pragma once

/*=========*/
/* IMPORTS */
/*=========*/

#include "GenericList.h"
#include "AudioDataPoint.h"

/*--------------------*/

using Audio::AudioDataPoint;
using BaseTypes::GenericTypes::GenericList;

/*====================*/

namespace Audio {

    /**
     * Returns name of audio data point list type
     *
     * @return type name
     */
    String _audioDataPointListTypeName ();
    
    /*--------------------*/

    /**
     * An <C>AudioDataPointList</C> object is a list of audio
     * data points with arbitrary indexed access to positions in the
     * list.  Indexing starts at zero and is consecutive.  Lists
     * also allow duplicate elements.
     */
    struct AudioDataPointList
        : public GenericList<AudioDataPoint,
                             AudioDataPoint::toString,
                             _audioDataPointListTypeName >
    {

        /*--------------------*/
        /* data change        */
        /*--------------------*/

        /**
         * Sets all entries in data point list to zero starting at
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
         * Appends data points from <C>other</C> with a count of
         * <C>count</C> to current dataPoint list.
         *
         * @param[in] other  list to be appended to current list
         * @param[in] count  the number of data points to be appended;
         *                   if missing, all data points from other
         *                   will be used
         */
        void extend (IN AudioDataPointList& other,
                     IN Natural count = Natural::maximumValue());

        /*--------------------*/

        /**
         * Returns slice of <C>list</C> starting at
         * <C>firstPosition</C> up to - but not including -
         * <C>lastPosition</C>; in Python style negative indices are
         * allowed signifying counting from the end
         *
         * @param[in] list           list to be sliced
         * @param[in] firstPosition  index position of first element
         *                           of result
         * @param[in] lastPosition   index position of first element
         *                           not included in result
         * @return  list value at index position
         */
        static AudioDataPointList
        slice (IN AudioDataPointList& list,
               IN Integer firstPosition = 0,
               IN Integer lastPosition = Integer::maximumValue());

    };

}

/*============================================================*/

#ifndef DEBUG
    /* production code is inlined */
    #include "AudioDataPointList.cpp-inc"
#endif
