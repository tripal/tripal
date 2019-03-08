<?php
/**
 * @file
 * Provides an application programming interface (API) for managing variables
 * associated with Tripal managed content.
 */

/**
 * @defgroup tripal_variables_api Variables
 * @ingroup tripal_api
 * @{
 * Provides an application programming interface (API) for managing variables
 * associated with Tripal managed content. The Tripal Variables API
 * supports storing any type of variable such as a property or setting that
 * should be associated with a Tripal managed content.  Variables are
 * meant to store non-biological information only because biological data
 * should be stored together in the primary data store (e.g. Chado). Be aware
 * that any data stored as a Tripal Variable will not be made visible through
 * services such as Tripal Web Services and therefore can be a good place to
 * hide application specific settings.
 * @}
 */

/**
 * Adds a new variable name.
 *
 * @param $name
 *   The name of the variable
 * @param $description
 *   The description for the variable
 *
 * @return
 *   A record object containg the variable that was added if successful.
 *
 * @ingroup tripal_variables_api
 */
function tripal_insert_variable($name, $description) {
  $name = trim($name);
  if (!$name) {
    tripal_report_error('tripal', TRIPAL_ERROR,
      'Must have a variable name when adding a new Tripal Variable.', []);
    return NULL;
  }
  if (!$description) {
    tripal_report_error('tripal', TRIPAL_ERROR,
      'Must have a description when adding a new Tripal Variable.', []);
    return NULL;
  }

  // Make sure the variable is not a duplicate. If so, then just select
  // it and return the variable_id
  $variable = tripal_get_variable($name);
  if ($variable) {
    return $variable;
  }
  else {
    db_insert('tripal_variables')
      ->fields([
        'name' => $name,
        'description' => $description,
      ])
      ->execute();
    return tripal_get_variable($name);
  }
}

/**
 * Retrieves the variable name record.
 *
 * @param $name
 *   The name of the variable to retrieve
 *
 * @return
 *   A record object containg the variable.
 *
 * @ingroup tripal_variables_api
 */
function tripal_get_variable($name) {
  return db_select('tripal_variables', 'v')
    ->fields('v')
    ->condition('name', $name)
    ->execute()
    ->fetchObject();
}

// TODO: add functions for getting/retrieving variables from/to entities.
