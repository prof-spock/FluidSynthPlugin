/**
 * @file
 * The <C>C_StdIO</C> namespace specifies a facade for the C stdio
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

#include <stdio.h>

/*====================*/

namespace C_StdIO {

    /** qualified version of FILE from stdio */
    typedef ::FILE File;

    /** qualified version of stderr from stdio */
    inline File* stdError = stderr;
    
    /** qualified version of fclose from stdio */
    inline auto fclose    = ::fclose;

    /** qualified version of fopen from stdio */
    inline auto fopen     = ::fopen;

    /** qualified version of fprintf from stdio */
    inline auto fprintf   = ::fprintf;

    /** qualified version of fputs from stdio */
    inline auto fputs     = ::fputs;

    /** qualified version of fread from stdio */
    inline auto fread     = ::fread;

    /** qualified version of fseek from stdio */
    inline auto fseek     = ::fseek;

    /** qualified version of ftell from stdio */
    inline auto ftell     = ::ftell;

    /** qualified version of fwrite from stdio */
    inline auto fwrite    = ::fwrite;

}
