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
#include "ElementToStringProc.h"
#include "Integer.h"
#include "StringProc.h"

/*--------------------*/

using std::vector;
using BaseTypes::GenericTypes::ElementToStringProc;
using BaseTypes::GenericTypes::StringProc;
using BaseTypes::Primitives::Boolean;
using BaseTypes::Primitives::Natural;
using BaseTypes::Primitives::Integer;

/*====================*/

namespace BaseTypes::GenericTypes {

    /**
     * A <C>GenericList</C> is a generic list type of values with
     * zero-based arbitrary indexed access to positions in the list.
     * Indexing starts at zero and is consecutive.  Lists also allow
     * duplicate elements..  Also a mapping may be supplied that
     * defines how to convert each element to a string for printing
     * out the list.
     */
    template <typename ElementType,
              ElementToStringProc<ElementType> elementToString = nullptr,
              StringProc nameOfType = nullptr>
    struct GenericList : public vector<ElementType>
    {

        /** an abbreviation for the underlying vector type */
        typedef vector<ElementType> _ElementTypeVector;
        
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

        /**
         * Initializes list by data from <C>elementArray</C> with
         * <C>elementCount</C> entries.
         *
         * @param[in] elementArray  array of elements of underlying
         *                          type
         * @param[in] elementCount  count of elements to be used
         */
        static GenericList fromArray (IN ElementType* elementArray,
                                      IN Natural elementCount)
        {
            GenericList result{elementCount};

            for (Natural i = 0;  i < elementCount;  i++) {
                result.set(i, elementArray[i]);
            }

            return result;
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
            return &(_ElementTypeVector::data()[(size_t) position]);
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
            return &(_ElementTypeVector::data()[(size_t) position]);
        }

        /*--------------------*/

        /**
         * Converts list to linear string representation prefixed by
         * <C>nameOfType</C> template parameter; when string
         * conversion function <C>elementToString</C> is not defined,
         * some surrogate function is used
         *
         * @return  single string representation of list
         */
        String toString () const
        {
            String result = "";

            for (Natural i = 0;  i < length();  i++) {
                result += (i == 0 ? "" : ", ");

                if constexpr(elementToString == nullptr) {
                    result += "?" + std::to_string((int) i) + "?";
                } else {
                    const ElementType& element = at(i);
                    result += (*elementToString)(element);
                }
            }

            String typeName = (nameOfType == nullptr ? "List"
                               : (*nameOfType)());
            result = typeName + "(" + result + ")";
            return result;
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
            return _ElementTypeVector::at((size_t) position);
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
            return _ElementTypeVector::at((size_t) position);
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
            return _ElementTypeVector::at((size_t) position);
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
            return _ElementTypeVector::at((size_t) position);
        }

        /*--------------------*/

        /**
         * Returns first value in list.
         *
         * @return  list value at first position
         */
        const ElementType& first () const
        {
            return at(0);
        }

        /*--------------------*/

        /**
         * Returns last value in list.
         *
         * @return  list value at last position
         */
        const ElementType& last () const
        {
            return at(length() - 1);
        }

        /*--------------------*/

        /**
         * Returns slice of list starting at <C>firstPosition</C> up
         * to - but not including - <C>lastPosition</C>; in Python
         * style negative indices are allowed signifying counting from
         * the end
         *
         * @param[in] firstPosition  index position of first element
         *                           in list for result
         * @param[in] lastPosition   index position of first element
         *                           from list not included in result
         * @return sublist slice
         */
        static GenericList slice (IN GenericList& list,
                                  IN Integer firstPosition,
                                  IN Integer lastPosition
                                                 = Integer::maximumValue())
        {
            const Natural listLength = list.length();
            const Natural first =
                (firstPosition >= 0 ?
                 Natural{firstPosition}
                 : listLength - Natural{-firstPosition});
            Natural last =
                (lastPosition >= 0
                 ? Natural{lastPosition}
                 : listLength - Natural{-lastPosition});
            last = Natural::maximum(first,
                                    Natural::minimum(last, listLength));
            const Natural newSize = last - first;

            GenericList result{newSize};
            Natural i = 0;
            Natural j = first;

            while (i < newSize) {
                result.set(i++, list.at(j++));
            }

            return result;
        }

        /*--------------------*/
        /* property change    */
        /*--------------------*/

        /**
         * Appends <C>element</C> as last element.
         * @param[in] element  element to be appended to list
         */
        void append (IN ElementType& element)
        {
            _ElementTypeVector::emplace_back(element);
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
            _ElementTypeVector::clear();
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
            _ElementTypeVector::emplace(_ElementTypeVector::begin(),
                                         element);
        }

        /*--------------------*/

        /**
         * Prepend <C>otherList</C> to current list.
         *
         * @param[in] otherList  other list to be prepended to
         *                       current list
         */
        void prepend (IN GenericList& otherList)
        {
            const Natural otherLength = otherList.length();

            if (otherLength > 0) {
                const Natural listLength = length();
                setLength(listLength + otherLength);

                /* shift original data to the right */
                for (Natural i = listLength;  i > 0;) {
                    i--;
                    set(i + otherLength, at(i));
                }
                
                for (Natural i = 0;  i < otherLength;  i++) {
                    set(i, otherList.at(i));
                }
            }
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

        /**
         * Resizes list to <C>newSize</C> elements.
         *
         * @param[in] newSize  new size of list
         */
        void setLength (IN Natural newSize)
        {
            _ElementTypeVector::resize((size_t) newSize);
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
            _ElementTypeVector::resize((size_t) newSize, value);
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
            std::qsort(this->data(), this->size(),
                       sizeof(ElementType), comparator);
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
            return Natural{_ElementTypeVector::size()};
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

        /**
         * Tells first position where list contains <C>otherList</C>;
         * returns -1 when not found
         *
         * @param[in] otherList  other list to be searched in list
         * @return  first index position of <C>otherList</C> in list
         *          (starting with 0) or -1 when not found
         */
        Integer position (IN GenericList& otherList) const
        {
            const Natural currentLength = length();
            const Natural otherLength   = otherList.length();
            Integer result{-1};

            if (otherLength <= currentLength) {
                /* do a naive search */
                for (Natural i = 0;  i <= currentLength - otherLength;  i++) {
                    Boolean isFound = true;

                    for (Natural j = 0;  j < otherLength;  j++) {
                        if (at(i + j) != otherList.at(j)) {
                            isFound = false;
                            break;
                        }
                    }

                    if (isFound) {
                        result = i;
                        break;
                    }
                }
            }

            return result;
        }

        /*--------------------*/

        /**
         * Simple copy function for copying lists with same element type
         *
         * @param[inout] targetList  list where elements should go
         * @param[in]    sourceList  list where elements come from
         */
        template <typename ListTypeA, typename ListTypeB>
        static void assign (INOUT ListTypeA& targetList,
                            IN ListTypeB& sourceList)
        {
            targetList.clear();

            for (auto element : sourceList) {
                targetList.emplace_back(element);
            }
        }

    };

}
