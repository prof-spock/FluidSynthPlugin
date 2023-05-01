/**
 * @file
 * The <C>StringUtil</C> specification defines several string routines
 * not included in the standard C++ library (like, for example, prefix
 * calculation, whitespace stripping etc).
 *
 * @author Dr. Thomas Tensi
 * @date   2020-08
 */

/*====================*/

#pragma once

/*=========*/
/* IMPORTS */
/*=========*/

#include "Boolean.h"
#include "Character.h"
#include "Percentage.h"
#include "StringList.h"

/*--------------------*/

using BaseTypes::Primitives::Boolean;
using BaseTypes::Primitives::Byte;
using BaseTypes::Primitives::Character;
using BaseTypes::Primitives::Integer;
using BaseTypes::Primitives::Natural;
using BaseTypes::Primitives::Percentage;
using BaseTypes::Primitives::Real;
using BaseTypes::Containers::StringList;

/*====================*/

namespace BaseModules {

    /**
     * The <C>StringUtil</C> type provides several string routines not
     * included in the standard C++ library (like, for example, prefix
     * calculation, whitespace stripping etc).
     */
    struct StringUtil {

        /**
         * Tells whether <C>st</C> contains <C>substring</C> at or after
         * <C>startPosition</C>
         *
         * @param[in] st             the string to be searched for
         *                           substring
         * @param[in] substring      the string to search for in st
         * @param[in] startPosition  the start position of search
         * @return  information whether st contains substring after
         *          start position
         */
        static Boolean contains (IN String& st, IN String& substring,
                                 IN Natural startPosition = 0);

        /*--------------------*/

        /**
         * Tells whether <C>st</C> ends with <C>prefix</C>.
         *
         * @param[in] st      the string to be checked for suffix
         * @param[in] suffix  the string to be checked for being a
         *                    suffix
         * @return  information whether st ends with suffix
         */
        static Boolean endsWith (IN String& st, IN String& suffix);

        /*--------------------*/

        /**
         * Expands parameter %i by <C>replacementi</C> in <C>st</C>
         * and returns expanded version
         *
         * @param[in] st            the string with parameters
         * @param[in] replacement01  replacement string for %1
         * @param[in] replacement02  replacement string for %2
         * @param[in] replacement03  replacement string for %3
         * @param[in] replacement04  replacement string for %4
         * @param[in] replacement05  replacement string for %5
         * @param[in] replacement06  replacement string for %6
         * @param[in] replacement07  replacement string for %7
         * @param[in] replacement08  replacement string for %8
         * @param[in] replacement09  replacement string for %9
         * @param[in] replacement10  replacement string for %A
         * @return  expanded version of st
         */
        static String expand (IN String& st,
                              IN String& replacement01 = "",
                              IN String& replacement02 = "",
                              IN String& replacement03 = "",
                              IN String& replacement04 = "",
                              IN String& replacement05 = "",
                              IN String& replacement06 = "",
                              IN String& replacement07 = "",
                              IN String& replacement08 = "",
                              IN String& replacement09 = "",
                              IN String& replacement10 = "");

        /*--------------------*/

        /**
         * Returns position of <C>substring</C> in <C>st</C> starting
         * at <C>startPosition</C>; if not found, Natural::maxValue is
         * returned.
         *
         * @param[in] st             the string to be searched for
         *                           substring
         * @param[in] substring      the string to search for in st
         * @param[in] startPosition  the start position of search
         * @return  position where st contains substring or
         *          Natural::maxValue
         */
        static Natural find (IN String& st, IN String& substring,
                             IN Natural startPosition = 0);

        /*--------------------*/

        /**
         * Returns position of <C>substring</C> in <C>st</C> starting
         * at <C>startPosition</C> when starting from the end of
         * string; if not found, Natural::maximumValue is returned.
         *
         * @param[in] st             the string to be searched for
         *                           substring
         * @param[in] substring      the string to search for in st
         * @param[in] startPosition  the start position of search
         * @return  position where st contains substring or
         *          Natural::maxValue
         */
        static Natural
        findFromEnd (IN String& st,
                     IN String& substring,
                     IN Natural startPosition = Natural::maximumValue());

        /*--------------------*/

        /**
         * Tells whether <C>st</C> represents a byte.
         *
         * @param[in] st  the string with a byte value
         * @return  information whether string represents an byte or
         *          not
         */
        static Boolean isByte (IN String& st);

        /*--------------------*/

        /**
         * Tells whether <C>st</C> represents an integer.
         *
         * @param[in] st  the string with an integer value
         * @return  information whether string represents an integer or
         *          not
         */
        static Boolean isInt (IN String& st);

        /*--------------------*/

        /**
         * Tells whether <C>st</C> represents an natural.
         *
         * @param[in] st  the string with a natural value
         * @return  information whether string represents a natural or
         *          not
         */
        static Boolean isNatural (IN String& st);

        /*--------------------*/

        /**
         * Tells whether <C>st</C> represents a real value.
         *
         * @param[in] st  the string with a real value
         * @return  information whether string represents a real or not
         */
        static Boolean isReal (IN String& st);

        /*--------------------*/

        /**
         * Gives last character of <C>st</C>
         *
         * @param[in] st  non-empty string
         * @return last character of <C>st</C>
         */
        static Character lastChar (IN String& st);

        /*--------------------*/

        /**
         * Replaces all newlines in <C>st</C> by <C>replacement</C>
         * and returns result.
         *
         * @param[in] st           the string with all newlines to be
         *                         replaced by replacement
         * @param[in] replacement  replacement string for newlines
         * @return sanitized version of <C>st</C>
         */
        static String
        newlineReplacedString (IN String& st,
                               IN String& replacement = "#");

        /*--------------------*/

        /**
         * Returns prefix with at most <C>count</C> characters of
         * <C>st</C>.
         *
         * @param[in] st     the string to be cut
         * @param[in] count  number of characters to extract from front
         * @return  prefix with at most <C>count</C> characters
         */
        static String prefix (IN String& st, IN Natural count);

        /*--------------------*/

        /**
         * Replaces all occurrences of <C>pattern</C> in <C>st</C> by
         * <C>replacement</C> (without rescanning replaced parts).
         *
         * @param[inout] st        the string with all patterns to be
         *                         replaced by replacement
         * @param[in] pattern      literal pattern to be searched for
         * @param[in] replacement  replacement string
         */
        static void replace (INOUT String& st,
                             IN String& pattern,
                             IN String& replacement);

        /*--------------------*/

        /**
         * Splits <C>st</C> at <C>separator</C> and sets <C>prefix</C>
         * and <C>suffix</C>.  If separator is not found, returns
         * false.
         *
         * @param[in] st         the string to be split
         * @param[in] separator  the separation string
         * @param[out] prefix    the part before the separator
         * @param[out] suffix    the part after the separator
         * @return  information whether st contains separator
         */
        static
        Boolean splitAt (IN String& st, IN String& separator,
                         OUT String& prefix, OUT String& suffix);

        /*--------------------*/

        /**
         * Tells whether <C>st</C> starts with <C>prefix</C>.
         *
         * @param[in] st      the string to be checked for prefix
         * @param[in] prefix  the string to be checked for being prefix
         * @return  information whether st starts with prefix
         */
        static Boolean startsWith (IN String& st, IN String& prefix);

        /*--------------------*/

        /**
         * Removes white space from beginning and end of <C>st</C>.
         *
         * @param[in] st  the string with whitespace to be replaced
         * @return  sanitized version of string
         */
        static String strip (IN String& st);

        /*--------------------*/

        /**
         * Returns substring of <C>st</C> starting at <C>firstIndex</C>
         * with at most <C>count</C> characters.
         *
         * @param[in] st          the string to be cut
         * @param[in] firstIndex  the first position in st (starting
         *                        at 0)
         * @param[in] count       number of characters to extract
         * @return  substring from <C>firstIndex</C> with at most
         *          <C>count</C> characters
         */
        static String
        substring (IN String& st,
                   IN Natural firstIndex,
                   IN Natural count = Natural::maximumValue());

        /*--------------------*/

        /**
         * Converts <C>st</C> to a byte; if conversion fails, 0 is
         * returned
         *
         * @param[in] st  the string to be converted to byte
         * @return  byte value of string or zero
         */
        static Byte toByte (IN String& st);

        /*--------------------*/

        /**
         * Converts <C>st</C> to an int; if conversion fails,
         * <C>defaultValue</C> is returned
         *
         * @param[in] st            the string to be converted to integer
         * @param[in] defaultValue  the value to be used for bad conversion
         * @return  integer value of string or default
         */
        static Integer
        toInteger (IN String& st,
                   IN Integer defaultValue = Integer::maximumValue());

        /*--------------------*/

        /**
         * Converts <C>st</C> to a natural; if conversion fails, maximum
         * natural is returned
         *
         * @param[in] st  the string to be converted to natural
         * @return  natural value of string or maximum natural
         */
        static Natural toNatural (IN String& st);

        /*--------------------*/

        /**
         * Converts <C>st</C> to a natural; if conversion fails,
         * <C>defaultValue</C> is returned
         *
         * @param[in] st            the string to be converted to natural
         * @param[in] defaultValue  the value to be used for bad conversion
         * @return  natural value of string or default
         */
        static Natural toNatural (IN String& st,
                                  IN Natural defaultValue);

        /*--------------------*/

        /**
         * Converts <C>st</C> to a percentage value; if conversion
         * fails, NaN is returned
         *
         * @param[in] st  the string to be converted to percentage
         * @return  percentage value of string or NaN
         */
        static Percentage toPercentage (IN String& st);

        /*--------------------*/

        /**
         * Converts <C>st</C> to a real value; if conversion fails,
         * NaN is returned
         *
         * @param[in] st  the string to be converted to real
         * @return  real value of string or NaN
         */
        static Real toReal (IN String& st);

        /*--------------------*/

        /**
         * Converts <C>st</C> to a real value; if conversion fails,
         * <C>defaultValue</C> is returned
         *
         * @param[in] st            the string to be converted to real
         * @param[in] defaultValue  the value to be used for bad conversion
         * @return  real value of string or default
         */
        static Real toReal (IN String& st, IN Real defaultValue);

        /*--------------------*/

        /**
         * Converts boolean value <C>b</C> to string.
         *
         * @param[in] b  boolean value to be converted
         * @return  string representation of bool
         */
        static String toString (IN Boolean b);

        /*--------------------*/

        /**
         * Converts byte value <C>b</C> to string.
         *
         * @param[in] b  byte value to be converted
         * @return  string representation of byte
         */
        static String toString (IN Byte& b);

        /*--------------------*/

        /**
         * Converts real value <C>r</C> to string.
         *
         * @param[in] r  real value to be converted
         * @return  string representation of real
         */
        static String toString (IN Real r);

        /*--------------------*/

        /**
         * Converts integer <C>i</C> to string.
         *
         * @param[in] i  integer value to be converted
         * @return  string representation of integer
         */
        static String toString (IN Integer i);

        /*--------------------*/

        /**
         * Converts integer <C>i</C> to string with precision and
         * padding information.
         *
         * @param[in] i             integer value to be converted
         * @param[in] precision     minimum number of valid digits
         * @param[in] padCharacter  character to use for left padding
         * @return  string representation of integer
         */
        static String toString (IN Integer i,
                                IN Natural precision,
                                IN String padCharacter = "0");

        /*--------------------*/

        /**
         * Converts natural value <C>n</C> to string.
         *
         * @param[in] n  natural value to be converted
         * @return  string representation of natural
         */
        static String toString (IN Natural n);

        /*--------------------*/

        /**
         * Converts natural value <C>n</C> to string with precision
         * and padding information.
         *
         * @param[in] n             natural value to be converted
         * @param[in] precision     minimum number of valid digits
         * @param[in] padCharacter  character to use for left padding
         * @return  string representation of natural
         */
        static String toString (IN Natural n,
                                IN Natural precision,
                                IN String padCharacter = "0");

        /*--------------------*/

        /**
         * Converts string <C>st</C> to string literal representation;
         * in this representation the string is enclosed in double
         * quotes and inner double quotes are duplicated
         *
         * @param[in] st  the string to be converted to a printable string
         * @return  printable value of string
         */
        static String toPrintableString (IN String& st);

        /*--------------------*/

        /**
         * Converts printable string <C>st</C> to internal
         * representation; in this external representation the string
         * is enclosed in double quotes and inner double quotes are
         * duplicated
         *
         * @param[in] st  the string to be converted from a printable string
         * @return  internal value of string
         */
        static String fromPrintableString (IN String& st);

        /*--------------------*/

        /**
         * Converts string <C>st</C> to lower case (using the default
         * locale).
         *
         * @param[in] st  the string to be converted to lower case
         * @return  lower case version of string
         */
        static String toLowercase (IN String& st);

        /*--------------------*/

        /**
         * Converts string <C>st</C> to upper case (using the default
         * locale).
         *
         * @param[in] st  the string to be converted to upper case
         * @return  upper case version of string
         */
        static String toUppercase (IN String& st);

        /*--------------------*/
        /* GENERIC FUNCTIONS  */
        /*--------------------*/

        /** the separator between key an value in a map */
        inline static const String keyValueSeparator = " -> ";

        /** the separator between entries in a list */
        inline static const String entrySeparator = ", ";

        /*--------------------*/

        /**
         * Returns string representation of list <C>list</C> with type
         * name <C>typeName</C> where elements are comma-separated.
         *
         * @param[in] typeName  name of list type
         * @param[in] list      list of strings
         * @return  single string representation of list
         */
        inline
        static String _combineListElements (IN String& typeName,
                                            IN StringList& list)
        {
            return typeName + "(" + list.join(entrySeparator) + ")";
        }

        /*--------------------*/

        /**
         * Converts list <C>list</C> to linear string representation
         * prefixed by <C>typeName</C> assuming an explicit conversion
         * from element type to string
         *
         * @param[in] typeName  name of list type
         * @param[in] list      list of strings
         * @return  single string representation of list
         */
        template<typename List_T,
                 typename Element_T,
                 String elementToStringProc(IN Element_T&)>
        static String listToStringExplicit (IN String& typeName,
                                            IN List_T& list)
        {
            StringList printableStringList;

            for (Element_T element : list) {
                printableStringList.append(elementToStringProc(element));
            }

            return _combineListElements(typeName, printableStringList);
        }

        /*--------------------*/

        /**
         * Converts list <C>list</C> to linear string representation
         * prefixed by <C>typeName</C> assuming an object operation
         *
         * @param[in] typeName  name of list type
         * @param[in] list      list of strings
         * @return  single string representation of list
         */
        template<typename List_T, typename Element_T>
        static String listToStringImplicit (IN String& typeName,
                                            IN List_T& list)
        {
            StringList printableStringList;

            for (Element_T element : list) {
                printableStringList.append(element.toString());
            }

            return _combineListElements(typeName, printableStringList);
        }

        /*--------------------*/

        /**
         * Converts mapping <C>map</C> to linear string
         * representation prefixed by <C>typeName</C>; assumes key
         * is a string and value has some print function
         *
         * @param[in] typeName  name of list type
         * @param[in] map       map to be converted to string
         *                  representation
         * @return  string representation of map
         */
        template<typename Map_T, typename Value_T,
            String valueToStringProc(IN Value_T&)>
        static String stringMapToStringExplicit (IN String& typeName,
                                                 IN Map_T& map)
        {
            StringList printableStringList;

            for (auto iterator = map.cbegin();
                 iterator != map.cend();  iterator++) {
                const String key = toPrintableString(iterator->first);
                const String value = valueToStringProc(iterator->second);
                printableStringList
                    .append(key + keyValueSeparator + value);
            }

            return _combineListElements(typeName, printableStringList);
        }

    };

}

/*--------------------*/

/** a simple shortcut for the generic toString function in the
 * package */
#define TOSTRING BaseModules::StringUtil::toString
