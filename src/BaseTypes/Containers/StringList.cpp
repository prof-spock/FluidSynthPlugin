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

using BaseTypes::Primitives::String;

/** abbreviation for StringUtil */
using STR = BaseModules::StringUtil;

/*====================*/

String BaseTypes::Containers::_stringListTypeName ()
{
    return "StringList";
}

/*--------------------*/
/* constructors       */
/*--------------------*/

StringList StringList::fromList (IN initializer_list<String> list)
{
    StringList result{};

    for (const String& element : list) {
        result.append(element);
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
        position = STR::find(remainder, separator);

        if (position == undefined) {
            result.append(remainder);
        } else {
            result.append(STR::prefix(remainder, position));
            remainder = STR::substring(remainder,
                                       position + separatorLength);
        }
    } while (position != undefined);

    return result;
}


/*--------------------*/
/* type conversions   */
/*--------------------*/

String StringList::toString () const
{
    return GenericList::toString();
}

/*--------------------*/

String StringList::toString (IN StringList& list)
{
    return list.toString();
}

/*--------------------*/
/* complex functions  */
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

StringList StringList::slice (IN Integer firstPosition,
                              IN Integer lastPosition)
{
    return StringList{GenericList::slice(*this,
                                         firstPosition, lastPosition)};
}
