/**
 * @file
 * The <C>StringSet</C> specification defines sets of string values
 * with no duplicates and only simple containment access and the
 * ability to add and remove elements.
 *
 * @author Dr. Thomas Tensi
 * @date   2020-08
 */

/*====================*/

#pragma once

/*=========*/
/* IMPORTS */
/*=========*/

#include <set>
#include "StringList.h"

/*--------------------*/

using std::set;

/*====================*/

namespace BaseTypes::Containers {

    /**
     * A <C>StringSet</C> object is a set of string values with no
     * duplicates and only simple containment access and the ability
     * to add and remove elements.
     */
    struct StringSet : public set<String> {

        /**
         * Makes an empty string set.
         */
        StringSet ();

        /*--------------------*/

        /**
         * Destroys current string set.
         */
        ~StringSet ();

        /*--------------------*/

        /**
         * Makes string set from list of strings <C>list</C>.
         *
         * @param list  list of strings to be included in set
         * @return  string set with data from <C>list</C>
         */
        static StringSet makeFromList (IN StringList& list);

        /*--------------------*/

        /**
         * Returns printable representation of string set.
         *
         * @return string representation of string set
         */
        String toString () const;

        /*--------------------*/

        /**
         * Tells whether <C>element</C> occurs in current set.
         *
         * @param element  string to be searched for
         * @return information whether <C>element</C> is in set
         */
        Boolean contains (IN String& element) const;

        /*--------------------*/

        /**
         * Adds <C>element</C> to current set.  Does nothing if
         * element is already in current set.
         *
         * @param element  string to be included in set
         */
        void add (IN String& element);

        /*--------------------*/

        /**
         * Removes <C>element</C> from current set.  Does nothing if
         * element is not in current set.
         *
         * @param element  string to be excluded in set
         */
        void remove (IN String& element);

    };

}
