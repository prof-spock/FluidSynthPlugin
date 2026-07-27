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

#include <cstring>
#include "MyStdIO.h"

#include "Assertion.h"
#include "File.h"
#include "StringUtil.h"

/*--------------------*/

using BaseModules::File;

/** abbreviation for StringUtil */
using STR = BaseModules::StringUtil;

/*====================*/

/** alias name for name from stdio.h */
typedef C_StdIO::File* FilePointer;

/*====================*/

/** error message for already closed file */
static const String _ErrMsg_alreadyClosedFile =
    "file must be open before close";

/** error message for closed file when reading */
static const String _ErrMsg_fileClosedWhenReading =
    "file must be open for reading";

/** error message for closed file when writing */
static const String _ErrMsg_fileClosedWhenWriting =
    "file must be open for writing";

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
                  STR::expand("file mode must be known - %1", mode));
    FilePointer file = C_StdIO::fopen(fileName.c_str(), mode.c_str());
    _descriptor = file;
    Boolean result = (file != NULL);
    return result;
}

/*--------------------*/

void File::close ()
{
    Assertion_pre(isOpen(), _ErrMsg_alreadyClosedFile);
    FilePointer file = static_cast<FilePointer>(_descriptor);
    C_StdIO::fclose(file);
    _descriptor = NULL;
}

/*--------------------*/
/* access             */
/*--------------------*/

Natural File::read (INOUT ByteList& byteList,
                    IN Natural position,
                    IN Natural count)
{
    Assertion_pre(isOpen(), _ErrMsg_fileClosedWhenReading);
    FilePointer file = static_cast<FilePointer>(_descriptor);
    char charArray[512];
    const Natural chunkSize = sizeof(charArray);
    Natural totalBytesRead = 0;

    for (;;) {
        Natural listSize = byteList.length();
        Natural bytesToRead =
            Natural::minimum(chunkSize, count - totalBytesRead);
        Natural bytesRead =
            Natural{C_StdIO::fread(charArray, sizeof(char),
                                   size_t(bytesToRead), file)};

        if (listSize < position + totalBytesRead + bytesRead) {
            /* extend list to take another bytesRead bytes */
            byteList.setLength(listSize + bytesRead);
        }
        
        if (bytesRead == 0) {
            break;
        } else {
            char* ptr =
                reinterpret_cast<char*>(static_cast<Byte*>(byteList.asArray()));
            ptr += size_t(position + totalBytesRead);
            std::memcpy(ptr, charArray, static_cast<size_t>(bytesRead));
            totalBytesRead += bytesRead;
        }
    }

    return totalBytesRead;
}

/*--------------------*/

StringList File::readLines ()
{
    const String newlineReplacement = "%$XX";
    ByteList byteList;
    String st;

    read(byteList);
    st = byteList.decodeToString();
    st = STR::newlineReplacedString(st, newlineReplacement);
    StringList result =
        StringList::makeBySplit(st, newlineReplacement);

    return result;
}

/*--------------------*/

Natural File::write (IN ByteList& byteList,
                     IN Natural position,
                     IN Natural count)
{
    Assertion_pre(isOpen(), _ErrMsg_fileClosedWhenWriting);
    FilePointer file = static_cast<FilePointer>(_descriptor);
    const char* characterArray =
        reinterpret_cast<const char*>(byteList.asArray());
    Natural result = C_StdIO::fwrite(&characterArray[size_t(position)],
                                     1, size_t(count), file);
    return result;
}

/*--------------------*/

void File::writeString (IN String& st)
{
    Assertion_pre(isOpen(), _ErrMsg_fileClosedWhenWriting);
    FilePointer file = static_cast<FilePointer>(_descriptor);
    C_StdIO::fputs(st.c_str(), file);
}

/*--------------------*/
/* measurement        */
/*--------------------*/

Natural File::length (IN String& fileName)
{
    Natural result = 0;
    FilePointer file = C_StdIO::fopen(fileName.c_str(), "rb");

    if (file != NULL) {
        C_StdIO::fseek(file, 0, SEEK_END);
        result = size_t(C_StdIO::ftell(file));
        C_StdIO::fclose(file);
    }

    return result;
}

/*--------------------*/
/* queries            */
/*--------------------*/

Boolean File::isOpen () const
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
