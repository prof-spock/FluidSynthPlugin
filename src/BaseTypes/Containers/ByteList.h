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
#include "StringList.h"

/*--------------------*/

using std::initializer_list;

using BaseTypes::Containers::StringList;
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
    struct ByteList :
        public GenericList<Byte,
                           Byte::toString,
                           StringLiteral{"ByteList"}> {

        /*--------------------*/
        /* constructors       */
        /*--------------------*/

        /**
         * Initializes list of bytes from an initializer list
         * values.
         *
         * @param[in] list  initializer list of byte values
         * @return  list with values in order given
         */
        static ByteList fromList (IN initializer_list<Byte> list);

        /*--------------------*/
        /* type conversions   */
        /*--------------------*/

        /**
         * Converts to plain string with 8-bit-encoding
         *
         * @return decoded string
         */
        String decodeToString () const;

        /*--------------------*/

        /**
         * Returns representation of <C>list</C> as string list
         * relative to some <C>base</C>.
         *
         * @param[in] list       byte list to be converted
         * @param[in] base       conversion base
         * @param[in] precision  minimum count of digits per element
         * @param[in] padString  string for filling up the strings
         * @return string representation relative to base
         */
        StringList
        asStringListWithBase (IN Natural base,
                              IN Natural precision = 0,
                              IN String padString = "0") const;

    };

}
