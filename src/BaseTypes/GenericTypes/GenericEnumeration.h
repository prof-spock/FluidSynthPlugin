/**
 * @file
 * The <C>GenericEnumeration</C> specification and body defines a
 * generic type for simple enumeration types.
 *
 * An object may be compared for equality and inequality with other
 * objects of the same type and can be initialized with some literal
 * value.
 *
 * @author Dr. Thomas Tensi
 * @date   2020-08
 */

/*====================*/

#pragma once

/*=========*/
/* IMPORTS */
/*=========*/

#include "GlobalMacros.h"

/*====================*/

namespace BaseTypes::GenericTypes {

    /**
     * A <C>GenericEnumeration</C> is a generic type that defines a
     * wrapper type for enumeration types based on some underlying
     * elementary type.
     *
     * An object may be compared for equality and inequality with other
     * objects of the same type and can be initialized with some literal
     * value.
     */
    template <typename Type, typename ElementaryType>
    struct GenericEnumeration {

        /*--------------------*/
        /* constructors       */
        /*--------------------*/

        /**
         * Initializes object from elementary value <C>v</C>.
         *
         * @param[in] v  elementary type value
         */
        constexpr GenericEnumeration (IN ElementaryType v)
            : _value{v}
        {
        }

        /*--------------------*/
        /* comparisons        */
        /*--------------------*/

        /**
         * Returns whether <C>self</C> is equal to <C>other</C>.
         *
         * @param[in] self   first enumeration value
         * @param[in] other  second enumeration value
         * @return equality information
         */
        friend Boolean operator == (IN Type self, IN Type other)
        {
            return (self._value == other._value);
        }

        /*--------------------*/

        /**
         * Returns whether <C>self</C> is not equal to <C>other</C>.
         *
         * @param[in] self   first enumeration value
         * @param[in] other  second enumeration value
         * @return inequality information
         */
        friend Boolean operator != (IN Type self, IN Type other)
        {
            return (self._value != other._value);
        }

        /*--------------------*/
        /*--------------------*/

        protected:

            /** the internal representation */
            ElementaryType _value;

    };

}
