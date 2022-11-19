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

IntegerList IntegerList::fromList (IN initializer_list<Integer> list)
{
    IntegerList result{};

    for (Integer element : list) {
        result.append(element);
    }
            
    return result;
}

/*--------------------*/

String IntegerList::toString () const
{
    return _toString("IntegerList", Integer::toString);
}
