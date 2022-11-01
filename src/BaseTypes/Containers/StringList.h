/**
 * @file
 * The <C>StringList</C> specification defines sequences of string
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

#include "GenericSequence.h"

/*--------------------*/

using BaseTypes::Primitives::String_toString;

/*====================*/

namespace BaseTypes::Containers {

    /**
     * A <C>StringList</C> object is a sequence of string values with
     * arbitrary indexed access to positions in the sequence.
     * Indexing starts at zero and is consecutive.  Lists also allow
     * duplicate elements.
     */
    struct StringList : public GenericSequence<String, String_toString> {

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
