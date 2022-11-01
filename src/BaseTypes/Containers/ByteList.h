/**
 * @file
 * The <C>ByteList</C> specification defines sequences of byte values
 * with zero-based arbitrary indexed access to positions in the
 * sequence.
 *
 * @author Dr. Thomas Tensi
 * @date   2022-08
 */

/*====================*/

#pragma once

/*=========*/
/* IMPORTS */
/*=========*/

#include <initializer_list>
#include "Byte.h"
#include "GenericSequence.h"

/*--------------------*/

using std::initializer_list;
using BaseTypes::Primitives::Byte;

/*====================*/

namespace BaseTypes::Containers {

    /**
     * A <C>ByteList</C> object is a sequence of byte values with
     * arbitrary indexed access to positions in the sequence.
     * Indexing starts at zero and is consecutive.  Lists also allow
     * duplicate elements.
     */
    struct ByteList : public GenericSequence<Byte, Byte::toString>
    {

        /**
         * Initializes sequence of bytes from an initializer list
         * values.
         *
         * @param list  initializer list of byte values
         * @return  sequence with values in order given
         */
        static ByteList fromList (IN initializer_list<Byte> list);

        /*--------------------*/

        /**
         * Returns printable representation of list.
         *
         * @return string representation of list
         */
        String toString () const;

    };

}
