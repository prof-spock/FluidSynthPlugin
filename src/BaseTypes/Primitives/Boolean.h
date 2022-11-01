/**
 * @file
 * The <C>Boolean</C> specification and body provides a value object
 * type for boolean values.
 *
 * @author Dr. Thomas Tensi
 * @date   2022-08
 */

/*====================*/

#pragma once

/*=========*/
/* IMPORTS */
/*=========*/

#include "MyString.h"

/*====================*/

namespace BaseTypes::Primitives {

    /**
     * A <C>Boolean</C> object wraps a value of the associated
     * primitive type bool into a value object.
     */
    struct Boolean {

        /*--------------------*/
        /* constructors       */
        /*--------------------*/

        /**
         * Initializes boolean to an unspecified value.
         */
        Boolean ()
        {
            _value = false;
        }

        /*--------------------*/

        /**
         * Initializes boolean from primitive bool <C>b</C>.
         *
         * @param[in] b  bool value
         */
        Boolean (IN bool b)
        {
            _value = b;
        }

        /*--------------------*/
        /* type conversions   */
        /*--------------------*/

        /**
         * Returns string representation of boolean.
         *
         * @return  string representation
         */
        String toString () const
        {
            String result{_value ? "true" : "false"};
            return result;
        }

        /*--------------------*/

        /**
         * Returns the current value as a bool value.
         *
         * @return current value as a bool value
         */
        operator bool () const
        {
            return _value;
        }

        /*-----------------------------*/
        /* advanced functions (static) */
        /*-----------------------------*/

        /**
         * Returns string representation of boolean <C>b</C>.
         *
         * @param[in] b  boolean value to be converted to a string
         * @return  string representation
         */
        static String toString (IN Boolean& b)
        {
            return b.toString();
        }

        /*--------------------*/
        /*--------------------*/

        private:

            /** the internal representation */
            bool _value;

    };

}
