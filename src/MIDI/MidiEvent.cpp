/**
 * @file
 * The <C>MidiEvent</C> implementation defines the services of a
 * single MIDI event.
 *
 * @author Dr. Thomas Tensi
 * @date   2022-04
 */

/*=========*/
/* IMPORTS */
/*=========*/

#include "Assertion.h"
#include "Logging.h"
#include "MidiEvent.h"

/*--------------------*/

using MIDI::MidiEvent;
using MIDI::MidiEventKind;
using MIDI::MidiMetaEventKind;

/** abbreviation for StringUtil */
using STR = BaseModules::StringUtil;

/*==================================*/
/* MIDIEventKind - PRIVATE FEATURES */
/*==================================*/

/**
 * Returns internal event kind code for given event byte <C>b</C>.
 *
 * @param[in] b  event byte
 * @return  internal code of event kind
 */
static Byte _eventByteToKindCode (IN Byte b)
{
    Byte result;

    if (b == '\xF0') {
        result = 7;
    } else if (b == '\xFF') {
        result = 8;
    } else {
        result = ((char) b & '\x70') >> 4;
    }

    return result;
}

/*--------------------*/

/**
 * Returns string representation of event kind with internal code
 * <C>kindCode</C>.
 *
 * @param[in] kindCode  internal code of event kind
 * @return  string representation of midi event kind with that internal
 *          code
 */
static String _eventKindCodeToString (IN Byte kindCode)
{
    String result;

    if (kindCode == 0) {
        result = "noteOff";
    } else if (kindCode == 1) {
        result = "noteOn";
    } else if (kindCode == 2) {
        result = "polyTouch";
    } else if (kindCode == 3) {
        result = "controlChange";
    } else if (kindCode == 4) {
        result = "programChange";
    } else if (kindCode == 5) {
        result = "monoTouch";
    } else if (kindCode == 6) {
        result = "pitchBend";
    } else if (kindCode == 7) {
        result = "systemExclusive";
    } else if (kindCode == 8) {
        result = "meta";
    } else {
        Assertion_check(false, "illegal value for event kind number");
    }

    return result;
}

/*=================================*/
/* MIDIEventKind - PUBLIC FEATURES */
/*=================================*/

/*--------------------*/
/* constant values    */
/*--------------------*/

MidiEventKind MidiEventKind::noteOff{'\x80'};
MidiEventKind MidiEventKind::noteOn{'\x90'};
MidiEventKind MidiEventKind::polyTouch{'\xA0'};
MidiEventKind MidiEventKind::controlChange{'\xB0'};
MidiEventKind MidiEventKind::programChange{'\xC0'};
MidiEventKind MidiEventKind::monoTouch{'\xD0'};
MidiEventKind MidiEventKind::pitchBend{'\xE0'};
MidiEventKind MidiEventKind::systemExclusive{'\xF0'};
MidiEventKind MidiEventKind::meta{'\xFF'};

/*--------------------*/
/* con-/destruction   */
/*--------------------*/

MidiEventKind::MidiEventKind (IN Byte b)
{
    Logging_trace1(">>: %1", TOSTRING(b));
    Assertion_pre(b >= 128, "event byte must be greater than 127");
    _value = _eventByteToKindCode(b);
    Logging_trace1("<<: %1", this->toString());
}

/*--------------------*/

Boolean MidiEventKind::isValid (IN Byte b)
{
    return b >= 128;
}

/*--------------------*/
/* conversion         */
/*--------------------*/

String MidiEventKind::toString () const
{
    String result = "MidiEventKind.";
    result += _eventKindCodeToString(_value);
    return result;
}

/*--------------------*/

Natural MidiEventKind::byteCount () const
{
    Logging_trace1(">>: %1", this->toString());
    Natural result;

    if (*this == noteOff || *this == noteOn
        || *this == polyTouch || *this == controlChange
        || *this == pitchBend) {
        result = 3;
    } else if (*this == programChange || *this == monoTouch) {
        result = 2;
    } else if (*this == systemExclusive || *this == meta) {
        result = 0;
    } else {
        Assertion_check(false, "illegal value for event kind");
    }

    Logging_trace1("<<: %1", result.toString());
    return result;
}

/*--------------------*/
/* comparison         */
/*--------------------*/

Boolean MIDI::operator == (IN MidiEventKind& self,
                           IN MidiEventKind& other)
{
    return self._value == other._value;
}

/*--------------------*/

Boolean MIDI::operator != (IN MidiEventKind& self,
                           IN MidiEventKind& other)
{
    return self._value != other._value;
}


/*======================================*/
/* MidiMetaEventKind - PRIVATE FEATURES */
/*======================================*/

/**
 * Returns string representation of meta event with internal
 * code (and also external byte) <C>b</C>.
 *
 * @param[in] b  meta event byte
 * @return  string representation of midi meta event kind with that
 *          internal code
 */
static String _metaEventByteToString (IN Byte b)
{
    String result;

    if (b == '\x00') {
        result = "sequenceNumber";
    } else if (b == '\x01') {
        result = "text";
    } else if (b == '\x02') {
        result = "copyright";
    } else if (b == '\x03') {
        result = "trackName";
    } else if (b == '\x04') {
        result = "instrumentName";
    } else if (b == '\x05') {
        result = "lyric";
    } else if (b == '\x06') {
        result = "marker";
    } else if (b == '\x07') {
        result = "cuePoint";
    } else if (b == '\x20') {
        result = "channelPrefix";
    } else if (b == '\x2F') {
        result = "trackEnd";
    } else if (b == '\x51') {
        result = "tempo";
    } else if (b == '\x54') {
        result = "smpteOffset";
    } else if (b == '\x58') {
        result = "timeSignature";
    } else if (b == '\x59') {
        result = "keySignature";
    } else if (b == '\x7F') {
        result = "sequencerMeta";
    } else {
        Assertion_check(false, "illegal value for meta event kind");
    }

    return result;
}

/*--------------------*/

/**
 * Returns information whether meta event with internal code (and also
 * external byte) <C>b</C> has an associated text string.
 *
 * @param[in] b  meta event byte
 * @return  information whether MIDI meta event kind has text data
 */
static Boolean _metaEventHasTextData (IN Byte b)
{
    return (b >= '\x01' && b <= '\x07');
}

/*=====================================*/
/* MidiMetaEventKind - PUBLIC FEATURES */
/*=====================================*/

/*--------------------*/
/* constant values    */
/*--------------------*/

MidiMetaEventKind MidiMetaEventKind::sequenceNumber{'\x00'};
MidiMetaEventKind MidiMetaEventKind::text{'\x01'};
MidiMetaEventKind MidiMetaEventKind::copyright{'\x02'};
MidiMetaEventKind MidiMetaEventKind::trackName{'\x03'};
MidiMetaEventKind MidiMetaEventKind::instrumentName{'\x04'};
MidiMetaEventKind MidiMetaEventKind::lyric{'\x05'};
MidiMetaEventKind MidiMetaEventKind::marker{'\x06'};
MidiMetaEventKind MidiMetaEventKind::cuePoint{'\x07'};
MidiMetaEventKind MidiMetaEventKind::channelPrefix{'\x20'};
MidiMetaEventKind MidiMetaEventKind::trackEnd{'\x2F'};
MidiMetaEventKind MidiMetaEventKind::tempo{'\x51'};
MidiMetaEventKind MidiMetaEventKind::smpteOffset{'\x54'};
MidiMetaEventKind MidiMetaEventKind::timeSignature{'\x58'};
MidiMetaEventKind MidiMetaEventKind::keySignature{'\x59'};
MidiMetaEventKind MidiMetaEventKind::sequencerMeta{'\x7F'};

/*--------------------*/
/* con-/destruction   */
/*--------------------*/

MidiMetaEventKind::MidiMetaEventKind (IN Byte b)
    : _value{b}
{
    Logging_trace1(">>: %1", TOSTRING(b));
    Logging_trace1("<<: %1", this->toString());
}

/*--------------------*/

Boolean MidiMetaEventKind::isValid (IN Byte b)
{
    return (b <= '\x07'
            || b == '\x20' || b == '\x2F'
            || b == '\x51' || b == '\x54' || b == '\x58' || b == '\x59'
            || b == '\x7F');
}

/*--------------------*/
/* conversion         */
/*--------------------*/

String MidiMetaEventKind::toString () const
{
    String result = "MidiMetaEventKind.";
    result += _metaEventByteToString(_value);
    return result;
}

/*--------------------*/

Natural MidiMetaEventKind::byteCount () const
{
    Logging_trace1(">>: %1", this->toString());
    Natural result;

    if (*this == smpteOffset) {
        result = 6;
    } else if (*this == timeSignature) {
        result = 5;
    } else if (*this == tempo) {
        result = 4;
    } else if (*this == sequenceNumber || *this == keySignature) {
        result = 3;
    } else if (*this == channelPrefix) {
        result = 2;
    } else if (*this == trackEnd) {
        result = 1;
    } else if ((_value >= text._value && _value <= cuePoint._value)
               || *this == sequencerMeta) {
        result = 0;
    } else {
        Assertion_check(false, "illegal value for event kind");
    }

    Logging_trace1("<<: %1", result.toString());
    return result;
}

/*--------------------*/
/* comparison         */
/*--------------------*/

Boolean MidiMetaEventKind::operator == (IN MidiMetaEventKind& other) const
{
    return _value == other._value;
}

/*--------------------*/

Boolean MidiMetaEventKind::operator != (IN MidiMetaEventKind& other) const
{
    return _value != other._value;
}

/*==============================*/
/* MidiEvent - PRIVATE FEATURES */
/*==============================*/

namespace MIDI {

    /**
     * the internal representation of a MIDI event
     */
    struct _MidiEventDescriptor {

        /** the time of the event (in an arbitrary unit, typically MIDI
         * ticks) */
        Integer time;

        /** the list of midi bytes */
        ByteList midiDataList;

    };

}

/*====================*/

using MIDI::_MidiEventDescriptor;

/*====================*/

Byte MidiEvent::_getDataByteNOLOG (IN Natural index) const
{
    _MidiEventDescriptor& self =
        TOREFERENCE<_MidiEventDescriptor>(_descriptor);
    ByteList& midiDataList = self.midiDataList;
    Assertion_pre(index < midiDataList.size(),
                  "midi data list must be long enough");
    Byte result = midiDataList.at(index);
    return result;
}

/*=============================*/
/* MidiEvent - PUBLIC FEATURES */
/*=============================*/

MidiEvent::MidiEvent (IN Integer eventTime,
                      IN ByteList midiData)
{
    Logging_trace2(">>: eventTime = %1, data = %2",
                   eventTime.toString(), midiData.toString());

    _descriptor = new _MidiEventDescriptor();
    _MidiEventDescriptor& self =
        TOREFERENCE<_MidiEventDescriptor>(_descriptor);
    self.time         = eventTime;
    self.midiDataList = midiData;

    Logging_trace("<<");
}

/*--------------------*/

MidiEvent::MidiEvent (IN MidiEvent& event)
{
    Logging_trace(">>");

    _descriptor = new _MidiEventDescriptor();
    _MidiEventDescriptor& self  =
        TOREFERENCE<_MidiEventDescriptor>(_descriptor);
    _MidiEventDescriptor& other =
        TOREFERENCE<_MidiEventDescriptor>(event._descriptor);
    self.time         = other.time;
    self.midiDataList = other.midiDataList;
    
    Logging_trace("<<");
}

/*--------------------*/

MidiEvent::~MidiEvent ()
{
    Logging_trace(">>");
    _MidiEventDescriptor* ptr = (_MidiEventDescriptor*) _descriptor;
    delete ptr;
    Logging_trace("<<");
}
        
/*--------------------*/
/* assignment         */
/*--------------------*/

MidiEvent& MidiEvent::operator= (IN MidiEvent& otherEvent)
{
    Logging_trace(">>");

    if (this != &otherEvent) {
        _MidiEventDescriptor& self =
            TOREFERENCE<_MidiEventDescriptor>(_descriptor);
        _MidiEventDescriptor& other =
            TOREFERENCE<_MidiEventDescriptor>(otherEvent._descriptor);
        self = other;
    }

    Logging_trace1("<<: %1", toString());
    return *this;
}

/*--------------------*/
/* conversion         */
/*--------------------*/

String MidiEvent::toString () const
{
    _MidiEventDescriptor& self =
        TOREFERENCE<_MidiEventDescriptor>(_descriptor);
    ByteList& dataList = self.midiDataList;
    Natural dataIndex = 0;
    Natural dataListLength = dataList.length();
    String dataListRepresentation;

    if (dataListLength == 0) {
        dataListRepresentation = "{}";
    } else {
        Byte eventByte = dataList.at(dataIndex++);
        Byte eventKind = _eventByteToKindCode(eventByte);
        Boolean hasTextString = false;
        dataListRepresentation = _eventKindCodeToString(eventKind);

        if (eventByte == '\xFF') {
            Byte metaEventByte = dataList.at(dataIndex++);
            dataListRepresentation +=
                " " + _metaEventByteToString(metaEventByte);
            hasTextString = _metaEventHasTextData(metaEventByte);
        }

        dataListRepresentation += (hasTextString ? " '" : "");

        while (dataIndex < dataListLength) {
            Byte value = dataList.at(dataIndex++);
            dataListRepresentation += (hasTextString
                                       ? Character{value}.toString()
                                       : " " + value.toString());
        }

        dataListRepresentation += (hasTextString ? "'" : "");
    }
    
    String result =
        STR::expand("MidiEvent(time = %1, data = %2)",
                    TOSTRING(self.time), dataListRepresentation);
    return result;
}

/*--------------------*/
/* comparison         */
/*--------------------*/

Boolean MidiEvent::operator < (IN MidiEvent& other) const
{
    const _MidiEventDescriptor& self =
        TOREFERENCE<_MidiEventDescriptor>(_descriptor);
    const _MidiEventDescriptor& otherRecord =
        TOREFERENCE<_MidiEventDescriptor>(other._descriptor);
    Integer timeA = self.time;
    Integer timeB = otherRecord.time;
    Boolean result;

    if (timeA < timeB) {
        result = true;
    } else if (timeA > timeB) {
        result = false;
    } else {
        Byte metaCode = '\xFF';
        Byte trackEndCode = '\x2F';
        Byte eventCodeA = self.midiDataList[0];
        Byte eventCodeB = otherRecord.midiDataList[0];
        Boolean isTrackEndA =
            (eventCodeA == metaCode
             && self.midiDataList[1] == trackEndCode);
        Boolean isTrackEndB =
            (eventCodeB == metaCode
             && otherRecord.midiDataList[1] == trackEndCode);

        if (isTrackEndA) {
            result = false;
        } else if (isTrackEndB) {
            result = true;
        } else {
            result = (eventCodeA >= eventCodeB);
        }
    }

    return result;
}
    
/*--------------------*/
/* property access    */
/*--------------------*/

Integer MidiEvent::time () const
{
    Logging_trace(">>");
    const _MidiEventDescriptor& self =
        TOREFERENCE<_MidiEventDescriptor>(_descriptor);
    Integer result = self.time;
    Logging_trace1("<<: %1", TOSTRING(result));
    return result;
}
        
/*--------------------*/

ByteList MidiEvent::rawData () const
{
    Logging_trace(">>");
    const _MidiEventDescriptor& self =
        TOREFERENCE<_MidiEventDescriptor>(_descriptor);
    const ByteList& result = self.midiDataList;
    Logging_trace1("<<: %1", result.toString());
    return result;
}

/*--------------------*/  

Byte MidiEvent::getDataByte (IN Natural index) const
{
    Logging_trace1(">>: index = %1", TOSTRING(index));
    Byte result = _getDataByteNOLOG(index);
    Logging_trace1("<<: %1", TOSTRING(result));
    return result;
}

/*--------------------*/

MidiEventKind MidiEvent::kind () const
{
    Logging_trace(">>");
    MidiEventKind result{_getDataByteNOLOG(0)};
    Logging_trace1("<<: %1", result.toString());
    return result;
}

/*--------------------*/

MidiMetaEventKind MidiEvent::metaKind () const
{
    Logging_trace(">>");
    MidiEventKind eventKind{_getDataByteNOLOG(0)};
    Assertion_pre(eventKind == MidiEventKind::meta,
                  "event kind must be 'meta'");
    MidiMetaEventKind result{_getDataByteNOLOG(1)};
    Logging_trace1("<<: %1", result.toString());
    return result;
}

/*--------------------*/

Natural MidiEvent::channel () const
{
    Logging_trace(">>");
    Natural result = (Natural) (_getDataByteNOLOG(0) & 0xF);
    Logging_trace1("<<: %1", TOSTRING(result));
    return result;
}

/*--------------------*/
/* property change    */
/*--------------------*/

void MidiEvent::setTime (IN Integer eventTime)
{
    Logging_trace1(">>: %1", TOSTRING(eventTime));
    _MidiEventDescriptor& self =
        TOREFERENCE<_MidiEventDescriptor>(_descriptor);
    self.time = eventTime;
    Logging_trace1("<<: %1", toString());
}
