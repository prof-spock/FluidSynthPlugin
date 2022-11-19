/**
 * @file
 * The <C>StringList</C> specification defines lists of string
 * values with zero-based arbitrary indexed access to positions in the
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

/*--------------------*/

using std::initializer_list;
using BaseTypes::GenericTypes::GenericList;

/*====================*/

namespace BaseTypes::Containers {

    /**
     * A <C>StringList</C> object is a list of string values with
     * arbitrary indexed access to positions in the list.
     * Indexing starts at zero and is consecutive.  Lists also allow
     * duplicate elements.
     */
    struct StringList : public GenericList<String> {

        /**
         * Initializes list of strings from an initializer list
         * of values.
         *
         * @param[in] list  initializer list of string values
         * @return  list with values in order given
         */
        static StringList fromList (IN initializer_list<String> list);

        /*--------------------*/

        /**
         * Returns printable representation of list.
         *
         * @return string representation of list
         */
        String toString () const;

        /*--------------------*/

        /**
         * Concatenates all elements in list with <C>separator</C> and
         * returns them.
         *
         * @param[in] separator  string to be used as an element
         *                       separator
         * @return  string with all elements concatenated each by the
         *          separator
         */
        String join (IN String& separator) const;

        /*--------------------*/

        /**
         * Makes a string list by splitting <C>st</C> at
         * <C>separator</C>.
         *
         * @param[in] st         string to be split by <C>separator</C>
         * @param[in] separator  string to be used as a separator for
         *                       <C>st</C>
         * @return  string list with parts of <C>st</C> in order
         */
        static StringList makeBySplit (IN String& st,
                                       IN String& separator);

    };

}
