<?php
/**
 * @file
 * Provides an API to manage custom tables in Chado.
 */

use Drupal\tripal_chado\api\ChadoSchemaExtended;

/**
 * @defgroup tripal_custom_tables_api Chado Custom Tables
 * @ingroup tripal_chado_api
 * @{
 * Provides an application programming interface (API) for managing Chado
 *   tables. 
 *
 * @}
 */

/**
 * Edits a known custom table in the chado database. 
 * 
 * "Known" custom tables are tables that exist in the 
 * tripal_custom_tables already. If it is not in 
 * tripal_custom_tables, or the specified table does not exist, 
 * it will not attempt to make changes. It will also fail if the 
 * specified table is a materialized view.
 * 
 * @param int $table_id
 *   The numeric custom table ID.
 * @param string $table_name
 *   The table name,
 * @param array $schema
 *   An array containing the Drupal table schema.
 * @param bool $skip_if_exists
 *   True if the table should not be changed if it already exists. 
 *   False, to change it regardles.
 * @return bool
 *   True if the table was edited succesfully, False otherwise.
 */
function chado_edit_custom_table(int $table_id, string $table_name, array $schema, bool $skip_if_exists = True) : bool{
  $logger = \Drupal::service('tripal.logger');
  
  // Get connections to the Drupal tables and to Chado.
  $public = \Drupal::database();
  $transaction = $public->startTransaction();
  $chado = \Drupal::service('tripal_chado.database');
  $transaction_chado = $chado->startTransaction();

  try {
    // Create a new record.
    $record = new stdClass();
    $record->table_id = $table_id;
    $record->table_name = $table_name;
    $record->schema = serialize($schema);

    // Get the current custom table record.
    // Get the existing record for this custom table.
    $custom_table = $public
      ->select('tripal_custom_tables','tct')
      ->fields('tct', ['table_name', 'schema', 'mview_id'])
      ->condition('tct.table_id', $table_id)
      ->execute()
      ->fetchObject();
    
    // Check if the table is suitable for editing (not mview, doesn't exist in tripal_custom_tables table).
    if (!$custom_table) {
      throw new Exception('Custom table, "' . $custom_table . '", is not known to be a custom table. Please add it before trying to edit it.');
    }
    else if ($custom_table->mview_id) {
      throw new Exception('Please use the chado_edit_mview() function to edit this custom table as it is a materialized view.');
    }

    // If the user changed the table name, we want to drop the old one and force
    // creation of the new one.
    if ($custom_table->table_name != $table_name) {
      throw new Exception('You cannot rename the table without specifying specifying to re-create (or not skip if it exists).');
    }

    // Does the table exist? If not throw an error.  
    $table_exists = $chado->schema()->tableExists($custom_table->table_name);
    if (!$table_exists) {
      throw new Exception('The custom table does not exist, please add it it first.');
    }
    
    // If the table should be skipped if it exists then update the schema
    // defintion and warn the user.
    if ($skip_if_exists) {
      $public->update('tripal_custom_tables')
        ->fields(['schema' => $record->schema])
        ->condition('table_id', $record->table_id, '=')
        ->execute();
      \Drupal::messenger()->addWarning(t("Only the schema was updated in the tripal_custom_tables. The actual table structure within chado was not updated."), 'status');
    }    
    // If the table exits and should not re-created (skip_if_exists == False)
    // then delete the table so it can be re-created.
    else {
      chado_delete_custom_table($table_id);

      // Update the custom table record and run the create custom table function.
      $public->insert('tripal_custom_tables')
        ->fields([
          'table_id' => $record->table_id,
          'table_name' => $record->table_name,
          'schema' => $record->schema        
        ])
        ->execute();
             
      // Create the custom table
      $success = chado_create_custom_table($table_name, $schema, $skip_if_exists);
      if(!$success) {
        throw new Exception('Could not create the custom table in chado');
      }
    }
  }
  catch (Exception $e) {
    $transaction->rollback();
    $transaction_chado->rollback();
    $logger->error($e->getMessage());
    \Drupal::messenger()->addError($e->getMessage);    
    return False;
  }
  return True;
}

/**
 * Add a new table to the Chado schema.
 * 
 * This function is simply a wrapper for built-in database functionality
 * provided by Drupal, but ensures the table is created
 * inside the Chado schema rather than the Drupal schema.  
 * 
 * If the table already exists then it will be dropped and recreated using the
 * schema provided. However, it will only drop a table if it exsits in the
 * tripal_custom_tables table. This way the function cannot be used to
 * accidentally alter existing non custom tables.  If $skip_if_exists
 * is set then the table is simply added to the tripal_custom_tables
 * and no table is created in Chado.
 *
 * @param string $table
 *   The name of the custom table.
 * @param array $schema
 *   An array contiaining a Drupal table schema definition 
 * @param bool $skip_if_exists
 *   True to skip changing the table if it exists. False, to change the 
 *   table. This may result in lost data.
 * @param int $mview_id
 *   If this custom tabler is a materialized view, the mview ID.
 * @param bool $redirect
 *   If this form should redirect to a new page.
 *    
 * @return bool
 *   True if the table was created successfully, False otherwise.
 */
function chado_create_custom_table(string $table, array $schema, bool $skip_if_exists = TRUE, 
    int $mview_id = NULL, bool $redirect = TRUE) : bool {
  
  $logger = \Drupal::service('tripal.logger');
    
  // Basic argument checks
  if (!$table) {
    $logger->error('chado_create_custom_table(). Please provide a value for the $table argument to the chado_create_custom_table() function');
    return FALSE;
  }
  if (!$schema) {
    $logger->error('chado_create_custom_table(). Please provide a value for the $schema argument to the chado_create_custom_table() function');
    return FALSE;
  }
  if ($schema and !is_array($schema)) {
    $logger->error('chado_create_custom_table(). Please provide an array for the $schema argument to the chado_create_custom_table() function');
    return FALSE;
  }

  // This connection is the default which is 'public' schema
  $public = \Drupal::database();
  $transaction = $public->startTransaction();

  // This database connection is the 'chado' schema
  $chado = \Drupal::service('tripal_chado.database');
  $transaction_chado = $chado->startTransaction();
  
  try {
    $table_exists = $chado->schema()->tableExists($table);

    // Get the existing record for this custom table.
    $custom_table = $public
      ->select('tripal_custom_tables','tct')
      ->fields('tct', ['table_name'])
      ->condition('tct.table_name',$table)
      ->execute()
      ->fetchObject();
    
    // Create the table if it doesn't exist.
    if (!$table_exists) {
      $chado->schema()->createTable($table, $schema);
    }

    // Recreate the table if it exists, if it's in the list, and if we aren't 
    // skipping existing ones
    if ($table_exists and is_object($custom_table) and !$skip_if_exists) {
      $chado->schema()->dropTable($table);
      $chado->schema()->createTable($table, $schema);
      // TODO: the old code unset "referring_tables" from $schema,
      //       is this necessary?
    }

    // Add this table to tripal_custom_tables, dropping and re-adding if 
    // necessary.
    if ($custom_table) {
      $public->delete('tripal_custom_tables')
        ->condition('table_name', $table)
        ->execute();
    }

    $public->insert('tripal_custom_tables')
      ->fields([
        'table_name' => $table,
        'schema'     => serialize($schema),
        'mview_id'   => $mview_id
      ])
      ->execute();
  } 
  catch (Exception $e) {
    $transaction->rollback();
    $transaction_chado->rollback();
    $logger->error($e->getMessage());
    \Drupal::messenger()->addError($e->getMessage());    
    return False;
  }
  return True;
}

/**
 *  This function is used to validate a Drupal Schema API array prior to
 *  passing it ot the chado_create_custom_table_schema().  This function
 *  can be used in a form validate function or whenver a schema is provided by
 *  a user and needs validation.
 * 
 *  Current checks:
 *  - 'table' key exists
 *  - table name is lowercase
 *  - table name + index name       < 60 characters
 *  - table name + unique key name  < 60 characters
 *
 * @param array $schema_array
 *   An Drupal Schema API compatible array
 * 
 * @return string
 *   An empty string for success or a message string for failure
 */
function chado_validate_custom_table_schema($schema_array) {

  if (is_array($schema_array) and !array_key_exists('table', $schema_array)) {
    return "The schema array must have key named 'table'";
  }
  
  if (preg_match('/[ABCDEFGHIJKLMNOPQRSTUVWXYZ]/', $schema_array['table'])) {
    return "Postgres will automatically change the table name to lower-case. To prevent unwanted side-effects, please rename the table with all lower-case characters.";
  }
  
    // Check index length.
  if (array_key_exists('indexes', $schema_array)) {
    foreach ($schema_array['indexes'] as $index_name => $details) {
      if (strlen($schema_array['table'] . '_' . $index_name) > 60) {
        return "One ore more index names appear to be too long. For example: '" . $schema_array['table'] . '_' . $index_name .
        ".'  Index names are created by concatenating the table name with the index name provided " .
        "in the 'indexes' array of the schema. Please alter any indexes that when combined with the table name are " .
        "longer than 60 characters.";
      }
    }
  }
}

/**
 * Retrieve the custom table ID of the specified name
 *
 * @param string $table_name
 * @return int
 *   The unique identifier for the given table. FALSE if the
 *   table is not listed in the tripal_custom_tables table
 * 
 * @ingroup tripal_custom_tables_api
 */
function chado_get_custom_table_id(string $table_name) : int {
  $logger = \Drupal::service('tripal.logger');
  
  try {    
    $public = \Drupal::database();    
    $table_exists = $public->schema()->tableExists('tripal_custom_tables');
    if ($table_exists) {
      $table_id = $public->select('tripal_custom_tables','ct')
        ->fields('ct', ['table_id'])
        ->condition('ct.table_name', $table_name)
        ->execute()
        ->fetchField();      
      return $table_id;
    }  
  } 
  catch (Exception $e) {
    $logger->error($e->getMessage());
  }
  return NULL;  
}

/**
 * Retrieve a list of all Chado custom table names..
 *
 * @param boolean $include_mview
 *   True if the list of custom tables should include 
 *   materialized views.
 * @return array
 *  An associative array where the key and value pairs 
 *  are the table name.
 */
function chado_get_custom_table_names($include_mview = TRUE) : array {
  $logger = \Drupal::service('tripal.logger');
  
  try {
    $public = \Drupal::database();

    if ($include_mview) {
      $query = $public->select('tripal_custom_tables','ct')
        ->fields('ct', ['table_name'])
        ->execute();
    }
    else {
      $query = $public->select('tripal_custom_tables','ct')
        ->fields('ct', ['table_name'])
        ->isNull('ct.mview_id')
        ->execute();
    }

    $results = $query->fetchAll();
    foreach($results as $result) {
      $tables[$result->table_name] = $result->table_name;
    }
    if($tables) {
      asort($tables);
    }
    return $tables;
  } 
  catch (Exception $e) {
    $logger->error($e->getMessage());
  }
}

/**
 * Deletes the specified table
 *
 * @param int $table_id
 *   The numeric custom table ID.
 * @return bool
 *   True if the table was sucessfully deleted, False otherwise.
 */
function chado_delete_custom_table(int $table_id) : bool{
  $logger = \Drupal::service('tripal.logger');

  $public = \Drupal::database();
  $transaction = $public->startTransaction();

  // This database connection is the 'chado' schema
  $chado = \Drupal::service('tripal_chado.database');
  $transaction_chado = $chado->startTransaction();

  try {
    // Get details about this table from tripal_custom_tables
    $custom_table = $public->select('tripal_custom_tables', 'ct')
      ->fields('ct', ['table_id', 'table_name', 'schema', 'mview_id'])
      ->condition('ct.table_id', $table_id)
      ->execute()
      ->fetchObject();
    
    $table_exists = $chado->schema()->tableExists($custom_table->table_name);     
    
    // Don't delete if this table is a materialized view 
    if ($custom_table->mview_id) {
      throw new Exception("Please use the chado_delete_mview() function to delete this custom table as it is a materialized view. Table not deleted.");
    }
   
    if (!$custom_table or !$table_exists) {
      $message = "The table " . $custom_table->table_name . " is either not listed in the tripal_custom_tables table or it doesn't exist in the database.\n";
      throw new Exception($message);
    }

    // Remove the entry from the tripal_custom_tables table.
    $num_deleted = $public->delete('tripal_custom_tables')
      ->condition('table_id', $table_id)
      ->execute();
    
    // Drop the table from Chado (checking if it exists)
    $chado->schema()->dropTable($custom_table->table_name);
    $table_exists = $chado->schema()->tableExists($custom_table->table_name);        
    if ($table_exists) {
      throw new Exception('Could not delete the ' . $custom_table->table_name . ' table.');
    }
      
  } 
  catch (Exception $e) {
    $transaction->rollback();
    $transaction_chado->rollback();
    $logger->error($e->getMessage());
    \Drupal::logger('tripal_chado')->error($e->getMessage());
    return False;
  }
  return True;
}