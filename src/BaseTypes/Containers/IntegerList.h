/**
 * @file
 * The <C>IntegerList</C> specification defines sequences of integer
 * values with zero-based arbitrary indexed access to positions in the
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

/*--------------------*/

using std::initializer_list;

/*====================*/

namespace BaseTypes::Containers {

    /**
     * An <C>IntegerList</C> object is a sequence of integer values
     * with arbitrary indexed access to positions in the sequence.
     * Indexing starts at zero and is consecutive.  Lists also allow
     * duplicate elements.
     */
    struct IntegerList : public GenericSequence<Integer, Integer::toString>
    {

        /**
         * Initializes sequence of integers from an initializer list
         * of values.
         *
         * @param list  initializer list of integer values
         * @return  sequence with values in order given
         */
        static IntegerList fromList (IN initializer_list<Integer> list);

        /*--------------------*/

        /**
         * Returns printable representation of list.
         *
         * @return string representation of list
         */
        String toString () const;

    };

}
