<?php
/**
 * @file
 * Provides an API to manage custom tables in Chado.
 */

/**
 * @defgroup tripal_custom_tables_api Chado Custom Tables
 * @ingroup tripal_chado_api
 * @{
 * Provides an API to manage custom tables in Chado.
 * @}
 */

/**
 * Edits a custom table in the chado database. It supports
 * using the Drupal Schema API array.
 *
 * @param $table_id
 *   The table_id of the table to edit.
 * @param $table_name
 *   The name of the custom table.
 * @param $schema
 *   Use the Schema API array to define the custom table.
 * @param $skip_if_exists
 *   Set as TRUE to skip dropping and re-creation of the table.  This is
 *   useful if the table was already created through another means and you
 *   simply want to make Tripal aware of the table schema.
 *
 * @ingroup tripal_custom_tables_api
 */
function chado_edit_custom_table($table_id, $table_name, $schema, $skip_if_exists = 1) {

  $transaction = db_transaction();
  try {
    // Create a new record.
    $record = new stdClass();
    $record->table_id = $table_id;
    $record->table_name = $table_name;
    $record->schema = serialize($schema);

    // Get the current custom table record.
    $sql = "SELECT * FROM {tripal_custom_tables} WHERE table_id = :table_id";
    $results = db_query($sql, [':table_id' => $table_id]);
    $custom_table = $results->fetchObject();

    // If this is a materialized view then don't allow editing with this function.
    if ($custom_table->mview_id) {
      tripal_report_error('tripal_chado', TRIPAL_ERROR, "Please use the chado_edit_mview() function to edit this custom table as it is a materialized view.", []);
      drupal_set_message("This custom table is a materialized view. Please use the " . l('Materialized View', 'admin/tripal/storage/chado/mviews') . " interface to edit it.", 'error');
      return FALSE;
    }

    // If the user changed the table name, we want to drop the old one and force
    // creation of the new one.
    if ($custom_table->table_name != $table_name) {
      chado_query("DROP TABLE %s", $custom_table->table_name);
      $skip_if_exists = 0; // we want to create the table
    }

    // If skip creation is not set, then drop the table from chado if it exists.
    if (!$skip_if_exists) {
      if (db_table_exists($custom_table->table_name)) {
        chado_query("DROP TABLE %s", $custom_table->table_name);
        drupal_set_message(t("Custom Table " . $custom_table->table_name . " dropped"));
      }
    }

    // Update the custom table record and run the create custom table function.
    drupal_write_record('tripal_custom_tables', $record, 'table_id');
    $success = chado_create_custom_table($table_name, $schema, $skip_if_exists);

    // Re-add the custom table to the semantic web interface to pick up any
    // changes in fields.
    chado_add_semweb_table($table_name);
  } catch (Exception $e) {
    $transaction->rollback();
    watchdog_exception('tripal_chado', $e);
    $error = _drupal_decode_exception($e);
    drupal_set_message(t("Could not update custom table '%table_name': %message.",
      ['%table_name' => $table, '%message' => $error['!message']]), 'error');
    return FALSE;
  }
  return TRUE;
}

/**
 * Add a new table to the Chado schema.
 *
 * This function is simply a wrapper for  the db_create_table() function of
 * Drupal, but ensures the table is created
 * inside the Chado schema rather than the Drupal schema.  If the table already
 * exists then it will be dropped and recreated using the schema provided.
 * However, it will only drop a table if it exsits in the tripal_custom_tables
 * table. This way the function cannot be used to accidentally alter existing
 * non custom tables.  If $skip_if_exists is set then the table is simply
 * added to the tripal_custom_tables and no table is created in Chado.
 *
 * If you are creating a materialized view do not use this function, but rather
 * use the chado_add_mview(). A materialized view is also considered a custom
 * table and an entry for it will be added to both the tripal_mviews and
 * tripal_custom_tables tables, but only if the chado_add_mview() function is
 * used. The optional $mview_id parameters in this function is intended
 * for use by the chado_add_mview() function when it calls this function
 * to create the table.
 *
 * @param $table
 *   The name of the table to create.
 * @param $schema
 *   A Drupal-style Schema API definition of the table.
 * @param $skip_if_exists
 *   Set as TRUE to skip dropping and re-creation of the table if it already
 *   exists.  This is useful if the table was already created through another
 *   means and you simply want to make Tripal aware of the table schema.  If the
 *   table does not exist it will be created.
 * @param $mview_id
 *   Optional. If this custom table is also a materialized view then provide
 *   it's mview_id. This paramter is intended only when this function
 *   is called by the chado_add_mview() function. When creating a custom
 *   table you shouldn't need to use this parameter.
 * @param $redirect
 *   Optional (default: TRUE). By default this function redirects back to
 *   admin pages. However, when called by Drush we don't want to redirect. This
 *   parameter allows this to be used as a true API function.
 *
 * @return
 *   TRUE on success, FALSE on failure.
 *
 * @ingroup tripal_custom_tables_api
 */
function chado_create_custom_table($table, $schema, $skip_if_exists = TRUE,
                                   $mview_id = NULL, $redirect = TRUE) {

  if (!$table) {
    tripal_report_error('trp_ctables', TRIPAL_ERROR,
      'Please provide a value for the $table argument to the chado_create_custom_table() function');
    return FALSE;
  }
  if (!$schema) {
    tripal_report_error('trp_ctables', TRIPAL_ERROR,
      'Please provide a value for the $schema argument to the chado_create_custom_table() function');
    return FALSE;
  }
  if ($schema and !is_array($schema)) {
    tripal_report_error('trp_ctables', TRIPAL_ERROR,
      'Please provide an array for the $schema argument to the chado_create_custom_table() function');
    return FALSE;
  }
  // TODO: make sure the schema is valid by adding extra checks.


  global $databases;
  $created = 0;
  $recreated = 0;

  $chado_schema = chado_get_schema_name('chado');
  $chado_dot = $chado_schema . '.';

  $transaction = db_transaction();
  try {
    // See if the table entry already exists in the tripal_custom_tables table.
    $sql = "SELECT * FROM {tripal_custom_tables} WHERE table_name = :table_name";
    $results = db_query($sql, [':table_name' => $table]);
    $centry = $results->fetchObject();

    // Check to see if the table already exists in the chado schema.
    $exists = chado_table_exists($table);

    // If the table does not exist then create it.
    if (!$exists) {
      $ret = db_create_table($chado_dot . $table, $schema);
      $created = 1;
    }

    // If the table exists in Chado and in our custom table and
    // skip creation is turned off then drop and re-create the table.
    if ($exists and is_object($centry) and !$skip_if_exists) {

      // Drop the table we'll recreate it with the new schema.
      chado_query('DROP TABLE {' . $table . '}');
      // Remove any 'referring_tables' from the array as the 
      // db_create_table doesn't use that.
      $new_schema = $schema;
      if (array_key_exists('referring_tables', $new_schema)) {
        unset($new_schema['referring_tables']);
      }
      db_create_table($chado_dot . $table, $new_schema);
      $recreated = 1;
    }

    // Add an entry in the tripal_custom_table.
    $record = new stdClass();
    $record->table_name = $table;
    $record->schema = serialize($schema);
    if ($mview_id) {
      $record->mview_id = $mview_id;
    }

    // If an entry already exists then remove it.
    if ($centry) {
      $sql = "DELETE FROM {tripal_custom_tables} WHERE table_name = :table_name";
      db_query($sql, [':table_name' => $table]);
    }
    $success = drupal_write_record('tripal_custom_tables', $record);

    // Now add any foreign key constraints.
    if (($created or !$skip_if_exists) and array_key_exists('foreign keys', $schema)) {

      // Iterate through the foreign keys and add each one.
      $fkeys = $schema['foreign keys'];
      foreach ($fkeys as $fktable => $fkdetails) {
        $relations = $fkdetails['columns'];
        foreach ($relations as $left => $right) {
          $sql = '
            ALTER TABLE {' . $table . '}
              ADD CONSTRAINT ' . $table . '_' . $left . '_fkey FOREIGN KEY (' . $left . ')
              REFERENCES  {' . $fktable . '} (' . $right . ')
              ON DELETE CASCADE DEFERRABLE INITIALLY DEFERRED
          ';
          chado_query($sql);
        }
      }
    }

    // Add the custom table to the semantic web interface.
    chado_add_semweb_table($table);
  } catch (Exception $e) {
    $transaction->rollback();
    $error = $e->getMessage();
    watchdog_exception('tripal_chado', $e);
    drupal_set_message(t("Could not add custom table '%table_name': %message.",
      ['%table_name' => $table, '%message' => $error]), 'error');
    return FALSE;
  }

  if ($created) {
    drupal_set_message("Custom table, '" . $table . "' ,  created successfully.", 'status');
  }
  elseif ($recreated) {
    drupal_set_message("Custom table, '" . $table . "' ,  re-created successfully.", 'status');
  }
  else {
    drupal_set_message("Custom table, '" . $table . "' , already exists. Table structure not changed, but definition array has been saved.", 'status');
  }

  // Only redirect if asked to. This allows us to not try to redirect when this
  // function is called by Drush.
  if ($redirect) {
    if ($mview_id) {
      drupal_goto('admin/tripal/storage/chado/mviews/');
    }
    else {
      drupal_goto('admin/tripal/storage/chado/custom_tables');
    }
  }

  return TRUE;
}

/**
 * This function is used to validate a Drupal Schema API array prior to
 * passing it ot the chado_create_custom_table_schema().  This function
 * can be used in a form validate function or whenver a schema is provided by
 * a user and needs validation.
 *
 * @param $schema_array
 *   the Drupal Schema API compatible array.
 *
 * @return
 *   An empty string for success or a message string for failure.
 *
 * @ingroup tripal_custom_tables_api
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

  // Check unique key length.
  if (array_key_exists('unique keys', $schema_array)) {
    foreach ($schema_array['unique keys'] as $index_name => $details) {
      if (strlen($schema_array['table'] . '_' . $index_name) > 60) {
        return "One ore more unique key names appear to be too long. For example: '" . $schema_array['table'] . '_' . $index_name .
          ".'  Unique key names are created by concatenating the table name with the key name provided " .
          "in the 'unique keys' array of the schema. Please alter any unique keys that when combined with the table name are " .
          "longer than 60 characters.";
      }
    }
  }

}

/**
 * Retrieve the custom table id given the name.
 *
 * @param $table_name
 *   The name of the custom table.
 *
 * @return
 *   The unique identifier for the given table.
 *
 * @ingroup tripal_custom_tables_api
 */
function chado_get_custom_table_id($table_name) {
  if (db_table_exists('tripal_custom_tables')) {
    $sql = "SELECT * FROM {tripal_custom_tables} WHERE table_name = :table_name";
    $results = db_query($sql, [':table_name' => $table_name]);
    $custom_table = $results->fetchObject();
    if ($custom_table) {
      return $custom_table->table_id;
    }
  }

  return FALSE;
}

/**
 * Retrieves the list of custom tables in this site.
 *
 * @return
 *   An associative array where the key and value pairs are the table names.
 *
 * @ingroup tripal_custom_tables_api
 */
function chado_get_custom_table_names($include_mview = TRUE) {

  $sql = "SELECT table_name FROM {tripal_custom_tables}";
  if (!$include_mview) {
    $sql .= " WHERE mview_id IS NULL";
  }
  $resource = db_query($sql);

  foreach ($resource as $r) {
    $tables[$r->table_name] = $r->table_name;
  }

  asort($tables);
  return $tables;
}

/**
 * Deletes the specified custom table.
 *
 * @param $table_id
 *   The unique ID of the custom table for the action to be performed on.
 *
 * @ingroup tripal_custom_tables_api
 */
function chado_delete_custom_table($table_id) {
  global $user;

  $args = ["$table_id"];
  if (!$table_id) {
    return '';
  }

  // Get this table details.
  $sql = "SELECT * FROM {tripal_custom_tables} WHERE table_id = :table_id";
  $results = db_query($sql, [':table_id' => $table_id]);
  $custom_table = $results->fetchObject();

  // If this is a materialized view then don't allow deletion with this function.
  if ($custom_table->mview_id) {
    tripal_report_error('tripal_chado', TRIPAL_ERROR, "Please use the chado_delete_mview() function to delete this custom table as it is a materialized view. Table not deleted.", []);
    drupal_set_message("This custom table is a materialized view. Please use the " . l('Materialized View', 'admin/tripal/storage/chado/mviews') . " interface to delete it.", 'error');
    return FALSE;
  }

  // Remove the entry from the tripal_custom tables table.
  $sql = "DELETE FROM {tripal_custom_tables} WHERE table_id = $table_id";
  $success = db_query($sql);
  if ($success) {
    drupal_set_message(t("Custom Table '%name' removed", ['%name' => $custom_table->table_name]));
  }

  // Drop the table from chado if it exists.
  if (chado_table_exists($custom_table->table_name)) {
    $success = chado_query("DROP TABLE {" . $custom_table->table_name . "}");
    if ($success) {
      drupal_set_message(t("Custom Table '%name' dropped", ['%name' => $custom_table->table_name]));
    }
    else {
      tripal_report_error('tripal_chado', TRIPAL_ERROR, "Cannot drop the custom table: %name", ['%name' => $custom_table->table_name]);
      drupal_set_message(t("Cannot drop the custom table: '%name'", ['%name' => $custom_table->table_name]));
    }
  }
}
