/**
 * @file
 * The <C>GlobalMacros</C> specification defines simple macros
 * (e.g.\ for parameter modes).
 *
 * @author Dr. Thomas Tensi
 * @date   2020-08
 */

#pragma once

/*====================*/

/** (informal) parameter mode for in parameter */
#define IN const
/** (informal) parameter mode for inout parameter */
#define INOUT
/** (informal) parameter mode for out parameter */
#define OUT

/*====================*/

#ifdef DEBUG
    /** INLINE is set such that no inlining of potentially inlined functions
     *  occurs for debug mode */
    #define INLINE
#else
    /** INLINE is set such that potentially inlined functions are inlined
     *  occurs for production mode */
    #define INLINE inline
#endif

/*====================*/
