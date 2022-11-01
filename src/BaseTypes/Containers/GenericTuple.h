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
#include "Integer.h"

/*--------------------*/

using std::array;

using BaseTypes::Primitives::Natural;
using BaseTypes::Primitives::Integer;
using BaseTypes::Primitives::String;

/*====================*/

namespace BaseTypes::Containers {

    /**
     * A <C>GenericTuple</C> is a generic tuple type of values with
     * zero-based arbitrary indexed access to positions in the tuple
     * and a fixed length.  Indexing starts at zero and is
     * consecutive.  Tuples also allow duplicate elements.
     *
     * If the printing of the tuple shall be possible the
     * <C>elementToString</C> function must be provided.
     */
    template <typename T,
              size_t elementCount,
              String elementToString (IN T&) = nullptr>
    struct GenericTuple : public array<T, elementCount>
    {

        /*--------------------*/
        /* operators          */
        /*--------------------*/

        /**
         * Returns value reference to tuple at <C>position</C>.
         *
         * @param[in] position  index position
         * @return  tuple value reference at index position
         */
        T& operator [] (IN Natural position)
        {
            return array<T, elementCount>::at((size_t) position);
        }

        /*--------------------*/

        /**
         * Returns value in tuple at <C>position</C>.
         *
         * @param[in] position  index position
         * @return  tuple value at index position
         */
        const T& operator [] (IN Natural position) const
        {
            return array<T, elementCount>::at((size_t) position);
        }

        /*--------------------*/
        /* Methods            */
        /*--------------------*/

        /**
         * Returns value reference to tuple at <C>position</C>.
         *
         * @param[in] position  index position
         * @return  tuple value reference at index position
         */
        T& at (IN Natural position)
        {
            return array<T, elementCount>::at((size_t) position);
        }

        /*--------------------*/

        /**
         * Returns value in tuple at <C>position</C>.
         *
         * @param[in] position  index position
         * @return  tuple value at index position
         */
        const T& at (IN Natural position) const
        {
            return array<T, elementCount>::at((size_t) position);
        }

        /*--------------------*/

        /**
         * Tells whether current tuple contains <C>element</C>.
         *
         * @param[in] element  element to be searched in tuple
         * @return  information whether <C>element</C> occurs as element in
         *          tuple
         */
        Boolean contains (IN T& element) const
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
        Integer position (IN T& element) const
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
        
        /*--------------------*/

        /**
         * Sets element at <C>position</C> to <C>value</C>.
         *
         * @param[in] position  index position
         * @param[in] value     new value at position
         */
        void setAt (IN Natural position, IN T value)
        {
            at(position) = value;
        }

        /*--------------------*/
        /*--------------------*/

        protected:

            /**
             * Converts tuple to linear string representation
             * prefixed by <C>nameOfType</C> assuming an explicit
             * conversion from element type to string
             *
             * @param nameOfType  name of tuple type
             * @return  single string representation of tuple
             */
            String _toString (IN String& nameOfType) const
            {
                String result = "";

                for (const T& element : *this) {
                    result += (result.size() == 0 ? "" : ", ");
                    result += elementToString(element);
                }

                result = nameOfType + "(" + result + ")";
                return result;
            }

    };

}
