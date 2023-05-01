/**
 * @file
 * The <C>Assertion</C> specification defines macros for pre- and
 * postconditions in functions or methods.
 *
 * @author Dr. Thomas Tensi
 * @date   2020-08
 */

/*====================*/

#pragma once

/*=========*/
/* IMPORTS */
/*=========*/

#include <cassert>

/*====================*/

/**
 * Checks the <C>condition</C> and exits with <C>message</C> on
 * failure; this is the base routine called by the exported assertion
 * checks --> the DEBUG version does the checking, the RELEASE version
 * does nothing.
 *
 * @param condition  condition to be checked
 * @param message    message to be given in case of failure
 */
#define _ASSERT_WITH_MESSAGE(condition, message) \
    assert((condition))

/*--------------------*/

/**
 * Checks the <C>condition</C> and exits with <C>message</C> on
 * failure.
 *
 * @param condition  condition to be checked
 * @param message    message to be given in case of failure
 */
#define Assertion_check(condition, message) \
            _ASSERT_WITH_MESSAGE(condition, message)

/*--------------------*/

/**
 * Checks postcondition <C>condition</C> and exits with
 * <C>message</C> on failure.
 *
 * @param condition  postcondition to be checked
 * @param message    message to be given in case of failure
 */
#define Assertion_post(condition, message) \
            _ASSERT_WITH_MESSAGE(condition, message)

/*--------------------*/

/**
 * Checks precondition <C>condition</C> and exits with
 * <C>message</C> on failure.
 *
 * @param condition  precondition to be checked
 * @param message    message to be given in case of failure
 */
#define Assertion_pre(condition, message)  \
            _ASSERT_WITH_MESSAGE(condition, message)
