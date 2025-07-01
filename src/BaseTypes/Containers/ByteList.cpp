/**
 * @file
 * The <C>ByteList</C> body implements sequences of byte values with
 * zero-based arbitrary indexed access to positions in the sequence.
 *
 * @author Dr. Thomas Tensi
 * @date   2022-08
 */

/*=========*/
/* IMPORTS */
/*=========*/

#include <cstdarg>

#include "ByteList.h"
#include "StringUtil.h"

/*--------------------*/

using BaseTypes::Containers::ByteList;

/** abbreviation for StringUtil */
using STR = BaseModules::StringUtil;

/*====================*/

String BaseTypes::Containers::_byteListTypeName ()
{
    return "ByteList";
}
    
/*--------------------*/
/* constructors       */
/*--------------------*/

ByteList ByteList::fromList (IN initializer_list<Byte> list)
{
    ByteList result{};

    for (Byte element : list) {
        result.append(element);
    }
            
    return result;
}

/*--------------------*/

ByteList ByteList::fromString (IN String& st)
{
    const Natural byteCount = st.length();
    ByteList result;
    result.setLength(byteCount);

    for (Natural i = 0;  i < byteCount;  i++) {
        result.set(i, (Byte) (char) STR::characterAt(st, i));
    }

    return result;
}

/*--------------------*/
/* type conversions   */
/*--------------------*/

String ByteList::decodeToString () const
{
    String result;
    Natural byteCount = length();
    result.resize((int) byteCount);

    for (Natural i = 0;  i < byteCount;  i++) {
        result[(int) i] = (char) at(i);
    }

    return result;
}

/*--------------------*/

String ByteList::decodeToBase64String () const
{
    const Natural byteCount = length();
    static const char characterList[] =
        "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
    static const Character paddingCharacter = '=';

    /* find length which is a multiple of three */
    const Natural regularLength = byteCount - byteCount % 3;
    Natural i = 0;
    uint32_t value = 0;
    Boolean isInFirstPart = true;
    String result = "";
    int sourceShiftIndex = 0;

    while (isInFirstPart && i < regularLength
           || !isInFirstPart && i < byteCount) {
        uint8_t byte = (char) at(i++);
        uint8_t sourceShift = 8 * (2 - sourceShiftIndex++);
        value += (byte << sourceShift);

        if (sourceShiftIndex == 3 || i == byteCount) {
            uint32_t mask = 0x00FC0000;
            
            for (uint8_t k = 0;  k <= sourceShiftIndex;  k++) {
                uint8_t destinationShift = 6 * (3 - k);
                size_t maskedValue = (value & mask) >> destinationShift;
                Character ch{characterList[maskedValue]};
                STR::append(result, ch);
                mask >>= 6;
            }

            value = 0;
            sourceShiftIndex = 0;
        }

        isInFirstPart = (i < regularLength);
    }

    while (result.length() % 4 != 0) {
        STR::append(result, paddingCharacter);
    }

    return result;
}

/*--------------------*/

StringList
ByteList::asStringListWithBase (IN Natural base,
                                IN Natural precision,
                                IN String& padString) const
{
    StringList result;

    for (Byte element : *this) {
        Natural v = (char) element;
        String st = v.toStringWithBase(base, precision, padString);
        result.append(st);
    }

    return result;
}
