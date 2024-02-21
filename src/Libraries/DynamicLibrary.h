/**
 * @file
 * The <C>DynamicLibrary</C> class specifies a very simple and crude
 * wrapper around the dynamic library loading in Windows.
 *
 * @author Dr. Thomas Tensi
 * @date   2022-04
 */

/*====================*/
  
#pragma once
  
/*=========*/
/* IMPORTS */
/*=========*/

#include "Boolean.h"
#include "Object.h"
  
/*--------------------*/
  
using BaseTypes::Primitives::Boolean;
using BaseTypes::Primitives::Object;
using BaseTypes::Primitives::String;

/*====================*/

namespace Libraries {
  
    /**
     * An object-oriented wrapper around the fluidsynth library and
     * also allows its dynamic loading.
     */
    struct DynamicLibrary {

        /*--------------------*/
        /* con-/destruction   */
        /*--------------------*/

        /**
         * Constructs a new dynamic library from <C>libraryName</C>
         *
         * @param[in] libraryName  the name of the dynamic library
         */
        DynamicLibrary (IN String& libraryName);
  
        /*--------------------*/

        /**
         * Constructs new dynamic library from <C>otherLibrary</C>
         * (NOT AVAILABLE!)
         *
         * @param[in] otherLibrary  library to be copied
         */
        DynamicLibrary (IN DynamicLibrary& otherLibrary) = delete;

        /*--------------------*/

        /**
         * Destroys dynamic library object and unloads the underlying
         * library
         */
        ~DynamicLibrary ();
  
        /*--------------------*/
        /* assignment         */
        /*--------------------*/

        /**
         * Assigns current library from <C>otherLibrary</C>
         * (NOT AVAILABLE!)
         *
         * @param[in] otherLibrary  library to be assigned
         */
        DynamicLibrary& operator= (IN DynamicLibrary& otherLibrary)
            = delete;

        /*--------------------*/
        /* queries            */
        /*--------------------*/

        /**
         * Returns whether library has been correctly loaded.
         *
         * @return  information whether library is okay
         */
        Boolean isLoaded () const;
  
        /*--------------------*/

        /**
         * Returns the underlying library.
         *
         * @return  underlying technical library object
         */
        Object underlyingTechnicalLibrary () const;

        /*--------------------*/

        /**
         * Returns a function in the underlying library given by
         * <C>functionName</C>.
         *
         * @param[in] functionName  name of function as exported by
         *                          library
         * @return  pointer to function
         */
        Object getFunctionByName (IN String& functionName) const;
  
        /*--------------------*/
  
        private:

            /** the object managing the internal data */
            Object _descriptor;
  
    };
    
}
