/**
 * @file
 * The <C>Real</C> specification defines a value object type for real
 * values.
 *
 * @author Dr. Thomas Tensi
 * @date   2020-08
 */

/*====================*/

#pragma once

/*=========*/
/* IMPORTS */
/*=========*/

#include "Integer.h"
#undef max

/*====================*/

namespace BaseTypes::Primitives {

    /**
     * An <C>Real</C> object wraps a value of some associated
     * primitive float point type in an object.  An object of type
     * Real contains a single internal variable whose type is some
     * kind of floating point value.
     *
     * An important benefit of this approach is that all free-floating
     * functions on reals are gathered into a single class.
     */
    struct Real : GenericNumber<Real, double> {

        /*--------------------*/
        /* class properties   */
        /*--------------------*/

        /**
         * Tells whether the real value has double precision.
         *
         * @return information, whether a real value is a double
         */
        static Boolean hasDoublePrecision ();

        /*--------------------*/
        /* constructors       */
        /*--------------------*/

        /**
         * Initializes real to an unspecified value.
         */
        Real ();

        /*--------------------*/

        /**
         * Initializes real from double <C>d</C>.
         *
         * @param[in] d  double precision floating point value
         */
        Real (IN double d);

        /*--------------------*/

        /**
         * Initializes real from float <C>f</C>.
         *
         * @param[in] f  single precision floating point value
         */
        Real (IN float f);

        /*--------------------*/

        /**
         * Initializes real from integer <C>i</C>.
         *
         * @param[in] i  integer value
         */
        Real (IN Integer i);

        /*--------------------*/

        /**
         * Initializes real from natural <C>n</C>.
         *
         * @param[in] n  natural value
         */
        Real (IN Natural n);

        /*--------------------*/
        /* type conversions   */
        /*--------------------*/

        /**
         * Returns string representation of real with precision and
         * padding information.
         *
         * @param[in] precision                   minimum number of
         *                                        valid digits
         * @param[in] fractionalDigitCount        number of decimal
         *                                        digits
         * @param[in] padString                   string to use for
         *                                        left and right padding
         * @param[in] scientificNotationIsForced  tells whether number
         *                                        must be formatted
         *                                        using scientific
         *                                        notation
         * @return  string representation
         */
        String toString (IN Natural precision = 0,
                         IN Natural fractionalDigitCount = 0,
                         IN String& padString = "0",
                         IN Boolean scientificNotationIsForced = false) const;

        /*--------------------*/

        /**
         * Returns the current value as a double value upon
         * explicit conversion.
         *
         * @return current value as double
         */
        explicit operator double () const;

        /*--------------------*/

        /**
         * Returns the current value as a float value upon
         * explicit conversion.
         *
         * @return current value as float
         */
        explicit operator float () const;

        /*--------------------*/

        /**
         * Returns the current value as an int value upon
         * explicit conversion.
         *
         * @return current value as int
         */
        explicit operator int () const;

        /*--------------------*/

        /**
         * Returns the current value as an integer value upon
         * explicit conversion.
         *
         * @return current value as integer
         */
        explicit operator Integer () const;

        /*--------------------*/

        /**
         * Returns the current value as a natural value upon
         * explicit conversion.
         *
         * @return current value as natural
         */
        explicit operator Natural () const;

        /*-----------------------------*/
        /* advanced functions (static) */
        /*-----------------------------*/

        /**
         * Returns the absolute value of real <C>r</C> as real value.
         *
         * @param[in] r  some real value
         * @return  the absolute value of <C>r</C>
         */
        static Real abs (IN Real r);

        /*--------------------*/

        /**
         * Returns the ceiling of real <C>r</C> as real value.
         *
         * @param[in] r  some real value
         * @return  the least integer value that is greater or equal to
         *          <C>r</C>
         */
        static Real ceiling (IN Real r);

        /*--------------------*/

        /**
          * Returns the cosine of <C>x</C>.
          *
          * @param[in] x  real value
          * @return  the cosine of x
          */
        static Real cos (IN Real x);

        /*--------------------*/

        /**
          * Returns e^<C>x</C>.
          *
          * @param[in] x  real value
          * @return  e^x
          */
        static Real exp (IN Real x);

        /*--------------------*/

        /**
         * Returns the floor of real <C>r</C> as real value.
         *
         * @param[in] r  some real value
         * @return  the largest integer value that is smaller or equal to
         *          <C>r</C>
         */
        static Real floor (IN Real r);

        /*--------------------*/

        /**
         * Forces value <C>x</C> to range between <C>lowerEndPoint</C> and
         * <C>upperEndPoint</C>.
         *
         * @param[in] x          some percentage value
         * @param[in] lowerEndPoint   minimum acceptable percentage value
         * @param[in] upperEndPoint  maximum acceptable percentage value
         * @return value forced to range [<C>lowerEndPoint</C>,
         *                                <C>upperEndPoint</C>]
         * @pre lowerBound <= upperBound
         */
        static Real forceToInterval (IN Real x,
                                     IN Real lowerEndPoint,
                                     IN Real upperEndPoint);

        /*--------------------*/

        /**
         * Strips off integral part from <C>x</C> and only returns
         * digits after the decimal point; for negative <C>x</C>
         * values a negative value is produced
         *
         * @param[in] x  some real value
         * @return  the fractional part after the decimal point with the
         *          sign of x
         */
        static Real fractionalPart (IN Real x);

        /*--------------------*/

        /**
         * Strips off fractional part from <C>x</C> and only returns
         * digits before the decimal point; for negative <C>x</C>
         * values a negative value is produced
         *
         * @param[in] x  some real value
         * @return  the integral part before the decimal point with the
         *          sign of x
         */
        static Real integralPart (IN Real x);

        /*--------------------*/

        /**
          * Returns the natural logarithm of <C>x</C>.
          *
          * @param[in] x  real value
          * @return  ln(x)
          */
        static Real log (IN Real x);

        /*--------------------*/

        /**
          * Returns the real modulus; brings result into the range
          * [0,d)
          *
          * @param[in] x  some real value
          * @param[in] d  the positive real divisor value
          * @return remainder of division in range [0,d)
          */
        static Real mod (IN Real x, IN Real d);

        /*--------------------*/

        /**
         * Returns <C>base</C> to the <C>exponent</C> power.
         *
         * @param[in] base      some real value
         * @param[in] exponent  some real value
         * @return  base^exponent
         */
        static Real power (IN Real base, IN Real exponent);

        /*--------------------*/

        /**
         * Rounds <C>x</C> to <C>decimalPlaceCount</C> decimal places
         * and returns result.
         *
         * @param[in] x                  the value to be rounded
         * @param[in] decimalPlaceCount  the number of decimal places
         *                               to keep by rounding

         * @return  the value <C>x</C> rounded to
         *          <C>decimalPlaceCount</C> decimal places
         */
        static Real round (IN Real x,
                           IN Natural decimalPlaceCount = 0);

        /*--------------------*/

        /**
          * Returns the sine of <C>x</C>.
          *
          * @param[in] x  real value
          * @return  the sine of x
          */
        static Real sin (IN Real x);

        /*--------------------*/

        /**
          * Returns the hyperbolic sine of <C>x</C>.
          *
          * @param[in] x  real value
          * @return  the hyperbolic sine of x
          */
        static Real sinh (IN Real x);

        /*--------------------*/

        /**
          * Returns the square of <C>x</C>.
          *
          * @param[in] x  real value to be squared
          * @return x^2
          */
        static Real sqr (IN Real x);

        /*--------------------*/

        /**
          * Returns the square root of <C>x</C>.
          *
          * @param[in] x  real value to be rooted
          * @return x^{1/2}
          */
        static Real sqrt (IN Real x);

        /*--------------------*/

        /**
         * Returns string representation of real <C>r</C> with
         * precision and padding information.
         *
         * @param[in] r  real value to be converted to a string
         * @return  string representation
         */
        static String toString (IN Real& r);

        /*--------------------*/
        /* advanced functions */
        /*--------------------*/

        /**
         * Returns the absolute value of current value as real value.
         *
         * @return  the absolute value of current value
         */
        Real abs () const;

        /*--------------------*/

        /**
         * Returns the ceiling of current value as real value.
         *
         * @return  the least integer value that is greater or equal to
         *          current value
         */
        Real ceiling () const;

        /*--------------------*/

        /**
          * Returns the cosine of current value.
          *
          * @return  the cosine of current value
          */
        Real cos () const;

        /*--------------------*/

        /**
          * Returns e^ current value
          *
          * @param[in] x  real value
          * @return  e^x
          */
        Real exp () const;

        /*--------------------*/

        /**
         * Returns the floor of current value as real value.
         *
         * @return  the largest integer value that is smaller or equal
         *          to current value
         */
        Real floor () const;

        /*--------------------*/

        /**
         * Forces current value to range between <C>lowerEndPoint</C>
         * and <C>upperEndPoint</C> and returns result.
         *
         * @param[in] lowerEndPoint  minimum acceptable value
         * @param[in] upperEndPoint  maximum acceptable value
         * @return value forced to range [<C>lowerEndPoint</C>,
         *                                <C>upperEndPoint</C>]
         */
        Real forceToInterval (IN Real lowerEndPoint,
                              IN Real upperEndPoint) const;

        /*--------------------*/

        /**
         * Strips off integer part from current value and only returns
         * digits after the decimal point; for negative values also a
         * positive result is produced
         *
         * @return  the positive fractional part after the decimal point
         *          of current value
         */
        Real fractionalPart () const;

        /*--------------------*/

        /**
         * Strips off fractional part from current value and only
         * returns digits before the decimal point; for negative
         * values a negative value is produced
         *
         * @return  the integral part of current value before the
         *          decimal point with the sign
         */
        Real integralPart () const;

        /*--------------------*/

        /**
          * Returns the real modulus of current and other value;
          * brings result into the range [0,i)
          *
          * @param[in] d  the real divisor value
          * @return remainder of division in range [0,i)
          */
        Real mod (IN Real d) const;

        /*--------------------*/

        /**
          * Returns the natural logarithm of the current value.
          *
          * @return  ln(x)
          */
        Real log () const;

        /*--------------------*/

        /**
         * Returns current value to the <C>exponent</C> power.
         *
         * @param[in] exponent  some real value
         * @return  current^exponent
         */
        Real power (IN Real exponent) const;

        /*--------------------*/

        /**
         * Rounds current value to <C>decimalPlaceCount</C> decimal
         * places and returns result.
         *
         * @param[in] decimalPlaceCount  the number of decimal places to keep
         *                               by rounding
         * @return  the current real value rounded to
         *          <C>decimalPlaceCount</C> decimal places
         */
        Real round (IN Natural decimalPlaceCount = 0) const;

        /*--------------------*/

        /**
          * Returns the sine of current value.
          *
          * @return  the sine of current value
          */
        Real sin () const;

        /*--------------------*/

        /**
          * Returns the hyperbolic sine of current value.
          *
          * @return  the hyperbolic sine of current value
          */
        Real sinh () const;

        /*--------------------*/

        /**
          * Returns the square of current value.
          *
          * @return x^2
          */
        Real sqr () const;

        /*--------------------*/

        /**
          * Returns the square root of the current value.
          *
          * @return x^{1/2}
          */
        Real sqrt () const;

        /*--------------------*/
        /* constants          */
        /*--------------------*/

        static const Real zero;      /**< 0 */
        static const Real one;       /**< 1 */
        static const Real two;       /**< 2 */
        static const Real pi;        /**< pi */
        static const Real twoPi;     /**< 2*pi */
        static const Real ten;       /**< 10 */
        static const Real infinity;  /**< infinity */

    };

}
