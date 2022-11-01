/**
 * @file
 * The <C>String</C> body implements a standard string type.
 *
 * @author Dr. Thomas Tensi
 * @date   2020-08
 */

/*=========*/
/* IMPORTS */
/*=========*/

#include "MyString.h"

/*--------------------*/

using BaseTypes::Primitives::String;

/*====================*/

namespace BaseTypes::Primitives {

    String String_toString (IN String& st)
    {
        return "'" + st + "'";
    }

}
