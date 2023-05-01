/**
 * @file
 * The <C>Byte</C> specification and body provides a value object
 * type for byte values.
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
#include "Natural.h"

/*====================*/

namespace BaseTypes::Primitives {

    /**
     * A <C>Byte</C> object wraps a value of the associated
     * primitive type char into an object.
     */
    struct Byte : GenericNumber<Byte, uint8_t> {

        /*--------------------*/
        /* constructors       */
        /*--------------------*/

        /**
         * Initializes byte to an unspecified value.
         */
        Byte ()
        {
        }

        /*--------------------*/

        /**
         * Initializes byte from primitive char <C>c</C>.
         *
         * @param[in] c  char value
         */
        Byte (IN char c)
        {
            _value = c;
        }

        /*--------------------*/

        /**
         * Initializes byte from int <C>i</C>.
         *
         * @param[in] i  int value
         */
        Byte (IN int i)
        {
            _value = i;
        }

        /*--------------------*/
        /* type conversions   */
        /*--------------------*/

        /**
         * Returns string representation of byte.
         *
         * @return  string representation
         */
        String toString () const
        {
            String result;
            result += _value;
            return result;
        }

        /*--------------------*/

        /**
         * Returns the current value as a char value upon
         * explicit conversion.
         *
         * @return current value as a char value
         */
        explicit operator char () const
        {
            return (char) _value;
        }

        /*--------------------*/

        /**
         * Returns the current value as a natural value upon
         * explicit conversion.
         *
         * @return current value as a natural value
         */
        explicit operator Natural () const
        {
            return Natural{_value};
        }

        /*--------------------*/
        /* operators          */
        /*--------------------*/

        /**
         * Postincrements current value.
         *
         * @return  previous value of byte
         */
        Byte operator ++ (int)
        {
            return Byte{_value++};
        }

        /*--------------------*/

        /**
         * Preincrements current value.
         *
         * @return  current value of byte
         */
        Byte operator ++ ()
        {
            return Byte{++_value};
        }

        /*--------------------*/

        /**
         * Postdecrements current value.
         *
         * @return  previous value of byte
         */
        Byte operator -- (int)
        {
            return Byte{_value--};
        }

        /*--------------------*/

        /**
         * Predecrements current value.
         *
         * @return  current value of byte
         */
        Byte operator -- ()
        {
            return Byte{--_value};
        }

        /*-----------------------------*/
        /* advanced functions (static) */
        /*-----------------------------*/

        /**
         * Returns string representation of byte <C>b</C>.
         *
         * @param[in] b  byte value to be converted to a string
         * @return  string representation
         */
        static String toString (IN Byte& b)
        {
            return b.toString();
        }

    };

}
