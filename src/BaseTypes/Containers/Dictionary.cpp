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

/*====================*/

Dictionary Dictionary::makeFromList (IN StringList& list)
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

String Dictionary::toString () const
{
    return _toString("Dictionary",
                     StringUtil::toPrintableString,
                     StringUtil::toPrintableString);
}

/*--------------------*/

Dictionary Dictionary::makeFromString (IN String& st,
                                       IN String& entrySeparator,
                                       IN String& keyValueSeparator)
{
    Dictionary result;

    /* remove leading and trailing white space from separators to also
       allow omitted blanks in st */
    const String eSeparator = StringUtil::strip(entrySeparator);
    const String kvSeparator = StringUtil::strip(keyValueSeparator);

    const StringList list = StringList::makeBySplit(st, eSeparator);

    for (const String& dictionaryEntryString : list) {
        String key;
        String value;
        Boolean isOkay = StringUtil::splitAt(dictionaryEntryString, 
                                             kvSeparator, key, value);

        if (isOkay) {
            key   =
                StringUtil::fromPrintableString(StringUtil::strip(key));
            value =
                StringUtil::fromPrintableString(StringUtil::strip(value));
            result[key] = value;
        }
    }

    return result;
}
