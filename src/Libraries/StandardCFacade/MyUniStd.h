/**
 * @file
 * The <C>C_UniStd</C> namespace specifies a facade for the C unistd
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

#include <unistd.h>

/*====================*/

namespace C_UniStd {

    /** qualified version of sleep from unistd */
    inline auto sleep = ::sleep;

}
