/**
 * @file
 * The <C>GenericMap</C> specification and body defines maps from
 * arbitrary keys to arbitrary values.
 *
 * @author Dr. Thomas Tensi
 * @date   2022-11
 */

/*====================*/

#pragma once

/*=========*/
/* IMPORTS */
/*=========*/

#include <map>
#include "Assertion.h"
#include "GenericList.h"
#include "StringUtil.h"

/*--------------------*/

using std::map;
using BaseModules::StringUtil;

/*====================*/

namespace BaseTypes::GenericTypes {

    /**
     * A <C>GenericMap</C> object maps arbitrary keys to values.
     * Every key is associated with at most one value, where the key
     * identity is defined by standard equality comparison.
     */
    template <typename KeyType, typename ValueType> 
    struct GenericMap : public map<KeyType, ValueType> {

        /**
         * Makes an empty map.
         */
        GenericMap ()
        {
        }

        /*--------------------*/

        /**
         * Destroys current map.
         */
        ~GenericMap ()
        {
        }

        /*--------------------*/

        /**
         * Returns entry for <C>key</C>.
         *
         * @param[in] key           key within map
         * @return entry for <C>key</C> in map
         * @pre contains(key)
         */
        ValueType at (IN KeyType& key) const
        {
            Assertion_pre(contains(key), "key must be contained in map");
            return map<KeyType, ValueType>::at(key);
        }

        /*--------------------*/

        /**
         * Returns entry for <C>st</C> or otherwise
         * <C>defaultValue</C>.
         *
         * @param[in] key           key within map
         * @param[in] defaultValue  value to use when key is not found
         * @return entry for <C>key</C> in map
         */
        ValueType atWithDefault (IN KeyType& key,
                                 IN ValueType& defaultValue) const
        {
            return (contains(key) ? at(key) : defaultValue);
        }

        /*--------------------*/

        /**
         * Tells whether map has entry for <C>key</C>.
         *
         * @param[in] key  key to be tested for existence within map
         * @return information whether <C>key</C> is in map
         */
        Boolean contains (IN KeyType& key) const
        {
            return (map<KeyType, ValueType>::find(key)
                    != map<KeyType, ValueType>::end());
        }

        /*--------------------*/

        /**
         * Sets entry for <C>key</C> to <C>value</C>.
         *
         * @param[in] key    key within map
         * @param[in] value  new associated value for key
         */
        void set (IN String& key, IN String& value)
        {
            map<KeyType, ValueType>::at(key) = value;
        }

        /*--------------------*/
        /*--------------------*/

        protected:

            /**
             * Returns list of key elements.
             *
             * @return  list of keys
             */
            GenericList<KeyType> keyList () const
            {
                GenericList<KeyType> result;

                for (auto iterator = map<KeyType, ValueType>::begin();
                     iterator != map<KeyType, ValueType>::end();
                     iterator++) {
                    result.append(iterator->first);
                }

                return result;
            }


            /*--------------------*/
            /* conversion         */
            /*--------------------*/

            /** a function type for converting a key to a string */
            typedef String KeyToStringProc (IN KeyType&);

            /** a function type for converting a value to a string */
            typedef String ValueToStringProc (IN ValueType&);

            /*--------------------*/

            /**
             * Converts list to linear string representation
             * prefixed by <C>nameOfType</C> assuming an explicit
             * conversion from element type to string
             *
             * @param[in] nameOfType       name of list type
             * @param[in] keyToString  function for converting an
             *                             element to a string
             * @return  single string representation of list
             */
            String _toString (IN String& nameOfType,
                              IN KeyToStringProc keyToString,
                              IN ValueToStringProc valueToString) const
            {
                String result = "";
                GenericList<KeyType> keyList = this->keyList();

                for (Natural i = 0;  i < keyList.size();  i++) {
                    result += (i == 0 ? "" : ", ");
                    const KeyType& key = keyList[i];
                    const ValueType& value = at(key);
                    result += (keyToString(key)
                               + " -> "
                               + valueToString(value));
                }

                result = nameOfType + "(" + result + ")";
                return result;
            }

    };

}
