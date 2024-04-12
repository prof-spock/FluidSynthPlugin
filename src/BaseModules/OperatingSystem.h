/**
 * @file
 * The <C>OperatingSystem</C> specification defines a class for
 * several operating system related utility functions (like e.g. file
 * name transformations or checking whether a file exists).
 *
 * @author Dr. Thomas Tensi
 * @date   2022-08
 */

/*====================*/

#pragma once

/*=========*/
/* IMPORTS */
/*=========*/

#include "Boolean.h"
#include "Dictionary.h"
#include "Natural.h"

/*--------------------*/

using BaseTypes::Containers::Dictionary;
using BaseTypes::Primitives::Boolean;
using BaseTypes::Primitives::Natural;
using BaseTypes::Primitives::String;

/*====================*/

namespace BaseModules {

    /**
     * The <C>OperatingSystem</C> specification defines a class for
     * several operating system related utility functions (like
     * e.g. file name transformations or checking whether a file
     * exists).
     */
    struct OperatingSystem {

        /**
         * Checks whether directory named <C>directoryName</C> exists.
         *
         * @param[in] directoryName  name of directory to be checked
         *                           for existence
         * @return  information whether directory exists
         */
        static Boolean directoryExists (IN String& directoryName);

        /*--------------------*/

        /**
         * Checks whether file named <C>fileName</C> exists.
         *
         * @param[in] fileName  name of file to be checked for
         *                      existence
         * @return  information whether file exists
         */
        static Boolean fileExists (IN String& fileName);

        /*--------------------*/

        /**
         * Returns list of files in directory named
         * <C>directoryName</C>; if <C>plainFilesOnly</C> is set, only
         * plain files are returned, otherwise only the names of the
         * sub-directories.
         *
         * @param[in] directoryName   name of directory to scan for files
         * @param[in] plainFilesOnly  tells whether only normal files
         *                            should be returned
         * @return  list of file names found (without directory name)
         */
        static StringList fileNameList (IN String& directoryName,
                                        IN Boolean plainFilesOnly = true);

        /*--------------------*/

        /**
         * Returns the base name of file or directory name
         * <C>fileName</C>.
         *
         * @param[in] fileName  name of file or directory
         * @return  information on base name of file without
         *          enclosing directory
         */
        static String basename (IN String& fileName);

        /*--------------------*/

        /**
         * Returns the directory name of file or directory name
         * <C>fileName</C>.
         *
         * @param[in] fileName  name of file to be checked for
         *                      enclosing directory
         * @return  information on directory name of file
         */
        static String dirname (IN String& fileName);

        /*--------------------*/

        /**
         * Returns path of directory of current library or executable
         * file.
         *
         * @param[in] isExecutable  tells whether this is a library
         *                          or an executable
         * @return  path of executable or library
         */
        static String executableDirectoryPath (IN Boolean isExecutable);

        /*--------------------*/

        /**
         * Returns path of temporary directory as string.
         *
         * @return  path of temporary directory
         */
        static String temporaryDirectoryPath ();

        /*--------------------*/

        /**
         * Writes <C>message</C> to console terminated by a newline.
         *
         * @param[in] message  message to be written to console
         */
        static void writeMessageToConsole (IN String& message);

    };

}
