/**
 * @file
 * The <C>MidiPresetIdentification</C> body implements the
 * identification of a MIDI preset as a tuple of bank and program
 * number encoded as a string.
 *
 * @author Dr. Thomas Tensi
 * @date   2024-04
 */

/*=========*/
/* IMPORTS */
/*=========*/

#include "Logging.h"
#include "MidiPresetIdentification.h"

/*--------------------*/

using MIDI::MidiPresetIdentification;

/** abbreviation for StringUtil */
using STR = BaseModules::StringUtil;

/*====================*/

/*--------------------*/
/* construction       */
/*--------------------*/

MidiPresetIdentification::MidiPresetIdentification (IN String& st)
    : _data{st}
{
}

/*--------------------*/

MidiPresetIdentification
::MidiPresetIdentification (IN Natural bankNumber,
                            IN Natural programNumber)
    : _data{STR::expand("%1:%2",
                        TOSTRING(bankNumber), TOSTRING(programNumber))}
{
}

/*--------------------*/
/* type conversion    */
/*--------------------*/

String MidiPresetIdentification::toString () const
{
    return _data;
}

/*--------------------*/
/* measurement        */
/*--------------------*/

Boolean MidiPresetIdentification::isEmpty () const
{
    Logging_trace1(">>: %1", toString());
    Boolean result = (_data == "");
    Logging_trace1("<<: %1", TOSTRING(result));
    return result;
}

/*---------------------*/

Boolean MidiPresetIdentification::isValid () const
{
    Logging_trace1(">>: %1", toString());

    Natural bankNumber;
    Natural programNumber;
    Boolean isOkay = split(bankNumber, programNumber);

    Logging_trace1("<<: %1", TOSTRING(isOkay));
    return isOkay;
}

/*--------------------*/
/* change             */
/*--------------------*/

void MidiPresetIdentification::clear ()
{
    Logging_trace1(">>: %1", toString());
    _data = "";
    Logging_trace1("<<: %1", toString());
}

/*---------------------*/
/* complex calculation */
/*---------------------*/

Boolean
MidiPresetIdentification::split (OUT Natural& bankNumber,
                                 OUT Natural& programNumber) const
{
    Logging_trace1(">>: %1", toString());

    Boolean isOkay = true;
    const String separator = ":";
    String st;

    bankNumber = 0;

    if (!STR::contains(_data, separator)) {
        st = _data;
    } else {
        String bankNum;
        String progNum;
        STR::splitAt(_data, separator, bankNum, progNum);
        Boolean bankIsEmpty = (bankNum == "");

        if (!bankIsEmpty) {
            if (STR::isNatural(bankNum)) {
                bankNumber = STR::toNatural(bankNum);
            } else {
                isOkay = false;
            }
        }

        st = progNum;
    }

    programNumber = 0;

    if (isOkay) {
        isOkay = STR::isNatural(st);

        if (isOkay) {
            programNumber = STR::toNatural(st);
        }
    }    

    Logging_trace1("<<: %1", TOSTRING(isOkay));
    return isOkay;
}    
