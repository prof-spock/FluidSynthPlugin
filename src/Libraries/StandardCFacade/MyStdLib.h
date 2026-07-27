/**
 * @file
 * The <C>C_StdLib</C> namespace specifies a facade for the C stdlib
 * library.
 *
 * @author Dr. Thomas Tensi
 * @date   2026-01
 */

/*====================*/
  
#pragma once
  
/*=========*/
/* IMPORTS */
/*=========*/

#include <stdlib.h>

/*====================*/

namespace C_StdLib {

    /** qualified version of atexit from stdlib */
    inline auto atExit = ::atexit;

}
