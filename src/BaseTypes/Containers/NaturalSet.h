/**
 * @file
 * The <C>NaturalSet</C> specification defines sets of natural values
 * with no duplicates and only simple containment access and the
 * ability to add and remove elements.
 *
 * @author Dr. Thomas Tensi
 * @date   2020-08
 */

/*====================*/

#pragma once

/*=========*/
/* IMPORTS */
/*=========*/

#include "GenericSet.h"

/*--------------------*/

using BaseTypes::GenericTypes::GenericSet;

/*====================*/

namespace BaseTypes::Containers {

    /**
     * A <C>NaturalSet</C> object is a set of naturals with no
     * duplicates and only simple containment access and the ability
     * to add and remove elements.
     */
    struct NaturalSet : public GenericSet<Natural> {

    };

}
