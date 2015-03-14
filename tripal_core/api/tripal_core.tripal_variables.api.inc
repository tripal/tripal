<?php
/**
 * @file
 * Provides an application programming interface (API) for managing variables 
 * associated with Tripal managed Drupal nodes/
 */

/**
 * @defgroup tripal_variables_api Tripal Variables API
 * @ingroup tripal_core_api
 * @{
 * Provides an application programming interface (API) for managing variables 
 * associated with Tripal managed Drupal nodes. The Tripal Variables API 
 * supports storing any type of variable such as a property or setting that 
 * should be associated with a Tripal managed Drupal node.  Variables are
 * meant to store non-biological information only. All biological data should be 
 * housed in the Chado tables. Be aware that any data stored as a Tripal 
 * Variable will not be made visible through services such as Tripal Web 
 * Services and therefore can be a good place to hide application specific 
 * settings
 * @}
 *
 */

/**
 * Adds a new variable name.
 * 
 * @param $name
 *   The name of the variable
 * @param $description
 *   The description for the variable
 * @return 
 *   A record object containg the variable that was added if successful. 
 */
function tripal_insert_variable($name, $description) {
  $name = trim($name);
  if (!$name) {
    tripal_report_error('tripal_core', TRIPAL_ERROR,
        'Must have a variable name when adding a new Tripal Variable.', array());
    return NULL;
  }
  if (!$description) {
    tripal_report_error('tripal_core', TRIPAL_ERROR,
        'Must have a description when adding a new Tripal Variable.', array());
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
      ->fields(array(
        'name' => $name,
        'description' => $description,
      ))
      ->execute();
    return tripal_get_variable($name);
  }
}

/**
 * Retrieves the variable name record.
 * 
 * @param $name
 *   The name of the variable to retrieve
 * @return 
 *   A record object containg the variable.
 */
function tripal_get_variable($name) {
  return db_select('tripal_variables', 'v')
    ->fields('v')
    ->condition('name', $name)
    ->execute()
    ->fetchObject();
}

/**
 * Associates a variable and it's value to a node.
 * 
 * If a variable is already associated more with a node then the rank value
 * is used to differentiate them.  By default the rank is set to 0 and can
 * be manually set by using the $rank argument.  But, if left unspecified
 * the next available rank will automatically be used.
 * 
 * If the variable does not exist then it will be added automatically to 
 * prevent errors. However, modules developers should always add their 
 * variables first before using. If the variable already exists then it
 * will simply be re-used. 
 * 
 * @param $nid
 *   The node ID.
 * @param $name
 *   The name of the variable to associated with the node.
 * @param $value
 *   The value of the variable.
 * @param $rank
 *   The rank of the value. Default to zero.  
 * 
 * @return TRUE|FALSE
 *   Returns TRUE if the variable was associated with the node and false if 
 *   a failure occured.
 */
function tripal_add_node_variable($nid, $name, $value, $rank = 0) {
  
  // Make sure the variable exists. To prevent unwanted errors from 
  // umet dependencies (e.g. modules using other modules' variables) the variable
  // will be created automatically if it is missing.
  $variable = tripal_get_variable($name);
  if (!$variable) {
    $variable = tripal_insert_variable($name, "Added automatically. Please describe this variable.");
  }
  
  // Is the variable already associated with this node? If so, then find the
  // next avilable rank. If the $update_existing variable is true then just
  // update the record.
  $values = tripal_get_node_variables($nid, $name);
  if (count($values) > 0) {
    // Get the max rank and increment the rank to the next highest value.
    $max_rank = 0;
    foreach ($values as $value) {
      // If the user has specified a rank then we want ot honor that. If the
      // rank is already used then don't continue.
      if ($rank > 0 and $rank == $value->rank) {
        tripal_report_error('tripal_core', TRIPAL_ERROR,
            "The rank for the term, '$term', is already used for node $nid. " .
            "Cannot add the variable.", array());
        return FALSE;
      }
      if ($value->rank > $max_rank) {
        $max_rank = $value->rank;
      }
    }
    $rank = $max_rank++;
  }
  
  // Add the new variable.
  $node_variable_id = db_insert('tripal_node_variables')
    ->fields(array(
      'variable_id' => $variable->variable_id,
      'nid' => $nid,
      'value' => $value,
      'rank' => $rank
     ))
     ->execute();
  return $node_variable_id;
}
 
/**
 * Returns one or more variables assigned to a node.
 * 
 * An array is returned containing an object for each variable assigned 
 * to a term that matches the criteria.  If a name and rank are provided then
 * the variables returned must match.
 * 
 * @param $nid
 *   The node ID
 * @param $name
 *   Optional.  The name of the variable.
 * @param $rank
 *   Optional.  The rank of the variable to retreive.
 * @return
 *   An array of variable objects.
 */
function tripal_get_node_variables($nid, $name = '', $rank = '') {
  $variables = array();
  if (!$nid) {
    return $variables;
  }
  $query = db_select('tripal_node_variables', 'tnv')
    ->fields('tnv')
    ->condition('nid', $nid, '=');
  if ($name) {
    $variable = tripal_get_variable($name);
    $query->condition('variable_id', $variable->variable_id, '=');
  }
  if ($rank) {
    $query->condition('rank', $rank, '=');
  }
  $results = $query->execute();
  
  // Build the  variables array and return it.
  while($variable = $results->fetchObject()) {
    $variables[] = $variable;
  }
  return $variables;
}

/**
 * Removes variables assigned to a node.
 * 
 * @param $nid
 *   The node ID
 * @param $name
 *   Optional.  The name of the variable.
 * @param $rank
 *   Optional.  The rank of the variable to retreive.
 *   
 * @return 
 *   Return TRUE if deletion is successful, FALSE otherwise.
 */
function tripal_delete_node_variables($nid, $name = '', $rank = '') {
  if (!$nid) {
    return FALSE;
  }

  $query = db_delete('tripal_node_variables')
    ->condition('nid', $nid, '=');
  if ($name) {
    $variable = tripal_get_variable($name);
    $query->condition('variable_id', $variable->variable_id, '=');
  }
  if ($rank) {
    $query->condition('rank', $rank, '=');
  }
  return $query->execute();
}