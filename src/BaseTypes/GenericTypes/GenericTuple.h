/**
 * @file
 * The <C>GenericTuple</C> specification and body defines a
 * generic tuple type of values with zero-based arbitrary indexed
 * access to positions in the tuple and a fixed length.
 *
 * @author Dr. Thomas Tensi
 * @date   2020-08
 */

/*====================*/

#pragma once

/*=========*/
/* IMPORTS */
/*=========*/

#include <array>
#include "ElementToStringProc.h"
#include "Integer.h"
#include "StringProc.h"
#include "StringUtil.h"

/*--------------------*/

using std::array;

using BaseTypes::GenericTypes::ElementToStringProc;
using BaseTypes::GenericTypes::StringProc;
using BaseModules::StringUtil;

/*====================*/

namespace BaseTypes::GenericTypes {

    /**
     * A <C>GenericTuple</C> is a generic tuple type of values with
     * zero-based arbitrary indexed access to positions in the tuple
     * and a fixed length.  Indexing starts at zero and is
     * consecutive.  Tuples also allow duplicate elements.
     *
     * If the printing of the tuple shall be possible the
     * <C>elementToString</C> function must be provided.
     */
    template <typename ElementType,
              size_t elementCount,
              ElementToStringProc<ElementType> elementToString = nullptr,
              StringProc nameOfType = nullptr>
    struct GenericTuple : public array<ElementType, elementCount>
    {

        /*--------------------*/
        /* type conversion    */
        /*--------------------*/

        /**
         * Converts tuple to linear string representation
         * prefixed by <C>nameOfType</C> template parameter; when string
         * conversion function <C>elementToString</C> is not defined,
         * some surrogate function is used
         *
         * @return  single string representation of tuple
         */
        String toString () const
        {
            String result = "";

            for (Natural i = 0;  i < length();  i++) {
                result += (i == 0 ? "" : ", ");

                if (elementToString == nullptr) {
                    result += "?" + std::to_string((int) i) + "?";
                } else {
                    const ElementType& element = at(i);
                    result += (*elementToString)(element);
                }
            }

            String typeName = (nameOfType == nullptr ? "Tuple"
                               : (*nameOfType)());
            result = StringUtil::expand("%1(%2)", typeName, result);
            return result;
        }

        /*--------------------*/
        /* data access        */
        /*--------------------*/

        /**
         * Returns value in tuple at <C>position</C>.
         *
         * @param[in] position  index position
         * @return  tuple value at index position
         */
        const ElementType& operator [] (IN Natural position) const
        {
            return array<ElementType, elementCount>::at((size_t) position);
        }

        /*--------------------*/

        /**
         * Returns value in tuple at <C>position</C>.
         *
         * @param[in] position  index position
         * @return  tuple value at index position
         */
        const ElementType& at (IN Natural position) const
        {
            return array<ElementType, elementCount>::at((size_t) position);
        }

        /*--------------------*/
        /* data change        */
        /*--------------------*/

        /**
         * Returns value reference to tuple at <C>position</C>.
         *
         * @param[in] position  index position
         * @return  tuple value reference at index position
         */
        ElementType& operator [] (IN Natural position)
        {
            return array<ElementType, elementCount>::at((size_t) position);
        }

        /*--------------------*/

        /**
         * Returns value reference to tuple at <C>position</C>.
         *
         * @param[in] position  index position
         * @return  tuple value reference at index position
         */
        ElementType& at (IN Natural position)
        {
            return array<ElementType, elementCount>::at((size_t) position);
        }

        /*--------------------*/

        /**
         * Sets element at <C>position</C> to <C>value</C>.
         *
         * @param[in] position  index position
         * @param[in] value     new value at position
         */
        void set (IN Natural position, IN ElementType value)
        {
            at(position) = value;
        }

        /*--------------------*/
        /* measurement        */
        /*--------------------*/

        /**
         * Tells whether current tuple contains <C>element</C>.
         *
         * @param[in] element  element to be searched in tuple
         * @return  information whether <C>element</C> occurs as element in
         *          tuple
         */
        Boolean contains (IN ElementType& element) const
        {
            return (position(element) >= 0);
        }

        /*--------------------*/

        /**
         * Tells first position where tuple contains <C>element</C>;
         * returns -1 when not found
         *
         * @param[in] element  element to be searched in tuple
         * @return  first index position of <C>element</C> in tuple
         *          (starting with 0) or -1 when not found
         */
        Integer position (IN ElementType& element) const
        {
            Integer result{-1};

            for (Natural i = 0;  i < elementCount;  i++) {
                if (at(i) == element) {
                    result = Integer{i};
                    break;
                }
            }

            return result;
        }

        /*--------------------*/

        /**
         * Returns the length of tuple.
         *
         * @return  length of tuple
         */
        Natural length () const
        {
            return Natural{elementCount};
        }
        
    };

}
