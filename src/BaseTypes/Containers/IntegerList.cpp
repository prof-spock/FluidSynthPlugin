/**
 * @file
 * The <C>IntegerList</C> body implements lists of integer values
 * with zero-based arbitrary indexed access to positions in the
 * list.
 *
 * @author Dr. Thomas Tensi
 * @date   2020-08
 */

/*=========*/
/* IMPORTS */
/*=========*/

#include <cstdarg>
#include "IntegerList.h"

/*--------------------*/

using BaseTypes::Containers::IntegerList;

/*====================*/

String BaseTypes::Containers::_integerListTypeName ()
{
    return "IntegerList";
}

/*--------------------*/
/* constructors       */
/*--------------------*/

IntegerList IntegerList::fromList (IN initializer_list<Integer> list)
{
    IntegerList result{};

    for (Integer element : list) {
        result.append(element);
    }
            
    return result;
}

/*--------------------*/
/* functions          */
/*--------------------*/

Integer IntegerList::maximum () const
{
    Integer maximumValue = -Integer::maximumValue();

    for (Natural i = 0;  i < size();  i++) {
        const Integer value = at(i);
        maximumValue = (maximumValue < value ? value : maximumValue);
    }

    return maximumValue;
}

/*--------------------*/

Integer IntegerList::minimum () const
{
    Integer minimumValue = Integer::maximumValue();

    for (Natural i = 0;  i < size();  i++) {
        const Integer value = at(i);
        minimumValue = (minimumValue > value ? value : minimumValue);
    }

    return minimumValue;
}
