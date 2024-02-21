/**
 * @file
 * The <C>StringLiteral</C> specification and body defines a technical
 * class used as string parameter type in template declarations based
 * on
 * https://ctrpeach.io/posts/cpp20-string-literal-template-parameters/
 *
 * @author Dr. Thomas Tensi
 * @date   2023-10
 */

/*====================*/

#pragma once

/*=========*/
/* IMPORTS */
/*=========*/

#include <algorithm>
#include "MyString.h"

/*--------------------*/

using BaseTypes::Primitives::String;

/*====================*/

namespace BaseTypes::GenericTypes {

    /**
     * A technical class used as string parameter type in template
     * declarations
     */
    template <size_t count>
    struct StringLiteral {

        constexpr StringLiteral (const char (&str)[count])
        {
            std::copy_n(str, count, _value);
        }

        /*--------------------*/

        String toString () const
        {
            return String{_value};
        }

        /*--------------------*/

        char _value[count];

    };

}
