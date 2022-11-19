/**
 * @file
 * The <C>Dictionary</C> specification defines maps from string keys
 * to string values.
 *
 * @author Dr. Thomas Tensi
 * @date   2020-08
 */

/*====================*/

#pragma once

/*=========*/
/* IMPORTS */
/*=========*/

#include "GenericMap.h"

/*--------------------*/

using BaseTypes::GenericTypes::GenericMap;

/*====================*/

namespace BaseTypes::Containers {

    /**
     * A <C>Dictionary</C> object maps string keys to string values.
     * Every key is associated with at most one value, where the key
     * identity is defined by standard string comparison.
     */
    struct Dictionary : public GenericMap<String, String> {

        /**
         * Makes dictionary from list of strings <C>list</C>
         * taking even-indexed entries as keys and odd-indexed
         * entries as associated values.
         *
         * @param[in] list  list of strings with keys at even and
         *                  values at odd indexes
         * @return  dictionary with keys and values as given in
         *          <C>list</C>
         * @pre list.size() % 2 == 0
         */
        static Dictionary makeFromList (IN StringList& list);

        /*--------------------*/

        /**
         * Makes dictionary from a string <C>st</C>
         * gained by serializing a dictionary via <C>toString</C>;
         *
         * @param[in] st                 string with serialized form of
         *                               dictionary
         * @param[in] entrySeparator     separator between different entries
         * @param[in] keyValueSeparator  separator between a key and its
         *                               value
         * @return  dictionary with keys and values as given in
         *          <C>st</C>
         */
        static
        Dictionary makeFromString (
            IN String& st,
            IN String& entrySeparator = StringUtil::entrySeparator,
            IN String& keyValueSeparator = StringUtil::keyValueSeparator);

        /*--------------------*/

        /**
         * Returns printable representation of dictionary.
         *
         * @return string representation of dictionary
         */
        String toString () const;

    };

}
