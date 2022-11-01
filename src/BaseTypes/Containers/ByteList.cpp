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

ByteList ByteList::fromList (IN initializer_list<Byte> list)
{
    ByteList result{};

    for (Byte element : list) {
        result.append(element);
    }
            
    return result;
}

/*--------------------*/

String ByteList::toString () const
{
    return _toString("ByteList");
}
