/**
 * @file
 * The <C>MyArray</C> specification and body defines a
 * generic array type for stack allocated arrays represented
 * by a pointer to the first element.
 *
 * @author Dr. Thomas Tensi
 * @date   2021-07
 */

/*====================*/

#pragma once

/*=========*/
/* IMPORTS */
/*=========*/

#include <cstring>
    /** a qualified version of memcpy */
    #define CString_memcpy  memcpy

#include "Natural.h"

/*--------------------*/

using BaseTypes::Primitives::Natural;

/*====================*/

namespace BaseTypes::Containers {

    /**
     * Allocates an array of <C>count</C> elements of type
     * <C>ElementType</C> on the stack.
     *
     * @tparam    ElementType  type of element
     * @param[in] count        count of elements to be allocated
     * @return array with <C>count</C> entries of type <C>ElementType</C>
     */
    #define makeLocalArray(ElementType, count) \
        static_cast<ElementType*>(alloca(sizeof(ElementType) \
                                  * (size_t)(count)))

    /*--------------------*/

    /**
     * Sets all elements in <C>array</C> of <C>count</C> elements of
     * type <C>ElementType</C> to zero.
     *
     * @tparam    ElementType  type of element
     * @param[in] array        element array to be cleared
     * @param[in] count        count of elements to be cleared
     * @param[in] zero         zero value in given element type
     */
    template<typename ElementType>
    void clearArray (ElementType* array,
                     Natural count,
                     ElementType zero)
    {
        for (Natural i = 0;  i < count;  i++) {
            *array++ = zero;
        }
    }

    /*--------------------*/

    /**
     * Copies <C>count</C> elements from array pointed to by
     * <C>sourcePtr</C> of type <C>ElementType</C> to array pointed to
     * by <C>destinationPtr</C> (also of type <C>ElementType</C>).
     * Increments both pointers by <C>count</C>.
     *
     * @tparam       ElementType     element type of both arrays
     * @param[in]    sourcePtr       pointer to first source data element
     * @param[inout] destinationPtr  pointer to first data element in
     *                               destination
     * @param[in]    count           count of elements to be copied
     */
    template<typename ElementType>
    void copyArray (ElementType*& destinationPtr,
                    const ElementType*& sourcePtr,
                    Natural count)
    {
        const Natural byteCount = Natural{sizeof(ElementType)} * count;
        CString_memcpy(destinationPtr, sourcePtr, (size_t) byteCount);
        destinationPtr += (size_t) count;
    }

    /*--------------------*/

    /**
     * Copies <C>count</C> elements from array pointed to by
     * <C>sourcePtr</C> of type <C>SourceElementType</C> to array
     * pointed to by <C>destinationPtr</C> of type
     * <C>DestinationElementType</C>.  Increments both pointers by
     * <C>count</C>.
     *
     * @tparam     DestinationElementType  element type of destination array
     * @tparam     SourceElementType       element type of source array
     * @param[in]  sourcePtr               pointer to first source data
     *                                     element
     * @param[out] destinationPtr          pointer to first data element
     *                                     in destination
     * @param[in]  count                   count of elements to be copied
     */
    template<typename DestinationElementType, typename SourceElementType>
    void convertArray (OUT DestinationElementType* destinationPtr,
                       IN SourceElementType* sourcePtr,
                       IN Natural count)
    {
        for (Natural i = 0;  i < count;  i++) {
            *destinationPtr++ = (DestinationElementType) *sourcePtr++;
        }
    }

}
