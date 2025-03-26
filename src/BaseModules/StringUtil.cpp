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

#include <cctype>
#include <cstdarg>
#include <cwchar>

#include "Assertion.h"
#include "StringUtil.h"

/*--------------------*/

using BaseModules::StringUtil;
using STR = BaseModules::StringUtil;

/*====================*/

/** the value returned in the standard library when something is
  * not found  */
static const size_t _notFound = std::string::npos;

/** the character representing a blank character */
static const Character _blankCharacter{' '};

/** the character representing a minus sign in a number */
static const Character _minusCharacter{'-'};

/** the character representing a plus sign in a number */
static const Character _plusCharacter{'+'};

/** the list of sign characters */
static const String _signCharacterList = "+-";

/** the character representing a decimal point in a number
 *  (assuming an international locale) */
static const Character _decimalPointCharacter{'.'};

/** list of all digit characters */
static const String _digitCharacterList = "0123456789";

/** list of all lowercase alpha digit characters */
static const String _lcAlphaDigitCharacterList =
    "abcdefghijklmnopqrstuvwxyz";

/** list of all lowercase alpha digit characters */
static const String _ucAlphaDigitCharacterList =
    "ABCDEFGHIJKLMNOPQRSTUVWXYZ";

/*--------------------*/
/* PRIVATE ROUTINES   */
/*--------------------*/

#define isDigitChar(ch)   isdigit((char) ch)

/**
 * Analyzes <C>st</C> for being a correct natural or integer
 * (depending on <C>isIntegerString</C> relative to <C>base</C>.
 *
 * @param[in]  st               string to be analyzed
 * @param[in]  base             number base for string
 * @param[in]  isIntegerString  information whether this should be an
 *                              integer
 * @param[out] isNegative       information whether string is
 *                              negative
 * @param[out] digitString      string with digits
 * @return  information whether string conforms to integer or natural
 */
static Boolean _analyzeSimpleNumberString (IN String& st,
                                           IN Natural base,
                                           IN Boolean isIntegerString,
                                           OUT Boolean& isNegative,
                                           OUT String& digitString)
{
    const Natural stringLength = st.length();
    Boolean isOkay = (stringLength > 0);

    if (isOkay) {
        isNegative = (STR::characterAt(st, 0) == '-');
        Natural i = (isNegative ? 1 : 0);

        if (i + 2 < stringLength) {
            String prefix = STR::substring(st, i, 2);

            if (prefix == "0X" || prefix == "0x") {
                i += 2;
            }
        }

        /* at least one character must be left */
        digitString = STR::substring(st, i);
        isOkay = (digitString.length() > 0);
    
        String allowedCharacterList =
            (STR::prefix(_digitCharacterList, base)
             + STR::prefix(_lcAlphaDigitCharacterList, base - 10)
             + STR::prefix(_ucAlphaDigitCharacterList, base - 10));

        for (;  i < stringLength;  i++) {
            Character ch = STR::characterAt(st, i);

            if (!STR::contains(allowedCharacterList, ch)) {
                isOkay = false;
                break;
            }
        }
    }

    return isOkay;
}

/*--------------------*/

/**
 * Converts a plain string <C>st</C> containing digits of <C>base</C>
 * to a natural value.
 *
 * @param[in] st    string to be converter
 * @param[in] base  number base
 * @return  natural value
 */
Natural _convertDigitString (IN String& st,
                             IN Natural base)
{
    const Natural stringLength = st.length();
    Natural result = 0;
    Natural offset = _digitCharacterList.length();

    for (Natural i = 0;  i < stringLength;  i++) {
        Character ch = STR::characterAt(st, i);
        Natural digitValue = 0;

        if (STR::contains(_digitCharacterList, ch)) {
            digitValue = STR::find(_digitCharacterList, ch);
        } else if (STR::contains(_lcAlphaDigitCharacterList, ch)) {
            digitValue = (STR::find(_lcAlphaDigitCharacterList, ch)
                          + offset);
        } else if (STR::contains(_ucAlphaDigitCharacterList, ch)) {
            digitValue = (STR::find(_ucAlphaDigitCharacterList, ch)
                          + offset);
        }

        result = result * base + digitValue;
    }

    return result;
}

/*--------------------*/

/**
 * Converts <C>st</C> to a natural with a given base (typically 2, 8,
 * 16); if conversion fails, <C>defaultValue</C> is returned
 *
 * @param[in] st            the string to be converted to natural
 * @param[in] base          the base of the conversion
 * @param[in] defaultValue  the value to be used for bad conversion
 * @return  natural value of string for base or default
 */
static Natural _toNaturalWithBaseAndDefault (IN String& st,
                                             IN Natural base,
                                             IN Natural defaultValue)
{
    Natural result = defaultValue;
    Boolean isNegative;
    String digitString;
    Boolean isOkay =
        _analyzeSimpleNumberString(st, base, false,
                                   isNegative, digitString);

    if (isOkay) {
        result = _convertDigitString(digitString, base);
    }

    return result;
}

/*--------------------*/
/* PUBLIC ROUTINES    */
/*--------------------*/

void StringUtil::append (INOUT String& st, IN Character ch)
{
    st += TOSTRING(ch);
}

/*--------------------*/

Character StringUtil::characterAt (IN String& st, IN Natural position)
{
    return Character{st[(size_t) position]};
}

/*--------------------*/

Boolean StringUtil::contains (IN String& st, IN Character ch,
                              IN Natural startPosition)
{
    return find(st, ch, startPosition) != Natural::maximumValue();
}

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
                           IN String& replacement10,
                           IN String& replacement11,
                           IN String& replacement12,
                           IN String& replacement13,
                           IN String& replacement14,
                           IN String& replacement15)
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
    replace(result, "%B", replacement11);
    replace(result, "%C", replacement12);
    replace(result, "%D", replacement13);
    replace(result, "%E", replacement14);
    replace(result, "%F", replacement15);
    return result;
}

/*--------------------*/

Natural StringUtil::find (IN String& st, IN Character ch,
                          IN Natural startPosition)
{
    Natural result = st.find((char) ch, (size_t) startPosition);
    result = (result == _notFound ? Natural::maximumValue() : result);
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

Character StringUtil::firstCharacter (IN String& st)
{
    return Character{st[0]};
}

/*--------------------*/

Boolean StringUtil::isByte (IN String& st)
{
    return isNatural(st) && (toNatural(st) < 256);
}

/*--------------------*/

Boolean StringUtil::isInt (IN String& st)
{
    Boolean isNegative;
    String digitString;
    Boolean result =
        _analyzeSimpleNumberString(st, 10, true,
                                   isNegative, digitString);
    return result;
}

/*--------------------*/

Boolean StringUtil::isNatural (IN String& st, IN Natural base)
{
    Boolean isNegative;
    String digitString;
    Boolean result =
        _analyzeSimpleNumberString(st, base, false,
                                   isNegative, digitString);
    return result;
}

/*--------------------*/

Boolean StringUtil::isReal (IN String& st)
{
    Boolean result;
    const Natural stringLength = st.length();
    const Boolean hasSign =
        STR::contains(_signCharacterList, STR::firstCharacter(st));

    if (stringLength == 0) {
        result = false;
    } else if (stringLength == 1 && hasSign) {
        result = false;
    } else {
        enum struct State {
            atMantissaSign, inIntegralPart, inFractionalPart,
            atExponentSign, inExponent
        };

        State state = State::atMantissaSign;
        result = true;

        for (Natural i = 0;  i < stringLength;  i++) {
            const Character ch{characterAt(st, i)};

            if (state == State::atMantissaSign) {
                if (ch == _plusCharacter || ch == _minusCharacter
                    || ch == _blankCharacter || isDigitChar(ch)) {
                    state = State::inIntegralPart;
                } else if (ch == _decimalPointCharacter) {
                    state = State::inFractionalPart;
                } else {
                    result = false;
                    break;
                }
            } else if (state == State::inIntegralPart) {
                if (ch == _decimalPointCharacter) {
                    state = State::inFractionalPart;
                } else if (!isDigitChar(ch)) {
                    result = false;
                    break;
                }
            } else if (state == State::inFractionalPart) {
                if (ch == 'E' || ch == 'e') {
                    state = State::atExponentSign;
                } else if (!isDigitChar(ch)) {
                    result = false;
                    break;
                }
            } else if (state == State::atExponentSign) {
                if (ch == _plusCharacter || ch == _minusCharacter
                    || isDigitChar(ch)) {
                    state = State::inExponent;
                } else {
                    result = false;
                    break;
                }
            } else if (state == State::inExponent) {
                if (!isDigitChar(ch)) {
                    result = false;
                    break;
                }
            }
        }
    }

    return result;
}

/*--------------------*/

Character StringUtil::lastCharacter (IN String& st)
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

String StringUtil::paddedLeft (IN String& st,
                               IN Natural desiredLength,
                               IN String& ch)
{
    Assertion_check(ch.length() == 1,
                    "padding must use a single character");
    String result = st;

    while (result.length() < desiredLength) {
        result = ch + result;
    }

    return result;
}

/*--------------------*/

String StringUtil::paddedRight (IN String& st,
                                IN Natural desiredLength,
                                IN String& ch)
{
    Assertion_check(ch.length() == 1,
                    "padding must use a single character");
    String result = st;

    while (result.length() < desiredLength) {
        result += ch;
    }

    return result;
}

/*--------------------*/

String StringUtil::prefix (IN String& st, IN Natural count)
{
    return substring(st, 0, count);
}

/*--------------------*/

void StringUtil::prepend (INOUT String& st, IN Character ch)
{
    st = TOSTRING(ch) + st;
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

void StringUtil::setCharacterAt (INOUT String& st,
                                 IN Natural position,
                                 IN Character ch)
{
    st[(size_t) position] = (char) ch;
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
        suffix = substring(st, position + separatorLength);
        prefix = StringUtil::prefix(st, position);
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
        if (whiteSpace.find((char) characterAt(st, i - 1)) == _notFound) {
            break;
        }
    }

    const Natural lastRelevantIndex = i - Natural{1};

    for (i = 0;  i < stringLength;  i++) {
        if (whiteSpace.find((char) characterAt(st, i)) == _notFound) {
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

Byte StringUtil::toByte (IN String& st)
{
    return (isByte(st) ? Byte{(int) toNatural(st)} : Byte(0));
}

/*--------------------*/

Integer StringUtil::toInteger (IN String& st,
                               IN Integer defaultValue)
{
    Integer result;
    Natural base = 10;
    Boolean isNegative;
    String digitString;
    Boolean isOkay =
        _analyzeSimpleNumberString(st, base, true,
                                   isNegative, digitString);

    if (!isOkay) {
        result = defaultValue;
    } else {
        result = _convertDigitString(digitString, base);
        result *= (isNegative ? -1 : 1);
    }

    return result;
}

/*--------------------*/

Natural StringUtil::toNatural (IN String& st)
{
    return toNatural(st, Natural::maximumValue());
}

/*--------------------*/

Natural StringUtil::toNatural (IN String& st, IN Natural defaultValue)
{
    return _toNaturalWithBaseAndDefault(st, 10, defaultValue);
}

/*--------------------*/

Natural StringUtil::toNaturalWithBase (IN String& st,
                                       IN Natural base)
{
    return _toNaturalWithBaseAndDefault(st, base,
                                        Natural::maximumValue());
}

/*--------------------*/

Percentage StringUtil::toPercentage (IN String& st)
{
    return Percentage(toReal(st));
}

/*--------------------*/

Real StringUtil::toReal (IN String& st)
{
    return toReal(st, Real::maximumValue());
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

String StringUtil::toString (IN Byte b)
{
    return b.toString();
}

/*--------------------*/

String StringUtil::toString (IN Character ch)
{
    return ch.toString();
}

/*--------------------*/

String StringUtil::toString (IN Integer i)
{
    return toString(i, 0);
}

/*--------------------*/

String StringUtil::toString (IN Integer i,
                             IN Natural precision,
                             IN String& padString)
{
    return i.toString(precision, padString);
}

/*--------------------*/

String StringUtil::toString (IN Natural n)
{
    return toString(n, 0);
}

/*--------------------*/

String StringUtil::toString (IN Natural n,
                             IN Natural precision,
                             IN String& padString)
{
    return n.toString(precision, padString);
}

/*--------------------*/

String StringUtil::toStringWithBase (IN Natural n,
                                     IN Natural base,
                                     IN Natural precision,
                                     IN String& padString)
{
    return n.toStringWithBase(base, precision, padString);
}

/*--------------------*/

String StringUtil::toString (IN Real r)
{
    return std::to_string((double) r);
}

/*--------------------*/

String StringUtil::toString (IN Real r,
                             IN Natural precision,
                             IN Natural fractionalDigitCount,
                             IN String& padString,
                             IN Boolean scientificNotationIsForced)
{
    return r.toString(precision, fractionalDigitCount, padString,
                      scientificNotationIsForced);
}

/*--------------------*/

String StringUtil::toString (IN std::wstring& st)
{
    size_t length = st.size() + 1;
    String result;
    result.resize(length);
    const wchar_t* wCharPtr = const_cast<wchar_t*>(st.c_str());
    std::mbstate_t state{};
    std::wcsrtombs(&result[0], &wCharPtr, length, &state);
    return result;
}

/*--------------------*/

String StringUtil::toPrintableString (IN String& st)
{
    const String doubleQuote = "\"";
    String result = st;
    replace(result, doubleQuote, doubleQuote + doubleQuote);
    result = doubleQuote + result + doubleQuote;
    return result;
}

/*--------------------*/

String StringUtil::fromPrintableString (IN String& st)
{
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

    for (const Character ch : st) {
        result += std::tolower((char) ch);
    }

    return result;
}

/*--------------------*/

String StringUtil::toUppercase (IN String& st)
{
    String result;

    for (const Character ch : st) {
        result += std::toupper((char) ch);
    }

    return result;
}

/*--------------------*/

std::wstring StringUtil::toWideString (IN String& st)
{
    size_t length = st.size() + 1;
    std::wstring result;
    result.resize(length);
    wchar_t* wCharPtr = const_cast<wchar_t*>(result.c_str());
    const char* charPtr = st.c_str();
    std::mbstate_t state{};
    std::mbsrtowcs(wCharPtr, &charPtr, length, &state);
    return result;
}
