/**
 * @file
 * The <C>AudioSampleListVector</C> body implements the communication
 * buffer structure for the block processing of audio effects; it is a
 * list of several audio sample lists containing the input samples and
 * finally the (processed) output samples <I>(this is the formal CPP
 * file used when not doing inlining in production code)</I>.
 *
 * @author Dr. Thomas Tensi
 * @date   2020-08
 */

/*=========*/
/* IMPORTS */
/*=========*/

#include "AudioSampleListVector.h"

/*====================*/

#ifdef DEBUG
    /* module implementation contains functions */
    #include "AudioSampleListVector.cpp-inc"
#endif
