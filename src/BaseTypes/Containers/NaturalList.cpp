/**
 * @file
 * The <C>NaturalList</C> body implements sequences of non-negative
 * integer values with zero-based arbitrary indexed access to
 * positions in the sequence.
 *
 * @author Dr. Thomas Tensi
 * @date   2020-08
 */

/*=========*/
/* IMPORTS */
/*=========*/

#include <cstdarg>
#include "NaturalList.h"

/*--------------------*/

using BaseTypes::Containers::NaturalList;

/*====================*/

NaturalList NaturalList::fromList (IN initializer_list<Natural> list)
{
    NaturalList result{};

    for (Natural element : list) {
        result.append(element);
    }
            
    return result;
}

/*--------------------*/

String NaturalList::toString () const
{
    return _toString("NaturalList");
}

/*--------------------*/

Natural NaturalList::maximum () const
{
    Natural maximumValue = 0;

    for (Natural i = 0;  i < size();  i++) {
        const Natural value = at(i);
        maximumValue = (maximumValue < value ? value : maximumValue);
    }

    return maximumValue;
}
