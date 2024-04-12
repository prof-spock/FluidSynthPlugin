/**
 * @file
 * The <C>RealList</C> specification defines lists of real values
 * with zero-based arbitrary indexed access to positions in the
 * list.
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
#include "Real.h"

/*--------------------*/

using std::initializer_list;
using BaseTypes::GenericTypes::GenericList;
using BaseTypes::Primitives::Real;

/*====================*/

namespace BaseTypes::Containers {

    /**
     * Returns name of real list type
     *
     * @return type name
     */
    String _realListTypeName ();
    
    /*--------------------*/

    /**
     * A <C>RealList</C> object is a list of real values with
     * arbitrary indexed access to positions in the list.
     * Indexing starts at zero and is consecutive.  Lists also allow
     * duplicate elements.
     */
    struct RealList
        : public GenericList<Real,
                             Real::toString,
                             _realListTypeName > {

        /*--------------------*/
        /* constructors       */
        /*--------------------*/

        /**
         * Initializes list of reals from initializer list of
         * values.
         *
         * @param[in] list  initializer list of real values
         * @return  list with values in order given
         */
        static RealList fromList (IN initializer_list<Real> list);

        /*--------------------*/
        /* element change     */
        /*--------------------*/

        /**
         * Fills list with <C>value</C>.
         *
         * @param[in] value  value to be used for filling list
         */
        void fill (IN Real value);

        /*--------------------*/

        /**
         * Multiply list elements by <C>value</C>.
         *
         * @param[in] value  value to be used for multiplying each
         *                   element with
         */
        void multiply (IN Real value);

    };

}
