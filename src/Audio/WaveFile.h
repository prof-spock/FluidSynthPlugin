/**
 * @file
 * The <C>WaveFile</C> specification defines an object for accessing a
 * RIFF wave file with several channels.  Read and write access is
 * supported.
 *
 * @author Dr. Thomas Tensi
 * @date   2022-08
 */

/*====================*/

#pragma once

/*=========*/
/* IMPORTS */
/*=========*/

#include "AudioDataPointListVector.h"
#include "Natural.h"
#include "Object.h"

/*--------------------*/

using Audio::AudioDataPointListVector;
using BaseTypes::Primitives::Natural;
using BaseTypes::Primitives::Object;
using BaseTypes::Primitives::String;

/*====================*/

namespace Audio {

    /**
     * The result of a read or write operation for some wave file
     */
    enum WaveFileOperationResult {
        /** everything okay */
        okay = 1,

        /** failed */
        failed
    };

    /*--------------------*/
    
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
         * Constructs new wave file from <C>otherWaveFile</C>.
         * (NOT AVAILABLE!)
         *
         * @param[in] otherWaveFile  wave file to be copied
         */
        WaveFile (IN WaveFile& otherWaveFile)= delete;

        /*--------------------*/

        /**
         * Destroys wave file object.
         */
        ~WaveFile ();

        /*--------------------*/
        /* assignment         */
        /*--------------------*/

        /**
         * Assigns current wave file from <C>otherWaveFile</C>
         * (NOT AVAILABLE!)
         *
         * @param[in] otherWaveFile  wave file to be assigned
         */
        WaveFile& operator= (IN WaveFile& otherWaveFile) = delete;

        /*--------------------*/
        /* persistence        */
        /*--------------------*/

        /**
         * Reads wave file returning its <C>dataPointRate</C>, the
         * number of channels in <C>channelCount</C> each having
         * <C>audioFrameCount</C> data points per channel writing data
         * into <C>dataPointBuffer</C>; source format is characterized
         * by <C>typeCode</C> and <C>dataPointWidthInBytes</C>.
         *
         * @param[out] dataPointRate          "sample rate" of file
         * @param[out] channelCount           number of channels in buffer
         * @param[out] audioFrameCount        number of data points in each
         *                                    channel of data point buffer
         * @param[out] typeCode               type code (must be "I" or "R")
         * @param[out] dataPointWidthInBytes  number of bytes per data
         *                                    point (must be 1, 2, 3, 4
         *                                    or 8)
         * @param[out] dataPointBuffer        matrix of samples ordered by
         *                                    channel
         * @result result of operation
         */
        WaveFileOperationResult read
                (OUT Natural& sampleRate,
                 OUT Natural& channelCount,
                 OUT Natural& audioFrameCount,
                 OUT String& typeCode,
                 OUT Natural& dataPointWidthInBytes,
                 OUT AudioDataPointListVector& dataPointBuffer);

        /*--------------------*/

        /**
         * Writes to wave file with a sample rate of <C>sampleRate</C>
         * using <C>channelCount</C> channels having
         * <C>audioFrameCount</C> samples per channel taking data from
         * <C>dataPointBuffer</C>; the format of wave file is
         * characterized by <C>typeCode</C> and
         * <C>dataPointWidthInBytes</C>.
         *
         * @param[in] sampleRate             sample rate to be stored
         *                                   in file
         * @param[in] channelCount           number of channels in sample
         *                                   buffer
         * @param[in] audioFrameCount        number of samples in each
         *                                   channel of sample buffer
         * @param[in] typeCode               type code (must be "I" or "R")
         * @param[in] dataPointWidthInBytes  number of bytes per sample
         *                                   (must be 1, 2, 4 or 8)
         * @param[in] dataPointBuffer        matrix of data points
         *                                   ordered by channel
         * @result result of operation
         */
        WaveFileOperationResult write
                (IN Natural sampleRate,
                 IN Natural channelCount,
                 IN Natural audioFrameCount,
                 IN String& typeCode,
                 IN Natural dataPointWidthInBytes,
                 IN AudioDataPointListVector& dataPointBuffer);

        /*--------------------*/

        private:

            /** internal representation */
            Object _descriptor;

    };

}

/*--------------------*/

/**
 * Returns the string representation of <C>operationResult</C>.
 *
 * @param operationResult wave file operation result
 * @return string representation
 */
#define WaveFileOperationResult_toString(operationResult) \
    String((short)(operationResult) == 1 ? "okay" \
           : "failed")
