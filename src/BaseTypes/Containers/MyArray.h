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
    /** a qualified version of memset */
    #define CString_memset  memset

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
        (ElementType*) alloca((int) (count) * sizeof(ElementType))

    /*--------------------*/

    /**
     * Sets all elements in <C>array</C> of <C>count</C> elements of
     * type <C>ElementType</C> to zero.
     *
     * @param[in] array        element array to be cleared
     * @tparam    ElementType  type of element
     * @param[in] count        count of elements to be cleared
     */
    #define clearArray(array, ElementType, count) \
        CString_memset(array, 0, (int) (count) * sizeof(ElementType))

    /*--------------------*/

    /**
     * Copies <C>count</C> elements from array pointed to by
     * <C>sourcePtr</C> of type <C>ElementType</C> to array pointed to
     * by <C>targetPtr</C> (also of type <C>ElementType</C>).
     * Increments both pointers by <C>count</C>.
     *
     * @tparam       ElementType  element type of both arrays
     * @param[in]    sourcePtr    pointer to first source data element
     * @param[inout] targetPtr    pointer to first target data element
     * @param[in]    count        count of elements to be copied
     */
    template<typename ElementType>
    void copyArray (ElementType*& targetPtr,
                    const ElementType*& sourcePtr,
                    Natural count)
    {
        const Natural byteCount = Natural{sizeof(ElementType)} * count;
        CString_memcpy(targetPtr, sourcePtr, (size_t) byteCount);
        targetPtr += (size_t) count;
    }

    /*--------------------*/

    /**
     * Copies <C>count</C> elements from array pointed to by
     * <C>sourcePtr</C> of type <C>SourceElementType</C> to array
     * pointed to by <C>targetPtr</C> of type
     * <C>TargetElementType</C>.  Increments both pointers by
     * <C>count</C>.
     *
     * @tparam     TargetElementType  element type of target array
     * @tparam     SourceElementType  element type of source array
     * @param[in]  sourcePtr          pointer to first source data
     *                                element
     * @param[out] targetPtr          pointer to first target data
     *                                element
     * @param[in]  count              count of elements to be copied
     */
    template<typename TargetElementType, typename SourceElementType>
    void convertArray (OUT TargetElementType* targetPtr,
                       IN SourceElementType* sourcePtr,
                       IN Natural count)
    {
        for (Natural i = 0;  i < count;  i++) {
            *targetPtr++ = (TargetElementType) *sourcePtr++;
        }
    }

}
