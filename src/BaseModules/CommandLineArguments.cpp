/**
 * @file
 * The <C>CommandLineArguments</C> body implements a handler for
 * command-line arguments converting them to a list of abstract
 * arguments derived from concrete command-line strings describing the
 * actions to be taken in a program.
 *
 * @author Dr. Thomas Tensi
 * @date   2022-08
 */

/*====================*/

/*=========*/
/* IMPORTS */
/*=========*/

#include "Assertion.h"
#include "CommandLineArguments.h"
#include "Logging.h"

/*--------------------*/

using BaseModules::CommandLineArgument;
using BaseModules::CommandLineArgumentList;
using BaseModules::CommandLineArgumentHandler;

/*====================*/

/*--------------------*/
/* PROTOTYPES         */
/*--------------------*/

static Boolean _checkStringForKind (IN String& element,
                                    IN String& elementKind);

/*--------------------*/
/*--------------------*/

/**
 * Checks list of strings <C>elementList</C> whether those strings
 * are okay for given kind <C>elementKind</C>.
 *
 * @param[in] elementList   list of string elements
 * @param[in] elementKind   string to specify kind of elements
 *                          expected ("-", "S", "B", "N" or "R")
 * @return  information whether all elements in list are correct with
 *          respect to given kind
 */
static Boolean _checkElementListForKind (IN StringList& elementList,
                                         IN String& elementKind)
{
    Logging_trace2(">>: elementKind = %1, elementList = %2",
                   elementKind, elementList.toString());

    Boolean result = true;

    for (const String& element : elementList) {
        Boolean isOkay = _checkStringForKind(element, elementKind);

        if (!isOkay) {
            result = false;
            Logging_traceError2("element %1 does not conform to %2",
                                element, elementKind);
            break;
        }
    }
    
    Logging_trace1("<<: %1", TOSTRING(result));
    return result;
}

/*--------------------*/

/**
 * Checks string <C>st</C> whether it is okay for given kind
 * <C>elementKind</C>.
 *
 * @param[in] st           string to be checked
 * @param[in] elementKind  string to specify kind of string expected
 *                         ("-", "S", "B", "N" or "R")
 * @return  information string is correct with respect to given kind
 */
static Boolean _checkStringForKind (IN String& st,
                                    IN String& elementKind)
{
    Logging_trace2(">>: elementKind = %1, st = '%2'",
                   elementKind, st);

    Boolean result;
    String lcString = StringUtil::toLowercase(st);

    if (elementKind == "-") {
        /* unexpected kind */
        result = false;
    } else if (elementKind == "B") {
        /* a boolean value */
        result = (lcString == "0" || lcString == "1"
                  || lcString == "yes" || lcString == "no");
    } else if (elementKind == "N") {
        /* a natural value */
        result = StringUtil::toNatural(st) != Natural::maximumValue();
    } else if (elementKind == "R") {
        /* a real (floating point) value */
        result = StringUtil::toReal(st) != Real::maximumValue();
    } else if (elementKind == "S") {
        /* any string is okay value */
        result = true;
    } else {
        Logging_traceError1("unknown element kind - %1", elementKind);
    }
    
    Logging_trace1("<<: %1", TOSTRING(result));
    return result;
}

/*--------------------*/

/**
 * Looks up raw argument from command line named <C>argument</C>,
 * returns effective original name in <C>originalName</C>, associated
 * abstract argument name in <C>abstractName</C> and the
 * <C>parameterCount</C> of following parameters in command line
 * (typically 0 or 1) with expected kind given by
 * <C>parameterKind</C>; if argument contains a direct assignment, the
 * parameter is returns as a string in <C>embeddedParameter</C>.
 *
 * @param[in]  argument                         command line argument
 +                                              to be analyzed
 * @param[in]  rawArgumentNameToAbstractionMap  map from raw argument
 *                                              name to triple of strings
 * @param[out] originalName                     effective name of argument
 * @param[out] abstractName                     name of abstract argument
 *                                              associated with this
 *                                              concrete command line
 *                                              argument
 * @param[out] parameterCount                   count of following
 *                                              parameters for this
 *                                              abstraction on command
 *                                              line (typically 0 or 1)
 * @param[out] parameterKind                    string to specify kind
 *                                              of parameters expected
 *                                              ("-", "S", "B", "N" or "R")
 * @param[out] embeddedParameter                parameter embedded in
 *                                              option by an equal sign
 */
static
void _lookupArgument (IN String& argument,
                      IN Dictionary& rawArgumentNameToAbstractionMap,
                      OUT String& originalName,
                      OUT String& abstractName,
                      OUT Natural& parameterCount,
                      OUT String& parameterKind,
                      OUT String& embeddedParameter)
{
    Logging_trace1(">>: %1", argument);

    String defaultAbstractionData;
    String lookupKey;

    if (StringUtil::startsWith(argument, "-")) {
        /* this is an option, because it starts with a dash */
        Boolean containsEqualSign =
            StringUtil::splitAt(argument, "=",
                                originalName, embeddedParameter);
        defaultAbstractionData = "ERR_UNKNOWN_OPTION/0/-";
        lookupKey = originalName + (containsEqualSign ? "=" : "");
    } else {
        /* everything not starting with a dash is a file name */
        originalName = argument;
        embeddedParameter = "";

        defaultAbstractionData = "ERR_BADFILE/0/-";
        const String namePrefix = "NAME:*";
        String lowercasedArgument = StringUtil::toLowercase(argument);
        Natural dotPosition =
            StringUtil::findFromEnd(lowercasedArgument, ".");
        Boolean isFound = false;

        /* search for suffix first */
        if (dotPosition != Natural::maximumValue()) {
            String suffix =
                StringUtil::substring(lowercasedArgument, dotPosition);
            lookupKey = namePrefix + suffix;
            isFound =
                rawArgumentNameToAbstractionMap.contains(lookupKey);
        }

        /* when not successful, search for name only */
        if (!isFound) {
            lookupKey = namePrefix;
        }
    }

    const String abstractionDataSeparator = "/";
    String encodedOptionData =
        rawArgumentNameToAbstractionMap.at(lookupKey);
    StringList optionDataList =
        StringList::makeBySplit(encodedOptionData,
                                abstractionDataSeparator);

    Assertion_check(optionDataList.length() == 3,
                    StringUtil::expand("the encoded option data must"
                                       " have length 3 - %1",
                                       optionDataList.toString()));

    abstractName   = optionDataList.at(0);
    parameterCount = StringUtil::toNatural(optionDataList.at(1));
    parameterKind  = optionDataList.at(2);
    
    Logging_trace5("<<: originalName = %1,"
                   " abstractName = %2, parameterCount = %3,"
                   " parameterKind = %4, embeddedParameter = %5",
                   originalName, abstractName,
                   TOSTRING(parameterCount), parameterKind,
                   embeddedParameter);
}
    
/*--------------------*/
/* CommandLineArgument             */
/*--------------------*/

String CommandLineArgument::toString () const
{
    String st =
        StringUtil::expand("CommandLineArgument(originalName = %1,"
                           " abstractName = %2,"
                           " parameterList = %3)",
                           originalName, abstractName,
                           parameterList.toString());
    return st;
}

/*--------------------*/

String CommandLineArgument::toString (IN CommandLineArgument& argument)
{
    return argument.toString();
}

/*--------------------*/
/* CommandLineArgumentList         */
/*--------------------*/

String CommandLineArgumentList::toString () const
{
    return _toString("CommandLineArgumentList");
}
/*--------------------*/
/* CommandLineArgumentHandler */
/*--------------------*/

CommandLineArgumentHandler::CommandLineArgumentHandler ()
{
    Logging_trace(">>");
    Logging_trace("<<");
}

/*--------------------*/

CommandLineArgumentHandler::~CommandLineArgumentHandler ()
{
    Logging_trace(">>");
    Logging_trace("<<");
}

/*--------------------*/

CommandLineArgumentList
CommandLineArgumentHandler::convert
    (IN StringList& rawArgumentList,
     IN Dictionary& rawArgumentNameToAbstractionMap)
{
    Logging_trace2(">>: argumentList = %1, abstractionMap = %2",
                   rawArgumentList.toString(),
                   rawArgumentNameToAbstractionMap.toString());

    CommandLineArgumentList result;
    Natural argumentCount = rawArgumentList.length();

    /* iterate over all arguments except for command name */
    for (Natural i = 1;  i < argumentCount;  i++) {
        const String& rawArgument = rawArgumentList.at(i);
        CommandLineArgument option{ rawArgument, "ERR_UNKNOWN", {} };
        StringList parameterList;

        String abstractName;
        String originalName;
        String embeddedParameter;
        Natural parameterCount;
        String parameterKind;

        _lookupArgument(rawArgument, rawArgumentNameToAbstractionMap,
                       originalName, abstractName, parameterCount,
                      parameterKind, embeddedParameter);
        option.originalName = originalName;

        if (parameterCount + i >= argumentCount) {
            Logging_traceError2("not enough parameters to read"
                                " another %1 options for %2",
                                TOSTRING(parameterCount),
                                originalName);
            option.abstractName = "ERR_NOPARAMS";
        } else {
            if (rawArgument != originalName) {
                /* this is an assignment option */
                parameterList.append(embeddedParameter);
            }

            for (Natural j = 0;  j < parameterCount;  j++) {
                const String& parameter = rawArgumentList.at(++i);
                parameterList.append(parameter);
            }

            option.parameterList = parameterList;
            Boolean isOkay =
                _checkElementListForKind(parameterList, parameterKind);
            option.abstractName =
                (!isOkay ? "ERR_BADPARAMS" : abstractName);
        }

        result.append(option);
    }
    
    Logging_trace1("<<: %1", result.toString());
    return result;
}
