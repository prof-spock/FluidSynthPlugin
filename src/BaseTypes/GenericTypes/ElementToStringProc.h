/**
 * @file
 * The <C>ElementToStringProc</C> specification and body defines a
 * function type mapping some element type to a string.
 *
 * @author Dr. Thomas Tensi
 * @date   2023-10
 */

/*====================*/

#pragma once

/*=========*/
/* IMPORTS */
/*=========*/

#include "MyString.h"

/*--------------------*/

using BaseTypes::Primitives::String;

/*====================*/

namespace BaseTypes::GenericTypes {

    /**
     * An <C>ElementToStringProc</C> is a function type mapping some
     * element type to a string.
     */
    template <typename ElementType>
    using ElementToStringProc = String (*) (IN ElementType&);

}
