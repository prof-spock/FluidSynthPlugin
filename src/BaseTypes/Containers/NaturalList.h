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

#include "StringList.h"

/*--------------------*/

using BaseTypes::Containers::StringList;

/*====================*/

namespace BaseTypes::Containers {

    /**
     * Returns name of natural list type
     *
     * @return type name
     */
    String _naturalListTypeName ();
    
    /*--------------------*/

    /**
     * A <C>NaturalList</C> object is a list of non-negative
     * integer values with arbitrary indexed access to positions in
     * the list.  Indexing starts at zero and is consecutive.
     * Lists also allow duplicate elements.
     */
    struct NaturalList
        : public GenericList<Natural,
                             Natural::toString,
                             _naturalListTypeName > {

        /*--------------------*/
        /* constructors       */
        /*--------------------*/

        /**
         * Initializes list of naturals from an initializer list
         * values.
         *
         * @param[in] list  initializer list of natural values
         * @return  list with values in order given
         */
        static NaturalList fromList (IN initializer_list<Natural> list);

        /*--------------------*/
        /* type conversion    */
        /*--------------------*/

        /**
         * Returns representation of <C>list</C> as string list
         * relative to some <C>base</C>.
         *
         * @param[in] list       natural list to be converted
         * @param[in] base       conversion base
         * @param[in] precision  minimum count of digits per element
         * @param[in] padString  string for filling up the strings
         * @return string representation relative to base
         */
        StringList
        asStringListWithBase (IN Natural base,
                              IN Natural precision = 0,
                              IN String padString = "0") const;

        /*--------------------*/
        /* data access        */
        /*--------------------*/

        /**
         * Returns slice of list starting at <C>firstPosition</C> up
         * to - but not including - <C>lastPosition</C>; in Python
         * style negative indices are allowed signifying counting from
         * the end
         *
         * @param[in] firstPosition  index position of first element
         *                           of result
         * @param[in] lastPosition   index position of first element
         *                           not included in result
         * @return  sublist slice
         */
        NaturalList slice (IN Integer firstPosition,
                           IN Integer lastPosition
                                          = Integer::maximumValue());

        /*--------------------*/
        /* functions          */
        /*--------------------*/

        /**
         * Returns the maximum value in list.
         *
         * @return  maximum value
         */
        Natural maximum () const;

        /*--------------------*/

        /**
         * Returns the minimum value in list.
         *
         * @return  minimum value
         */
        Natural minimum () const;

    };

}
