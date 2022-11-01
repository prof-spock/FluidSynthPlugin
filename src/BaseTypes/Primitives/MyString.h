/**
 * @file
 * The <C>String</C> specification defines a standard string type.
 *
 * @author Dr. Thomas Tensi
 * @date   2020-08
 */

/*====================*/

#pragma once

/*=========*/
/* IMPORTS */
/*=========*/

#include <string>
#include "GlobalMacros.h"

/*====================*/

namespace BaseTypes::Primitives {

    /**
     * The <C>String</C> type represents (Unicode) character
     * sequences.
    */
    using String = std::string;

    /*--------------------*/

    /**
    * Returns simple printable string representation for <C>st</C>.
    * 
    * @param[in] st  string to be converted
    * @return string representation
    */
    String String_toString (IN String& st);

}
