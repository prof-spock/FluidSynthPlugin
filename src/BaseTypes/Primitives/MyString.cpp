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
#include "StringUtil.h"

/*--------------------*/

using BaseTypes::Primitives::String;

/** abbreviation for StringUtil */
using STR = BaseModules::StringUtil;

/*====================*/

namespace BaseTypes::Primitives {

    String String_toString (IN String& st)
    {
        return "'" + st + "'";
    }

    /*--------------------*/

    String String_normalize (IN String& st,
                             IN String& lineSeparator)
    {
        String result = st;
        STR::replace(result, "\n", lineSeparator);
        return result;
    }

}
