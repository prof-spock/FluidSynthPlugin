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

    /*--------------------*/

    /**
    * Returns <C>st</C> with newlines replaced by
    * <C>lineSeparator</C>.
    * 
    * @param[in] st             string to be converted
    * @param[in] lineSeparator  replacement string for newlines
    * @return string with newlines replaced
    */
    String String_normalize (IN String& st,
                             IN String& lineSeparator = "§");

}
