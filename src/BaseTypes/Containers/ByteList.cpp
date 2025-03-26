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

/*--------------------*/

using BaseTypes::Containers::ByteList;

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
