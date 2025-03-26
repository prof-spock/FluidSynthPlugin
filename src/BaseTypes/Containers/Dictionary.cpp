/**
 * @file
 * The <C>Dictionary</C> body implements maps from string keys to
 * string values.
 *
 * @author Dr. Thomas Tensi
 * @date   2020-08
 */

/*=========*/
/* IMPORTS */
/*=========*/

#include "Assertion.h"
#include "Dictionary.h"

/*--------------------*/

using BaseTypes::Containers::Dictionary;

/** abbreviation for StringUtil */
using STR = BaseModules::StringUtil;

/*====================*/

String BaseTypes::Containers::_dictionaryTypeName ()
{
    return "Dictionary";
}
    
/*--------------------*/
/* constructors       */
/*--------------------*/

Dictionary Dictionary::fromList (IN StringList& list)
{
    Dictionary result;

    Assertion_pre(list.size() % 2 == 0, "list must have even length");
    Boolean isKey = true;
    String key;

    for (const String& st : list) {
        if (isKey) {
            key = st;
        } else {
            result[key] = st;
        }

        isKey = !isKey;
    }

    return result;
}

/*--------------------*/

Dictionary Dictionary::makeFromString (IN String& st,
                                       IN String& entrySeparator,
                                       IN String& keyValueSeparator)
{
    Dictionary result;

    /* remove leading and trailing white space from separators to also
       allow omitted blanks in st */
    const String eSeparator = STR::strip(entrySeparator);
    const String kvSeparator = STR::strip(keyValueSeparator);

    const StringList list = StringList::makeBySplit(st, eSeparator);

    for (const String& dictionaryEntryString : list) {
        String key;
        String value;
        Boolean isOkay =
            (STR::contains(dictionaryEntryString, kvSeparator)
             && STR::splitAt(dictionaryEntryString, 
                             kvSeparator, key, value));

        if (isOkay) {
            key   = STR::fromPrintableString(STR::strip(key));
            value = STR::fromPrintableString(STR::strip(value));
            result[key] = value;
        }
    }

    return result;
}

/*--------------------*/

StringList Dictionary::makeKeyListFromString (IN String& st,
                                              IN String& entrySeparator,
                                              IN String& keyValueSeparator)
{
    StringList result;

    /* remove leading and trailing white space from separators to also
       allow omitted blanks in st */
    const String eSeparator = STR::strip(entrySeparator);
    const String kvSeparator = STR::strip(keyValueSeparator);

    const StringList list = StringList::makeBySplit(st, eSeparator);

    for (const String& dictionaryEntryString : list) {
        String key;
        String value;
        Boolean isOkay =
            (STR::contains(dictionaryEntryString, kvSeparator)
             && STR::splitAt(dictionaryEntryString, 
                             kvSeparator, key, value));

        if (isOkay) {
            key   = STR::fromPrintableString(STR::strip(key));
            result.append(key);
        }
    }

    return result;
}
