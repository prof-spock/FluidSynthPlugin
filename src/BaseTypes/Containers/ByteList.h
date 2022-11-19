/**
 * @file
 * The <C>ByteList</C> specification defines lists of byte values
 * with zero-based arbitrary indexed access to positions in the
 * list.
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
#include "GenericList.h"

/*--------------------*/

using std::initializer_list;
using BaseTypes::GenericTypes::GenericList;
using BaseTypes::Primitives::Byte;

/*====================*/

namespace BaseTypes::Containers {

    /**
     * A <C>ByteList</C> object is a list of byte values with
     * arbitrary indexed access to positions in the list.
     * Indexing starts at zero and is consecutive.  Lists also allow
     * duplicate elements.
     */
    struct ByteList : public GenericList<Byte> {

        /**
         * Initializes list of bytes from an initializer list
         * values.
         *
         * @param[in] list  initializer list of byte values
         * @return  list with values in order given
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
