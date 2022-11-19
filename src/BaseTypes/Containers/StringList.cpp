/**
 * @file
 * The <C>StringList</C> body implements lists of string values
 * with zero-based arbitrary indexed access to positions in the
 * list.
 *
 * @author Dr. Thomas Tensi
 * @date   2020-08
 */

/*=========*/
/* IMPORTS */
/*=========*/

#include "Assertion.h"
#include "StringUtil.h"

/*--------------------*/

using BaseModules::StringUtil;
using BaseTypes::Primitives::String;
using BaseTypes::Primitives::String_toString;

/*====================*/

StringList StringList::fromList (IN initializer_list<String> list)
{
    StringList result{};

    for (String element : list) {
        result.append(element);
    }
            
    return result;
}

/*--------------------*/

String StringList::toString () const
{
    return _toString("StringList", String_toString);
}

/*--------------------*/

String StringList::join (IN String& separator) const
{
    String result;
    Natural length = size();

    for (Natural i = 0;  i < length;  i++) {
        result += (i > 0 ? separator : "") + at(i);
    }

    return result;
}

/*--------------------*/

StringList StringList::makeBySplit (IN String& st,
                                    IN String& separator)
{
    const Natural undefined = Natural::maximumValue();
    const Natural separatorLength = separator.length();
    Assertion_pre(separatorLength > 0, "separator must be non-empty");

    StringList result;
    String remainder = st;
    Natural position;

    do {
        position = StringUtil::find(remainder, separator);

        if (position == undefined) {
            result.append(remainder);
        } else {
            result.append(StringUtil::prefix(remainder, position));
            remainder = StringUtil::substring(remainder,
                                              position + separatorLength);
        }
    } while (position != undefined);

    return result;
}
