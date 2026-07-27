/**
 * @file
 * The <C>AudioDataPointFifoQueue</C> body implements a fifo queue for
 * audio samples with popping off access to the front and appending to
 * the back of the queue <I>(this is the formal CPP file used when not
 * doing inlining in production code)</I>.
 *
 * @author Dr. Thomas Tensi
 * @date   2022-09
 */

/*=========*/
/* IMPORTS */
/*=========*/

#include "AudioDataPointFifoQueue.h"

/*====================*/

#ifdef DEBUG
    /* module implementation contains functions */
    #include "AudioDataPointFifoQueue.cpp-inc"
#endif
