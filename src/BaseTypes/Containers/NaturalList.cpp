/**
 * @file
 * The <C>NaturalList</C> body implements lists of non-negative
 * integer values with zero-based arbitrary indexed access to
 * positions in the list.
 *
 * @author Dr. Thomas Tensi
 * @date   2020-08
 */

/*=========*/
/* IMPORTS */
/*=========*/

#include <cstdarg>

#include "NaturalList.h"
#include "StringUtil.h"

/*--------------------*/

using BaseTypes::Containers::NaturalList;

/** abbreviation for StringUtil */
using STR = BaseModules::StringUtil;

/*====================*/

String BaseTypes::Containers::_naturalListTypeName ()
{
    return "NaturalList";
}

/*--------------------*/
/* constructors       */
/*--------------------*/

NaturalList NaturalList::fromList (IN initializer_list<Natural> list)
{
    NaturalList result{};

    for (Natural element : list) {
        result.append(element);
    }
            
    return result;
}

/*--------------------*/
/* type conversion    */
/*--------------------*/

StringList
NaturalList::asStringListWithBase (IN Natural base,
                                   IN Natural precision,
                                   IN String& padString) const
{
    StringList result;

    for (Natural element : *this) {
        String st = element.toStringWithBase(base, precision, padString);
        result.append(st);
    }

    return result;
}

/*--------------------*/
/* data access        */
/*--------------------*/

NaturalList NaturalList::slice (IN Integer firstPosition,
                                IN Integer lastPosition)
{
    return NaturalList{GenericList::slice(*this,
                                          firstPosition,
                                          lastPosition)};
}

/*--------------------*/
/* functions          */
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

/*--------------------*/

Natural NaturalList::minimum () const
{
    Natural minimumValue = Natural::maximumValue();

    for (Natural i = 0;  i < size();  i++) {
        const Natural value = at(i);
        minimumValue = (minimumValue > value ? value : minimumValue);
    }

    return minimumValue;
}
