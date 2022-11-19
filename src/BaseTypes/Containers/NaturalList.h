/**
 * @file
 * The <C>NaturalList</C> specification defines lists of
 * non-negative integer values with zero-based arbitrary indexed
 * access to positions in the list.
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
#include "GenericList.h"

/*--------------------*/

using std::initializer_list;
using BaseTypes::GenericTypes::GenericList;

/*====================*/

namespace BaseTypes::Containers {

    /**
     * A <C>NaturalList</C> object is a list of non-negative
     * integer values with arbitrary indexed access to positions in
     * the list.  Indexing starts at zero and is consecutive.
     * Lists also allow duplicate elements.
     */
    struct NaturalList : public GenericList<Natural>
    {

        /**
         * Initializes list of naturals from an initializer list
         * values.
         *
         * @param[in] list  initializer list of natural values
         * @return  list with values in order given
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
