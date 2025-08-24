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
#include <filesystem>
#include <stdio.h>
    /** qualified version of fprintf from stdio */
    #define StdIO_fprintf  fprintf
    /** qualified version of stderr from stdio */
    #define StdIO_stderr   stderr
#include <thread>

#include "File.h"
#include "Logging.h"

#include "OperatingSystem.h"

/*====================*/

#ifdef _WINDOWS
    #include "MyWindows.h"
#else
    #include <unistd.h>
        #define UniStd_sleep  sleep
    #include <dlfcn.h>
#endif

/*--------------------*/

using BaseModules::File;
using BaseModules::OperatingSystem;
using BaseTypes::Primitives::Time;
using BaseTypes::Primitives::Time_addDuration;
using BaseTypes::Primitives::Time_withinProcess;

namespace FileSystem = std::filesystem;

/** abbreviation for StringUtil */
using STR = BaseModules::StringUtil;

/*====================*/

#ifdef _WINDOWS

    /**
     * Returns path of directory of current library or executable
     * file.
     *
     * @param[in] isExecutable  tells whether this is a library
     *                          or an executable
     * @return  path of executable or library
     */
    static String _executableDirectoryPath (IN Boolean isExecutable)
    {
        Windows::HMODULE component;

        if (isExecutable) {
            component = NULL;
        } else {
            Windows::DWORD flags =
                (GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS | 
                 GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT);
            const Windows::LPWSTR address =
                reinterpret_cast<Windows::LPWSTR>(&_executableDirectoryPath);
            Windows::GetModuleHandleExW(flags, address, &component);
        }
        
        Windows::WCHAR wResult[MAX_PATH];
        Windows::GetModuleFileNameW(component, wResult, MAX_PATH);
        String result = TOSTRING(std::wstring(wResult));
        result = OperatingSystem::dirname(result);
        return result;
    }

    /*--------------------*/

    /**
     * Sleeps for <C>duration</C> seconds.
     *
     * @param[in] duration   sleep duration in seconds
     */
    static void _sleepForDuration (IN float duration)
    {
        Windows::Sleep(duration * 1000.0);
    }
#else

    /*========================*/
    /* UNIX/MACOS DEFINITIONS */
    /*========================*/

    /**
     * Returns path of directory of current library or executable
     * file.
     *
     * @param[in] isExecutable  tells whether this is a library
     *                          or an executable
     * @return  path of executable or library
     */
    static String _executableDirectoryPath (IN Boolean isExecutable)
    {
        String result;

        if (isExecutable) {
            ssize_t effectiveLength;
            constexpr size_t length = 1000;
            char executablePath[length];
            effectiveLength = readlink("/proc/self/exe", executablePath,
                                       length);
            result = std::string(executablePath);

            if (effectiveLength < 0 || effectiveLength == length) {
                Logging_traceError("--: readlink failed");
            }
        } else {
            Dl_info libraryData;
            Boolean isOkay =
                (dladdr(reinterpret_cast<void*>(&_executableDirectoryPath),
                        &libraryData) != 0);
            result = isOkay ? TOSTRING(libraryData.dli_fname) : "";
        }

        return result;
    }

    /*--------------------*/

    /**
     * Sleeps for <C>duration</C> seconds.
     *
     * @param[in] duration   sleep duration in seconds
     */
    static void _sleepForDuration (IN float duration)
    {
        UniStd_sleep(duration);
    }
#endif  

/*====================*/
/* LOCAL FUNCTIONS    */
/*====================*/

/**
 * Reads associated value for environment variable named
 * <C>variableName</C> into <C>value</C>; returns false when not found.
 *
 * @param[in]  variableName  name of environment variable
 * @param[out] value         associated variable value (if any)
 * @return  information whether variable is set
 */
static Boolean _environmentValue (IN String& variableName,
                                  OUT String& value)
{
    char* envValue = std::getenv(variableName.c_str());
    Boolean result = false;

    if (envValue != NULL) {
        result = true;
        value = String{envValue};
    }
    
    return result;
}

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

String OperatingSystem::basename (IN String& fileName,
                                  IN Boolean extensionIsShown)
{
    Logging_trace2(">>: fileName = %1, extensionIsShown = %2",
                   fileName, TOSTRING(extensionIsShown));

    String result;

    const Natural undefined = Natural::maximumValue(); 
    Natural aPosition = STR::findFromEnd(fileName, "/");
    Natural bPosition = STR::findFromEnd(fileName, "\\");
    Natural position =
        (bPosition == undefined ? aPosition
         : (aPosition == undefined ? bPosition
            : Natural::maximum(aPosition, bPosition)));

    if (position == undefined) {
        result = fileName;
    } else {
        result = STR::substring(fileName, position + 1);
    }

    if (!extensionIsShown) {
        Natural dotPosition = STR::findFromEnd(result, ".");

        if (position != undefined) {
            result = STR::prefix(result, dotPosition);
        }
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
         : (aPosition == undefined ? bPosition
            : Natural::maximum(aPosition, bPosition)));

    if (position == undefined) {
        result = ".";
    } else {
        result = STR::prefix(fileName, position);
    }
    
    Logging_trace1("<<: %1", result);
    return result;
}

/*--------------------*/

String OperatingSystem::environmentValue (IN String& variableName)
{
    Logging_trace1(">>: '%1'", variableName);

    String result = "";
    _environmentValue(variableName, result);

    Logging_trace1("<<: '%1'", result);
    return result;
}

/*--------------------*/

String OperatingSystem::executableDirectoryPath (IN Boolean isExecutable)
{
    Logging_trace(">>");
    String result = _executableDirectoryPath(isExecutable);
    Logging_trace1("<<: %1", result);
    return result;
}

/*--------------------*/

/**
 * Sleeps for <C>duration</C>.
 *
 * @param[in] duration  sleep duration (in seconds)
 */
void OperatingSystem::sleep (IN Duration duration)
{
    Time startTime = Time_withinProcess();
    Time endTime   = Time_addDuration(startTime, duration);
    Time stopTime = 0.0;

    while (stopTime < endTime) {
        stopTime = Time_withinProcess();
        _sleepForDuration(((double) duration) / 10.0);
    }
}

/*--------------------*/

String OperatingSystem::systemName ()
{
    Logging_trace(">>");
    #ifdef _WINDOWS
        String result = "Windows";
    #else
        #ifdef UNIX
            String result = "Unix";
        #else
            String result = "MacOS";
        #endif
    #endif

    Logging_trace1("<<: %1", result);
    return result;
}

/*--------------------*/

String OperatingSystem::temporaryDirectoryPath ()
{
    Logging_trace(">>");

    Boolean isOkay;
    String result = "/tmp";

    isOkay = _environmentValue("tmp", result);

    if (!isOkay) {
        isOkay = _environmentValue("temp", result);
    }

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
