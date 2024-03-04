/**
 * @file
 * The <C>StringProc</C> specification and body defines a
 * function type returning some string.
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
     * A <C>StringProc</C> is a function type returning some string.
     */
    typedef String (*StringProc) (void);

}
