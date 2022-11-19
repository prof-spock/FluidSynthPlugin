/**
 * @file
 * The <C>GenericNumber</C> specification and body defines a template
 * for wrapper objects for elementary numeric types.
 *
 * Such an object just contains a single internal variable whose type
 * is some kind of elementary type (int, long, double, ...).
 *
 * An important benefit of this approach is that all free-floating
 * functions on the elementary type can be gathered into a single class.
 *
 * Very important is that the wrapper type abstraction does not impose
 * a runtime penalty.
 *
 * @author Dr. Thomas Tensi
 * @date   2020-08
 */

/*====================*/

#pragma once

/*=========*/
/* IMPORTS */
/*=========*/

#include <limits>
#include "Boolean.h"

using BaseTypes::Primitives::Boolean;

/*====================*/

namespace BaseTypes::GenericTypes {

    /**
     * A <C>GenericNumber</C> is a generic number type that defines a
     * wrapper around elementary numeric types (like int, long, etc.);     *
     * such a type contains a single internal variable of this
     * elementary type and provides restricted access to it.
     *
     * An important benefit of this approach is that all free-floating
     * functions on the elementary type can be gathered into a single
     * class.
     *
     * Very important is that the wrapper type abstraction does not
     * impose a runtime penalty.
     */
    template <typename Type, typename ElementaryType>
    struct GenericNumber {

        /*--------------------*/
        /* type conversions   */
        /*--------------------*/

        /**
         * Returns the current value as a primitive value upon
         * explicit conversion.
         *
         * @return current value
         */
        explicit operator ElementaryType () const
        {
            return _value;
        }

        /*----------------------*/
        /* arithmetic operators */
        /*----------------------*/

        /**
         * Returns negative of current value.
         *
         * @return negation of current value
         */
        Type operator - () const
        {
            return Type{-_value};
        }

        /*--------------------*/

        /**
         * Return sum of current value and <C>other</C>.
         *
         * @param[in] other   value to be added
         * @return  sum of current value and other
         */
         Type operator + (IN Type other) const
        {
            return Type{_value + other._value};
        }

        /*--------------------*/

        /**
         * Returns difference of current value and <C>other</C>.
         *
         * @param[in] other   value to be subtracted
         * @return  difference of current value and other
         */
        Type operator - (IN Type other) const
        {
            return Type{_value - other._value};
        }

        /*--------------------*/

        /**
         * Returns product of current value and <C>other</C>.
         *
         * @param[in] other   value to be multiplied
         * @return  product of current value and other
         */
        Type operator * (IN Type other) const
        {
            return Type{_value * other._value};
        }

        /*--------------------*/

        /**
         * Returns quotient of current value and <C>other</C>.
         *
         * @param[in] other   value to be divided by
         * @return  quotient of current value and other
         */
        Type operator / (IN Type other) const
        {
            return Type{_value / other._value};
        }

        /*--------------------*/

        /**
         * Returns bitwise-and of current value and <C>other</C>.
         *
         * @param[in] other   value to be combined with
         * @return  bitwise-and of current value and other
         */
        Type operator & (IN Type other) const
        {
            return Type{_value & other._value};
        }

        /*--------------------*/

        /**
         * Returns bitwise-or of current value and <C>other</C>.
         *
         * @param[in] other   value to be combined with
         * @return  bitwise-or of current value and other
         */
        Type operator | (IN Type other) const
        {
            return Type{_value | other._value};
        }

        /*----------------------*/
        /* assignment operators */
        /*----------------------*/

        /**
         * Assign sum of current value and <C>other</C> to current
         * value
         *
         * @param[in] other   value to be added
         */
        void operator += (IN Type other)
        {
            _value += other._value;
        }

        /*--------------------*/

        /**
         * Assign difference of current value and <C>other</C> to current
         * value
         *
         * @param[in] other   value to be subtracted
         */
        void operator -= (IN Type other)
        {
            _value -= other._value;
        }

        /*--------------------*/

        /**
         * Assign product of current value and <C>other</C> to current
         * value.
         *
         * @param[in] other   value to be multiplied
         */
        void operator *= (IN Type other)
        {
            _value *= other._value;
        }

        /*--------------------*/

        /**
         * Assign quotient of current value and <C>other</C> to current
         * value.
         *
         * @param[in] other   value to be divided by
         */
        void operator /= (IN Type other)
        {
            _value /= other._value;
        }

        /*----------------------*/
        /* comparison operators */
        /*----------------------*/

        /**
         * Checks whether current value and <C>other</C> are equal.
         *
         * @param[in] other   value to be compared
         * @result  returns equality of <C>this</C> and <C>other</C>
         */
        Boolean operator == (IN Type other) const
        {
            return (_value == other._value);
        }

        /*--------------------*/

        /**
         * Checks whether current value and <C>other</C> are not equal.
         *
         * @param[in] other   value to be compared
         * @result  returns inequality of <C>this</C> and <C>other</C>
         */
        Boolean operator != (IN Type other) const
        {
            return (_value != other._value);
        }

        /*--------------------*/

        /**
         * Checks whether current value is less than <C>other</C>.
         *
         * @param[in] other   value to be compared
         * @result  returns information whether <C>this</C> is less
         *          than <C>other</C>
         */
        Boolean operator < (IN Type other) const
        {
            return (_value < other._value);
        }

        /*--------------------*/

        /**
         * Checks whether current value is less than or equal to
         * <C>other</C>.
         *
         * @param[in] other   value to be compared
         * @result  returns information whether <C>this</C> is not
         *          greater than <C>other</C>
         */
        Boolean operator <= (IN Type other) const
        {
            return (_value <= other._value);
        }

        /*--------------------*/

        /**
         * Checks whether current value is greater than <C>other</C>.
         *
         * @param[in] other   value to be compared
         * @result  returns information whether <C>this</C> is greater
         *          than <C>other</C>
         */
        Boolean operator > (IN Type other) const
        {
            return (_value > other._value);
        }

        /*--------------------*/

        /**
         * Checks whether current value is greater than or equal to
         * <C>other</C>.
         *
         * @param[in] other   value to be compared
         * @result  returns information whether <C>this</C> is not
         *          less than <C>other</C>
         */
        Boolean operator >= (IN Type other) const
        {
            return (_value >= other._value);
        }

        /*--------------------*/
        /* advanced functions */
        /*--------------------*/

        /**
         * Returns whether current value lies in the closed interval
         * from <C>lowerEndPoint</C> to <C>upperEndPoint</C>.
         *
         * @param[in] lowerEndPoint  smaller end point of interval
         * @param[in] upperEndPoint  larger end point of interval
         * @return  information whether current value is between lowerEndPoint
         *          and upperEndPoint with both values included
         */
        Boolean
        isInInterval (IN Type lowerEndPoint, IN Type upperEndPoint) const
        {
            return (lowerEndPoint._value <= _value
                    && _value <= upperEndPoint._value);
        }

        /*--------------------*/

        /**
         * Returns the maximum of <C>x</C> and <C>y</C> as
         * value.
         *
         * @param[in] x  some value
         * @param[in] y  some value
         * @return  the maximum of both values
         */
        static Type maximum (IN Type x, IN Type y)
        {
            return (x > y ? x : y);
        }

        /*--------------------*/

        /**
         * Returns the minimum of <C>x</C> and <C>y</C> as value.
         *
         * @param[in] x  some value
         * @param[in] y  some value
         * @return  the minimum of both values
         */
        static Type minimum (IN Type x, IN Type y)
        {
            return (x < y ? x : y);
        }

        /*--------------------*/

        /**
         * Returns the maximum value.
         *
         * @return maximum value of Type
         */
        static Type maximumValue ()
        {
            return Type{std::numeric_limits<ElementaryType>::max()};
        }

        /*--------------------*/
        /*--------------------*/

        protected:

            /** the internal representation */
            ElementaryType _value;

    };

}
