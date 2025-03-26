/**
 * @file
 * The <C>BuildInformation</C> body implements four functions
 * returning build date and time, version and information whether this
 * is a debug build.
 *
 * @author Dr. Thomas Tensi
 * @date   2024-03
 */

/*====================*/

/*=========*/
/* IMPORTS */
/*=========*/

#include "BuildInformation.h"

using Main::BuildInformation;

/*====================*/


String BuildInformation::date ()
{
    return __DATE__;
}

/*--------------------*/

Boolean BuildInformation::isDebugBuild ()
{
    #ifdef DEBUG
        return true;
    #else
        return false;
    #endif
}

/*--------------------*/

String BuildInformation::time ()
{
    return __TIME__;
}

/*--------------------*/

String BuildInformation::version ()
{
    return "0.7.6";
}
