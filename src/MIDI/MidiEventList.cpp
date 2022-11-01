/**
 * @file
 * The <C>MIDIEventList</C> body implements sequences of integer values
 * with zero-based arbitrary indexed access to positions in the
 * sequence.
 *
 * @author Dr. Thomas Tensi
 * @date   2020-08
 */

/*=========*/
/* IMPORTS */
/*=========*/

#include <cstdarg>

#include "Logging.h"
#include "MidiEventList.h"
#include "Object.h"

/*--------------------*/

using MIDI::MidiEventList;

/*====================*/
/* PRIVATE FEATURES   */
/*====================*/

/**
 * A comparator for two midi events (disguised as void pointers).
 *
 * @param[in] a  first midi event
 * @param[in] b  second midi event
 * @return  comparison formation returning -1 the first event is less
 *          than the second, +1 for vice versa and 0 if the events are
 *          equivalent
 */
static int _midiEventComparator (IN void* a, IN void* b)
{
    MidiEvent& eventA = TOREFERENCE<MidiEvent>((Object) a);
    MidiEvent& eventB = TOREFERENCE<MidiEvent>((Object) b);
    int result = 0;

    if (eventA < eventB) {
        result = -1;
    } else if (eventB < eventA) {
        result = +1;
    }

    return result;
}
    
/*====================*/
/* PUBLIC FEATURES    */
/*====================*/

/*--------------------*/
/* construction       */
/*--------------------*/

MidiEventList MidiEventList::fromList (IN initializer_list<MidiEvent> list)
{
    MidiEventList result{};

    for (MidiEvent element : list) {
        result.append(element);
    }
            
    return result;
}

/*--------------------*/
/* change             */
/*--------------------*/

String MidiEventList::toString () const
{
    return _toString("MidiEventList");
}

/*--------------------*/
/* change             */
/*--------------------*/

void MidiEventList::merge (IN MidiEventList& other)
{
    const Natural currentLength{length()};
    const Natural otherLength{other.length()};
    Logging_trace4(">>: list = %1, count = %2,"
                   " other = %3, other count = %4",
                   this->toString(), TOSTRING(currentLength),
                   other.toString(), TOSTRING(otherLength));

    MidiEventList temp;
    Natural i = 0;
    Natural j = 0;

    while (i < currentLength || j < otherLength) {
        Boolean currentIsAppended;

        if (i == currentLength) {
            currentIsAppended = false;
        } else if (j == otherLength) {
            currentIsAppended = true;
        } else {
            const MidiEvent& eventA = at(i);
            const MidiEvent& eventB = other.at(j);
            
            if (eventA < eventB) {
                currentIsAppended = true;
            } else if (eventB < eventA) {
                currentIsAppended = false;
            } else {
                currentIsAppended = true;
            }
        }

        if (currentIsAppended) {
            temp.append(at(i++));
        } else {
            temp.append(other.at(j++));
        }
    }
    
    this->clear();
    this->append(temp);
    Logging_trace1("<<: count = %1", TOSTRING(length()));
}

/*--------------------*/

void MidiEventList::shiftEventTimes (IN Integer offset)
{
    Logging_trace1(">>: %1", TOSTRING(offset));

    for (MidiEvent& event : *this) {
        event.setTime(event.time() + offset);
    }
            
    Logging_trace("<<");
}
