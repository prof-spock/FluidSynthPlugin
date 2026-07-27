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

using Audio::AudioDataPoint;
using Audio::AudioDataPointList;
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

/** the allowed data point widths for integer samples */
static const NaturalList allowedIntDataPointWidthList =
    NaturalList::fromList({1, 2, 3, 4});

/** the allowed data point widths for real samples */
static const NaturalList allowedRealDataPointWidthList =
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
 * <C>channelCount</C>, <C>dataPointRate</C> and
 * <C>dataPointWidthInBytes</C>.
 *
 * @param[in]    byteList               source byte list to be read from
 * @param[inout] position               first source position to be read
 *                                      from
 * @param[out]   isPCMData              information whether this is a
 *                                      format with integer samples
 * @param[out]   fileSize               the count in bytes of the complete
 *                                      file
 * @param[out]   channelCount           number of channels in data point
 *                                      buffer
 * @param[out]   dataPointRate          "sample rate" within file
 * @param[out]   dataPointWidthInBytes  number of bytes per data point
 *                                      (must be 1, 2, 3, 4 or 8)
 */
static void
WaveFile__readHeader (IN ByteList& byteList,
                      INOUT Natural& position,
                      IN Natural expectedFileSize,
                      OUT Boolean& isPCMData,
                      OUT Natural& channelCount,
                      OUT Natural& dataPointRate,
                      OUT Natural& dataPointWidthInBytes)
{
    Logging_trace1(">>: position = %1", TOSTRING(position));

    Natural expectedBitsPerDataPoint;
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
        dataPointRate = WaveFile__readNatural(byteList, position, 4);
        const Natural bytesPerSecond =
            WaveFile__readNatural(byteList, position, 4);
        dataPointWidthInBytes = bytesPerSecond / (dataPointRate * channelCount);

        const Natural bytesPerAudioFrame =
            WaveFile__readNatural(byteList, position, 2);
        String st = String("bytes per audio frame %1 times data point rate %2"
                           " should give bytes per second %3");
        Assertion_check(bytesPerAudioFrame * dataPointRate == bytesPerSecond,
                        STR::expand(st, TOSTRING(bytesPerAudioFrame),
                                    TOSTRING(dataPointRate),
                                    TOSTRING(bytesPerSecond)));

        expectedBitsPerDataPoint = dataPointWidthInBytes * 8;
        Natural bitsPerDataPoint = WaveFile__readNatural(byteList, position, 2);
        WaveFile__assertEquality(expectedBitsPerDataPoint, bitsPerDataPoint,
                                 "bits per data point");
    }

    if (!isPCMData) {
        /* read extension data */
        Natural extensionSizeInHeader =
            WaveFile__readNatural(byteList, position, 2);
        WaveFile__assertEquality(extensionSize, extensionSizeInHeader,
                                 "extension size");

        Natural bitsPerDataPoint = WaveFile__readNatural(byteList, position, 2);
        WaveFile__assertEquality(expectedBitsPerDataPoint, bitsPerDataPoint,
                                 "bits per data point");

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
                   " dataPointRate = %5, dataPointWidthInBytes = %6",
                   TOSTRING(position), TOSTRING(isPCMData),
                   TOSTRING(fileSize), TOSTRING(channelCount),
                   TOSTRING(dataPointRate), TOSTRING(dataPointWidthInBytes));
}

/*--------------------*/

/**
 * Reads data <C>byteList</C> at <C>position</C> into
 * <C>dataPointBuffer</C> as integer data characterized by
 * <C>totalDataPointCount</C>, <C>channelCount</C>,
 * <C>audioFrameCount</C>, <C>dataPointWidthInBytes</C> and
 * <C>scalingFactor</C> and updates <C>position</C>.
 *
 * @param[inout] byteList               source byte list for data points
 * @param[inout] position               first position in byte list to
 *                                      be read from
 * @param[in]    totalDataPointCount    total number of data points
 * @param[in]    channelCount           number of channels
 * @param[in]    audioFrameCount        number of audio frames
 * @param[in]    dataPointWidthInBytes  number of bytes per data point
 *                                      (must be 1, 2, 3 or 4)
 * @param[in]    scalingFactor          real factor to scale data
 * @param[out]   dataPointBuffer        data point buffer to be read
 *                                      into from byte list
 */
static void
WaveFile__readIntDataFromByteList (IN ByteList& byteList,
                                   INOUT Natural& position,
                                   IN Natural totalDataPointCount,
                                   IN Natural channelCount,
                                   IN Natural audioFrameCount,
                                   IN Natural dataPointWidthInBytes,
                                   IN Real scalingFactor,
                                   OUT AudioDataPointListVector& dataPointBuffer)
{
    Logging_trace6(">>: position = %1, totalDataPointCount = %2,"
                   " channelCount = %3, audioFrameCount = %4,"
                   " dataPointWidthInBytes = %5, scalingFactor = %6",
                   TOSTRING(position), TOSTRING(totalDataPointCount),
                   TOSTRING(channelCount), TOSTRING(audioFrameCount),
                   TOSTRING(dataPointWidthInBytes), TOSTRING(scalingFactor));

    Natural lastPosition =
        position + totalDataPointCount * dataPointWidthInBytes - 1;
    WaveFile__assertIndexInRange(byteList, lastPosition);

    IntegerList rawDataList{totalDataPointCount};
    dataPointBuffer.setLength(channelCount);
    dataPointBuffer.setFrameCount(audioFrameCount);

    for (Natural i = 0;  i < totalDataPointCount;  i++) {
        rawDataList[i] =
            WaveFile__readInteger(byteList, position, dataPointWidthInBytes);
    }

    Logging_trace("--: copying raw data to data point buffer");

    for (Natural channel = 0;  channel < channelCount;  channel++) {
        AudioDataPointList& dataPointList = dataPointBuffer[channel];
        Natural i = channel;

        for (Natural j = 0;  j < audioFrameCount;  j++) {
            const AudioDataPoint dataPoint =
                Real{rawDataList[i]} / scalingFactor;
            dataPointList[j] = dataPoint;
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
 * <C>dataPointBuffer</C> as floating point data characterized by
 * <C>totalDataPointCount</C>, <C>channelCount</C>,
 * <C>audioFrameCount</C> and <C>dataPointWidthInBytes</C> updates
 * <C>position</C>.
 *
 * @param[inout] byteList               source byte list for data points
 * @param[inout] position               first position in byte list to
 *                                      be read from
 * @param[in]    totalDataPointCount    total number of data points
 * @param[in]    channelCount           number of channels
 * @param[in]    audioFrameCount        number of audio frames
 * @param[in]    dataPointWidthInBytes  number of bytes per data point
 *                                      (must be 1, 2, 3 or 4)
 * @param[out]   dataPointBuffer        data point buffer to be read
 *                                      into from byte list
 */
static void
WaveFile__readRealDataFromByteList
        (IN ByteList& byteList,
         INOUT Natural& position,
         IN Natural totalDataPointCount,
         IN Natural channelCount,
         IN Natural audioFrameCount,
         IN Natural dataPointWidthInBytes,
         OUT AudioDataPointListVector& dataPointBuffer)
{
    Logging_trace5(">>: position = %1, totalDataPointCount = %2,"
                   " channelCount = %3, audioFrameCount = %4,"
                   " dataPointWidthInBytes = %5",
                   TOSTRING(position), TOSTRING(totalDataPointCount),
                   TOSTRING(channelCount), TOSTRING(audioFrameCount),
                   TOSTRING(dataPointWidthInBytes));

    Natural lastPosition =
        position + totalDataPointCount * dataPointWidthInBytes - 1;
    WaveFile__assertIndexInRange(byteList, lastPosition);

    RealList rawDataList{totalDataPointCount};
    dataPointBuffer.setLength(channelCount);
    dataPointBuffer.setFrameCount(audioFrameCount);

    for (Natural i = 0;  i < totalDataPointCount;  i++) {
        rawDataList[i] =
            WaveFile__readReal(byteList, position, dataPointWidthInBytes);
    }

    Logging_trace("--: copying raw data to data point buffer");

    for (Natural channel = 0;  channel < channelCount;  channel++) {
        AudioDataPointList& dataPointList = dataPointBuffer[channel];
        Natural i = channel;

        for (Natural j = 0;  j < audioFrameCount;  j++) {
            const AudioDataPoint dataPoint = rawDataList[i];
            dataPointList[j] = dataPoint;
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
 * <C>channelCount</C>, <C>dataPointRate</C> and
 * <C>dataPointWidthInBytes</C>.
 *
 * @param[inout] byteList               destination byte list to be
 *                                      written to
 * @param[inout] position               first destination position to
 *                                      be written to
 * @param[in]    isPCMData              information whether this is a
 *                                      format with integer samples
 * @param[in]    fileSize               the count in bytes of the
 *                                      complete file
 * @param[in]    channelCount           number of channels in data
 *                                      point buffer
 * @param[in]    audioFrameCount        number of audio frames in data
 *                                      point buffer
 * @param[in]    dataPointRate          data point rate to be stored in
 *                                      file
 * @param[in]    dataPointWidthInBytes  number of bytes per data point
 *                                      (must be 1, 2, 4 or 8)
 */
static void
WaveFile__writeHeader(INOUT ByteList& byteList,
                      INOUT Natural& position,
                      IN Boolean isPCMData,
                      IN Natural fileSize,
                      IN Natural channelCount,
                      IN Natural audioFrameCount,
                      IN Natural dataPointRate,
                      IN Natural dataPointWidthInBytes)
{
    Logging_trace7(">>: position = %1, isPCMData = %2, fileSize = %3,"
                   " channelCount = %4, audioFrameCount = %5,"
                   " dataPointRate = %6, dataPointWidthInBytes = %7",
                   TOSTRING(position), TOSTRING(isPCMData),
                   TOSTRING(fileSize), TOSTRING(channelCount),
                   TOSTRING(audioFrameCount), TOSTRING(dataPointRate),
                   TOSTRING(dataPointWidthInBytes));
    
    WaveFile__writeString(byteList, position, "RIFF");
    WaveFile__writeInteger(byteList, position, 4, fileSize - 8);
    WaveFile__writeString(byteList, position, "WAVE");

    /* write data for the format data section */
    const Natural dataKind = (isPCMData ? 1 : 0xFFFE);
    const Natural extensionSize = 22;
    const Natural formatSectionSize =
        Natural{16} + (isPCMData ? 0 : extensionSize + 2);
    const Natural bitsPerDataPoint = dataPointWidthInBytes * 8;

    WaveFile__writeString(byteList, position, "fmt ");
    WaveFile__writeInteger(byteList, position, 4, formatSectionSize);
    WaveFile__writeInteger(byteList, position, 2, dataKind);
    WaveFile__writeInteger(byteList, position, 2, channelCount);
    WaveFile__writeInteger(byteList, position, 4, dataPointRate);
    WaveFile__writeInteger(byteList, position, 4,
                           dataPointRate * channelCount * dataPointWidthInBytes);
    WaveFile__writeInteger(byteList, position, 2,
                           channelCount * dataPointWidthInBytes);
    WaveFile__writeInteger(byteList, position, 2, bitsPerDataPoint);

    if (!isPCMData) {
        /* write extension data */
        Natural subformatPart;
        WaveFile__writeInteger(byteList, position, 2, extensionSize);
        WaveFile__writeInteger(byteList, position, 2, bitsPerDataPoint);
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
 * Writes data from <C>dataPointBuffer</C> to <C>byteList</C> at
 * <C>position</C> as integer data characterized by
 * <C>totalDataPointCount</C>, <C>channelCount</C>,
 * <C>audioFrameCount</C>, <C>dataPointWidthInBytes</C> and
 * <C>scalingFactor</C> and updates <C>position</C>.
 *
 * @param[inout] byteList               destination byte list for data
 *                                      points
 * @param[inout] position               first position in byte list to
 *                                      be written
 * @param[in]    totalDataPointCount    total number of data points in
 *                                      buffer
 * @param[in]    channelCount           number of channels in data
 *                                      point buffer
 * @param[in]    audioFrameCount        number of audio frames in
 *                                      data point buffer
 * @param[in]    dataPointWidthInBytes  number of bytes per data point
 *                                      (must be 1, 2, 3 or 4)
 * @param[in]    scalingFactor          real factor to scale data from
 *                                      data point buffer
 * @param[in]    dataPointBuffer        data point buffer to be written
 *                                      to byte list
 */
static void
WaveFile__writeIntDataToByteList
        (INOUT ByteList& byteList,
         INOUT Natural& position,
         IN Natural totalDataPointCount,
         IN Natural channelCount,
         IN Natural audioFrameCount,
         IN Natural dataPointWidthInBytes,
         IN Real scalingFactor,
         IN AudioDataPointListVector& dataPointBuffer)
{
    Logging_trace6(">>: position = %1, totalDataPointCount = %2,"
                   " channelCount = %3, audioFrameCount = %4,"
                   " dataPointWidthInBytes = %5, scalingFactor = %6",
                   TOSTRING(position), TOSTRING(totalDataPointCount),
                   TOSTRING(channelCount), TOSTRING(audioFrameCount),
                   TOSTRING(dataPointWidthInBytes), TOSTRING(scalingFactor));

    IntegerList rawDataList{totalDataPointCount};

    for (Natural channel = 0;  channel < channelCount;  channel++) {
        const AudioDataPointList& dataPointList = dataPointBuffer[channel];
        Natural i = channel;

        for (Natural j = 0;  j < audioFrameCount;  j++) {
            const AudioDataPoint dataPoint = dataPointList[j];
            Real r = (dataPoint >= 1.0 ? scalingFactor - 1.0
                      : (dataPoint < -1.0 ? -scalingFactor
                         : dataPoint * scalingFactor));
            Integer s = (Integer) r;
            rawDataList[i] = s;
            i += channelCount;
        }
    }

    Logging_trace("--: copying raw data to byte list");

    for (Natural i = 0;  i < totalDataPointCount;  i++) {
        WaveFile__writeInteger(byteList, position, dataPointWidthInBytes,
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
 * Writes data from <C>dataPointBuffer</C> to <C>byteList</C> at
 * <C>position</C> as floating point data characterized by
 * <C>totalDataPointCount</C>, <C>channelCount</C>,
 * <C>audioFrameCount</C> and <C>dataPointWidthInBytes</C> and updates
 * <C>position</C>.
 *
 * @param[inout] byteList               destination byte list for data
 *                                      points
 * @param[inout] position               first position in byte list
 *                                      to be written
 * @param[in]    totalDataPointCount    total number of data points in
 *                                      buffer
 * @param[in]    channelCount           number of channels in data
 *                                      point buffer
 * @param[in]    audioFrameCount        number of audio frames in data
 *                                      point buffer
 * @param[in]    dataPointWidthInBytes  number of bytes per data point
 *                                      (must be 4 or 8)
 * @param[in]    dataPointBuffer        data point buffer to be written
 *                                      to byte list
 */
static void
WaveFile__writeRealDataToByteList
    (INOUT ByteList& byteList,
     INOUT Natural& position,
     IN Natural totalDataPointCount,
     IN Natural channelCount,
     IN Natural audioFrameCount,
     IN Natural dataPointWidthInBytes,
     IN AudioDataPointListVector& dataPointBuffer)
{
    Logging_trace5(">>: position = %1, totalDataPointCount = %2,"
                   " channelCount = %3, audioFrameCount = %4,"
                   " dataPointWidthInBytes = %5",
                   TOSTRING(position), TOSTRING(totalDataPointCount),
                   TOSTRING(channelCount), TOSTRING(audioFrameCount),
                   TOSTRING(dataPointWidthInBytes));

    RealList rawDataList{totalDataPointCount};

    for (Natural channel = 0;  channel < channelCount;  channel++) {
        const AudioDataPointList& dataPointList = dataPointBuffer[channel];
        Natural i = channel;

        for (Natural j = 0;  j < audioFrameCount;  j++) {
            const AudioDataPoint dataPoint = dataPointList[j];
            rawDataList[i] = dataPoint;
            i += channelCount;
        }
    }

    Logging_trace("--: copying raw data to byte list");

    for (Natural i = 0;  i < totalDataPointCount;  i++) {
        WaveFile__writeReal(byteList, position, dataPointWidthInBytes,
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

WaveFileOperationResult WaveFile::read (OUT Natural& dataPointRate,
                                        OUT Natural& channelCount,
                                        OUT Natural& audioFrameCount,
                                        OUT String& typeCode,
                                        OUT Natural& dataPointWidthInBytes,
                                        OUT AudioDataPointListVector& dataPointBuffer)
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
                             dataPointRate, dataPointWidthInBytes);

        /* read data point data section */
        WaveFile__skipString(byteList, position, "data");
        const Natural dataSectionSize =
            WaveFile__readNatural(byteList, position, 4);
        const Natural totalDataPointCount =
            dataSectionSize / dataPointWidthInBytes;
        audioFrameCount = totalDataPointCount / channelCount;

        Logging_trace("--: copying raw data list to data point buffer");

        if (isPCMData) {
            const Real scalingFactor =
                Real::power(2.0, Real{dataPointWidthInBytes * 8 - 1});
            WaveFile__readIntDataFromByteList(byteList, position,
                                              totalDataPointCount,
                                              channelCount, audioFrameCount,
                                              dataPointWidthInBytes, scalingFactor,
                                              dataPointBuffer);
        } else {
            WaveFile__readRealDataFromByteList(byteList, position,
                                               totalDataPointCount,
                                               channelCount, audioFrameCount,
                                               dataPointWidthInBytes,
                                               dataPointBuffer);
        }

        Logging_trace1("--: transferred %1 bytes file",
                       TOSTRING(fileSize));
    } catch (const AssertionError& e) {
        Logging_traceError1("operation failed with '%1'", e.what());
        result = failed;
    }

    Logging_trace6("<<: result = %1,"
                   " dataPointRate = %2, channelCount = %3,"
                   " audioFrameCount = %4, typeCode = '%5',"
                   " dataPointWidthInBytes = %6",
                   WaveFileOperationResult_toString(result),
                   TOSTRING(dataPointRate), TOSTRING(channelCount),
                   TOSTRING(audioFrameCount), typeCode,
                   TOSTRING(dataPointWidthInBytes));
    return result;
}

/*--------------------*/

WaveFileOperationResult WaveFile::write (IN Natural dataPointRate,
                                         IN Natural channelCount,
                                         IN Natural audioFrameCount,
                                         IN String& typeCode,
                                         IN Natural dataPointWidthInBytes,
                                         IN AudioDataPointListVector& dataPointBuffer)
{
    Logging_trace5(">>: dataPointRate = %1, channelCount = %2,"
                   " audioFrameCount = %3, typeCode = '%4',"
                   " dataPointWidthInBytes = %5",
                   TOSTRING(dataPointRate), TOSTRING(channelCount),
                   TOSTRING(audioFrameCount), typeCode,
                   TOSTRING(dataPointWidthInBytes));

    WaveFileOperationResult result = okay;

    try {
        /* assertion check */
        Assertion_pre((typeCode == "I" || typeCode == "R"),
                      "type code must be 'I' or 'R'");
        Assertion_pre((typeCode != "I"
                       || allowedIntDataPointWidthList
                              .contains(dataPointWidthInBytes)),
                      "data point width for int must be 1, 2, 3 or 4");
        Assertion_pre((typeCode != "R"
                       || allowedRealDataPointWidthList
                              .contains(dataPointWidthInBytes)),
                      "data point width for real must be 4 or 8");

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
        const Natural totalDataPointCount =
            channelCount * audioFrameCount;
        /* make an even section size */
        const Natural dataSectionSize =
            (totalDataPointCount * dataPointWidthInBytes
             + (totalDataPointCount * dataPointWidthInBytes) % 2);
        const Natural fileSize =
            headerSize + factChunkSize + dataSectionSize;

        ByteList byteList{fileSize};
        Natural position = 0;

        /* write RIFF file header */
        WaveFile__writeHeader(byteList, position,
                              isPCMData, fileSize, channelCount,
                              audioFrameCount, dataPointRate,
                              dataPointWidthInBytes);

        /* write sample data section */
        WaveFile__writeString(byteList, position, "data");
        WaveFile__writeInteger(byteList, position, 4, dataSectionSize);

        Logging_trace("--: copying data point buffer to raw data list");

        if (isPCMData) {
            const Real scalingFactor =
                Real::power(2.0, Real{dataPointWidthInBytes * 8 - 1});
            WaveFile__writeIntDataToByteList(byteList, position,
                                             totalDataPointCount,
                                             channelCount, audioFrameCount,
                                             dataPointWidthInBytes,
                                             scalingFactor,
                                             dataPointBuffer);
        } else {
            WaveFile__writeRealDataToByteList(byteList, position,
                                              totalDataPointCount,
                                              channelCount, audioFrameCount,
                                              dataPointWidthInBytes,
                                              dataPointBuffer);
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
