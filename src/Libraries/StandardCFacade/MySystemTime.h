/**
 * @file
 * The <C>C_SystemTime</C> namespace specifies a facade for the C time
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

#include <time.h>

/*====================*/

namespace C_SystemTime {

    /** qualified version of timespec type from time */
    typedef struct timespec TimeSpec;

    /** qualified version of timespec_get from time */
    void currentClockTime (TimeSpec& ts) {
        timespec_get((timespec *)(&ts), TIME_UTC);
    }

}
