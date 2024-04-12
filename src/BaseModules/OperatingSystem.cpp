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

#include "File.h"
#include "Logging.h"
#include "OperatingSystem.h"

/*====================*/

#ifdef _WIN32
    #include "MyWindows.h"
#else
    #include <unistd.h>
    #include <dlfcn.h>
#endif

/*--------------------*/

using BaseModules::File;
using BaseModules::OperatingSystem;

namespace FileSystem = std::filesystem;

/** abbreviation for StringUtil */
using STR = BaseModules::StringUtil;

/*====================*/

#ifdef _WIN32

    /**
     * Returns path of directory of current library or executable
     * file.
     *
     * @param[in] isExecutable  tells whether this is a library
     *                          or an executable
     * @return  path of executable or library
     */
    String _executableDirectoryPath (IN Boolean isExecutable)
    {
        Windows::HMODULE component;

        if (isExecutable) {
            component = NULL;
        } else {
            Windows::DWORD flags =
                (GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS | 
                 GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT);
            Windows::LPWSTR address =
                (Windows::LPWSTR) &_executableDirectoryPath;
            Windows::GetModuleHandleExW(flags, address, &component);
        }
        
        Windows::WCHAR wResult[MAX_PATH];
        Windows::GetModuleFileNameW(component, wResult, MAX_PATH);
        String result = TOSTRING(std::wstring(wResult));
        result = OperatingSystem::dirname(result);
        return result;
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
    String _executableDirectoryPath (IN Boolean isExecutable)
    {
        String result;

        if (isExecutable) {
            size_t effectiveLength;
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
                (dladdr((void*) &_executableDirectoryPath, &libraryData) != 0);
            result = isOkay ? TOSTRING(libraryData.dli_fname) : "";
        }

        return result;
    }

#endif  

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
         : (aPosition == undefined ? bPosition
            : Natural::maximum(aPosition, bPosition)));

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

String OperatingSystem::executableDirectoryPath (IN Boolean isExecutable)
{
    Logging_trace(">>");
    String result = _executableDirectoryPath(isExecutable);
    Logging_trace1("<<: %1", result);
    return result;
}

/*--------------------*/

String OperatingSystem::temporaryDirectoryPath ()
{
    Logging_trace(">>");

    char* environmentPath = std::getenv("tmp");
    environmentPath = (environmentPath != NULL ? environmentPath
                       : std::getenv("temp"));
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
