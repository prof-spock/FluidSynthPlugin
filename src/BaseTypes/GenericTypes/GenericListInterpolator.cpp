/**
 * @file
 * The <C>GenericListInterpolator</C> body implements the mechanism
 * for the object doing an interpolation on a list of values.
 *
 * @author Dr. Thomas Tensi
 * @date   2025-04
 */

/*====================*/

/*=========*/
/* IMPORTS */
/*=========*/

#include "GenericListInterpolator.h"

/*====================*/

GenericMap<Natural, IntegerList> GenericListInterpolator::_orderToOffsetMap;
GenericMap<Natural, RealList>    GenericListInterpolator::_orderToWeightMap;
