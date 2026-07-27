/**
 * @file
 * The <C>AudioDataPointListVector</C> body implements the
 * communication buffer structure for the block processing of audio
 * effects; it is a list of several audio data points lists containing
 * the input data points and finally the (processed) output data
 * points <I>(this is the formal CPP file used when not doing inlining
 * in production code)</I>.
 *
 * @author Dr. Thomas Tensi
 * @date   2020-08
 */

/*=========*/
/* IMPORTS */
/*=========*/

#include "AudioDataPointListVector.h"

/*====================*/

#ifdef DEBUG
    /* module implementation contains functions */
    #include "AudioDataPointListVector.cpp-inc"
#endif
