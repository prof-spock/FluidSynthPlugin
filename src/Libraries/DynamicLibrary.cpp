/**
 * @file
 * The <C>DynamicLibrary</C> class implements a very simple and crude
 * wrapper around the dynamic library loading in Windows.
 *
 * @author Dr. Thomas Tensi
 * @date   2022-04
 */

/*=========*/
/* IMPORTS */
/*=========*/

#include "DynamicLibrary.h"
#include "Logging.h"

#ifdef _WIN32
    /* WINDOWS INCLUDE FILE */
    #include "MyWindows.h"
#else
    /* UNIX/MACOS INCLUDE FILE */
    #include <dlfcn.h>
#endif

/*--------------------*/

using Libraries::DynamicLibrary;

/** abbreviation for StringUtil */
using STR = BaseModules::StringUtil;

/*========================*/

#ifdef _WIN32
    /*=====================*/
    /* WINDOWS DEFINITIONS */
    /*=====================*/

    Boolean _addToSearchPath (IN String& searchPath)
    {
        std::wstring sp = STR::toWideString(searchPath);
        return Windows::SetDllDirectoryW((Windows::LPCWSTR) sp.c_str());
    }

    /*--------------------*/

    Object _loadLibrary (IN String& pathName)
    {
        Windows::DWORD dwFlags = LOAD_LIBRARY_SEARCH_DEFAULT_DIRS;
        Object result =
            (Object) Windows::LoadLibraryExA((char*) pathName.c_str(),
                                             NULL, dwFlags);

        if (result == NULL) {
            /* just for debugging the library loading */
            Natural errorCode{(size_t) Windows::GetLastError()};
            String message{STR::expand("load error %1",
                                       TOSTRING(errorCode))};
        }
        
        return result;
    }

    /*--------------------*/

    void _freeLibrary (Object descriptor)
    {
        Windows::FreeLibrary((Windows::HMODULE) descriptor);
    }

    /*--------------------*/

    Object _getFunctionByName (IN Object descriptor,
                               IN String& functionName)
    {
        return Windows::GetProcAddress((Windows::HMODULE) descriptor,
                                       (char*) functionName.c_str());
    }

#else
    /*========================*/
    /* UNIX/MACOS DEFINITIONS */
    /*========================*/

    Boolean _addToSearchPath (IN String& searchPath)
    {
        return false;
    }

    /*--------------------*/

    Object _loadLibrary (IN String& pathName)
    {
        Object result = (Object) dlopen((char*) pathName.c_str(),
                                        RTLD_NOW);

        if (result == NULL) {
            /* just for debugging the library loading */
            String message{(char *) dlerror()};
        }

        return result;
    }

    /*--------------------*/

    void _freeLibrary (Object descriptor)
    {
        dlclose(descriptor);
    }

    /*--------------------*/

    Object _getFunctionByName (IN Object descriptor,
                               IN String& functionName)
    {
        return dlsym(descriptor, (char*) functionName.c_str());
    }

#endif

/*====================*/
/* PUBLIC FEATURES    */
/*====================*/

/*--------------------*/
/* con-/destruction   */
/*--------------------*/

DynamicLibrary::DynamicLibrary (IN String& libraryName)
{
    Logging_trace1(">>: %1", libraryName);
    _descriptor = _loadLibrary(libraryName);
    Logging_trace1("<<: %1",
                   (_descriptor != NULL ? "loaded" : "not loaded"));
}
  
/*--------------------*/

DynamicLibrary::~DynamicLibrary ()
{
    Logging_trace(">>");

    if (_descriptor != NULL) {
        _freeLibrary(_descriptor);
        _descriptor = NULL;
    }

    Logging_trace("<<");
}
  
/*--------------------*/
/* configuration      */
/*--------------------*/

void DynamicLibrary::addToSearchPath (IN String& searchPath)
{
    Logging_trace1(">>: %1", searchPath);
    _addToSearchPath(searchPath);
    Logging_trace("<<");
}

/*--------------------*/
/* queries            */
/*--------------------*/

Boolean DynamicLibrary::isLoaded () const
{
    Logging_trace(">>");
    Boolean result = (_descriptor != NULL);
    Logging_trace1("<<: %1", TOSTRING(result));
    return result;
}

/*--------------------*/

Object DynamicLibrary::underlyingObject () const
{
    return _descriptor;
}

/*--------------------*/

Object DynamicLibrary::getFunctionByName (IN String& functionName) const
{
    Logging_trace1(">>: %1", functionName);

    Object result = NULL;

    if (_descriptor != NULL) {
        result = _getFunctionByName(_descriptor, functionName);
    }

    Logging_trace1("<<: %1",
                   (result != NULL ? "found" : "not found"));
    return result;
}
