/**
 * @file
 * The <C>RealList</C> specification defines sequences of real values
 * with zero-based arbitrary indexed access to positions in the
 * sequence.
 *
 * @author Dr. Thomas Tensi
 * @date   2020-08
 */

/*====================*/

#pragma once

/*=========*/
/* IMPORTS */
/*=========*/

#include <initializer_list>
#include "GenericSequence.h"
#include "Real.h"

/*--------------------*/

using std::initializer_list;
using BaseTypes::Primitives::Real;

/*====================*/

namespace BaseTypes::Containers {

    /**
     * A <C>RealList</C> object is a sequence of real values with
     * arbitrary indexed access to positions in the sequence.
     * Indexing starts at zero and is consecutive.  Lists also allow
     * duplicate elements.
     */
    struct RealList : public GenericSequence<Real, Real::toString>
    {

        /**
         * Initializes sequence of reals from initializer list of
         * values.
         *
         * @param list  initializer list of real values
         * @return  sequence with values in order given
         */
        static RealList fromList (IN initializer_list<Real> list);

        /*--------------------*/

        /**
         * Returns printable representation of list.
         *
         * @return string representation of list
         */
        String toString () const;

        /*--------------------*/

        /**
         * Fills list with <C>value</C>.
         *
         * @param value  value to be used for filling list
         */
        void fill (IN Real value);

        /*--------------------*/

        /**
         * Multiply list elements by <C>value</C>.
         *
         * @param value  value to be used for multiplying each element
         *               with
         */
        void multiply (IN Real value);

    };

}
