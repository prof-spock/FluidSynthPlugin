/**
 * @file
 * The <C>Percentage</C> specification and body provides a simple
 * percentage type.
 *
 * @author Dr. Thomas Tensi
 * @date   2020-08
 */

/*====================*/

#pragma once

/*=========*/
/* IMPORTS */
/*=========*/

#include "Real.h"

/*====================*/

namespace BaseTypes::Primitives {

    /**
     * A <C>Percentage</C> object is real-valued and represents a
     * percentage (normalized to the 0 to 100 range).
     */
    struct Percentage : public Real {

        /*--------------------*/
        /* constructors       */
        /*--------------------*/

        /**
         * Initializes percentage to an unspecified value.
         */
        Percentage ()
        {
        }

        /*--------------------*/

        /**
         * Initializes percentage from double precision value <C>d</C>.
         *
         * @param[in] d  double precision floating point value
         */
        Percentage (IN double d)
        {
            _value = d;
        }

        /*--------------------*/

        /**
         * Initializes percentage from single precision value <C>f</C>.
         *
         * @param[in] f  single precision floating point value
         */
        Percentage (IN float f)
        {
            _value = f;
        }

        /*--------------------*/

        /**
         * Initializes percentage from real value <C>r</C>.
         *
         * @param[in] r  real value
         */
        Percentage (IN Real r)
            : Percentage((double) r)
        {
        }

        /*--------------------*/
        /* type conversions   */
        /*--------------------*/

        /**
         * Returns string representation of percentage with precision
         * and padding information.
         *
         * @param[in] precision             minimum number of valid
         *                                  digits
         * @param[in] fractionalDigitCount  number of decimal digits
         * @param[in] padString             string to use for left
         *                                  padding
         * @return  string representation
         */
        String toString (IN Natural precision = 0,
                         IN Natural fractionalDigitCount = 0,
                         IN String& padString = "0") const
        {
            String result = std::to_string(_value);
            const Natural length{result.size()};
            Natural padCount =
                (precision >  length ? precision - length : 0);

            while (padCount-- > 0) {
                result = padString + result;
            }

            return result;
        }

        /*--------------------*/
        /**
         * Returns string representation of percentage <C>p</C>.
         *
         * @param[in] p  percentage value to be converted to a string
         * @return  string representation
         */
        static String toString (IN Percentage& p)
        {
            return p.toString();
        }

        /*--------------------*/
        /* operators          */
        /*--------------------*/

        /**
         * Return sum of current value and <C>other</C>.
         *
         * @param[in] other   value to be added
         * @return  sum of current value and other
         */
         Percentage operator + (IN Percentage other) const
        {
            return Percentage{_value + other._value};
        }

        /*--------------------*/

        /**
         * Returns difference of current value and <C>other</C>.
         *
         * @param[in] other   value to be subtracted
         * @return  difference of current value and other
         */
        Percentage operator - (IN Percentage other) const
        {
            return Percentage{_value - other._value};
        }

        /*--------------------*/
        /* advanced functions */
        /*--------------------*/

        /**
         * Forces value <C>x</C> to a percentage between 0 and 100
         *
         * @param[in] x   some percentage value
         * @return  value forced to range [0, 100]
         */
        static Percentage forceToPercentage (IN Percentage x)
        {
            return Percentage{x < 0.0 ? 0.0 : (x > 100.0 ? 100.0 : x)};
        }

        /*--------------------*/

        /**
         * Returns <C>percentage</C> of real <C>value</C>.
         *
         * @param[in] percentage  the percentage to be found
         * @param[in] value       the real value to be scaled
         * @return  adjusted value by percentage
         */
        static Real percentOf (IN Percentage percentage,
                               IN Real value)
        {
            return percentage * value / Real{100.0};
        }

        /*--------------------*/

        /**
         * Returns <C>percentage</C> of integer <C>value</C>.
         *
         * @param[in] percentage  the percentage to be found
         * @param[in] value       the integer value to be scaled
         * @return  adjusted value by percentage
         */
        static Integer percentOf (IN Percentage percentage,
                                  IN Integer value)
        {
            return Integer{(int) percentOf(percentage, Real{value})};
        }

        /*--------------------*/

        /**
         * Returns <C>percentage</C> of natural <C>value</C>.
         *
         * @param[in] percentage  the percentage to be found
         * @param[in] value       the natural value to be scaled
         * @return  adjusted value by percentage
         */
        static Natural percentOf (IN Percentage percentage,
                                  IN Natural value)
        {
            return Natural{(size_t) percentOf(percentage, Integer{value})};
        }

        /*--------------------*/
        /* advanced methods   */
        /*--------------------*/

        /**
         * Forces current value to a value between 0 and 100
         *
         * @return  current value forced to range [0, 100]
         */
        Percentage forceToPercentage () const
        {
            return forceToPercentage(*this);
        }

        /*--------------------*/

        /**
         * Returns percentage of <C>value</C> from current percentage.
         *
         * @param[in] value       the value to be scaled
         * @return  adjusted value by percentage
         */
        template <typename T>
        T of (IN T value) const
        {
            return percentOf(*this, value);
        }

    };

}
