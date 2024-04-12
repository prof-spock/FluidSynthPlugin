/**
 * @file
 * The <C>Environment</C> specification defines operating system
 * services related to environment variables
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
     * The <C>Environment</C> specification defines a class for
     * several operating system services related to the environment
     * variables.
     */
    struct Environment {

        /**
         * Returns mapping from variable names to associated values
         * from the environment.
         *
         * @return  mapping of environment variable names to string
         *          values
         */
        static Dictionary all ();

        /*--------------------*/

        /**
         * Adapts string <C>st</C> by trying to match environment
         * variables as possible prefices; returns whether replacement
         * was done
         *
         * @param[inout] st  original string to be adapted
         * @return  information whether prefix was changed
         */
        static Boolean replacePrefix (INOUT String& st);

        /*--------------------*/

        /**
         * Expands environment variables in <C>st</C>.
         *
         * @param[in] st  original string with environment variables
         * @return  string with environment variables replaced
         */
        static String expand (IN String& st);

        /*--------------------*/

        /**
         * Returns associated value for <C>variableName</C> as string
         * from the environment.
         *
         * @param[in] variableName  name of environment variable
         * @param[in] defaultValue  value to be returned when
         *                          environment variable is not set
         * @return  associated environment value as string
         */
        static String value (IN String variableName,
                             IN String defaultValue = "???");


    };

}
