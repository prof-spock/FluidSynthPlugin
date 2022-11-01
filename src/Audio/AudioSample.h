/**
 * @file
 * The <C>AudioSample</C> specification and body provides the
 * definition of the audio sample (a real value).
 *
 * @author Dr. Thomas Tensi
 * @date   2020-08
 */

/*====================*/

#pragma once

/*=========*/
/* IMPORTS */
/*=========*/

#include "Real.h"

/*--------------------*/

using BaseTypes::Primitives::Real;
using BaseTypes::Primitives::String;

/*====================*/

namespace Audio {

    /**
     * An <C>AudioSample</C> represents a single audio sample (a 32bit
     * float)
     */
    typedef Real AudioSample;

    /*--------------------*/

    /**
     * Converts audio sample <C>sample</C> to string.
     *
     * @param[in] sample  audio sample to be converted to string
     * @return  string representation of sample
     */
    static String toString (IN AudioSample& sample) {
        return sample.toString();
    }

}
