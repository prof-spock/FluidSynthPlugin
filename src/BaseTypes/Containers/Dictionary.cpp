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

Dictionary::Dictionary ()
    : map()
{
}

/*--------------------*/

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

Dictionary::~Dictionary ()
{
}

/*--------------------*/

String Dictionary::toString () const
{
    return StringUtil
           ::stringMapToStringExplicit<Dictionary,
                                       String,
                                       StringUtil::toPrintableString>("Dictionary", *this);
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

/*--------------------*/

String Dictionary::atWithDefault (IN String& key,
                                  IN String& defaultValue) const
{
    return (contains(key) ? at(key) : defaultValue);
}

/*--------------------*/

Boolean Dictionary::contains (IN String& key) const
{
    return (find(key) != end());
}

/*--------------------*/

void Dictionary::set (IN String& key, IN String& value)
{
    insert_or_assign(key,  value);
}
