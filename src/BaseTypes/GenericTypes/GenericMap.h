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
     * Additionally two mappings may be supplied that define how to
     * convert each keys and values to a string for printing out the
     * map.
     */
    template <typename KeyType, typename ValueType,
              ElementToStringProc<KeyType> keyToString   = nullptr,
              ElementToStringProc<ValueType> valueToString = nullptr,
              StringProc nameOfType = nullptr> 
    struct GenericMap : public map<KeyType, ValueType>
    {

        /** an abbreviation for the underlying map type */
        typedef map<KeyType, ValueType> _UnderlyingMap;

        /** an abbreviation for a list of keys */
        typedef GenericList<KeyType> _KeyList;

        /** an abbreviation for a list of values */
        typedef GenericList<ValueType> _ValueList;

        /*--------------------*/
        /* con-/destructor    */
        /*--------------------*/

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
        virtual ~GenericMap ()
        {
        }

        /*--------------------*/
        /* type conversion    */
        /*--------------------*/

        /**
         * Converts map to linear string representation prefixed by
         * <C>nameOfType</C> template parameter; when string
         * conversion functions <C>keyToString</C> and
         * <C>valueToString</C> are not defined, some surrogate
         * functions are used
         *
         * @return  single string representation of map
         */
        String toString () const
        {
            String result = "";
            _KeyList keyList = this->keyList();
            String keyValueSeparator = " -> ";

            for (Natural i = 0;  i < keyList.size();  i++) {
                result += (i == 0 ? "" : ", ");
                const KeyType& key = keyList[i];
                const ValueType& value = at(key);
                const String iAsString = TOSTRING(i);

                result +=
                    ((keyToString == nullptr
                      ? StringUtil::expand("k%1", iAsString)
                      : (*keyToString)(key))
                     + keyValueSeparator
                     + (valueToString == nullptr
                        ? StringUtil::expand("v%1", iAsString)
                        : (*valueToString)(value)));
            }

            String typeName = (nameOfType == nullptr ? "Map"
                               : (*nameOfType)());
            result = StringUtil::expand("%1(%2)", typeName, result);
            return result;
        }

        /*--------------------*/

        /**
         * Returns string representation of map.
         *
         * @param[in] map  map to be converted to a string
         * @return  string representation
         */
        static String toString (IN GenericMap& map)
        {
            return map.toString();
        }

        /*--------------------*/
        /* data access        */
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
            return _UnderlyingMap::at(key);
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
        /* measurement        */
        /*--------------------*/

        /**
         * Tells whether map has entry for <C>key</C>.
         *
         * @param[in] key  key to be tested for existence within map
         * @return information whether <C>key</C> is in map
         */
        Boolean contains (IN KeyType& key) const
        {
            return (_UnderlyingMap::find(key)
                    != _UnderlyingMap::end());
        }

        /*--------------------*/

        /**
         * Tells whether map is empty.
         *
         * @return information whether map is empty
         */
        Boolean isEmpty () const
        {
            return _UnderlyingMap::empty();
        }

        /*-----------------------*/
        /* aggregate data access */
        /*-----------------------*/

        /**
         * Returns list of key elements.
         *
         * @return  list of keys
         */
        _KeyList keyList () const
        {
            _KeyList result;

            for (auto iterator = _UnderlyingMap::begin();
                 iterator != _UnderlyingMap::end();
                 iterator++) {
                result.append(iterator->first);
            }

            return result;
        }

        /*--------------------*/

        /**
         * Returns list of value elements.
         *
         * @return  list of values
         */
        _ValueList valueList () const
        {
            _ValueList result;

            for (auto iterator = _UnderlyingMap::begin();
                 iterator != _UnderlyingMap::end();
                 iterator++) {
                result.append(iterator->second);
            }

            return result;
        }

        /*--------------------*/
        /* change             */
        /*--------------------*/

        /**
         * Removes entry for <C>key</C>.
         *
         * @param[in] key    key within map
         */
        void remove (IN KeyType& key)
        {
            _UnderlyingMap::erase(key);
        }

        /*--------------------*/

        /**
         * Sets entry for <C>key</C> to <C>value</C>.
         *
         * @param[in] key    key within map
         * @param[in] value  new associated value for key
         */
        void set (IN KeyType& key, IN ValueType& value)
        {
            _UnderlyingMap::insert_or_assign(key, value);
        }

    };

}
