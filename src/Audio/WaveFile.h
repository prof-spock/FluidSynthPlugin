/**
 * @file
 * The <C>WaveFile</C> specification defines an object for accessing
 * a RIFF wave file with several channels.  Currently only write
 * access is supported.
 *
 * @author Dr. Thomas Tensi
 * @date   2022-08
 */

/*====================*/

#pragma once

/*=========*/
/* IMPORTS */
/*=========*/

#include "AudioSampleListVector.h"
#include "Natural.h"
#include "Object.h"

/*--------------------*/

using Audio::AudioSampleListVector;
using BaseTypes::Primitives::Natural;
using BaseTypes::Primitives::Object;
using BaseTypes::Primitives::String;

/*====================*/

namespace Audio {

    /**
     * An object for accessing a RIFF wave file with several channels;
     * currently only write access is supported
     */
    struct WaveFile {

        /*--------------------*/
        /* con-/destruction   */
        /*--------------------*/

        /**
         * Constructs new wave file object with <C>name</C>.
         *
         * @param[in] name  file name of wave file
         */
        WaveFile (IN String& name);

        /*--------------------*/

        /**
         * Destroys wave file object.
         */
        ~WaveFile ();

        /*--------------------*/
        /* persistence        */
        /*--------------------*/

        /**
         * Writes to wave file with a sample rate of <C>sampleRate</C>
         * using <C>channelCount</C> channels having
         * <C>audioFrameCount</C> samples per channel taking data from
         * <C>sampleBuffer</C>; target format is characterized by
         * <C>typeCode</C> and <C>sampleWidthInBytes</C>.
         *
         * @param[in] sampleRate          sample rate to be stored
         *                                in file
         * @param[in] channelCount        number of channels in sample
         *                                buffer
         * @param[in] audioFrameCount     number of samples in each
         *                                channel of sample buffer
         * @param[in] typeCode            type code (must be "I" or "R")
         * @param[in] sampleWidthInBytes  number of bytes per sample
         *                                (must be 1, 2, 4 or 8)
         * @param[in] sampleBuffer        matrix of samples ordered by
         *                                channel
         */
        void write (IN Natural sampleRate,
                    IN Natural channelCount,
                    IN Natural audioFrameCount,
                    IN String typeCode,
                    IN Natural sampleWidthInBytes,
                    IN AudioSampleListVector& sampleBuffer);

        /*--------------------*/

        private:

            /** internal representation */
            Object _descriptor;

    };

}
