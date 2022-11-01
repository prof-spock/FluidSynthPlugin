/**
 * @file
 * The <C>GenericMatrix</C> specification defines a generic matrix
 * type with a single element type parameter <C>T</C>.
 *
 * @author Dr. Thomas Tensi
 * @date   2020-08
 */

/*====================*/

#pragma once

/*=========*/
/* IMPORTS */
/*=========*/

#include "GenericSequence.h"

/*--------------------*/

using BaseTypes::Containers::GenericSequence;

/*====================*/

namespace BaseTypes::Containers {

    /**
     * A <C>GenericMatrix</C> is a generic (two-dimensional) matrix
     * type with a single element type parameter <C>T</C>.
     */
    template <typename T>
    struct GenericMatrix {

        /**
         * Makes an empty matrix.
         */
        GenericMatrix ()
            : _rowCount{0},
              _columnCount{0},
              _data{}
        {
        }

        /*--------------------*/

        /**
         * Defines size of matrix to be (<C>rowCount</C>,
         * <C>columnCount</C>).
         *
         * @param[in] rowCount     number of rows in matrix
         * @param[in] columnCount  number of columns in matrix
         */
        void resize (IN Natural rowCount, IN Natural columnCount)
        {
            _rowCount = rowCount;
            _columnCount = columnCount;
            _data.setLength(rowCount);

            for (auto& row : _data) {
                row.setLength(columnCount);
            }
        }

        /*--------------------*/

        /**
         * Returns the number of rows in matrix.
         *
         * @return row count of matrix
         */
        Natural rowCount() const
        {
            return _rowCount;
        }

        /*--------------------*/

        /**
         * Returns the number of columns in matrix
         *
         * @return column count of matrix
         */
        Natural columnCount() const
        {
            return _columnCount;
        }

        /*--------------------*/

        /**
         * Returns entry at (<C>rowIndex</C>, <C>columnIndex</C>);
         * fails if some index is out of range
         *
         * @param[in] rowIndex     row position of element in matrix
         * @param[in] columnIndex  column position of element in matrix
         * @return  element at given matrix position
         */
        T& at (IN Natural rowIndex, IN Natural columnIndex)
        {
            return _data[rowIndex][columnIndex];
        }

        /*--------------------*/

        /**
         * Returns entry at (<C>rowIndex</C>, <C>columnIndex</C>);
         * fails if some index is out of range
         *
         * @param[in] rowIndex     row position of element in matrix
         * @param[in] columnIndex  column position of element in matrix
         * @return  element at given matrix position
         */
        const T& at (IN Natural rowIndex, IN Natural columnIndex) const
        {
            return _data[rowIndex][columnIndex];
        }

        /*--------------------*/

        /**
         * Returns sequence at <C>rowIndex</C> via parenth operator;
         * fails if some index is out of range
         *
         * @param[in] rowIndex     row position of element in matrix
         * @return  element at given matrix position
         */
        GenericSequence<T>& operator [] (IN Natural rowIndex)
        {
            return _data[rowIndex];
        }

        /*--------------------*/

        /**
         * Returns sequence at <C>rowIndex</C> via parenth operator;
         * fails if some index is out of range
         *
         * @param[in] rowIndex     row position of element in matrix
         * @return  element at given matrix position
         */
        const GenericSequence<T>& operator [] (IN Natural rowIndex) const
        {
            return _data[rowIndex];
        }

        /*--------------------*/
        /*--------------------*/

        private:

            /** the number of rows in this matrix */
            Natural _rowCount;

            /** the number of columns in this matrix */
            Natural _columnCount;

            /** the internal container */
            GenericSequence<GenericSequence<T> > _data;

    };

}
