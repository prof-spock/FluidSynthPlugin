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

/*====================*/

Boolean OperatingSystem::fileExists (IN String& fileName)
{
    Logging_trace1(">>: %1", fileName);

    File file;
    Boolean result = file.open(fileName, "r");
    file.closeConditionally();

    Logging_trace1("<<: %1", TOSTRING(result));
    return result;
}

/*--------------------*/

String OperatingSystem::dirname (IN String& fileName)
{
    Logging_trace1(">>: %1", fileName);

    String result;

    const Natural undefined = Natural::maximumValue(); 
    Natural aPosition = StringUtil::findFromEnd(fileName, "/");
    Natural bPosition = StringUtil::findFromEnd(fileName, "\\");
    Natural position =
        (bPosition == undefined ? aPosition
         : (aPosition < bPosition ? bPosition : aPosition));

    if (position == undefined) {
        result = ".";
    } else {
        result = StringUtil::prefix(fileName, position);
    }
    
    Logging_trace1("<<: %1", result);
    return result;
}

/*--------------------*/

String OperatingSystem::temporaryDirectoryPath ()
{
    char* environmentPath = (char*) StdLib_getenv("tmp");
    environmentPath = (environmentPath != nullptr ? environmentPath
                       : (char*) StdLib_getenv("temp"));
    environmentPath = (environmentPath != nullptr ? environmentPath
                       : (char*) "/tmp");
    String result = String(environmentPath);
    return result;
}

/*--------------------*/

void OperatingSystem::writeMessageToConsole (IN String& message)
{
    Logging_trace1(">>: %1", message);
    StdIO_fprintf(StdIO_stderr, "%s\n", message.c_str());
    Logging_trace("<<");
}
