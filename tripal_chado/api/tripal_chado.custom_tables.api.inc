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
 *   The table_id of the table to edit
 * @param $table_name
 *   The name of the custom table
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
    // Create a new record
    $record = new stdClass();
    $record->table_id = $table_id;
    $record->table_name = $table_name;
    $record->schema = serialize($schema);

    // get the current custom table record
    $sql = "SELECT * FROM {tripal_custom_tables} WHERE table_id = :table_id";
    $results = db_query($sql, array(':table_id' => $table_id));
    $custom_table = $results->fetchObject();

    // if this is a materialized view then don't allow editing with this function
    if ($custom_table->mview_id) {
      tripal_report_error('tripal_chado', TRIPAL_ERROR, "Please use the tripal_edit_mview() function to edit this custom table as it is a materialized view.", array());
      drupal_set_message("This custom table is a materialized view. Please use the "  . l('Materialized View', 'admin/tripal/storage/chado/mviews') . " interface to edit it.", 'error');
      return FALSE;
    }

    // if the user changed the table name, we want to drop the old one and force
    // creation of the new one.
    if ($custom_table->table_name != $table_name) {
      chado_query("DROP TABLE %s", $custom_table->table_name);
      $skip_if_exists = 0; // we want to create the table
    }

    // if skip creation is not set, then drop the table from chado if it exists
    if (!$skip_if_exists) {
      if (db_table_exists($custom_table->table_name)) {
        chado_query("DROP TABLE %s", $custom_table->table_name);
        drupal_set_message(t("Custom Table " . $custom_table->table_name . " dropped"));
      }
    }

    // update the custom table record and run the create custom table function
    drupal_write_record('tripal_custom_tables', $record, 'table_id');
    $success = chado_create_custom_table ($table_name, $schema, $skip_if_exists);

    // Re-add the custom table to the semantic web interface to pick up any
    // changes in fields.
    tripal_add_chado_semweb_table($table_name);
  }
  catch (Exception $e) {
    $transaction->rollback();
    watchdog_exception('tripal_chado', $e);
    $error = _drupal_decode_exception($e);
    drupal_set_message(t("Could not update custom table '%table_name': %message.",
    array('%table_name' => $table, '%message' => $error['!message'])), 'error');
    return FALSE;
  }
  return TRUE;
}

/**
 * Add a new table to the Chado schema. This function is simply a wrapper for
 * the db_create_table() function of Drupal, but ensures the table is created
 * inside the Chado schema rather than the Drupal schema.  If the table already
 * exists then it will be dropped and recreated using the schema provided.
 * However, it will only drop a table if it exsits in the tripal_custom_tables
 * table. This way the function cannot be used to accidentally alter existing
 * non custom tables.  If $skip_if_exists is set then the table is simply
 * added to the tripal_custom_tables and no table is created in Chado.
 *
 * If you are creating a materialized view do not use this function, but rather
 * use the tripal_add_mview(). A materialized view is also considered a custom table
 * and an entry for it will be added to both the tripal_mviews and
 * tripal_custom_tables tables, but only if the tripal_add_mview() function is
 * used. The optional $mview_id parameters in this function is intended
 * for use by the tripal_add_mview() function when it calls this function
 * to create the table.
 *
 * @param $table
 *   The name of the table to create.
 * @param $schema
 *   A Drupal-style Schema API definition of the table
 * @param $skip_if_exists
 *   Set as TRUE to skip dropping and re-creation of the table if it already
 *   exists.  This is useful if the table was already created through another
 *   means and you simply want to make Tripal aware of the table schema.  If the
 *   table does not exist it will be created.
 * @param $mview_id
 *   Optional. If this custom table is also a materialized view then provide
 *   it's mview_id. This paramter is intended only when this function
 *   is called by the tripal_add_mview() function. When creating a custom
 *   table you shouldn't need to use this parameter.
 * @param $redirect
 *   Optional (default: TRUE). By default this function redirects back to
 *   admin pages. However, when called by Drush we don't want to redirect. This
 *   parameter allows this to be used as a true API function.
 * @return
 *   TRUE on success, FALSE on failure
 *
 * @ingroup tripal_custom_tables_api
 */
function chado_create_custom_table($table, $schema, $skip_if_exists = TRUE, $mview_id = NULL, $redirect = TRUE) {
  global $databases;
  $created = 0;
  $recreated = 0;

  $chado_schema = tripal_get_schema_name('chado');
  $chado_dot = $chado_schema . '.';

  $transaction = db_transaction();
  try {
    // see if the table entry already exists in the tripal_custom_tables table.
    $sql = "SELECT * FROM {tripal_custom_tables} WHERE table_name = :table_name";
    $results = db_query($sql, array(':table_name' => $table));
    $centry = $results->fetchObject();

    // check to see if the table already exists in the chado schema
    $exists = chado_table_exists($table);

    // if the table does not exist then create it
    if (!$exists) {
      $ret = db_create_table($chado_dot . $table, $schema);
      $created = 1;
    }

    // if the table exists in Chado and in our custom table and
    // skip creation is turned off then drop and re-create the table
    if ($exists and is_object($centry) and !$skip_if_exists) {

      // drop the table we'll recreate it with the new schema
      chado_query('DROP TABLE {' . $table . '}');
      // remove any 'referring_tables' from the array as the db_create_table doesn't use that
      $new_schema = $schema;
      if (array_key_exists('referring_tables', $new_schema)) {
        unset($new_schema['referring_tables']);
      }
      db_create_table($chado_dot . $table, $new_schema);
      $recreated = 1;
    }

    // add an entry in the tripal_custom_table
    $record = new stdClass();
    $record->table_name = $table;
    $record->schema = serialize($schema);
    if ($mview_id) {
      $record->mview_id = $mview_id;
    }

    // if an entry already exists then remove it
    if ($centry) {
      $sql = "DELETE FROM {tripal_custom_tables} WHERE table_name = :table_name";
      db_query($sql, array(':table_name' => $table));
    }
    $success = drupal_write_record('tripal_custom_tables', $record);

    // now add any foreign key constraints
    if (($created or !$skip_if_exists) and array_key_exists('foreign keys', $schema)) {

      // iterate through the foreign keys and add each one
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

    // Add the custom table to the semantic web interface
    tripal_add_chado_semweb_table($table);
  }
  catch (Exception $e) {
    $transaction->rollback();
    watchdog_exception('tripal_chado', $e);
    $error = _drupal_decode_exception($e);
    drupal_set_message(t("Could not add custom table '%table_name': %message.",
      array('%table_name' => $table, '%message' => $error['!message'])), 'error');
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
 *   the Drupal Schema API compatible array
 *
 * @return
 *   An empty string for success or a message string for failure
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


  // check index length
  if (array_key_exists('indexes', $schema_array)) {
    foreach ($schema_array['indexes'] as $index_name => $details) {
      if (strlen($schema_array['table'] . '_' . $index_name) > 60) {
        return "One ore more index names appear to be too long. For example: '" .  $schema_array['table'] . '_' . $index_name .
        ".'  Index names are created by concatenating the table name with the index name provided " .
        "in the 'indexes' array of the schema. Please alter any indexes that when combined with the table name are " .
        "longer than 60 characters.";
      }
    }
  }

  // check unique key length
  if (array_key_exists('unique keys', $schema_array)) {
    foreach ($schema_array['unique keys'] as $index_name => $details) {
      if (strlen($schema_array['table'] . '_' . $index_name) > 60) {
        return "One ore more unique key names appear to be too long. For example: '" .  $schema_array['table'] . '_' . $index_name .
        ".'  Unique key names are created by concatenating the table name with the key name provided " .
        "in the 'unique keys' array of the schema. Please alter any unique keys that when combined with the table name are " .
        "longer than 60 characters.";
      }
    }
  }

}
/**
 * Retrieve the custom table id given the name
 *
 * @param $table_name
 *   The name of the custom table
 *
 * @return
 *   The unique identifier for the given table
 *
 * @ingroup tripal_custom_tables_api
 */
function chado_get_custom_table_id($table_name) {
  if (db_table_exists('tripal_custom_tables')) {
    $sql = "SELECT * FROM {tripal_custom_tables} WHERE table_name = :table_name";
    $results = db_query($sql, array(':table_name' => $table_name));
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
 * @returns
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
 * Deletes the specified custom table
 *
 * @param $table_id
 *   The unique ID of the custom table for the action to be performed on
 *
 * @ingroup tripal_custom_tables_api
 */
function chado_delete_custom_table($table_id) {
  global $user;

  $args = array("$table_id");
  if (!$table_id) {
    return '';
  }

  // get this table details
  $sql = "SELECT * FROM {tripal_custom_tables} WHERE table_id = :table_id";
  $results = db_query($sql, array(':table_id' => $table_id));
  $custom_table = $results->fetchObject();

  // if this is a materialized view then don't allow deletion with this function
  if ($custom_table->mview_id) {
    tripal_report_error('tripal_chado', TRIPAL_ERROR, "Please use the tripal_delete_mview() function to delete this custom table as it is a materialized view. Table not deleted.", array());
    drupal_set_message("This custom table is a materialized view. Please use the "  . l('Materialized View', 'admin/tripal/storage/chado/mviews') . " interface to delete it.", 'error');
    return FALSE;
  }

  // remove the entry from the tripal_custom tables table
  $sql = "DELETE FROM {tripal_custom_tables} WHERE table_id = $table_id";
  $success = db_query($sql);
  if ($success) {
    drupal_set_message(t("Custom Table '%name' removed", array('%name' => $custom_table->table_name)));
  }

  // drop the table from chado if it exists
  if (chado_table_exists($custom_table->table_name)) {
    $success = chado_query("DROP TABLE {" .  $custom_table->table_name . "}");
    if ($success) {
      drupal_set_message(t("Custom Table '%name' dropped", array('%name' => $custom_table->table_name)));
    }
    else {
      tripal_report_error('tripal_chado', TRIPAL_ERROR, "Cannot drop the custom table: %name", array('%name' => $custom_table->table_name));
      drupal_set_message(t("Cannot drop the custom table: '%name'", array('%name' => $custom_table->table_name)));
    }
  }
}
