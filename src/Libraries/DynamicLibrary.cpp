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

/*--------------------*/

using Libraries::DynamicLibrary;

#ifdef _WIN32
    /*=====================*/
    /* WINDOWS DEFINITIONS */
    /*=====================*/

    #define DLLImport  __declspec(dllimport)
    #define STDCALL    __stdcall

    /* types */
    typedef int BOOL;
    typedef const char *LPCSTR;
    struct HINSTANCE__ {int unused;};
    typedef struct HINSTANCE__ *HMODULE;
    typedef __int64 (STDCALL *FARPROC)();

    /* function prototypes */
    extern "C" DLLImport BOOL STDCALL FreeLibrary(HMODULE hLibModule);

    extern "C" DLLImport FARPROC STDCALL GetProcAddress(HMODULE hModule,
                                             LPCSTR lpProcName);

    extern "C" DLLImport HMODULE STDCALL LoadLibraryA(LPCSTR lpLibFileName);

    /*--------------------*/

    Object DL_loadLibrary (IN String& pathName) {
        return (Object) LoadLibraryA((char*) pathName.c_str());
    }

    /*--------------------*/

    void DL_freeLibrary (Object descriptor) {
        FreeLibrary((HMODULE) descriptor);
    }

    /*--------------------*/

    Object DL_getFunctionByName (IN Object descriptor,
                                 IN String& functionName)
    {
        return GetProcAddress((HMODULE) descriptor,
                              (char*) functionName.c_str());
    }

#else
    /*========================*/
    /* UNIX/MACOS DEFINITIONS */
    /*========================*/

   #include <dlfcn.h>

    /*--------------------*/

    Object DL_loadLibrary (IN String& pathName) {
        return (Object) dlopen((char*) pathName.c_str(), 0);
    }

    /*--------------------*/

    void DL_freeLibrary (Object descriptor) {
        dlclose(descriptor);
    }

    /*--------------------*/

    Object DL_getFunctionByName (IN Object descriptor,
                                 IN String& functionName)
    {
        return dlsym(descriptor, (char*) functionName.c_str());
    }

#endif

/*====================*/
/* PUBLIC FEATURES    */
/*====================*/

DynamicLibrary::DynamicLibrary (IN String& libraryName)
{
    Logging_trace1(">>: %1", libraryName);
    
    _descriptor = DL_loadLibrary(libraryName);

    Logging_trace1("<<: %1",
                   (_descriptor != NULL ? "loaded" : "not loaded"));
}
  
/*--------------------*/

DynamicLibrary::~DynamicLibrary ()
{
    Logging_trace(">>");

    if (_descriptor != NULL) {
        DL_freeLibrary(_descriptor);
        _descriptor = NULL;
    }

    Logging_trace("<<");
}
  
/*--------------------*/

Object DynamicLibrary::library () const
{
    return _descriptor;
}

/*--------------------*/

Object DynamicLibrary::getFunctionByName (IN String& functionName) const
{
    Logging_trace1(">>: %1", functionName);

    Object result = NULL;

    if (_descriptor != NULL) {
        result = DL_getFunctionByName(_descriptor, functionName);
    }

    Logging_trace1("<<: %1",
                   (result != NULL ? "found" : "not found"));
    return result;
}
