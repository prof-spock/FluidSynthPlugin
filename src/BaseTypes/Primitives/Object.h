/**
 * @file
 * The <C>Object</C> specification and body provides the root object
 * type.
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

/*====================*/

namespace BaseTypes::Primitives {

    /**
     * The <C>Object</C> type is the root class for all pointer
     * objects.
     */
    using Object = void*;

    /*--------------------*/

    /**
     * The <C>ObjectSet</C> type represents sets of (pointer) objects.
     */
    using ObjectSet = std::set<Object>;

}

/*--------------------*/

/**
 * Converts <C>object</C> - a void pointer - to a type reference.
 * 
 * @param[in] object  object to be converted to reference
 * @return reference of appropriate type
 */
template <typename Type>
Type& TOREFERENCE (BaseTypes::Primitives::Object object)
{
    return *((Type*) object);
}
