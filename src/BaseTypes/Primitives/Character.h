/**
 * @file
 * The <C>Character</C> specification and body provides a value object
 * type for character values.
 *
 * @author Dr. Thomas Tensi
 * @date   2022-08
 */

/*====================*/

#pragma once

/*=========*/
/* IMPORTS */
/*=========*/

#include <cmath>
#include "Byte.h"

/*====================*/

namespace BaseTypes::Primitives {

    /**
     * A <C>Character</C> object wraps a value of the associated
     * primitive type char into an object.
     */
    struct Character : GenericNumber<Character, char> {

        /*--------------------*/
        /* constructors       */
        /*--------------------*/

        /**
         * Initializes character to an unspecified value.
         */
        Character ()
        {
        }

        /*--------------------*/

        /**
         * Initializes character from primitive char <C>c</C>.
         *
         * @param[in] c  char value
         */
        Character (IN char c)
        {
            _value = c;
        }

        /*--------------------*/

        /**
         * Initializes character from byte value <C>b</C>.
         *
         * @param[in] b  byte value giving character code
         */
        Character (IN Byte b)
        {
            _value = (char) b;
        }

        /*--------------------*/

        /**
         * Initializes character from natural value <C>n</C>.
         *
         * @param[in] n  natural value giving character code
         */
        Character (IN Natural n)
        {
            _value = (char) (size_t) n;
        }

        /*--------------------*/
        /* type conversions   */
        /*--------------------*/

        /**
         * Returns string representation of character.
         *
         * @return  string representation
         */
        String toString () const
        {
            String result;
            result += _value;
            return result;
        }

    };

}
