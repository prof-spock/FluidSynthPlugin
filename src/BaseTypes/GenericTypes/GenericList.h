/**
 * @file
 * The <C>GenericList</C> specification and body defines a
 * generic list type of values with zero-based arbitrary indexed
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

#include <vector>

#include "Boolean.h"
#include "Integer.h"

/*--------------------*/

using std::vector;
using BaseTypes::Primitives::Boolean;
using BaseTypes::Primitives::Natural;
using BaseTypes::Primitives::Integer;
using BaseTypes::Primitives::String;

/*====================*/

namespace BaseTypes::GenericTypes {

    /**
     * A <C>GenericList</C> is a generic list type of values
     * with zero-based arbitrary indexed access to positions in the
     * list.  Indexing starts at zero and is consecutive.
     * Lists also allow duplicate elements.
     */
    template <typename ElementType>
    struct GenericList : public vector<ElementType>
    {
        /*--------------------*/
        /* con-/destruction   */
        /*--------------------*/

        /**
         * Initializes list to an empty list.
         */
        GenericList ()
        {
        }

        /*--------------------*/

        /**
         * Initializes list by data from <C>elementArray</C> with
         * <C>elementCount</C> entries.
         *
         * @param[in] elementArray  array of elements of underlying
         *                          type
         * @param[in] elementCount  count of elements to be used
         */
        GenericList (IN ElementType* elementArray,
                     IN Natural elementCount)
        {
            setLength(elementCount);

            for (Natural i = 0;  i < elementCount;  i++) {
                setAt(i, elementArray[i]);
            }
        }

        /*--------------------*/

        /**
         * Initializes list to a list of length
         * <C>initialSize</C>.
         *
         * @param[in] initialSize  initial length of list
         */
        GenericList (IN Natural initialSize)
        {
            setLength(initialSize);
        }

        /*--------------------*/

        /**
         * Initializes list by data from <C>otherList</C>
         *
         * @param[in] otherList  list to be copied into
         *                           current
         */
        GenericList (IN GenericList& otherList)
        {
            clear();
            append(otherList);
        }

        /*--------------------*/
        /* conversion         */
        /*--------------------*/

        /**
         * Returns pointer to n-th element of list.
         *
         * @param[in] position  first position to be addressed
         * @return  pointer to n-th element
         */
        ElementType* asArray (IN Natural position = 0)
        {
            return &(vector<ElementType>::data()[(size_t) position]);
        }

        /*--------------------*/

        /**
         * Returns read pointer to n-th element of list.
         *
         * @param[in] position  first position to be addressed
         * @return  read pointer to n-th element
         */
        const ElementType* asArray (IN Natural position = 0) const
        {
            return &(vector<ElementType>::data()[(size_t) position]);
        }

        /*--------------------*/
        /* property access    */
        /*--------------------*/

        /**
         * Returns value reference to list at <C>position</C>.
         *
         * @param[in] position  index position
         * @return  list value reference at index position
         */
        ElementType& operator [] (IN Natural position)
        {
            return vector<ElementType>::at((size_t) position);
        }

        /*--------------------*/

        /**
         * Returns value in list at <C>position</C>.
         *
         * @param[in] position  index position
         * @return  list value at index position
         */
        const ElementType& operator [] (IN Natural position) const
        {
            return vector<ElementType>::at((size_t) position);
        }

        /*--------------------*/

        /**
         * Returns value reference to list at <C>position</C>.
         *
         * @param[in] position  index position
         * @return  list value reference at index position
         */
        ElementType& at (IN Natural position)
        {
            return vector<ElementType>::at((size_t) position);
        }

        /*--------------------*/

        /**
         * Returns value in list at <C>position</C>.
         *
         * @param[in] position  index position
         * @return  list value at index position
         */
        const ElementType& at (IN Natural position) const
        {
            return vector<ElementType>::at((size_t) position);
        }

        /*--------------------*/
        /* property change    */
        /*--------------------*/

        /**
         * Assigns <C>otherList</C> to current.
         *
         * @param[in] otherList  other list to be assigned to
         *                           current list
         * @return  reference to current list
         */
        GenericList& operator= (const GenericList& otherList)
        {
            clear();
            append(otherList);
            return *this;
        }

        /*--------------------*/

        /**
         * Appends <C>element</C> as last element.
         * @param[in] element  element to be appended to list
         */
        void append (IN ElementType& element)
        {
            vector<ElementType>::emplace_back(element);
        }

        /*--------------------*/

        /**
         * Append <C>otherList</C> to current list.
         *
         * @param[in] otherList  other list to be appended to
         *                           current list
         */
        void append (IN GenericList& otherList)
        {
            for (Natural i = 0;  i < otherList.size();  i++) {
                const ElementType& element = otherList.at(i);
                append(element);
            }
        }

        /*--------------------*/

        /**
         * Clears current list by setting its length to zero.
         */
        void clear ()
        {
            vector<ElementType>::clear();
        }

        /*--------------------*/

        /**
         * Checks that list has at least <C>requiredLength</C>
         * elements.
         *
         * @param[in] requiredLength  required length of list
         */
        void ensureLength (IN Natural requiredLength)
        {
            if (requiredLength > length()) {
                setLength(requiredLength);
            }
        }

        /*--------------------*/

        /**
         * Prepends <C>element</C> as first element.
         * @param[in] element  element to be prepended to list
         */
        void prepend (IN ElementType& element)
        {
            vector<ElementType>::emplace(vector<ElementType>::begin(), element);
        }

        /*--------------------*/

        /**
         * Sets element at <C>position</C> to <C>value</C>.
         *
         * @param[in] position  index position
         * @param[in] value     new value at position
         */
        void setAt (IN Natural position, IN ElementType value)
        {
            at(position) = value;
        }

        /*--------------------*/

        /**
         * Resizes list to <C>newSize</C> elements.
         *
         * @param[in] newSize  new size of list
         */
        void setLength (IN Natural newSize)
        {
            vector<ElementType>::resize((size_t) newSize);
        }

        /*--------------------*/

        /**
         * Resizes list to <C>newSize</C> elements and fills it
         * with <C>value</C>.
         *
         * @param[in] newSize  new size of list
         * @param[in] value    value to be filled into list
         */
        void setLength (IN Natural newSize, IN ElementType value)
        {
            vector<ElementType>::resize((size_t) newSize, value);
        }

        /*--------------------*/

        /**
         * A <C>Comparator</C> type compares two objects and returns
         * -1 for a smaller first object, +1 for a smaller second object
         * and 0 for equality
         */
        typedef int Comparator (const void*, const void*);
        
        /*--------------------*/

        /**
         * Sorts current list in place by <C>comparator</C>.
         *
         * @param[in] comparator  comparison function returning -1 if
         *                        the first argument is less than the
         *                        second, +1 if the first argument is
         *                        greater than the second and 0 if the
         *                        arguments are equivalent
         */
        void sort (Comparator& comparator)
        {
            std::qsort(this->data(), this->size(), sizeof(ElementType), comparator);
        }

        /*--------------------*/
        /* measurement        */
        /*--------------------*/

        /**
         * Tells whether current list contains <C>element</C>.
         *
         * @param[in] element  element to be searched in list
         * @return  information whether <C>element</C> occurs as element
         *          in list
         */
        Boolean contains (IN ElementType& element) const
        {
            return (position(element) >= 0);
        }

        /*--------------------*/

        /**
         * Tells whether current list is empty.
         *
         * @return  information whether no <C>element</C> occurs in
         *          list
         */
        Boolean isEmpty () const
        {
            return (this->size() == 0);
        }

        /*--------------------*/

        /**
         * Returns the length of list.
         *
         * @return  length of list
         */
        Natural length () const
        {
            return Natural{vector<ElementType>::size()};
        }
        
        /*--------------------*/

        /**
         * Tells first position where list contains <C>element</C>;
         * returns -1 when not found
         *
         * @param[in] element  element to be searched in list
         * @return  first index position of <C>element</C> in list
         *          (starting with 0) or -1 when not found
         */
        Integer position (IN ElementType& element) const
        {
            Integer result{-1};

            for (Natural i = 0;  i < this->size();  i++) {
                if (at(i) == element) {
                    result = Integer{i};
                    break;
                }
            }

            return result;
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
             * Converts list to linear string representation
             * prefixed by <C>nameOfType</C> assuming an explicit
             * conversion from element type to string
             *
             * @param[in] nameOfType       name of list type
             * @param[in] elementToString  function for converting an
             *                             element to a string
             * @return  single string representation of list
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
