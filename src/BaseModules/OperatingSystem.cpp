/**
 * @file
 * The <C>OperatingSystem</C> body implements a class for several
 * operating system related utility functions (like e.g. file name
 * transformations or checking whether a file exists).
 *
 * @author Dr. Thomas Tensi
 * @date   2022-08
 */

/*=========*/
/* IMPORTS */
/*=========*/

#include <cstdlib>
    /** qualified version of getenv from stdlib */
    #define StdLib_getenv getenv

#include <filesystem>

#include <stdio.h>
    /** qualified version of fprintf from stdio */
    #define StdIO_fprintf  fprintf
    /** qualified version of stderr from stdio */
    #define StdIO_stderr   stderr

#include "File.h"
#include "Logging.h"
#include "OperatingSystem.h"

/*--------------------*/

using BaseModules::File;
using BaseModules::OperatingSystem;

namespace FileSystem = std::filesystem;

/** abbreviation for StringUtil */
using STR = BaseModules::StringUtil;

/*====================*/

Boolean OperatingSystem::directoryExists (IN String& directoryName)
{
    Logging_trace1(">>: %1", directoryName);
    Boolean result = FileSystem::is_directory(directoryName);
    Logging_trace1("<<: %1", TOSTRING(result));
    return result;
}

/*--------------------*/

Boolean OperatingSystem::fileExists (IN String& fileName)
{
    Logging_trace1(">>: %1", fileName);
    Boolean result = FileSystem::is_regular_file(fileName);
    Logging_trace1("<<: %1", TOSTRING(result));
    return result;
}

/*--------------------*/

StringList OperatingSystem::fileNameList (IN String& directoryName,
                                          IN Boolean plainFilesOnly)
{
    Logging_trace2(">>: directory = %1, plainFilesOnly = %2",
                   directoryName, TOSTRING(plainFilesOnly));

    StringList result;

    for (auto& file : FileSystem::directory_iterator(directoryName)) {
        Boolean isPlainFile = file.is_regular_file();
        String fileName = file.path().filename().string();

        if (isPlainFile == plainFilesOnly) {
            result.append(fileName);
        }
    }
    
    Logging_trace1("<<: %1", result.toString());
    return result;
}

/*--------------------*/

String OperatingSystem::basename (IN String& fileName)
{
    Logging_trace1(">>: %1", fileName);

    String result;

    const Natural undefined = Natural::maximumValue(); 
    Natural aPosition = STR::findFromEnd(fileName, "/");
    Natural bPosition = STR::findFromEnd(fileName, "\\");
    Natural position =
        (bPosition == undefined ? aPosition
         : (aPosition < bPosition ? bPosition : aPosition));

    if (position == undefined) {
        result = fileName;
    } else {
        result = STR::substring(fileName, position + 1);
    }
    
    Logging_trace1("<<: %1", result);
    return result;
}

/*--------------------*/

String OperatingSystem::dirname (IN String& fileName)
{
    Logging_trace1(">>: %1", fileName);

    String result;

    const Natural undefined = Natural::maximumValue(); 
    Natural aPosition = STR::findFromEnd(fileName, "/");
    Natural bPosition = STR::findFromEnd(fileName, "\\");
    Natural position =
        (bPosition == undefined ? aPosition
         : (aPosition < bPosition ? bPosition : aPosition));

    if (position == undefined) {
        result = ".";
    } else {
        result = STR::prefix(fileName, position);
    }
    
    Logging_trace1("<<: %1", result);
    return result;
}

/*--------------------*/

String OperatingSystem::environmentValue (IN String variableName,
                                          IN String defaultValue)
{
    Logging_trace2(">>: variable = '%1', default = '%2'",
                   variableName, defaultValue);
    char* value = (char*) StdLib_getenv((char*) variableName.c_str());
    String result;

    if (value == NULL) {
        result = defaultValue;
    } else {
        result = String(value);
    }

    Logging_trace1("<<: %1", result);
    return result;
}

/*--------------------*/

String OperatingSystem::temporaryDirectoryPath ()
{
    Logging_trace(">>");

    char* environmentPath = (char*) StdLib_getenv("tmp");
    environmentPath = (environmentPath != NULL ? environmentPath
                       : (char*) StdLib_getenv("temp"));
    environmentPath = (environmentPath != NULL ? environmentPath
                       : (char*) "/tmp");
    String result = String(environmentPath);

    Logging_trace1("<<: %1", result);
    return result;
}

/*--------------------*/

void OperatingSystem::writeMessageToConsole (IN String& message)
{
    Logging_trace1(">>: %1", message);
    StdIO_fprintf(StdIO_stderr, "%s\n", message.c_str());
    Logging_trace("<<");
}
