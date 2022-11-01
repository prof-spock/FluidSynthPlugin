/**
 * @file
 * The <C>GenericSequence</C> specification and body defines a
 * generic sequence type of values with zero-based arbitrary indexed
 * access to positions in the sequence.
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

namespace BaseTypes::Containers {

    /**
     * A <C>GenericSequence</C> is a generic sequence type of values
     * with zero-based arbitrary indexed access to positions in the
     * sequence.  Indexing starts at zero and is consecutive.
     * Sequences also allow duplicate elements.
     *
     * If the printing of the sequence shall be possible the
     * <C>elementToString</C> function must be provided.
     */
    template <typename T, String elementToString (IN T&) = nullptr>
    struct GenericSequence : public vector<T>
    {
        /*--------------------*/
        /* con-/destruction   */
        /*--------------------*/

        /**
         * Initializes sequence to an empty sequence.
         */
        GenericSequence ()
        {
        }

        /*--------------------*/

        /**
         * Initializes sequence by data from <C>elementArray</C> with
         * <C>elementCount</C> entries.
         *
         * @param[in] elementArray  array of elements of underlying
         *                          type
         * @param[in] elementCount  count of elements to be used
         */
        GenericSequence (IN T* elementArray,
                         IN Natural elementCount)
        {
            setLength(elementCount);

            for (Natural i = 0;  i < elementCount;  i++) {
                setAt(i, elementArray[i]);
            }
        }

        /*--------------------*/

        /**
         * Initializes sequence to a sequence of length
         * <C>initialSize</C>.
         *
         * @param[in] initialSize  initial length of sequence
         */
        GenericSequence (IN Natural initialSize)
        {
            setLength(initialSize);
        }

        /*--------------------*/

        /**
         * Initializes sequence by data from <C>otherSequence</C>
         *
         * @param[in] otherSequence  sequence to be copied into
         *                           current
         */
        GenericSequence (IN GenericSequence& otherSequence)
        {
            clear();
            append(otherSequence);
        }

        /*--------------------*/
        /* conversion         */
        /*--------------------*/

        /**
         * Returns pointer to n-th element of sequence.
         *
         * @param position  first position to be addressed
         * @return  pointer to n-th element
         */
        T* asArray (IN Natural position = 0)
        {
            return &(vector<T>::data()[(size_t) position]);
        }

        /*--------------------*/

        /**
         * Returns read pointer to n-th element of sequence.
         *
         * @param position  first position to be addressed
         * @return  read pointer to n-th element
         */
        const T* asArray (IN Natural position = 0) const
        {
            return &(vector<T>::data()[(size_t) position]);
        }

        /*--------------------*/
        /* property access    */
        /*--------------------*/

        /**
         * Returns value reference to sequence at <C>position</C>.
         *
         * @param[in] position  index position
         * @return  sequence value reference at index position
         */
        T& operator [] (IN Natural position)
        {
            return vector<T>::at((size_t) position);
        }

        /*--------------------*/

        /**
         * Returns value in sequence at <C>position</C>.
         *
         * @param[in] position  index position
         * @return  sequence value at index position
         */
        const T& operator [] (IN Natural position) const
        {
            return vector<T>::at((size_t) position);
        }

        /*--------------------*/

        /**
         * Returns value reference to sequence at <C>position</C>.
         *
         * @param[in] position  index position
         * @return  sequence value reference at index position
         */
        T& at (IN Natural position)
        {
            return vector<T>::at((size_t) position);
        }

        /*--------------------*/

        /**
         * Returns value in sequence at <C>position</C>.
         *
         * @param[in] position  index position
         * @return  sequence value at index position
         */
        const T& at (IN Natural position) const
        {
            return vector<T>::at((size_t) position);
        }

        /*--------------------*/
        /* property change    */
        /*--------------------*/

        /**
         * Assigns <C>otherSequence</C> to current.
         *
         * @param[in] otherSequence  other sequence to be assigned to
         *                           current sequence
         * @return  reference to current sequence
         */
        GenericSequence& operator= (const GenericSequence& otherSequence)
        {
            clear();
            append(otherSequence);
            return *this;
        }

        /*--------------------*/

        /**
         * Appends <C>element</C> as last element.
         * @param[in] element  element to be appended to sequence
         */
        void append (IN T& element)
        {
            vector<T>::emplace_back(element);
        }

        /*--------------------*/

        /**
         * Append <C>otherSequence</C> to current sequence.
         *
         * @param[in] otherSequence  other sequence to be appended to
         *                           current sequence
         */
        void append (IN GenericSequence& otherSequence)
        {
            for (Natural i = 0;  i < otherSequence.size();  i++) {
                const T& element = otherSequence.at(i);
                append(element);
            }
        }

        /*--------------------*/

        /**
         * Clears current sequence by setting its length to zero.
         */
        void clear ()
        {
            vector<T>::clear();
        }

        /*--------------------*/

        /**
         * Checks that sequence has at least <C>requiredLength</C>
         * elements.
         *
         * @param[in] requiredLength  required length of sequence
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
         * @param[in] element  element to be prepended to sequence
         */
        void prepend (IN T& element)
        {
            vector<T>::emplace(vector<T>::begin(), element);
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

        /**
         * Resizes sequence to <C>newSize</C> elements.
         *
         * @param[in] newSize  new size of sequence
         */
        void setLength (IN Natural newSize)
        {
            vector<T>::resize((size_t) newSize);
        }

        /*--------------------*/

        /**
         * Resizes sequence to <C>newSize</C> elements and fills it
         * with <C>value</C>.
         *
         * @param[in] newSize  new size of sequence
         * @param[in] value    value to be filled into sequence
         */
        void setLength (IN Natural newSize, IN T value)
        {
            vector<T>::resize((size_t) newSize, value);
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
         * Sorts current sequence in place by <C>comparator</C>.
         *
         * @param[in] comparator  comparison function returning -1 if
         *                        the first argument is less than the
         *                        second, +1 if the first argument is
         *                        greater than the second and 0 if the
         *                        arguments are equivalent
         */
        void sort (Comparator& comparator)
        {
            std::qsort(this->data(), this->size(), sizeof(T), comparator);
        }

        /*--------------------*/
        /* measurement        */
        /*--------------------*/

        /**
         * Tells whether current sequence contains <C>element</C>.
         *
         * @param[in] element  element to be searched in sequence
         * @return  information whether <C>element</C> occurs as element
         *          in sequence
         */
        Boolean contains (IN T& element) const
        {
            return (position(element) >= 0);
        }

        /*--------------------*/

        /**
         * Tells whether current sequence is empty.
         *
         * @return  information whether no <C>element</C> occurs in
         *          sequence
         */
        Boolean isEmpty () const
        {
            return (this->size() == 0);
        }

        /*--------------------*/

        /**
         * Returns the length of sequence.
         *
         * @return  length of sequence
         */
        Natural length () const
        {
            return Natural{vector<T>::size()};
        }
        
        /*--------------------*/

        /**
         * Tells first position where sequence contains <C>element</C>;
         * returns -1 when not found
         *
         * @param[in] element  element to be searched in sequence
         * @return  first index position of <C>element</C> in sequence
         *          (starting with 0) or -1 when not found
         */
        Integer position (IN T& element) const
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

            /**
             * Converts sequence to linear string representation
             * prefixed by <C>nameOfType</C> assuming an explicit
             * conversion from element type to string
             *
             * @param nameOfType  name of sequence type
             * @return  single string representation of sequence
             */
            String _toString (IN String& nameOfType) const
            {
                String result = "";

                for (const T& element : *this) {
                    result += (result.size() == 0 ? "" : ", ");
                    result += (elementToString == nullptr
                               ? "???" : elementToString(element));
                }

                result = nameOfType + "(" + result + ")";
                return result;
            }

    };

}
