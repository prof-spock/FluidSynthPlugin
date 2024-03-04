/**
 * @file
 * The <C>RealList</C> body implements lists of real values
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
#include "RealList.h"

/*--------------------*/

using BaseTypes::Containers::RealList;

/*====================*/

String BaseTypes::Containers::_realListTypeName ()
{
    return "RealList";
}

/*--------------------*/
/* constructors       */
/*--------------------*/

RealList RealList::fromList (IN initializer_list<Real> list)
{
    RealList result{};

    for (Real element : list) {
        result.append(element);
    }

    return result;
}

/*--------------------*/
/* element change     */
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
