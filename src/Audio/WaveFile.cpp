/**
 * @file
 * The <C>WaveFile</C> specification defines an object for accessing
 * a RIFF wave file with several channels.  Currently only write
 * access is supported.
 *
 * @author Dr. Thomas Tensi
 * @date   2022-08
 */

/*=========*/
/* IMPORTS */
/*=========*/

#include "Assertion.h"
#include "Character.h"
#include "File.h"
#include "IntegerList.h"
#include "Logging.h"
#include "NaturalList.h"
#include "RealList.h"
#include "WaveFile.h"

/*--------------------*/

using Audio::AudioSample;
using Audio::AudioSampleList;
using Audio::WaveFile;
using BaseModules::File;
using BaseTypes::Containers::IntegerList;
using BaseTypes::Containers::NaturalList;
using BaseTypes::Containers::RealList;
using BaseTypes::Primitives::Character;
using BaseTypes::Primitives::Integer;

/*====================*/

static void WaveFile__writeInt (INOUT ByteList& byteList,
                                INOUT Natural& position,
                                IN Integer n,
                                IN Natural byteCount);

static void WaveFile__writeString (INOUT ByteList& byteList,
                                   INOUT Natural& position,
                                   IN String& st);

/*====================*/

/** an integer variable */
static const Integer _testInteger = 1;

/** tells whether this machine is big endian */
static const Boolean isBigEndian =
    (*((uint8_t*) &_testInteger) == 0);

/*====================*/

/**
 * Writes RIFF WAVE header to byte list <C>byteList</C> at
 * <C>position</C> using parameters <C>isPCMData</C> telling whether
 * this is a float or integer format, <C>dataSectionSize</C>,
 * <C>channelCount</C>, <C>sampleRate</C> and
 * <C>sampleWidthInBytes</C>.
 *
 * @param[inout] byteList            target byte list to be written to
 * @param[inout] position            first target position to be written
 *                                   to
 * @param[in]    isPCMData           information whether this is a
 *                                   format with integer samples
 * @param[in]    fileSize            the count in bytes of the complete
 *                                   file
 * @param[in]    channelCount        number of channels in sample buffer
 * @param[in]    audioFrameCount     number of audio frames in sample buffer
 * @param[in]    sampleRate          sample rate to be stored in file
 * @param[in]    sampleWidthInBytes  number of bytes per sample
 *                                   (must be 1, 2, 4 or 8)
 */
static void
WaveFile__writeHeader(INOUT ByteList& byteList,
                      INOUT Natural& position,
                      IN Boolean isPCMData,
                      IN Natural fileSize,
                      IN Natural channelCount,
                      IN Natural audioFrameCount,
                      IN Natural sampleRate,
                      IN Natural sampleWidthInBytes)
{
    Logging_trace7(">>: position = %1, isPCMData = %2, fileSize = %3,"
                   " channelCount = %4, audioFrameCount = %5,"
                   " sampleRate = %6, sampleWidthInBytes = %7",
                   TOSTRING(position), TOSTRING(isPCMData),
                   TOSTRING(fileSize), TOSTRING(channelCount),
                   TOSTRING(audioFrameCount), TOSTRING(sampleRate),
                   TOSTRING(sampleWidthInBytes));
    
    WaveFile__writeString(byteList, position, "RIFF");
    WaveFile__writeInt(byteList, position, fileSize - 8, 4);
    WaveFile__writeString(byteList, position, "WAVE");

    /* write data for the format data section */
    const Natural dataKind = (isPCMData ? 1 : 0xFFFE);
    const Natural extensionSize = 22;
    const Natural formatSectionSize =
        Natural{16} + (isPCMData ? 0 : extensionSize + 2);
    const Natural bitsPerSample = sampleWidthInBytes * 8;

    WaveFile__writeString(byteList, position, "fmt ");
    WaveFile__writeInt(byteList, position, formatSectionSize, 4);
    WaveFile__writeInt(byteList, position, dataKind, 2);
    WaveFile__writeInt(byteList, position, channelCount, 2);
    WaveFile__writeInt(byteList, position, sampleRate, 4);
    WaveFile__writeInt(byteList, position,
                       sampleRate * channelCount * sampleWidthInBytes,
                       4);
    WaveFile__writeInt(byteList, position,
                       channelCount * sampleWidthInBytes, 2);
    WaveFile__writeInt(byteList, position, bitsPerSample, 2);

    if (!isPCMData) {
        /* write extension data */
        Natural subformatPart;
        WaveFile__writeInt(byteList, position, extensionSize, 2);
        WaveFile__writeInt(byteList, position, bitsPerSample, 2);
        const Natural speakerPositionMask = 0;
        WaveFile__writeInt(byteList, position, speakerPositionMask, 4);
        subformatPart = (size_t) 0x00000003;
        WaveFile__writeInt(byteList, position, subformatPart, 4);
        subformatPart = (size_t) 0x00100000;
        WaveFile__writeInt(byteList, position, subformatPart, 4);
        subformatPart = (size_t) 0x0aa000080;
        WaveFile__writeInt(byteList, position, subformatPart, 4);
        subformatPart = (size_t) 0x0719b3800;
        WaveFile__writeInt(byteList, position, subformatPart, 4);
        
        /* write fact chunk */
        const Natural factSectionSize = 4;
        const Natural totalSampleCount =
            audioFrameCount * channelCount;
        WaveFile__writeString(byteList, position, "fact");
        WaveFile__writeInt(byteList, position, factSectionSize, 4);
        //WaveFile__writeInt(byteList, position, totalSampleCount, 4);
        WaveFile__writeInt(byteList, position, audioFrameCount, 4);
    }

    Logging_trace("<<");
}

/*--------------------*/

/**
 * Writes <C>n</C> to byte list <C>byteList</C> at <C>position</C> using
 * <C>byteCount</C> bytes in little-endian format and updates
 * <C>position</C>.
 *
 * @param[inout] byteList     target byte list to be written to
 * @param[inout] position   first target position to be written to
 * @param[in]    n          integer value to be written
 * @param[in]    byteCount  count of byte used for integer
 */
static void WaveFile__writeInt (INOUT ByteList& byteList,
                                INOUT Natural& position,
                                IN Integer n,
                                IN Natural byteCount)
{
    int v = (int) n;

    for (Natural i = 0;  i < byteCount;  i++) {
        byteList[position++] = ((int) v & 0xFF);
        v >>= 8;
    }
}

/*--------------------*/

/**
 * Writes data from <C>sampleBuffer</C> as integer data characterized
 * by <C></C>, <C></C>, <C></C> to <C>byteList</C> at <C>position</C>
 * and updates <C>position</C>.
 *
 * @param[inout] byteList            target byte list for sample data
 * @param[inout] position            first position in byte list to be
 *                                   written
 * @param[in]    sampleBuffer        sample buffer to be written to byte
 *                                   list
 * @param[in]    totalSampleCount    total number of samples in sample buffer
 * @param[in]    channelCount        number of channels in sample buffer
 * @param[in]    audioFrameCount     number of audio frames in sample buffer
 * @param[in]    sampleWidthInBytes  number of bytes per sample
 *                                   (must be 1, 2, 3 or 4)
 * @param[in]    scalingFactor       real factor to scale data from sample
 *                                   buffer
 */
static void
WaveFile__writeIntDataToByteList (INOUT ByteList& byteList,
                                  INOUT Natural& position,
                                  IN AudioSampleListVector& sampleBuffer,
                                  IN Natural totalSampleCount,
                                  IN Natural channelCount,
                                  IN Natural audioFrameCount,
                                  IN Natural sampleWidthInBytes,
                                  IN Real scalingFactor)
{
    Logging_trace6(">>: position = %1, totalSampleCount = %2,"
                   " channelCount = %3, audioFrameCount = %4,"
                   " sampleWidthInBytes = %5, scalingFactor = %6",
                   TOSTRING(position), TOSTRING(totalSampleCount),
                   TOSTRING(channelCount), TOSTRING(audioFrameCount),
                   TOSTRING(sampleWidthInBytes), TOSTRING(scalingFactor));

    IntegerList rawDataList{totalSampleCount};

    for (Natural channel = 0;  channel < channelCount;  channel++) {
        const AudioSampleList& sampleList = sampleBuffer[channel];
        Natural i = channel;

        for (Natural j = 0;  j < audioFrameCount;  j++) {
            const AudioSample sample = sampleList[j];
            Real r = (sample >= 1.0 ? scalingFactor - 1.0
                      : (sample < -1.0 ? -scalingFactor
                         : sample * scalingFactor));
            Integer s = (Integer) r;
            rawDataList[i] = s;
            i += channelCount;
        }
    }

    Logging_trace("--: copying raw data to byte list");

    for (Natural i = 0;  i < totalSampleCount;  i++) {
        WaveFile__writeInt(byteList, position, rawDataList[i],
                           sampleWidthInBytes);
    }

    Logging_trace1("<<: position = %1", TOSTRING(position));
}

/*--------------------*/

/**
 * Writes <C>r</C> to byte list <C>byteList</C> at <C>position</C>
 * using <C>byteCount</C> bytes in little-endian format and updates
 * <C>position</C>.
 *
 * @param[inout] byteList   target byte list to be written to
 * @param[inout] position   first target position to be written to
 * @param[in]    r          real value to be written
 * @param[in]    byteCount  count of byte used for real value
 */
static void WaveFile__writeReal (INOUT ByteList& byteList,
                                 INOUT Natural& position,
                                 IN Real r,
                                 IN Natural byteCount)
{
    float f = (float) r;
    double d = (double) r;
    Byte* sourcePtr = (byteCount == 4 ? (Byte*) &f : (Byte*) &d);
    Byte* targetPtr = &byteList[position];
    position += byteCount;

    if (isBigEndian) {
        sourcePtr += ((int) byteCount - 1);

        for (Natural i = 0; i < byteCount; i++) {
            *targetPtr++ = *sourcePtr--;
        }
    } else {
        for (Natural i = 0; i < byteCount; i++) {
            *targetPtr++ = *sourcePtr++;
        }
    }
}

/*--------------------*/

/**
 * Writes data from <C>sampleBuffer</C> as floating point data
 * characterized by <C></C>, <C></C>, <C></C> to <C>byteList</C> at
 * <C>position</C> and updates <C>position</C>.
 *
 * @param[inout] byteList            target byte list for sample data
 * @param[inout] position            first position in byte list to be
 *                                   written
 * @param[in]    sampleBuffer        sample buffer to be written to byte
 *                                   list
 * @param[in]    totalSampleCount    total number of samples in sample buffer
 * @param[in]    channelCount        number of channels in sample buffer
 * @param[in]    audioFrameCount     number of audio frames in sample buffer
 * @param[in]    sampleWidthInBytes  number of bytes per sample
 *                                   (must be 4 or 8)
 */
static void
WaveFile__writeRealDataToByteList (INOUT ByteList& byteList,
                                   INOUT Natural& position,
                                   IN AudioSampleListVector& sampleBuffer,
                                   IN Natural totalSampleCount,
                                   IN Natural channelCount,
                                   IN Natural audioFrameCount,
                                   IN Natural sampleWidthInBytes)
{
    Logging_trace5(">>: position = %1, totalSampleCount = %2,"
                   " channelCount = %3, audioFrameCount = %4,"
                   " sampleWidthInBytes = %5",
                   TOSTRING(position), TOSTRING(totalSampleCount),
                   TOSTRING(channelCount), TOSTRING(audioFrameCount),
                   TOSTRING(sampleWidthInBytes));

    RealList rawDataList{totalSampleCount};

    for (Natural channel = 0;  channel < channelCount;  channel++) {
        const AudioSampleList& sampleList = sampleBuffer[channel];
        Natural i = channel;

        for (Natural j = 0;  j < audioFrameCount;  j++) {
            const AudioSample sample = sampleList[j];
            rawDataList[i] = sample;
            i += channelCount;
        }
    }

    Logging_trace("--: copying raw data to byte list");

    for (Natural i = 0;  i < totalSampleCount;  i++) {
        WaveFile__writeReal(byteList, position, rawDataList[i],
                            sampleWidthInBytes);
    }

    Logging_trace1("<<: position = %1", TOSTRING(position));
}

/*--------------------*/

/**
 * Writes string <C>st</C> to byte list <C>byteList</C> at
 * <C>position</C> and updates <C>position</C>.
 *
 * @param[inout] byteList     target byte list to be written to
 * @param[inout] position   first target position to be written to
 * @param[in]    st         string value to be written
 */
static void WaveFile__writeString (INOUT ByteList& byteList,
                                   INOUT Natural& position,
                                   IN String& st)
{
    for (const Character& ch : st) {
        byteList[position++] = (char) ch;
    }
}

/*--------------------*/
/* con-/destruction   */
/*--------------------*/

WaveFile::WaveFile (IN String& name)
{
    Logging_trace1(">>: %1", name);
    _descriptor = new String(name);
    Logging_trace("<<");
}

/*--------------------*/

WaveFile::~WaveFile ()
{
    String& st = TOREFERENCE<String>(_descriptor);
    Logging_trace1(">>: %1", st);
    delete &st;
    Logging_trace("<<");
}

/*--------------------*/

void WaveFile::write (IN Natural sampleRate,
                      IN Natural channelCount,
                      IN Natural audioFrameCount,
                      IN String typeCode,
                      IN Natural sampleWidthInBytes,
                      IN AudioSampleListVector& sampleBuffer)
{
    Logging_trace5(">>: sampleRate = %1, channelCount = %2,"
                   " audioFrameCount = %3, typeCode = '%4',"
                   " sampleWidthInBytes = %5",
                   TOSTRING(sampleRate), TOSTRING(channelCount),
                   TOSTRING(audioFrameCount), typeCode,
                   TOSTRING(sampleWidthInBytes));

    /* assertion check */
    const NaturalList allowedIntSampleWidthList =
        NaturalList::fromList({1, 2, 3, 4});
    const NaturalList allowedRealSampleWidthList =
        NaturalList::fromList({4, 8});
    Assertion_pre((typeCode == "I" || typeCode == "R"),
                  "type code must be 'I' or 'R'");
    Assertion_pre((typeCode != "I"
                   || allowedIntSampleWidthList.contains(sampleWidthInBytes)),
                  "sample width for int must be 1, 2, 3 or 4");
    Assertion_pre((typeCode != "R"
                   || allowedRealSampleWidthList.contains(sampleWidthInBytes)),
                  "sample width for real must be 4 or 8");

    /* process */
    String& fileName = TOREFERENCE<String>(_descriptor);
    File waveFile;
    Boolean isOkay = waveFile.open(fileName, "wb");
    Assertion_check(isOkay,
                    StringUtil::expand("file must be writable: %1",
                                       fileName));

    const Boolean isPCMData = (typeCode == "I");
    const Natural headerSize = (isPCMData ? 44 : 68);
    const Natural factChunkSize = (isPCMData ? 0 : 12);
    const Natural totalSampleCount =
        channelCount * audioFrameCount;
    /* make an even section size */
    const Natural dataSectionSize =
        (totalSampleCount * sampleWidthInBytes
         + (totalSampleCount * sampleWidthInBytes) % 2);
    const Natural fileSize =
        headerSize + factChunkSize + dataSectionSize;

    ByteList byteList{fileSize};
    Natural position = 0;
    
    /* write RIFF file header */
    WaveFile__writeHeader(byteList, position,
                          isPCMData, fileSize, channelCount,
                          audioFrameCount, sampleRate, sampleWidthInBytes);

    /* write sample data section */
    WaveFile__writeString(byteList, position, "data");
    WaveFile__writeInt(byteList, position, dataSectionSize, 4);

    Logging_trace("--: copying sample buffer to raw data list");

    if (isPCMData) {
        const Real scalingFactor =
            Real::power(2.0, Real{sampleWidthInBytes * 8 - 1});
        WaveFile__writeIntDataToByteList(byteList, position,
                                         sampleBuffer,
                                         totalSampleCount,
                                         channelCount, audioFrameCount,
                                         sampleWidthInBytes, scalingFactor);
    } else {
        WaveFile__writeRealDataToByteList(byteList, position,
                                          sampleBuffer,
                                          totalSampleCount,
                                          channelCount, audioFrameCount,
                                          sampleWidthInBytes);
    }

    Logging_trace("--: writing byte list to file");
    waveFile.write(byteList, 0, fileSize);
    waveFile.close();

    Logging_trace("<<");
}
