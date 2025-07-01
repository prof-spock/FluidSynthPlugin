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
#include "OperatingSystem.h"
#include "RealList.h"
#include "WaveFile.h"

/*--------------------*/

using Audio::AudioSample;
using Audio::AudioSampleList;
using Audio::WaveFile;
using Audio::WaveFileOperationResult;
using BaseModules::File;
using BaseModules::OperatingSystem;
using BaseTypes::Containers::IntegerList;
using BaseTypes::Containers::NaturalList;
using BaseTypes::Containers::RealList;
using BaseTypes::Primitives::Character;
using BaseTypes::Primitives::Integer;

/** abbreviation for StringUtil */
using STR = BaseModules::StringUtil;

/*----------------------*/
/* FORWARD DECLARATIONS */
/*----------------------*/

static Integer WaveFile__readInteger (IN ByteList& byteList,
                                      INOUT Natural& position,
                                      IN Natural byteCount);

static Natural WaveFile__readNatural (IN ByteList& byteList,
                                      INOUT Natural& position,
                                      IN Natural byteCount);

static void WaveFile__skipBytes (IN ByteList& byteList,
                                 INOUT Natural& position,
                                 IN Natural byteCount);

static void WaveFile__skipString (IN ByteList& byteList,
                                  INOUT Natural& position,
                                  IN String& reference);

static void WaveFile__writeInteger (INOUT ByteList& byteList,
                                    INOUT Natural& position,
                                    IN Natural byteCount,
                                    IN Integer n);

static void WaveFile__writeString (INOUT ByteList& byteList,
                                   INOUT Natural& position,
                                   IN String& st);

/*====================*/
/* PRIVATE FEATURES   */
/*====================*/

/** an integer variable */
static const Integer testInteger = 1;

/** tells whether this machine is big endian */
static const Boolean isBigEndian =
    (*((uint8_t*) & testInteger) == 0);

/** the allowed sample widths for integer samples */
static const NaturalList allowedIntSampleWidthList =
    NaturalList::fromList({1, 2, 3, 4});

/** the allowed sample widths for real samples */
static const NaturalList allowedRealSampleWidthList =
    NaturalList::fromList({4, 8});

/*--------------------*/
/* error messages     */
/*--------------------*/

/** error message when values are not identical */
static const String ErrMsg_nonIdenticalValues =
    "expected XX = %1 and XX in header = %2 should be identical";

/** error message when read buffer is empty */
static const String ErrMsg_bufferIsEmpty =
    "read buffer exhausted (%1 bytes)";

/*--------------------*/

/**
 * Does an assertion check for <C>byteList</C> being accessible
 * at <C>position</C> or just pointing to the element after list
 *
 * @param byteList  byte list to be accessed
 * @param position  index into byte list
 */
static void WaveFile__assertIndexInRange (IN ByteList& byteList,
                                          IN Natural position)
{
    Natural listLength = byteList.length();
    Assertion_check(position < listLength,
                    STR::expand(ErrMsg_bufferIsEmpty, TOSTRING(listLength)));
}

/*--------------------*/

/**
 * Does an assertion check for equality of <C>expectedValue</C> and
 * <C>value</C> with <C>kind</C> giving an indication about the
 * variable kind
 *
 * @param expectedValue  value expected
 * @param value          value found
 * @param kind           text describing the value kind
 */
static void WaveFile__assertEquality (IN Natural expectedValue,
                                      IN Natural value,
                                      IN String& kind)
{
    String st = ErrMsg_nonIdenticalValues;
    STR::replace(st, "XX", kind);
    Assertion_check(expectedValue == value,
                    STR::expand(st,
                                TOSTRING(expectedValue), TOSTRING(value)));
}

/*--------------------*/

/**
 * Reads RIFF WAVE header from byte list <C>byteList</C> at
 * <C>position</C> filling parameters <C>isPCMData</C> telling whether
 * this is a float or integer format, <C>dataSectionSize</C>,
 * <C>channelCount</C>, <C>sampleRate</C> and
 * <C>sampleWidthInBytes</C>.
 *
 * @param[in]    byteList            source byte list to be read from
 * @param[inout] position            first source position to be read from
 * @param[out]   isPCMData           information whether this is a
 *                                   format with integer samples
 * @param[out]   fileSize            the count in bytes of the complete
 *                                   file
 * @param[out]   channelCount        number of channels in sample buffer
 * @param[out]   sampleRate          sample rate within file
 * @param[out]   sampleWidthInBytes  number of bytes per sample
 *                                   (must be 1, 2, 4 or 8)
 */
static void
WaveFile__readHeader (IN ByteList& byteList,
                      INOUT Natural& position,
                      IN Natural expectedFileSize,
                      OUT Boolean& isPCMData,
                      OUT Natural& channelCount,
                      OUT Natural& sampleRate,
                      OUT Natural& sampleWidthInBytes)
{
    Logging_trace1(">>: position = %1", TOSTRING(position));

    Natural expectedBitsPerSample;
    const Natural extensionSize = 22;
    Natural fileSize;

    {
        /* read general data */
        WaveFile__skipString(byteList, position, "RIFF");
        fileSize = WaveFile__readNatural(byteList, position, 4) + 8;
        WaveFile__assertEquality(expectedFileSize, fileSize, "file size");
        WaveFile__skipString(byteList, position, "WAVE");

        /* read data of the format data section */
        WaveFile__skipString(byteList, position, "fmt ");
        const Natural formatSectionSize =
            WaveFile__readNatural(byteList, position, 4);
        Natural dataKind = WaveFile__readNatural(byteList, position, 2);
        isPCMData = (dataKind == 1);

        const Natural expectedFormatSectionSize =
            Natural{16} + (isPCMData ? 0 : extensionSize + 2);
        WaveFile__assertEquality(expectedFormatSectionSize,
                                 formatSectionSize, "format section size");
    
        channelCount = WaveFile__readNatural(byteList, position, 2);
        sampleRate = WaveFile__readNatural(byteList, position, 4);
        const Natural bytesPerSecond =
            WaveFile__readNatural(byteList, position, 4);
        sampleWidthInBytes = bytesPerSecond / (sampleRate * channelCount);

        const Natural bytesPerAudioFrame =
            WaveFile__readNatural(byteList, position, 2);
        String st = String("bytes per audio frame %1 times sample rate %2"
                           " should give bytes per second %3");
        Assertion_check(bytesPerAudioFrame * sampleRate == bytesPerSecond,
                        STR::expand(st, TOSTRING(bytesPerAudioFrame),
                                    TOSTRING(sampleRate),
                                    TOSTRING(bytesPerSecond)));

        expectedBitsPerSample = sampleWidthInBytes * 8;
        Natural bitsPerSample = WaveFile__readNatural(byteList, position, 2);
        WaveFile__assertEquality(expectedBitsPerSample, bitsPerSample,
                                 "bits per sample");
    }

    if (!isPCMData) {
        /* read extension data */
        Natural extensionSizeInHeader =
            WaveFile__readNatural(byteList, position, 2);
        WaveFile__assertEquality(extensionSize, extensionSizeInHeader,
                                 "extension size");

        Natural bitsPerSample = WaveFile__readNatural(byteList, position, 2);
        WaveFile__assertEquality(expectedBitsPerSample, bitsPerSample,
                                 "bits per sample");

        WaveFile__skipBytes(byteList, position, 20);

        /* read fact chunk */
        WaveFile__skipString(byteList, position, "fact");

        const Natural expectedFactSectionSize = 4;
        Natural factSectionSize =
            WaveFile__readNatural(byteList, position, 4);
        WaveFile__assertEquality(expectedFactSectionSize,
                                 factSectionSize, "fact section size");

        Natural audioFrameCount =
            WaveFile__readNatural(byteList, position, 4);
    }

    Logging_trace6("<<: position = %1, isPCMData = %2,"
                   " fileSize = %3, channelCount = %4,"
                   " sampleRate = %5, sampleWidthInBytes = %6",
                   TOSTRING(position), TOSTRING(isPCMData),
                   TOSTRING(fileSize), TOSTRING(channelCount),
                   TOSTRING(sampleRate), TOSTRING(sampleWidthInBytes));
}

/*--------------------*/

/**
 * Reads data <C>byteList</C> at <C>position</C> into
 * <C>sampleBuffer</C> as integer data characterized by
 * <C>totalSampleCount</C>, <C>channelCount</C>,
 * <C>audioFrameCount</C>, <C>sampleWidthInBytes</C> and
 * <C>scalingFactor</C> and updates <C>position</C>.
 *
 * @param[inout] byteList            source byte list for sample data
 * @param[inout] position            first position in byte list to be
 *                                   read from
 * @param[in]    totalSampleCount    total number of samples
 * @param[in]    channelCount        number of channels
 * @param[in]    audioFrameCount     number of audio frames
 * @param[in]    sampleWidthInBytes  number of bytes per sample
 *                                   (must be 1, 2, 3 or 4)
 * @param[in]    scalingFactor       real factor to scale data
 * @param[out]   sampleBuffer        sample buffer to be read into from
 *                                   byte list
 */
static void
WaveFile__readIntDataFromByteList (IN ByteList& byteList,
                                   INOUT Natural& position,
                                   IN Natural totalSampleCount,
                                   IN Natural channelCount,
                                   IN Natural audioFrameCount,
                                   IN Natural sampleWidthInBytes,
                                   IN Real scalingFactor,
                                   OUT AudioSampleListVector& sampleBuffer)
{
    Logging_trace6(">>: position = %1, totalSampleCount = %2,"
                   " channelCount = %3, audioFrameCount = %4,"
                   " sampleWidthInBytes = %5, scalingFactor = %6",
                   TOSTRING(position), TOSTRING(totalSampleCount),
                   TOSTRING(channelCount), TOSTRING(audioFrameCount),
                   TOSTRING(sampleWidthInBytes), TOSTRING(scalingFactor));

    Natural lastPosition =
        position + totalSampleCount * sampleWidthInBytes - 1;
    WaveFile__assertIndexInRange(byteList, lastPosition);

    IntegerList rawDataList{totalSampleCount};
    sampleBuffer.setLength(channelCount);
    sampleBuffer.setFrameCount(audioFrameCount);

    for (Natural i = 0;  i < totalSampleCount;  i++) {
        rawDataList[i] =
            WaveFile__readInteger(byteList, position, sampleWidthInBytes);
    }

    Logging_trace("--: copying raw data to sample buffer");

    for (Natural channel = 0;  channel < channelCount;  channel++) {
        AudioSampleList& sampleList = sampleBuffer[channel];
        Natural i = channel;

        for (Natural j = 0;  j < audioFrameCount;  j++) {
            const AudioSample sample = Real{rawDataList[i]} / scalingFactor;
            sampleList[j] = sample;
            i += channelCount;
        }
    }

    Logging_trace1("<<: position = %1", TOSTRING(position));
}

/*--------------------*/

/**
 * Reads <C>byteCount</C> bytes from byte list <C>byteList</C> at
 * <C>position</C>, updates <C>position</C> and returns integer in
 * little-endian format.
 *
 * @param[in]    byteList   source byte list to be read from
 * @param[inout] position   first source position to be read from
 * @param[in]    byteCount  count of byte used for natural
 * @return  little-endian integer value read
 */
static Integer WaveFile__readInteger (IN ByteList& byteList,
                                      INOUT Natural& position,
                                      IN Natural byteCount)
{
    Natural lastPosition = position + byteCount - 1;
    WaveFile__assertIndexInRange(byteList, lastPosition);

    int v = 0;
    Boolean isNegative;
    int mask = 0x80;

    for (Natural i = 0;  i < byteCount;  i++) {
        char b = (char) byteList[lastPosition - i];

        if (i == 0) {
            /* extract sign bit */
            isNegative = ((b & 0x80) > 0);
            b &= 0x7F;
        }

        mask = mask << 8;
        v = v << 8 | ((char) b & 0xFF);
    }

    if (isNegative) {
        v -= mask;
    }
    
    position = lastPosition + 1;
    return Integer{v};
}

/*--------------------*/

/**
 * Reads <C>byteCount</C> bytes from byte list <C>byteList</C> at
 * <C>position</C>, updates <C>position</C> and returns natural in
 * little-endian format.
 *
 * @param[in]    byteList   source byte list to be read from
 * @param[inout] position   first source position to be read from
 * @param[in]    byteCount  count of byte used for natural
 * @return  little-endian natural value read
 */
static Natural WaveFile__readNatural (IN ByteList& byteList,
                                      INOUT Natural& position,
                                      IN Natural byteCount)
{
    Natural lastPosition = position + byteCount - 1;
    WaveFile__assertIndexInRange(byteList, lastPosition);

    int v = 0;

    for (Natural i = 0;  i < byteCount;  i++) {
        Byte b = byteList[lastPosition - i];
        v = v << 8 | ((char) b & 0xFF);
    }

    position = lastPosition + 1;
    return Natural{v};
}

/*--------------------*/

/**
 * Reads as real value from byte list <C>byteList</C> at
 * <C>position</C> using <C>byteCount</C> bytes in little-endian
 * format and updates <C>position</C>.
 *
 * @param[inout] byteList   source byte list to be read from
 * @param[inout] position   first source position to be read from
 * @param[in]    byteCount  count of byte used for real value
 * @return  real value
 */
static Real WaveFile__readReal (IN ByteList& byteList,
                                INOUT Natural& position,
                                IN Natural byteCount)
{
    Natural lastPosition = position + byteCount - 1;
    WaveFile__assertIndexInRange(byteList, lastPosition);

    float f = 0;
    double d = 0;
    Boolean isDouble = (byteCount == 8);
    Byte* sourcePtr = &((ByteList) byteList)[position];
    Byte* destinationPtr = (isDouble ? (Byte*) &d : (Byte*) &f);

    if (isBigEndian) {
        sourcePtr += ((int) byteCount - 1);

        for (Natural i = 0; i < byteCount; i++) {
            *destinationPtr++ = *sourcePtr--;
        }
    } else {
        for (Natural i = 0; i < byteCount; i++) {
            *destinationPtr++ = *sourcePtr++;
        }
    }

    position = lastPosition + 1;
    return (isDouble ? Real{d} : Real{f});
}

/*--------------------*/

/**
 * Reads data <C>byteList</C> at <C>position</C> into
 * <C>sampleBuffer</C> as floating point data characterized by
 * <C>totalSampleCount</C>, <C>channelCount</C>,
 * <C>audioFrameCount</C> and <C>sampleWidthInBytes</C> updates
 * <C>position</C>.
 *
 * @param[inout] byteList            source byte list for sample data
 * @param[inout] position            first position in byte list to be
 *                                   read from
 * @param[in]    totalSampleCount    total number of samples
 * @param[in]    channelCount        number of channels
 * @param[in]    audioFrameCount     number of audio frames
 * @param[in]    sampleWidthInBytes  number of bytes per sample
 *                                   (must be 1, 2, 3 or 4)
 * @param[out]   sampleBuffer        sample buffer to be read into from
 *                                   byte list
 */
static void
WaveFile__readRealDataFromByteList (IN ByteList& byteList,
                                    INOUT Natural& position,
                                    IN Natural totalSampleCount,
                                    IN Natural channelCount,
                                    IN Natural audioFrameCount,
                                    IN Natural sampleWidthInBytes,
                                    OUT AudioSampleListVector& sampleBuffer)
{
    Logging_trace5(">>: position = %1, totalSampleCount = %2,"
                   " channelCount = %3, audioFrameCount = %4,"
                   " sampleWidthInBytes = %5",
                   TOSTRING(position), TOSTRING(totalSampleCount),
                   TOSTRING(channelCount), TOSTRING(audioFrameCount),
                   TOSTRING(sampleWidthInBytes));

    Natural lastPosition =
        position + totalSampleCount * sampleWidthInBytes - 1;
    WaveFile__assertIndexInRange(byteList, lastPosition);

    RealList rawDataList{totalSampleCount};
    sampleBuffer.setLength(channelCount);
    sampleBuffer.setFrameCount(audioFrameCount);

    for (Natural i = 0;  i < totalSampleCount;  i++) {
        rawDataList[i] =
            WaveFile__readReal(byteList, position, sampleWidthInBytes);
    }

    Logging_trace("--: copying raw data to sample buffer");

    for (Natural channel = 0;  channel < channelCount;  channel++) {
        AudioSampleList& sampleList = sampleBuffer[channel];
        Natural i = channel;

        for (Natural j = 0;  j < audioFrameCount;  j++) {
            const AudioSample sample = rawDataList[i];
            sampleList[j] = sample;
            i += channelCount;
        }
    }

    Logging_trace1("<<: position = %1", TOSTRING(position));
}

/*--------------------*/

/**
 * Reads <C>byteCount</C> bytes from byte list <C>byteList</C> at
 * <C>position</C> and updates <C>position</C> without returning
 * anything.
 *
 * @param[in]    byteList   source byte list to be read from
 * @param[inout] position   first source position to be read from
 * @param[in]    byteCount  count of bytes skipped
 */

static void WaveFile__skipBytes (IN ByteList& byteList,
                                 INOUT Natural& position,
                                 IN Natural byteCount)
{
    Natural lastPosition = position + byteCount - 1;
    WaveFile__assertIndexInRange(byteList, lastPosition);
    position = lastPosition + 1;
}

/*--------------------*/

/**
 * Reads bytes from byte list <C>byteList</C> at <C>position</C>,
 * updates <C>position</C> and checks whether bytes match
 * <C>reference</C>.
 *
 * @param[in]    byteList   source byte list to be read from
 * @param[inout] position   first source position to be read from
 * @param[in]    reference  string reference to be matched
 */
static void WaveFile__skipString (IN ByteList& byteList,
                                  INOUT Natural& position,
                                  IN String& reference)
{
    Natural count = reference.size();
    Natural lastPosition = position + count - 1;
    WaveFile__assertIndexInRange(byteList, lastPosition);

    String st = "";

    for (Natural i = 0;  i < count;  i++) {
        Byte b = byteList[position + i];
        st += (char) b;
    }

    position = lastPosition + 1;
    Assertion_post(st == reference,
                   STR::expand("string '%1' at %2 must be identical to '%3'",
                               st, TOSTRING(position), reference));
}

/*--------------------*/
/* WRITE OPERATIONS   */
/*--------------------*/

/**
 * Writes RIFF WAVE header to byte list <C>byteList</C> at
 * <C>position</C> using parameters <C>isPCMData</C> telling whether
 * this is a float or integer format, <C>dataSectionSize</C>,
 * <C>channelCount</C>, <C>sampleRate</C> and
 * <C>sampleWidthInBytes</C>.
 *
 * @param[inout] byteList            destination byte list to be
 *                                   written to
 * @param[inout] position            first destination position to be
 *                                   written to
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
    WaveFile__writeInteger(byteList, position, 4, fileSize - 8);
    WaveFile__writeString(byteList, position, "WAVE");

    /* write data for the format data section */
    const Natural dataKind = (isPCMData ? 1 : 0xFFFE);
    const Natural extensionSize = 22;
    const Natural formatSectionSize =
        Natural{16} + (isPCMData ? 0 : extensionSize + 2);
    const Natural bitsPerSample = sampleWidthInBytes * 8;

    WaveFile__writeString(byteList, position, "fmt ");
    WaveFile__writeInteger(byteList, position, 4, formatSectionSize);
    WaveFile__writeInteger(byteList, position, 2, dataKind);
    WaveFile__writeInteger(byteList, position, 2, channelCount);
    WaveFile__writeInteger(byteList, position, 4, sampleRate);
    WaveFile__writeInteger(byteList, position, 4,
                           sampleRate * channelCount * sampleWidthInBytes);
    WaveFile__writeInteger(byteList, position, 2,
                           channelCount * sampleWidthInBytes);
    WaveFile__writeInteger(byteList, position, 2, bitsPerSample);

    if (!isPCMData) {
        /* write extension data */
        Natural subformatPart;
        WaveFile__writeInteger(byteList, position, 2, extensionSize);
        WaveFile__writeInteger(byteList, position, 2, bitsPerSample);
        const Natural speakerPositionMask = 0;
        WaveFile__writeInteger(byteList, position, 4, speakerPositionMask);
        subformatPart = (size_t) 0x00000003;
        WaveFile__writeInteger(byteList, position, 4, subformatPart);
        subformatPart = (size_t) 0x00100000;
        WaveFile__writeInteger(byteList, position, 4, subformatPart);
        subformatPart = (size_t) 0x0aa000080;
        WaveFile__writeInteger(byteList, position, 4, subformatPart);
        subformatPart = (size_t) 0x0719b3800;
        WaveFile__writeInteger(byteList, position, 4, subformatPart);
        
        /* write fact chunk */
        const Natural factSectionSize = 4;
        WaveFile__writeString(byteList, position, "fact");
        WaveFile__writeInteger(byteList, position, 4, factSectionSize);
        WaveFile__writeInteger(byteList, position, 4, audioFrameCount);
    }

    Logging_trace("<<");
}

/*--------------------*/

/**
 * Writes <C>n</C> to byte list <C>byteList</C> at <C>position</C> using
 * <C>byteCount</C> bytes in little-endian format and updates
 * <C>position</C>.
 *
 * @param[inout] byteList   destination byte list to be written to
 * @param[inout] position   first destination position to be written to
 * @param[in]    byteCount  count of byte used for integer
 * @param[in]    n          integer value to be written
 */
static void WaveFile__writeInteger (INOUT ByteList& byteList,
                                    INOUT Natural& position,
                                    IN Natural byteCount,
                                    IN Integer n)
{
    int v = (int) n;

    for (Natural i = 0;  i < byteCount;  i++) {
        byteList[position++] = ((int) v & 0xFF);
        v >>= 8;
    }
}

/*--------------------*/

/**
 * Writes data from <C>sampleBuffer</C> to <C>byteList</C> at
 * <C>position</C> as integer data characterized by
 * <C>totalSampleCount</C>, <C>channelCount</C>,
 * <C>audioFrameCount</C>, <C>sampleWidthInBytes</C> and
 * <C>scalingFactor</C> and updates <C>position</C>.
 *
 * @param[inout] byteList            destination byte list for sample data
 * @param[inout] position            first position in byte list to be
 *                                   written
 * @param[in]    totalSampleCount    total number of samples in sample buffer
 * @param[in]    channelCount        number of channels in sample buffer
 * @param[in]    audioFrameCount     number of audio frames in sample buffer
 * @param[in]    sampleWidthInBytes  number of bytes per sample
 *                                   (must be 1, 2, 3 or 4)
 * @param[in]    scalingFactor       real factor to scale data from sample
 *                                   buffer
 * @param[in]    sampleBuffer        sample buffer to be written to byte
 *                                   list
 */
static void
WaveFile__writeIntDataToByteList (INOUT ByteList& byteList,
                                  INOUT Natural& position,
                                  IN Natural totalSampleCount,
                                  IN Natural channelCount,
                                  IN Natural audioFrameCount,
                                  IN Natural sampleWidthInBytes,
                                  IN Real scalingFactor,
                                  IN AudioSampleListVector& sampleBuffer)
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
        WaveFile__writeInteger(byteList, position, sampleWidthInBytes,
                               rawDataList[i]);
    }

    Logging_trace1("<<: position = %1", TOSTRING(position));
}

/*--------------------*/

/**
 * Writes <C>r</C> to byte list <C>byteList</C> at <C>position</C>
 * using <C>byteCount</C> bytes in little-endian format and updates
 * <C>position</C>.
 *
 * @param[inout] byteList   destination byte list to be written to
 * @param[inout] position   first destination position to be written to
 * @param[in]    byteCount  count of byte used for real value
 * @param[in]    r          real value to be written
 */
static void WaveFile__writeReal (INOUT ByteList& byteList,
                                 INOUT Natural& position,
                                 IN Natural byteCount,
                                 IN Real r)
{
    float f = (float) r;
    double d = (double) r;
    Byte* sourcePtr = (byteCount == 4 ? (Byte*) &f : (Byte*) &d);
    Byte* destinationPtr = &byteList[position];
    position += byteCount;

    if (isBigEndian) {
        sourcePtr += ((int) byteCount - 1);

        for (Natural i = 0; i < byteCount; i++) {
            *destinationPtr++ = *sourcePtr--;
        }
    } else {
        for (Natural i = 0; i < byteCount; i++) {
            *destinationPtr++ = *sourcePtr++;
        }
    }
}

/*--------------------*/

/**
 * Writes data from <C>sampleBuffer</C> to <C>byteList</C> at
 * <C>position</C> as floating point data characterized by
 * <C>totalSampleCount</C>, <C>channelCount</C>,
 * <C>audioFrameCount</C> and <C>sampleWidthInBytes</C> and updates
 * <C>position</C>.
 *
 * @param[inout] byteList            destination byte list for sample data
 * @param[inout] position            first position in byte list to be
 *                                   written
 * @param[in]    totalSampleCount    total number of samples in sample buffer
 * @param[in]    channelCount        number of channels in sample buffer
 * @param[in]    audioFrameCount     number of audio frames in sample buffer
 * @param[in]    sampleWidthInBytes  number of bytes per sample
 *                                   (must be 4 or 8)
 * @param[in]    sampleBuffer        sample buffer to be written to byte
 *                                   list
 */
static void
WaveFile__writeRealDataToByteList (INOUT ByteList& byteList,
                                   INOUT Natural& position,
                                   IN Natural totalSampleCount,
                                   IN Natural channelCount,
                                   IN Natural audioFrameCount,
                                   IN Natural sampleWidthInBytes,
                                   IN AudioSampleListVector& sampleBuffer)
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
        WaveFile__writeReal(byteList, position, sampleWidthInBytes,
                            rawDataList[i]);
    }

    Logging_trace1("<<: position = %1", TOSTRING(position));
}

/*--------------------*/

/**
 * Writes string <C>st</C> to byte list <C>byteList</C> at
 * <C>position</C> and updates <C>position</C>.
 *
 * @param[inout] byteList   destination byte list to be written to
 * @param[inout] position   first destination position to be written to
 * @param[in]    st         string value to be written
 */
static void WaveFile__writeString (INOUT ByteList& byteList,
                                   INOUT Natural& position,
                                   IN String& st)
{
    for (const Character ch : st) {
        byteList[position++] = (char) ch;
    }
}

/*====================*/
/* EXPORTED FEATURES  */
/*====================*/

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

WaveFileOperationResult WaveFile::read (OUT Natural& sampleRate,
                                        OUT Natural& channelCount,
                                        OUT Natural& audioFrameCount,
                                        OUT String& typeCode,
                                        OUT Natural& sampleWidthInBytes,
                                        OUT AudioSampleListVector& sampleBuffer)
{
    Logging_trace(">>");

    WaveFileOperationResult result = okay;

    try {
        /* open file */
        const String& fileName = TOREFERENCE<String>(_descriptor);
        Assertion_check(OperatingSystem::fileExists(fileName),
                        STR::expand("file must exist - %1", fileName));
        Natural fileSize = File::length(fileName);
        ByteList byteList;

        {
            File waveFile;
            const Boolean isOkay = waveFile.open(fileName, "rb");
            Assertion_check(isOkay,
                            STR::expand("file must be readable: %1", fileName));

            byteList.setLength(fileSize);
            waveFile.read(byteList, 0, fileSize);
            waveFile.close();
        }

        /* process */
        Boolean isPCMData;
        Natural position = 0;

        /* write RIFF file header */
        WaveFile__readHeader(byteList, position,
                             fileSize, isPCMData, channelCount,
                             sampleRate, sampleWidthInBytes);

        /* read sample data section */
        WaveFile__skipString(byteList, position, "data");
        const Natural dataSectionSize =
            WaveFile__readNatural(byteList, position, 4);
        const Natural totalSampleCount =
            dataSectionSize / sampleWidthInBytes;
        audioFrameCount = totalSampleCount / channelCount;

        Logging_trace("--: copying raw data list to sample buffer");

        if (isPCMData) {
            const Real scalingFactor =
                Real::power(2.0, Real{sampleWidthInBytes * 8 - 1});
            WaveFile__readIntDataFromByteList(byteList, position,
                                              totalSampleCount,
                                              channelCount, audioFrameCount,
                                              sampleWidthInBytes, scalingFactor,
                                              sampleBuffer);
        } else {
            WaveFile__readRealDataFromByteList(byteList, position,
                                               totalSampleCount,
                                               channelCount, audioFrameCount,
                                               sampleWidthInBytes,
                                               sampleBuffer);
        }

        Logging_trace1("--: transferred %1 bytes file",
                       TOSTRING(fileSize));
    } catch (const AssertionError& e) {
        Logging_traceError1("operation failed with '%1'", e.what());
        result = failed;
    }

    Logging_trace6("<<: result = %1,"
                   " sampleRate = %2, channelCount = %3,"
                   " audioFrameCount = %4, typeCode = '%5',"
                   " sampleWidthInBytes = %6",
                   WaveFileOperationResult_toString(result),
                   TOSTRING(sampleRate), TOSTRING(channelCount),
                   TOSTRING(audioFrameCount), typeCode,
                   TOSTRING(sampleWidthInBytes));
    return result;
}

/*--------------------*/

WaveFileOperationResult WaveFile::write (IN Natural sampleRate,
                                         IN Natural channelCount,
                                         IN Natural audioFrameCount,
                                         IN String& typeCode,
                                         IN Natural sampleWidthInBytes,
                                         IN AudioSampleListVector& sampleBuffer)
{
    Logging_trace5(">>: sampleRate = %1, channelCount = %2,"
                   " audioFrameCount = %3, typeCode = '%4',"
                   " sampleWidthInBytes = %5",
                   TOSTRING(sampleRate), TOSTRING(channelCount),
                   TOSTRING(audioFrameCount), typeCode,
                   TOSTRING(sampleWidthInBytes));

    WaveFileOperationResult result = okay;

    try {
        /* assertion check */
        Assertion_pre((typeCode == "I" || typeCode == "R"),
                      "type code must be 'I' or 'R'");
        Assertion_pre((typeCode != "I"
                       || allowedIntSampleWidthList
                              .contains(sampleWidthInBytes)),
                      "sample width for int must be 1, 2, 3 or 4");
        Assertion_pre((typeCode != "R"
                       || allowedRealSampleWidthList
                              .contains(sampleWidthInBytes)),
                      "sample width for real must be 4 or 8");

        /* open file */
        const String& fileName = TOREFERENCE<String>(_descriptor);
        File waveFile;
        Boolean isOkay = waveFile.open(fileName, "wb");
        Assertion_check(isOkay,
                        STR::expand("file must be writable: %1", fileName));

        /* process */
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
        WaveFile__writeInteger(byteList, position, 4, dataSectionSize);

        Logging_trace("--: copying sample buffer to raw data list");

        if (isPCMData) {
            const Real scalingFactor =
                Real::power(2.0, Real{sampleWidthInBytes * 8 - 1});
            WaveFile__writeIntDataToByteList(byteList, position,
                                             totalSampleCount,
                                             channelCount, audioFrameCount,
                                             sampleWidthInBytes, scalingFactor,
                                             sampleBuffer);
        } else {
            WaveFile__writeRealDataToByteList(byteList, position,
                                              totalSampleCount,
                                              channelCount, audioFrameCount,
                                              sampleWidthInBytes,
                                              sampleBuffer);
        }

        Logging_trace("--: writing byte list to file");
        waveFile.write(byteList, 0, fileSize);
        waveFile.close();
    } catch (const AssertionError& e) {
        Logging_traceError1("operation failed with '%1'", e.what());
        result = failed;
    }

    Logging_trace1("<<: %1", WaveFileOperationResult_toString(result));
    return result;
}
