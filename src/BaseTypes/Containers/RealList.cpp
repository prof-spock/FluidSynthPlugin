/**
 * @file
 * The <C>RealList</C> body implements sequences of real values
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
#include "RealList.h"

/*--------------------*/

using BaseTypes::Containers::RealList;

/*====================*/

RealList RealList::fromList (IN initializer_list<Real> list)
{
    RealList result{};

    for (Real element : list) {
        result.append(element);
    }

    return result;
}

/*--------------------*/

String RealList::toString () const
{
    return _toString("RealList");
}

/*--------------------*/

void RealList::fill (IN Real value)
{
    std::fill(begin(), end(), value);
}

/*--------------------*/

void RealList::multiply (IN Real value)
{
    for (Real& entry : *this) {
        entry *= value;
    }
}
