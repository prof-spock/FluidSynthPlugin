/**
 * @file
 * The <C>AudioDataPoint</C> specification and body provides the
 * definition of the audio data point (a real value at some time
 * position).
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
     * An <C>AudioDataPoint</C> represents a single audio data point
     * (a real value)
     */
    typedef Real AudioDataPoint;

}
