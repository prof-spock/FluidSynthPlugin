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
#include "Natural.h"

/*--------------------*/

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
         * Checks whether file names <C>fileName</C> exists.
         *
         * @param[in] fileName  name of file to be checked for
         *                      existence
         * @return  information whether file exists
         */
        static Boolean fileExists (IN String& fileName);

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
