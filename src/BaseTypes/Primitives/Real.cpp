/**
 * @file
 * The <C>Real</C> body implements a value object type for real
 * values.
 *
 * @author Dr. Thomas Tensi
 * @date   2020-08
 */

/*====================*/

/*=========*/
/* IMPORTS */
/*=========*/

#include <cmath>

#include "MyString.h"
#include "Real.h"
#include "StringUtil.h"

/*--------------------*/

using BaseTypes::Primitives::String;

/** abbreviation for StringUtil */
using STR = BaseModules::StringUtil;

/*====================*/

/*--------------------*/
/* LOCAL VARIABLES    */
/*--------------------*/

/** the list of digits */
static const String _digitList = "0123456789";

/*--------------------*/
/* PROTOTYPES         */
/*--------------------*/

static Real _tenPower (IN Natural exponent);

/*--------------------*/
/* LOCAL FUNCTIONS    */
/*--------------------*/

/**
 * Analyzes <C>value</C> for going outside of the provided digits
 * counts <C>integralDigitCount</C> and <C>fractionalDigitCount</C>;
 * updates value and counts and returns non-empty <C>suffix</C> when
 * exponential notation is needed
 *
 * @param[inout] value                       value to be checked and
 *                                           adapted
 * @param[in]    scientificNotationIsForced  tells whether number must be
 *                                           formatted using scientific
 *                                           notation
 * @param[inout] integralDigitCount          number of integral digits
 * @param[inout] fractionalDigitCount        number of fractional digits
 * @param[out]   suffix                      exponent (when applicable)
 */
static void _adaptForScientificNotation (INOUT Real& value,
                                         IN Boolean scientificNotationIsForced,
                                         INOUT Natural& integralDigitCount,
                                         INOUT Natural& fractionalDigitCount,
                                         OUT String& suffix)
{
    Real logarithm = std::log10((float) value);
    Integer digitCount = (Integer) logarithm.floor();
    Boolean needsScientificNotation;

    if (scientificNotationIsForced) {
        needsScientificNotation = true;
    } else if (value >= 1.0) {
        needsScientificNotation =
            (digitCount + 1 > integralDigitCount);
    } else {
        needsScientificNotation = (-digitCount > fractionalDigitCount);
    }

    if (!needsScientificNotation) {
        suffix = "";
    } else {
        /* make space for exponent */
        fractionalDigitCount += integralDigitCount - 1;
        integralDigitCount = 1;
        fractionalDigitCount = (fractionalDigitCount < 4 ? 0
                                : fractionalDigitCount - 4);
        Boolean isLessThanOne = (digitCount < 0);
        suffix = String{"E"} + (isLessThanOne ? "-" : "+") + "..";
        Natural d{(size_t) digitCount.abs()};
        STR::setCharacterAt(suffix,
                            2,
                            STR::characterAt(_digitList, d / 10));
        STR::setCharacterAt(suffix,
                            3,
                            STR::characterAt(_digitList,d % 10));
        Real factor = _tenPower(d);
        value = (isLessThanOne ? value * factor : value / factor);
    }
}   

/*--------------------*/

/**
 * Returns ten to the <C>exponent</C>-th power.
 *
 * @param[in] exponent  exponent of potentiation
 * @return  10 power exponent
 */
static Real _tenPower (IN Natural exponent)
{
    Real result{Real::one};

    for (Natural count = 0;  count < exponent;  count++) {
        result *= Real::ten;
    }

    return result;
}

/*====================*/

/*--------------------*/
/* class properties   */
/*--------------------*/

Boolean Real::hasDoublePrecision ()
{
    return true;
}

/*--------------------*/
/* constructors       */
/*--------------------*/

Real::Real ()
{
}

/*--------------------*/

Real::Real (IN double d)
{
    _value = d;
}

/*--------------------*/

Real::Real (IN float f)
{
    _value = f;
}

/*--------------------*/

Real::Real (IN Integer i)
{
    _value = (int) i;
}

/*--------------------*/

Real::Real (IN Natural n)
{
    _value = (size_t) n;
}

/*--------------------*/
/* type conversions   */
/*--------------------*/

String Real::toString (IN Natural precision,
                       IN Natural fractionalDigitCount,
                       IN String& padString,
                       IN Boolean scientificNotationIsForced) const
{
    const Natural p = Natural::maximum(precision, fractionalDigitCount + 1);
    const Natural totalWidth = p + 2;  /* take care of sign and decimal point */
    Natural integralDigitCount = p - fractionalDigitCount;
    Natural fracDigitCount = fractionalDigitCount;
    String result;

    if (std::isnan(_value)) {
        result = STR::paddedRight("NaN", totalWidth, " ");
    } else if (std::isinf(_value)) {
        result = (std::signbit(_value) ? "-inf" : "+inf");
        result = STR::paddedRight(result, totalWidth, " ");
    } else {
        String integralPartAsString;
        String fractionalPartAsString;
        String sign;
        String suffix;

        if (_value == 0.0) {
            sign = " ";
            integralPartAsString = "0";
            fractionalPartAsString = "";
        } else {
            Real v = abs(_value);
            sign = (_value < 0.0 ? "-" : "+");

            /* switch to scientific notation when value is too large
             * or too small */
            _adaptForScientificNotation(v,
                                        scientificNotationIsForced,
                                        integralDigitCount,
                                        fracDigitCount,
                                        suffix);

            /* round last digit of fractional part */
            v += Real::one / _tenPower(fracDigitCount) / Real::two;
        
            /* construct integral and fractional parts */
            Natural iP = (Natural) v.integralPart();

            do {
                integralPartAsString = (STR::substring(_digitList, iP % 10, 1)
                                        + integralPartAsString);
                iP /= 10;
            } while (iP > 0);

            Real fP = v.fractionalPart();

            while (fP > 0.0
                   && fractionalPartAsString.length() < fracDigitCount) {
                fP *= Real::ten;
                Natural digit = (Natural) fP.floor();
                fP -= Real{digit};
                fractionalPartAsString += STR::substring(_digitList, digit, 1);
            }
        }
            
        result =
            STR::expand("%1%2.%3%4",
                        sign,
                        STR::paddedLeft(integralPartAsString,
                                        integralDigitCount,
                                        padString),
                        STR::paddedRight(fractionalPartAsString,
                                         fractionalDigitCount,
                                         padString),
                        suffix);
    }

    return result;
}

/*--------------------*/

Real::operator double () const
{
    return (double) _value;
}

/*--------------------*/

Real::operator float () const
{
    return (float) _value;
}

/*--------------------*/

Real::operator int () const
{
    return (int) _value;
}

/*--------------------*/

Real::operator Integer () const
{
    return Integer{(int) _value};
}

/*--------------------*/

Real::operator Natural () const
{
    return Natural{(size_t) _value};
}

/*-----------------------------*/
/* advanced functions (static) */
/*-----------------------------*/

Real Real::abs (IN Real r)
{
    return Real{std::abs(r._value)};
}

/*--------------------*/

Real Real::ceiling (IN Real r)
{
    return Real{std::ceil(r._value)};
}

/*--------------------*/

Real Real::cos (IN Real x)
{
    return Real{std::cos(x._value)};
}

/*--------------------*/

Real Real::exp (IN Real x)
{
    return Real{std::exp(x._value)};
}

/*--------------------*/

Real Real::floor (IN Real r)
{
    return Real{std::floor(r._value)};
}

/*--------------------*/

Real Real::forceToInterval (IN Real x,
                            IN Real lowerEndPoint,
                            IN Real upperEndPoint)
{
    Assertion_pre(lowerEndPoint <= upperEndPoint,
                  "interval must be non-empty");
    return (x < lowerEndPoint ? lowerEndPoint
            : (x > upperEndPoint ? upperEndPoint : x));
}

/*--------------------*/

Real Real::fractionalPart (IN Real x)
{
    double result = std::abs(x._value);
    result -= std::floor(result);
    result = copysign(result, x._value);
    return Real{result};
}

/*--------------------*/

Real Real::integralPart (IN Real x)
{
    return x - x.fractionalPart();
}

/*--------------------*/

/**
 * Returns the natural logarithm of <C>x</C>.
 *
 * @param[in] x  real value
 * @return  ln(x)
 */
Real Real::log (IN Real x)
{
    return Real{std::log(x._value)};
}

/*--------------------*/

Real Real::mod (IN Real x,
                IN Real d)
{
    const double xv = x._value;
    const double dv = d._value;
    double result = fmod(xv, dv);
    result += (result < 0 ? dv : 0.0);
    return result;
}

/*--------------------*/

Real Real::power (IN Real base,
                  IN Real exponent)
{
    return Real{pow(base._value, exponent._value)};
}

/*--------------------*/

Real Real::round (IN Real x,
                  IN Natural decimalPlaceCount)
{
    const double factor =
        std::pow(10.0, (double) decimalPlaceCount);
    return ((int) Integer::round(x._value * factor) / factor);
}

/*--------------------*/

Real Real::sin (IN Real x)
{
    return Real{std::sin(x._value)};
}

/*--------------------*/

Real Real::sinh (IN Real x)
{
    return Real{std::sinh(x._value)};
}

/*--------------------*/

/**
 * Returns the square of <C>x</C>.
 *
 * @param[in] x  real value to be squared
 * @return x^2
 */
Real Real::sqr (IN Real x)
{
    return x * x;
}

/*--------------------*/

Real Real::sqrt (IN Real x)
{
    return Real{std::sqrt(x._value)};
}

/*--------------------*/

String Real::toString (IN Real& r)
{
    return r.toString();
}

/*--------------------*/
/* advanced functions */
/*--------------------*/

Real Real::abs () const
{
    return abs(*this);
}

/*--------------------*/

Real Real::ceiling () const
{
    return ceiling(*this);
}

/*--------------------*/

Real Real::cos () const
{
    return cos(*this);
}

/*--------------------*/

Real Real::exp () const
{
    return exp(*this);
}

/*--------------------*/

Real Real::floor () const
{
    return floor(*this);
}

/*--------------------*/

Real Real::forceToInterval (IN Real lowerEndPoint,
                            IN Real upperEndPoint) const
{
    return forceToInterval(*this, lowerEndPoint, upperEndPoint);
}

/*--------------------*/

Real Real::fractionalPart () const
{
    return fractionalPart(*this);
}

/*--------------------*/

Real Real::integralPart () const
{
    return integralPart(*this);
}

/*--------------------*/

Real Real::mod (IN Real d) const
{
    return mod(*this, d);
}

/*--------------------*/

Real Real::log () const
{
    return log(*this);
}

/*--------------------*/

Real Real::power (IN Real exponent) const
{
    return power(*this, exponent);
}

/*--------------------*/

Real Real::round (IN Natural decimalPlaceCount) const
{
    return round(*this, decimalPlaceCount);
}

/*--------------------*/

Real Real::sin () const
{
    return sin(*this);
}

/*--------------------*/

Real Real::sinh () const
{
    return sinh(*this);
}

/*--------------------*/

Real Real::sqr () const
{
    return sqr(*this);
}

/*--------------------*/

Real Real::sqrt () const
{
    return sqrt(*this);
}

/*--------------------*/
/* constants          */
/*--------------------*/

const Real Real::zero{0.0};
const Real Real::one{1.0};
const Real Real::two{2.0};
const Real Real::pi{3.141592653589793238462643383279};
const Real Real::twoPi{two * pi};
const Real Real::ten{10.0};
const Real Real::infinity{maximumValue()};
