/**
 * @file
 * The <C>StringUtil</C> body implements several string routines not
 * included in the standard C++ library (like, for example, prefix
 * calculation, whitespace stripping etc).
 *
 * @author Dr. Thomas Tensi
 * @date   2020-08
 */

/*=========*/
/* IMPORTS */
/*=========*/

#include <cassert>
#include <cctype>
#include <cstdarg>
#include "StringUtil.h"

/*--------------------*/

using BaseModules::StringUtil;

/*====================*/

/** the value returned in the standard library when something is
  * not found  */
static const size_t _notFound = std::string::npos;

/** the character representing a minus sign in a number */
static const Character _minusCharacter{'-'};

/** the character representing a decimal point in a number
 *  (assuming an international locale) */
static const Character _decimalPointCharacter{'.'};

/** the value representing an undefined real value (e.g.
 *  (when a conversion to real fails) */
static const Real _undefinedReal = Real::maximumValue();


/*--------------------*/
/* INTERNAL ROUTINES  */
/*--------------------*/

/**
 * Returns character at <C>position</C> in <C>st</C>.
 *
 * @param[in] st        the string to be indexed
 * @param[in] position  the character index (starting at 0)
 * @return  character at position
 */
static Character _at (IN String& st, IN Natural position)
{
    return Character{st[(size_t) position]};
}

/*--------------------*/
/*--------------------*/

Boolean StringUtil::contains (IN String& st, IN String& substring,
                              IN Natural startPosition)
{
    return find(st, substring, startPosition) != Natural::maximumValue();
}

/*--------------------*/

Boolean StringUtil::endsWith (IN String& st, IN String& suffix)
{
    const Natural stLength = st.size();
    const Natural suffixLength = suffix.size();
    Boolean result = false;

    if (stLength >= suffixLength) {
        result = (substring(st, stLength - suffixLength) == suffix);
    }

    return result;
}

/*--------------------*/

String StringUtil::expand (IN String& st,
                           IN String& replacement01,
                           IN String& replacement02,
                           IN String& replacement03,
                           IN String& replacement04,
                           IN String& replacement05,
                           IN String& replacement06,
                           IN String& replacement07,
                           IN String& replacement08,
                           IN String& replacement09,
                           IN String& replacement10)
{
    String result = st;
    replace(result, "%1", replacement01);
    replace(result, "%2", replacement02);
    replace(result, "%3", replacement03);
    replace(result, "%4", replacement04);
    replace(result, "%5", replacement05);
    replace(result, "%6", replacement06);
    replace(result, "%7", replacement07);
    replace(result, "%8", replacement08);
    replace(result, "%9", replacement09);
    replace(result, "%A", replacement10);
    return result;
}

/*--------------------*/

Natural StringUtil::find (IN String& st, IN String& substring,
                          IN Natural startPosition)
{
    Natural result = st.find(substring, (size_t) startPosition);
    result = (result == _notFound ? Natural::maximumValue() : result);
    return result;
}

/*--------------------*/

Natural StringUtil::findFromEnd (IN String& st, IN String& substring,
                                 IN Natural startPosition)
{
    Natural result = st.rfind(substring, (size_t) startPosition);
    result = (result == _notFound ? Natural::maximumValue() : result);
    return result;
}

/*--------------------*/

Boolean StringUtil::isNatural (IN String& st)
{
    const Natural stringLength = st.length();
    Boolean result = (stringLength > 0);

    for (Natural i = 0;  i < stringLength;  i++) {
        if (!isdigit((char) _at(st, i))) {
            result = false;
            break;
        }
    }

    return result;
}

/*--------------------*/

Boolean StringUtil::isInt (IN String& st)
{
    Boolean result = false;

    if (st > "") {
        Natural firstIndex = (_at(st, 0) == _minusCharacter ? 1 : 0);
        result = isNatural(substring(st, firstIndex));
    }

    return result;
}

/*--------------------*/

Boolean StringUtil::isReal (IN String& st)
{
    Boolean result;
    const Natural stringLength = st.length();

    if (stringLength == 0) {
        result = false;
    } else if (stringLength == 1 && _at(st, 0) == _minusCharacter) {
        result = false;
    } else {
        result = true;
        Boolean isBeforeDecimalPoint = true;
        const Natural firstIndex = (_at(st, 0) == _minusCharacter ? 1 : 0);

        for (Natural i = firstIndex; i < stringLength;  i++) {
            const Character ch{_at(st, i)};

            if (ch == _decimalPointCharacter && isBeforeDecimalPoint) {
                isBeforeDecimalPoint = false;
            } else if (!isdigit((char) ch)) {
                result = false;
                break;
            }
        }
    }

    return result;
}

/*--------------------*/

Character StringUtil::lastChar(IN String& st)
{
    return Character{st[st.length() - 1]};
}

/*--------------------*/

String StringUtil::newlineReplacedString (IN String& st,
                                          IN String& replacement)
{
    String result = st;
    replace(result, "\r\n", replacement);
    replace(result, "\n", replacement);
    replace(result, "\r", replacement);
    return result;
}

/*--------------------*/

String StringUtil::prefix (IN String& st, IN Natural count)
{
    return substring(st, 0, count);
}

/*--------------------*/

void StringUtil::replace (INOUT String& st,
                          IN String& pattern,
                          IN String& replacement)
{
    const Natural patternLength = pattern.length();
    const Natural replacementLength = replacement.length();
    Natural position = 0;

    while (true) {
        position = st.find(pattern, (size_t) position);

        if (position == _notFound) {
            break;
        }

        st.replace((size_t) position, (size_t) patternLength,
                   replacement);
        position += replacementLength;
    }
}

/*--------------------*/

Boolean StringUtil::splitAt (IN String& st, IN String& separator,
                             OUT String& prefix, OUT String& suffix)
{
    const Natural position = st.find(separator);
    const Natural separatorLength = separator.length();
    const Boolean isFound = (position != _notFound);

    if (!isFound) {
        prefix = st;
        suffix = "";
    } else {
        prefix = StringUtil::prefix(st, position);
        suffix = substring(st, position + separatorLength);
    }

    return isFound;
}

/*--------------------*/

Boolean StringUtil::startsWith (IN String& st, IN String& prefix)
{
    const Natural stLength = st.size();
    const Natural prefixLength = prefix.size();
    Boolean result = false;

    if (stLength >= prefixLength) {
        result = (StringUtil::prefix(st, prefixLength) == prefix);
    }

    return result;
}

/*--------------------*/

String StringUtil::strip (IN String& st)
{
    const String whiteSpace = " \t\r\n";
    const Natural stringLength = st.length();
    Natural i;

    for (i = stringLength;  i > 0; i--) {
        if (whiteSpace.find((char) _at(st, i - 1)) == _notFound) {
            break;
        }
    }

    const Natural lastRelevantIndex = i - Natural{1};

    for (i = 0;  i < stringLength;  i++) {
        if (whiteSpace.find((char) _at(st, i)) == _notFound) {
            break;
        }
    }

    const Natural firstRelevantIndex = i;
    const Natural length =
        lastRelevantIndex - firstRelevantIndex + Natural{1};

    return substring(st, firstRelevantIndex, length);
}

/*--------------------*/

String StringUtil::substring (IN String& st,
                              IN Natural firstIndex, IN Natural count)
{
    return st.substr((size_t) firstIndex, (size_t) count);
}

/*--------------------*/

Integer StringUtil::toInteger (IN String& st,
                               IN Integer defaultValue)
{
    return (isInt(st) ? Integer{std::stoi(st)} : defaultValue);
}

/*--------------------*/

Natural StringUtil::toNatural (IN String& st)
{
    return toNatural(st, 1000000);
}

/*--------------------*/

Natural StringUtil::toNatural (IN String& st, IN Natural defaultValue)
{
    return (isNatural(st) ? (Natural) std::stoi(st) : defaultValue);
}

/*--------------------*/

Percentage StringUtil::toPercentage (IN String& st)
{
    return Percentage(std::stod(st));
}

/*--------------------*/

Real StringUtil::toReal (IN String& st)
{
    return toReal(st, _undefinedReal);
}

/*--------------------*/

Real StringUtil::toReal (IN String& st, IN Real defaultValue)
{
    return (isReal(st) ? Real(std::stod(st)) : defaultValue);
}

/*--------------------*/

String StringUtil::toString (IN Boolean b)
{
    return (b ? "true" : "false");
}

/*--------------------*/

String StringUtil::toString (IN Byte& b)
{
    return b.toString();
}

/*--------------------*/

String StringUtil::toString (IN Real r)
{
    return std::to_string((double) r);
}

/*--------------------*/

String StringUtil::toString (IN Integer i) {
    return toString(i, 0);
}

/*--------------------*/

String StringUtil::toString (IN Integer i,
                             IN Natural precision,
                             IN String padCharacter) {
    return i.toString(precision, padCharacter);
}

/*--------------------*/

String StringUtil::toString (IN Natural n) {
    return toString(n, 0);
}

/*--------------------*/

String StringUtil::toString (IN Natural n,
                             IN Natural precision,
                             IN String padCharacter) {
    return n.toString(precision, padCharacter);
}

/*--------------------*/

String StringUtil::toPrintableString (IN String& st) {
    const String doubleQuote = "\"";
    String result = st;
    replace(result, doubleQuote, doubleQuote + doubleQuote);
    result = doubleQuote + result + doubleQuote;
    return result;
}

/*--------------------*/

String StringUtil::fromPrintableString (IN String& st) {
    const String doubleQuote = "\"";
    const String strippedSt = strip(st);
    const Natural stringLength = strippedSt.length();
    String result;

    if (!startsWith(st, doubleQuote)) {
        result = st;
    } else {
        result = substring(st, 1, stringLength - 2);
        replace(result, doubleQuote + doubleQuote, doubleQuote);
    }

    return result;
}

/*--------------------*/

String StringUtil::toLowercase (IN String& st)
{
    String result;

    for (const Character& ch : st) {
        result += std::tolower((char) ch);
    }

    return result;
}

/*--------------------*/

String StringUtil::toUppercase (IN String& st)
{
    String result;

    for (const Character& ch : st) {
        result += std::toupper((char) ch);
    }

    return result;
}
