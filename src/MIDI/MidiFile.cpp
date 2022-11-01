/**
 * @file
 * The <C>MidiFile</C> implementation implements the services of a
 * MIDI file; currently only file reading is supported.
 *
 * @author Dr. Thomas Tensi
 * @date   2022-08
 */

/*=========*/
/* IMPORTS */
/*=========*/

#include "Assertion.h"
#include "File.h"
#include "Logging.h"
#include "MidiFile.h"
#include "NaturalList.h"
#include "OperatingSystem.h"

/*--------------------*/

using BaseModules::File;
using BaseModules::OperatingSystem;
using BaseTypes::Containers::NaturalList;
using MIDI::MidiEvent;
using MIDI::MidiEventKind;
using MIDI::MidiMetaEventKind;
using MIDI::MidiFile;

/* aliases for function names */

/** abbreviation for StringUtil::expand */
#define expand  StringUtil::expand

/*--------------------*/
/* forward declarations */
/*----------------------*/

static NaturalList _readNaturalList (IN ByteList& byteList,
                                     INOUT Natural& position,
                                     IN Natural byteCount);

static String _readString (IN ByteList& byteList,
                           INOUT Natural& position,
                           IN Natural byteCount);

static Natural _readVariableLengthNatural (IN ByteList& byteList,
                                           INOUT Natural& position);

/*--------------------*/
/* internal features  */
/*--------------------*/

/** the string used a a RIFF file head marker for a MIDI file */
static String _fileHead{"MThd"};

/** the string used a a RIFF track head marker for a MIDI file */
static String _trackHead{"MTrk"};

/*--------------------*/
/*--------------------*/

/**
 * Reads integer in midi stream <C>byteList</C> at <C>position</C> with
 * length <C>byteCount</C>, returns it and updates <C>position</C>.
 *
 * @param[in]    byteList   list of bytes in file
 * @param[inout] position   start position of read operation to be
 *                          updated
 * @param[in]    byteCount  length of string to be read
 * @param[in]    isSigned   information whether value is considered
 *                          signed
 * @return  integer value
 */
static Integer _readInteger (IN ByteList& byteList,
                             INOUT Natural& position,
                             IN Natural byteCount,
                             IN Boolean isSigned = true)
{
    Integer result{0};
    Integer maxValue{0};

    for (Natural i = 0;  i < byteCount;  i++) {
        Byte value{byteList[position++]};
        maxValue = maxValue * 256 + 255;
        result = result * 256 + (uint8_t) value;
    }

    if (isSigned) {
        Integer referenceValue{(maxValue + 1) / 2};
        result = (result < referenceValue
                  ? result
                  : result - maxValue - 1);
    }

    return result;
}

/*--------------------*/

/**
 * Reads natural in midi stream <C>byteList</C> at <C>position</C> with
 * length <C>byteCount</C>, returns it and updates <C>position</C>.
 *
 * @param[in]    byteList   list of bytes in file
 * @param[inout] position   start position of read operation to be
 *                          updated
 * @param[in]    byteCount  length of string to be read
 * @return natural value
 */
static Natural _readNatural (IN ByteList& byteList,
                             INOUT Natural& position,
                             IN Natural byteCount)
{
    Integer i = _readInteger(byteList, position, byteCount, false);
    return Natural{(size_t) i};
}


/*--------------------*/

/**
 * Reads MIDI event in midi stream <C>byteList</C> at <C>position</C>
 * (with its length defined by the event kind) happening on
 * <C>time</C>, returns it and updates <C>position</C>.
 *
 * @param[in]    time       the time of the MIDI event (in ticks)
 * @param[in]    byteList   list of bytes in file
 * @param[inout] position   start position of read operation to be
 *                          updated
 * @return midi event
 */
static MidiEvent _readMidiEvent (IN Natural time,
                                 IN ByteList& byteList,
                                 INOUT Natural& position)
{
    Natural eventLength;
    NaturalList eventDataList;

    Logging_trace1(">>: position = %1", TOSTRING(position));

    Natural eventByte = _readNatural(byteList, position, 1);
    Assertion_check(MidiEventKind::isValid(eventByte),
                    expand("bad MIDI format: expected"
                           " event byte, got %1",
                           TOSTRING(eventByte)));
    MidiEventKind eventKind{eventByte};
    Logging_trace2("--: eventByte = %1, eventKind = %2",
                   TOSTRING(eventByte), eventKind.toString());

    if (eventKind == MidiEventKind::systemExclusive) {
        /* TODO: read up to F7? */
        Assertion_check(false, "sysEx not yet implemented");
    } else if (eventKind == MidiEventKind::meta) {
        Natural metaEventKindByte = _readNatural(byteList, position, 1);
        Assertion_check(MidiMetaEventKind::isValid(metaEventKindByte),
                        expand("bad MIDI format: expected"
                               " meta event byte, got %1",
                               TOSTRING(metaEventKindByte)));
        eventLength = _readVariableLengthNatural(byteList, position);
        eventDataList = _readNaturalList(byteList, position, eventLength);
        eventDataList.prepend(metaEventKindByte);
        eventDataList.prepend(eventByte);
    } else {
        /* process a channel message */
        eventLength = eventKind.byteCount() - 1;
        eventDataList = _readNaturalList(byteList, position, eventLength);
        eventDataList.prepend(eventByte);
    }

    MidiEvent result{time, eventDataList};
    
    Logging_trace1("<<: %1", result.toString());
    return result;
}

/*--------------------*/

/**
 * Reads header in midi stream <C>byteList</C> at <C>position</C>,
 * returns its data in <C>trackCount</C>, <C>fileType</C> and
 * <C>timeDivision</C> and updates <C>position</C>.
 *
 * @param[in]    byteList      list of bytes in file
 * @param[inout] position      start position of read operation to be
 *                             updated
 * @param[out]   fileType      type of MIDI file
 * @param[out]   trackCount    number of tracks in file
 * @param[out]   timeDivision  number of milliseconds in quarter note
 */
static void _readMidiHeader (IN ByteList& byteList,
                             INOUT Natural& position,
                             OUT Natural& fileType,
                             OUT Natural& trackCount,
                             OUT Natural& timeDivision)
{
    Logging_trace1(">>: position = %1", TOSTRING(position));

    String headerString = _readString(byteList, position, 4);
    Natural length      = _readNatural(byteList, position, 4);
    fileType            = _readNatural(byteList, position, 2);
    trackCount          = _readNatural(byteList, position, 2);
    timeDivision        = _readNatural(byteList, position, 2);

    Assertion_check(headerString == _fileHead,
                    "midi header chunk expected");
    Assertion_check(length == 6, "midi header must have length 6");
    Assertion_check(fileType <= 2, "midi format must be 0, 1, or 2");

    Logging_trace4("<<: position = %1, fileType = %2, trackCount = %3,"
                   " timeDivision = %4",
                   TOSTRING(position), TOSTRING(fileType),
                   TOSTRING(trackCount), TOSTRING(timeDivision));
}

/*--------------------*/

/**
 * Reads track in midi stream <C>byteList</C> at <C>position</C>,
 * returns its data in <C>trackCount</C>, <C>fileType</C> and
 * <C>timeDivision</C> and updates <C>position</C>.
 *
 * @param[in]    byteList       list of bytes in file
 * @param[inout] position       start position of read operation to be
 *                              updated
 * @param[out]   midiEventList  ordered list of midi events on this
 *                              track
 */
static void _readMidiTrack (IN ByteList& byteList,
                            INOUT Natural& position,
                            OUT MidiEventList& midiEventList)
{
    Logging_trace1(">>: position = %1", TOSTRING(position));

    String header  = _readString(byteList, position, 4);
    Natural length = _readNatural(byteList, position, 4);

    Assertion_check(header == _trackHead,
                    expand("track header chunk %1 expected - found %2",
                           _trackHead, header));

    Natural currentTime{0};
    Boolean isAtTrackEnd{false};
    midiEventList.clear();

    while (!isAtTrackEnd) {
        const Natural deltaTime =
            _readVariableLengthNatural(byteList, position);
        currentTime += deltaTime;
        MidiEvent midiEvent =
            _readMidiEvent(currentTime, byteList, position);
        midiEventList.append(midiEvent);
        isAtTrackEnd =
            (midiEvent.kind() == MidiEventKind::meta
             && midiEvent.metaKind() == MidiMetaEventKind::trackEnd);
    }
    
    Logging_trace1("<<: eventCount = %1",
                   TOSTRING(midiEventList.length()));
}

/*--------------------*/

/**
 * Reads list of natural values in midi stream <C>byteList</C> at
 * <C>position</C> with length <C>byteCount</C>, returns it and
 * updates <C>position</C>.
 *
 * @param[in]    byteList   list of bytes in file
 * @param[inout] position   start position of read operation to be
 *                          updated
 * @param[in]    byteCount  length of list to be read (in bytes)
 * @return natural list
 */
static NaturalList _readNaturalList (IN ByteList& byteList,
                                     INOUT Natural& position,
                                     IN Natural byteCount)
{
    Logging_trace1(">>: position = %1", TOSTRING(position));

    NaturalList result{};

    for (Natural i = 0;  i < byteCount;  i++) {
        Natural value = _readNatural(byteList, position, 1);
        result.append(value);
    }
    
    Logging_trace1("<<: %1", TOSTRING(result.length()));
    return result;
}

/*--------------------*/

/**
 * Reads string in midi stream <C>byteList</C> at <C>position</C> with
 * length <C>byteCount</C>, returns it and updates <C>position</C>.
 *
 * @param[in] byteList      list of bytes in file
 * @param[in] position      start position of read operation to be
 *                          updated
 * @param[in] byteCount     length of string to be read
 * @return  string read from byte list
 */
static String _readString (IN ByteList& byteList,
                           INOUT Natural& position,
                           IN Natural byteCount)
{
    String result;

    for (Natural i = 0;  i < byteCount;  i++) {
        result += (char) byteList[position];
        position++;
    }

    return result;
}

/*--------------------*/

/**
 * Reads natural value in midi stream in variable bytes (until top bit
 * is unset) scanning <C>byteList</C> at <C>position</C>, returns it
 * and updates <C>position</C>.
 *
 * @param[in] byteList      list of bytes in file
 * @param[in] position      start position of read operation to be
 *                          updated
 * @return  natural value
 */
static Natural _readVariableLengthNatural (IN ByteList& byteList,
                                           INOUT Natural& position)
{
    Boolean isDone{false};
    Natural result{0};

    while (!isDone) {
        uint8_t value = (uint8_t) byteList.at(position++);
        isDone = (value < 128);
        value = value & 127;
        result = result * 128 + value;
    }

    return result;
}

/*--------------------*/
/* con-/destruction   */
/*--------------------*/

MidiFile::MidiFile (IN String& name)
{
    Logging_trace1(">>: %1", name);
    _descriptor = new String{name};
    Logging_trace("<<");
}

/*--------------------*/

MidiFile::~MidiFile ()
{
    Logging_trace(">>");
    String* s = (String*) _descriptor;
    delete s;
    Logging_trace("<<");
}

/*--------------------*/
/* access             */
/*--------------------*/

void MidiFile::read (OUT Natural& fileType,
                     OUT Natural& timeDivision,
                     OUT MidiEventList& midiEventList)
{
    Logging_trace(">>");
    String& fileName = TOREFERENCE<String>(_descriptor);
    Assertion_pre(OperatingSystem::fileExists(fileName),
                  expand("file must exist: %1", fileName));

    /* find length of file */
    Natural byteCount = File::length(fileName);
    Logging_trace1("--: byteCount = %1", TOSTRING(byteCount));

    /* allocate bytes accordingly and read file into byte list */
    ByteList byteList{byteCount};
    File inputFile;
    Boolean isOkay = inputFile.open(fileName, "rb");
    Assertion_check(isOkay,
                    expand("file must be readable: %1", fileName));

    Natural countRead = inputFile.read(byteList, 0, byteCount);
    Assertion_check(countRead == byteCount,
                    expand("must be able to read %1, read %2 instead",
                           TOSTRING(byteCount), TOSTRING(countRead)));
    inputFile.close();

    /* read MIDI file header */
    Natural position{0};
    Natural trackCount;
    _readMidiHeader(byteList, position,
                    fileType, trackCount, timeDivision);
    midiEventList.clear();

    /* read MIDI tracks */
    for (Natural i = 0;  i < trackCount;  i++) {
        Logging_trace1("--: trackIndex = %1", TOSTRING(i));
        MidiEventList trackEventList;
        _readMidiTrack(byteList, position, trackEventList);
        midiEventList.merge(trackEventList);
    }

    Logging_trace3("<<: fileType = %1, timeDivision = %2,"
                   " eventList = %3",
                   TOSTRING(fileType), TOSTRING(timeDivision),
                   midiEventList.toString());
}
