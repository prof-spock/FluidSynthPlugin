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
#include "ElementToStringProc.h"
#include "Natural.h"
#include "StringProc.h"
#include "StringUtil.h"

/*--------------------*/

using std::set;
using BaseModules::StringUtil;
using BaseTypes::GenericTypes::ElementToStringProc;
using BaseTypes::GenericTypes::StringProc;

/*====================*/

namespace BaseTypes::GenericTypes {

    /**
     * A <C>GenericSet</C> object is a set of homogenous values with
     * no duplicates, only simple containment access and the ability
     * to add and remove elements.  Also a mapping may be supplied
     * that defines how to convert each element to a string for
     * printing out the set.
     */
    template <typename ElementType,
              ElementToStringProc<ElementType> elementToString = nullptr,
              StringProc nameOfType = nullptr>
    struct GenericSet : public set<ElementType> {

        /*--------------------------*/
        /* constructors/destructors */
        /*--------------------------*/

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
        virtual ~GenericSet ()
        {
        }

        /*--------------------*/
        /* type conversion    */
        /*--------------------*/

        /**
         * Converts set to linear string representation prefixed by
         * <C>nameOfType</C> template parameter; when string
         * conversion function <C>elementToString</C> is not defined,
         * some surrogate function is used
         *
         * @return  single string representation of set
         */
        String toString () const
        {
            String result = "";
            Natural i = 0;

            for (const ElementType& element : *this) {
                result += (i == 0 ? "" : ", ");

                if (elementToString == nullptr) {
                    result += "?" + std::to_string((int) i) + "?";
                } else {
                    result += (*elementToString)(element);
                }

                i++;
            }

            String typeName = (nameOfType == nullptr ? "Set"
                               : (*nameOfType)());
            result = StringUtil::expand("%1(%2)", typeName, result);
            return result;
        }

        /*--------------------*/
        /* measurement        */
        /*--------------------*/

        /**
         * Tells whether set is empty.
         *
         * @return information whether set is empty
         */
        Boolean isEmpty () const
        {
            return set<ElementType>::length() == 0;
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
        /* element access     */
        /*--------------------*/

        /**
         * Returns some element in set (when not empty)
         *
         * @return  some element
         */
        ElementType someElement ()
        {
            *(set<ElementType>::begin());
        }

        /*--------------------*/
        /* change             */
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

    };

}
