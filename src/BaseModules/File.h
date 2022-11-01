/**
 * @file
 * The <C>File</C> specification defines a class for simple file
 * handling based on C stdio.  Note that this module does no logging
 * because it is used by the logging module itself.
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
#include "ByteList.h"
#include "Object.h"

/*--------------------*/

using BaseTypes::Containers::ByteList;
using BaseTypes::Primitives::Boolean;
using BaseTypes::Primitives::Object;
using BaseTypes::Primitives::String;

/*====================*/

namespace BaseModules {

    /**
     * A <C>File</C> object provided methods for simple file handling
     * based on C stdio.  Note that this module does no logging
     * because it is used by the logging module itself.
     */
    struct File {

        /*--------------------*/
        /* con-/destruction   */
        /*--------------------*/

        /**
         * Defines new file object (without opening it)
         */
        File ();

        /*--------------------*/

        /**
         * Destroys file (and closes it before).
         */
        ~File ();

        /*--------------------*/
        /* status change      */
        /*--------------------*/

        /**
         * Opens a file named <C>fileName</C> in C mode
         * <C>mode</C>.
         *
         * @param[in]  fileName  name of file to open
         * @param[in]  mode      C open mode ("r", "w", "rb", "wb", "a"
                                 and "ab")
         * @return  information whether operation has been successful
         */
        Boolean open (IN String& fileName, IN String& mode);

        /*--------------------*/

        /**
         * Closes file.
         */
        void close ();

        /*--------------------*/
        /* access             */
        /*--------------------*/

        /**
         * Reads at most <C>count</C> bytes into <C>byteList</C> at
         * <C>position</C> and returns number of bytes read.
         *
         * @param[inout] byteList  list of bytes to be changed
         * @param[in]    position  position in byte list where first
         *                         read byte goes
         * @param[in]    count     number of bytes to read at most
         * @return  number of bytes read actually
         */
        Natural read (INOUT ByteList& byteList,
                      IN Natural position,
                      IN Natural count);

        /*--------------------*/

        /**
         * Writes <C>st</C> to file.
         *
         * @param[in] byteList  byte list to be written to file
         * @param[in] position  position in byte list from where first
         *                      byte is written
         * @param[in] count     number of bytes to write
         * @return  number of bytes actually written
         */
        Natural write (IN ByteList& byteList,
                       IN Natural position,
                       IN Natural count);

        /*--------------------*/

        /**
         * Writes <C>st</C> to file.
         *
         * @param[in] st  string to be written to file
         */
        void writeString (IN String& st);

        /*--------------------*/
        /* measurement        */
        /*--------------------*/

        /**
         * Tells the length of file in bytes.  Note that file given by
         * <C>fileName</C> may not be currently open.
         *
         * @param[in] fileName  name of file to be scanned
         * @return  length of file in raw bytes
         */
        static Natural length (IN String& fileName);

        /*--------------------*/
        /* queries            */
        /*--------------------*/

        /**
         * Tells whether file is open (or not).
         *
         * @return  information whether file is open
         */
        Boolean isOpen ();

        /*--------------------*/
        /* complex commands   */
        /*--------------------*/

        /**
         * Closes file if still open.
         */
        void closeConditionally ();

        /*--------------------*/
        /*--------------------*/

        private:
  
            /** the internal representation of the file */
            Object _descriptor;

    };

}
