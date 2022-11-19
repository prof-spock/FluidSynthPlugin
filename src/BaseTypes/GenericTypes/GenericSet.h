/**
 * @file
 * The <C>GenericSet</C> specification and body defines a generic set
 * type for values with no duplicates, only simple containment access
 * and the ability to add and remove elements.
 *
 * @author Dr. Thomas Tensi
 * @date   2022-11
 */

/*====================*/

#pragma once

/*=========*/
/* IMPORTS */
/*=========*/

#include <set>
#include "Boolean.h"

/*--------------------*/

using std::set;
using BaseTypes::Primitives::Boolean;
using BaseTypes::Primitives::String;

/*====================*/

namespace BaseTypes::GenericTypes {

    /**
     * A <C>GenericSet</C> object is a set of homogenous values with
     * no duplicates, only simple containment access and the ability
     * to add and remove elements.
     */
    template <typename ElementType>    
    struct GenericSet : public set<ElementType> {

        /**
         * Makes an empty set.
         */
        GenericSet ()
        {
        }

        /*--------------------*/

        /**
         * Destroys current set.
         */
        ~GenericSet ()
        {
        }

        /*--------------------*/

        /**
         * Removes all elements from current set.
         */
        void clear ()
        {
            set<ElementType>::clear();
        }

        /*--------------------*/

        /**
         * Tells whether <C>element</C> occurs in current set.
         *
         * @param element  string to be searched for
         * @return information whether <C>element</C> is in set
         */
        Boolean contains (IN ElementType& element) const
        {
            return (set<ElementType>::find(element)
                    != set<ElementType>::end());
        }

        /*--------------------*/

        /**
         * Adds <C>element</C> to current set.  Does nothing if
         * element is already in current set.
         *
         * @param element  element to be included in set
         */
        void add (IN ElementType& element)
        {
            set<ElementType>::insert(element);
        }

        /*--------------------*/

        /**
         * Removes <C>element</C> from current set.  Does nothing if
         * element is not in current set.
         *
         * @param element  element to be excluded from set
         */
        void remove (IN ElementType& element)
        {
            set<ElementType>::erase(element);
        }

        /*--------------------*/
        /*--------------------*/

        protected:

            /*--------------------*/
            /* conversion         */
            /*--------------------*/

            /** a function type for converting an element to a string */
            typedef String ElementToStringProc (IN ElementType&);

            /*--------------------*/

            /**
             * Converts set to linear string representation
             * prefixed by <C>nameOfType</C> assuming an explicit
             * conversion from element type to string
             *
             * @param nameOfType       name of set type
             * @param elementToString  function for converting an
             *                         element to a string
             * @return  single string representation of set
             */
            String _toString (IN String& nameOfType,
                              IN ElementToStringProc elementToString) const
            {
                String result = "";

                for (const ElementType& element : *this) {
                    result += (result.size() == 0 ? "" : ", ");
                    result += (elementToString == nullptr
                               ? "???" : elementToString(element));
                }

                result = nameOfType + "(" + result + ")";
                return result;
            }

    };

}
