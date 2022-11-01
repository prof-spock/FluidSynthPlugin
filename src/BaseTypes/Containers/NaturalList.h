/**
 * @file
 * The <C>NaturalList</C> specification defines sequences of
 * non-negative integer values with zero-based arbitrary indexed
 * access to positions in the sequence.
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
     * A <C>NaturalList</C> object is a sequence of non-negative
     * integer values with arbitrary indexed access to positions in
     * the sequence.  Indexing starts at zero and is consecutive.
     * Lists also allow duplicate elements.
     */
    struct NaturalList : public GenericSequence<Natural, Natural::toString>
    {

        /**
         * Initializes sequence of naturals from an initializer list
         * values.
         *
         * @param list  initializer list of natural values
         * @return  sequence with values in order given
         */
        static NaturalList fromList (IN initializer_list<Natural> list);

        /*--------------------*/

        /**
         * Returns printable representation of list.
         *
         * @return string representation of list
         */
        String toString () const;

        /*--------------------*/

        /**
         * Returns the maximum value in list.
         *
         * @return  maximum value
         */
        Natural maximum () const;

    };

}
