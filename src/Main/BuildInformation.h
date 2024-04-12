/**
 * @file
 * The <C>BuildInformation</C> specification defines four functions
 * returning build date and time, version and information whether this
 * is a debug build.
 *
 * @author Dr. Thomas Tensi
 * @date   2024-03
 */

/*====================*/

#pragma once

/*=========*/
/* IMPORTS */
/*=========*/

#include "Boolean.h"

using BaseTypes::Primitives::Boolean;
using BaseTypes::Primitives::String;

/*====================*/

namespace Main {

    /** information about the build: time, date, version */
    struct BuildInformation {

        /** the build date as a string */
        static String date ();

        /*--------------------*/

        /** information whether this is a debug build */
        static Boolean isDebugBuild ();

        /*--------------------*/

        /** the build time as a string */
        static String time ();

        /*--------------------*/

        /** the program version as a string */
        static String version ();

    };

}
