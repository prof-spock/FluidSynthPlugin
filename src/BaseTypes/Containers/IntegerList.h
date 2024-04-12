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
#include "GenericList.h"

/*--------------------*/

using std::initializer_list;
using BaseTypes::GenericTypes::GenericList;

/*====================*/

namespace BaseTypes::Containers {

    /**
     * Returns name of integer list type
     *
     * @return type name
     */
    String _integerListTypeName ();
    
    /*--------------------*/

    /**
     * An <C>IntegerList</C> object is a list of integer values
     * with arbitrary indexed access to positions in the list.
     * Indexing starts at zero and is consecutive.  Lists also allow
     * duplicate elements.
     */
    struct IntegerList
        : public GenericList<Integer,
                             Integer::toString,
                             _integerListTypeName > {

        /*--------------------*/
        /* constructors       */
        /*--------------------*/

        /**
         * Initializes list of integers from an initializer list
         * of values.
         *
         * @param[in] list  initializer list of integer values
         * @return  list with values in order given
         */
        static IntegerList fromList (IN initializer_list<Integer> list);

        /*--------------------*/
        /* functions          */
        /*--------------------*/

        /**
         * Returns the maximum value in list.
         *
         * @return  maximum value
         */
        Integer maximum () const;

        /*--------------------*/

        /**
         * Returns the minimum value in list.
         *
         * @return  minimum value
         */
        Integer minimum () const;

    };

}
