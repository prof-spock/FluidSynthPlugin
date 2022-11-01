/**
 * @file
 * The <C>File</C> body implements a class for simple file
 * handling based on C stdio.  Note that this module does no logging
 * because it is used by the logging module itself.
 *
 * @author Dr. Thomas Tensi
 * @date   2022-08
 */

/*=========*/
/* IMPORTS */
/*=========*/

#include <stdio.h>
    /** qualified version of FILE from stdio */
    #define StdIO_FILE    FILE
    /** qualified version of fclose from stdio */
    #define StdIO_fclose  fclose
    /** qualified version of fopen from stdio */
    #define StdIO_fopen   fopen
    /** qualified version of fputs from stdio */
    #define StdIO_fputs   fputs
    /** qualified version of fread from stdio */
    #define StdIO_fread(ptr, tSize, count, stream) \
            fread(ptr, (size_t) tSize, (size_t) count, stream)
    /** qualified version of fseek from stdio */
    #define StdIO_fseek   fseek
    /** qualified version of ftell from stdio */
    #define StdIO_ftell   ftell
    /** qualified version of fwrite from stdio */
    #define StdIO_fwrite  fwrite

#include "Assertion.h"
#include "File.h"
#include "StringUtil.h"

/*--------------------*/

using BaseModules::File;
using BaseModules::StringUtil;

/*====================*/

/** alias name for name from stdio.h */
typedef StdIO_FILE* FilePointer;

/*--------------------*/
/* con-/destruction   */
/*--------------------*/

File::File ()
    : _descriptor{NULL}
{
}

/*--------------------*/

File::~File ()
{
    closeConditionally();
}

/*--------------------*/
/* status change      */
/*--------------------*/

Boolean File::open (IN String& fileName, IN String& mode)
{
    Assertion_pre((mode == "a" || mode == "ab"
                   || mode == "r" || mode == "rb"
                   || mode == "w" || mode == "wb"),
                  StringUtil::expand("file mode must be known - %1",
                                     mode));
    FilePointer file = StdIO_fopen(fileName.c_str(), mode.c_str());
    _descriptor = file;
    Boolean result = (file != NULL);
    return result;
}

/*--------------------*/

void File::close ()
{
    if (_descriptor != NULL) {
        FilePointer file = (FilePointer) _descriptor;
        StdIO_fclose(file);
        _descriptor = NULL;
    }
}

/*--------------------*/
/* access             */
/*--------------------*/

Natural File::read (INOUT ByteList& byteList,
                    IN Natural position,
                    IN Natural count)
{
    Assertion_pre(isOpen(), "file must be open for reading");
    FilePointer file = (FilePointer) _descriptor;
    char* characterArray = (char*) byteList.asArray();
    Natural result =
        StdIO_fread(characterArray, 1, count, file);
    return result;
}

/*--------------------*/

Natural File::write (IN ByteList& byteList,
                     IN Natural position,
                     IN Natural count)
{
    Assertion_pre(isOpen(), "file must be open for writing");
    FilePointer file = (FilePointer) _descriptor;
    char* characterArray = (char*) byteList.asArray();
    Natural result = StdIO_fwrite(&characterArray[(size_t) position],
                                  1, (size_t) count, file);
    return result;
}

/*--------------------*/

void File::writeString (IN String& st)
{
    Assertion_pre(isOpen(), "file must be open for writing");
    FilePointer file = (FilePointer) _descriptor;
    StdIO_fputs(st.c_str(), file);
}

/*--------------------*/
/* measurement        */
/*--------------------*/

Natural File::length (IN String& fileName)
{
    FilePointer file = StdIO_fopen(fileName.c_str(), "rb");
    StdIO_fseek(file, 0, SEEK_END);
    Natural result = (size_t) StdIO_ftell(file);
    StdIO_fclose(file);
    return result;
}

/*--------------------*/
/* queries            */
/*--------------------*/

Boolean File::isOpen ()
{
    return _descriptor != NULL;
}

/*--------------------*/
/* complex commands   */
/*--------------------*/

void File::closeConditionally ()
{
    if (isOpen()) {
        close();
    }
}
