/**
 * @file
 * The <C>Environment</C> body implements operating system services
 * related to environment variables
 *
 * @author Dr. Thomas Tensi
 * @date   2024-03
 */

/*=========*/
/* IMPORTS */
/*=========*/

#include <cstdlib>

#include "Environment.h"
#include "Logging.h"

/*====================*/

using BaseModules::Environment;

/** abbreviation for StringUtil */
using STR = BaseModules::StringUtil;

/*====================*/

#ifdef _WIN32

    /*=====================*/
    /* WINDOWS DEFINITIONS */
    /*=====================*/

    /** list of environment variables */
    static char** _environmentSettingsList = _environ;

#else

    /*========================*/
    /* UNIX/MACOS DEFINITIONS */
    /*========================*/

    extern char ** environ;
    /** list of environment variables */
    static char** _environmentSettingsList = environ;

#endif  

/*====================*/

/** lead in for environment variable references in a string */
static const String _leadIn = "${";

/** lead out for environment variable references in a string */
static const String _leadOut = "}";

/*--------------------*/

/**
 * Returns <C>variableName</C> with environment variable braces.
 *
 * @param[in] variableName  environment variable name
 */
static String _bracedVariableName (IN String& variableName)
{
    Logging_trace1(">>: %1", variableName);
    String result = _leadIn + variableName + _leadOut;
    Logging_trace1("<<: %1", result);
    return result;
}

/*====================*/

Dictionary Environment::all ()
{
    Logging_trace(">>");

    char** ptr = _environmentSettingsList;
    const String entrySeparator = "§§§";
    String st;

    while (*ptr != NULL) {
        st = (st
              + (st == "" ? "" : entrySeparator)
              + String{*ptr++});
    }
    
    Dictionary result =
        Dictionary::makeFromString(st, entrySeparator, "=");
    Logging_trace1("<<: %1", result.toString());
    return result;
}

/*--------------------*/

String Environment::expand (IN String& st)
{
    Logging_trace1(">>: %1", st);

    String result = st;
    const Natural notFound = Natural::maximumValue();
    Natural leadInPosition = 0;
    
    while (true) {
        leadInPosition = STR::find(result, _leadIn, leadInPosition);

        if (leadInPosition == notFound) {
            break;
        }

        Natural leadOutPosition =
            Natural::minimum(result.length(),
                             STR::find(result, _leadOut, leadInPosition));
        Natural variableNamePosition = leadInPosition + _leadIn.length();
        Natural variableNameLength = leadOutPosition - variableNamePosition;
        String variableName =
            STR::substring(result,
                           variableNamePosition, variableNameLength);
        char* variableValue = std::getenv(variableName.c_str());

        if (variableValue == NULL) {
            /* start search one position further... */
            leadInPosition++;
        } else {
            String key = _bracedVariableName(variableName);
            STR::replace(result, key, String{variableValue});
        }
    }
    
    Logging_trace1("<<: %1", result);
    return result;
}

/*--------------------*/

Boolean Environment::replacePrefix (INOUT String& st)
{
    Logging_trace1(">>: %1", st);

    const Natural minimumRelevantLength = 5;
    String bestVariableName;
    Natural prefixLength = minimumRelevantLength;
    Dictionary environment = all();
    Boolean result = false;
    
    for (auto & [variableName, variableValue] : environment) {
        Natural valueLength = variableValue.length();

        if (valueLength > prefixLength
            && STR::startsWith(st, variableValue)) {
            /* this is a new and longer match */
            prefixLength = valueLength;
            bestVariableName = variableName;
        }
    }

    if (bestVariableName > "") {
        String bestVariableValue = environment.at(bestVariableName);
        bestVariableName = _bracedVariableName(bestVariableName);
        STR::replace(st, bestVariableValue, bestVariableName);
        result = true;
    }
    
    Logging_trace2("<<: result = %1, st = %2",
                   TOSTRING(result), st);
    return result;
}

/*--------------------*/

String Environment::value (IN String& variableName,
                           IN String& defaultValue)
{
    Logging_trace2(">>: variable = '%1', default = '%2'",
                   variableName, defaultValue);
    char* value = std::getenv(variableName.c_str());
    String result;

    if (value == NULL) {
        result = defaultValue;
    } else {
        result = String(value);
    }

    Logging_trace1("<<: %1", result);
    return result;
}
