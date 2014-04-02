<?php

/**
 * @file
 * Provides an application programming interface (API) for working the example module
 */

/**
 * @defgroup tripal_example_api Example API
 * @ingroup tripal_api
 * @{
 * Provides an application programming interface (API) for working with features
 * @}
 */
 
// EXPLANATION: This file is used for housing any API function that your
// module creates.  It is suggested you follow the naming style for Tripal 
// API functions. Below are naming rules for Tripal API functions:
//
// 1) Prefix the function with 'chado_' for all insert, updates, delete or get
//    function on a table in the chado schema.  Tripal should provide all of these 
//    functions for tables that come with Chado.  But, you can use this prefix 
//    for API functions on custom tables that your module may add.
// 2) For all other functions prefix the function with your module name. So
//    for this example module the prefix would be 'tripal_example_'
// 3) A 'verb' should follow the function prefix indicating the action that
//    the function performs. Commonly used verbs are 'get', 'insert', 'update'
//    'associate', 'execute', etc.  An exception is for functions that
//    test state rather than perform an action (e.g. does something exist).  
//    For functions that test state use 'does' instead of a verb 
//    (e.g. tripal_example_does_pub_exist) 
// 4) Follow the verb with a short predicate describing what is being acted upon.
//