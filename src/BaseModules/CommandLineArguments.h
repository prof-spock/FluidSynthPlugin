/**
 * @file
 * The <C>CommandLineArguments</C> specification defines a handler for
 * command-line arguments converting them to a list of abstract
 * arguments derived from concrete command-line strings describing the
 * actions to be taken in a program.
 *
 * @author Dr. Thomas Tensi
 * @date   2022-08
 */

/*====================*/

#pragma once

/*=========*/
/* IMPORTS */
/*=========*/

#include "Dictionary.h"
#include "GenericList.h"

/*--------------------*/

using BaseTypes::Containers::Dictionary;
using BaseTypes::GenericTypes::GenericList;

/*====================*/

namespace BaseModules {

    /**
     * An <C>CommandLineArgument</C> is an abstraction for a concrete
     * command line argument combining an abstract name (encoding the
     * action to be taken in a main program) with a list of associated
     * parameters
     */
    struct CommandLineArgument {

        /** effective name of the argument in the command line */
        String originalName;
        
        /** abstract name of argument for steering the processing in
         * program */
        String abstractName;

        /** the list of collected parameters for this command line
         * argument */
        StringList parameterList;
        
        /*--------------------*/

        /**
         * Returns a string representation of command line argument.
         *
         * @return string representation
         */
        String toString () const;

        /*--------------------*/

        /**
         * Returns a string representation of <C>argument</C>.
         *
         * @param[in] argument  argument to be converted to string
         * @return string representation
         */
        static String toString (IN CommandLineArgument& argument);
        
    };
        
    /*--------------------*/
        
    /**
     * Returns name of command line argument list type
     *
     * @return type name
     */
    static String _commandLineArgumentListTypeName () {
        return "CommandLineArgumentList";
    }
    
    /*--------------------*/

    /**
     * An <C>CommandLineArgumentList</C> object is a list of abstract
     * arguments for the main program derived from the raw command-line
     * arguments
     */
    struct CommandLineArgumentList
        : public GenericList<CommandLineArgument,
                             CommandLineArgument::toString,
                             &_commandLineArgumentListTypeName > {

    };

    /*--------------------*/
        
    /**
     * A <C>CommandLineArgumentHandler</C> object is able to convert
     * command-line arguments into a list of abstracted arguments
     * derived from concrete command-line strings describing the
     * actions to be taken in a program. */
    struct CommandLineArgumentHandler {

        /**
         * Sets up a handler for command line arguments.
         */
        CommandLineArgumentHandler ();

        /*--------------------*/

        /**
         * Destroys option handler object.
         */
        ~CommandLineArgumentHandler ();

        /*--------------------*/

        /**
         * Converts <C>argumentList</C> from the command line into a
         * list of abstract options with associated parameters.
         *
         * @param[in] rawArgumentList                  list of unprocessed
         *                                             arguments from the
         *                                             command line
         * @param[in] rawArgumentNameToAbstractionMap  map from raw
         *                                             argument name to
         *                                             triple of strings
         * @return  list of abstract arguments to be processed in program
         */
        CommandLineArgumentList
        convert (IN StringList& rawArgumentList,
                 IN Dictionary& rawArgumentNameToAbstractionMap);

    };

}
