<?php
/**
 * @file
 * Provides an API to manage custom tables in Chado.
 */

/**
 * @defgroup tripal_custom_tables_api Tripal Custom Tables API
 * @ingroup tripal_core_api
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
 * @param $skip_creation
 *   Set as TRUE to skip dropping and re-creation of the table.  This is
 *   useful if the table was already created through another means and you
 *   simply want to make Tripal aware of the table schema.
 *
 * @ingroup tripal_custom_tables_api
 */
function chado_edit_custom_table($table_id, $table_name, $schema, $skip_creation = 1) {

  // Create a new record
  $record = new stdClass();
  $record->table_id = $table_id;
  $record->table_name = $table_name;
  $record->schema = serialize($schema);

  // get the current custom table record
  $sql = "SELECT * FROM {tripal_custom_tables} WHERE table_id = :table_id";
  $results = db_query($sql, array(':table_id' => $table_id));
  $custom_table = $results->fetchObject();

  // if the user changed the table name, we want to drop the old one and force
  // creation of the new one.
  if ($custom_table->table_name != $table_name) {
    chado_query("DROP TABLE %s", $custom_table->table_name);
    $skip_creation = 0; // we want to create the table
  }

  // if skip creation is not set, then drop the table from chado if it exists
  if (!$skip_creation) {
    if (db_table_exists($custom_table->table_name)) {
      chado_query("DROP TABLE %s", $custom_table->table_name);
      drupal_set_message(t("Custom Table '%name' dropped", array('%name' => $custom_table->table_name)));
    }
  }

  // update the custom table record and re-create the table in Chado
  if (drupal_write_record('tripal_custom_tables', $record, 'table_id')) {

    // drop the table from chado if it exists
    if (!$skip_creation) {
      if (db_table_exists($custom_table->table_name)) {
        chado_query("DROP TABLE %s", $custom_table->table_name);
        drupal_set_message(t("Custom Table '%name' dropped", array('%name' => $custom_table->table_name)));
      }

      // re-create the table
      if (!chado_create_custom_table ($table_name, $schema)) {
        drupal_set_message(t("Could not create the custom table. Check Drupal error report logs."));
      }
      else {
        drupal_set_message(t("Custom table '%name' created", array('%name' => $table_name)));
      }
    }
    // TODO: add FK constraints
  }
}

/**
 * Add a new table to the Chado schema. This function is simply a wrapper for
 * the db_create_table() function of Drupal, but ensures the table is created
 * inside the Chado schema rather than the Drupal schema.  If the table already
 * exists then it will be dropped and recreated using the schema provided.
 * However, it will only drop a table if it exsits in the tripal_custom_tables
 * table. This way the function cannot be used to accidentally alter existing
 * non custom tables.  If $skip_creation is set then the table is simply
 * added to the tripal_custom_tables and no table is created in Chado.
 *
 * @param $table
 *   The name of the table to create.
 * @param $schema
 *   A Drupal-style Schema API definition of the table
 * @param $skip_creation
 *   Set as TRUE to skip dropping and re-creation of the table if it already
 *   exists.  This is useful if the table was already created through another
 *   means and you simply want to make Tripal aware of the table schema.  If the
 *   table does not exist it will be created.
 *
 * @return
 *   TRUE on success, FALSE on failure
 *
 * @ingroup tripal_custom_tables_api
 */
function chado_create_custom_table($table, $schema, $skip_creation = 1) {
  global $databases;
  $created = 0;
  $recreated = 0;

  // see if the table entry already exists in the tripal_custom_tables table.
  $sql = "SELECT * FROM {tripal_custom_tables} WHERE table_name = :table_name";
  $results = db_query($sql, array(':table_name' => $table));
  $centry = $results->fetchObject();

  // check to see if the table already exists in the chado schema
  $exists = chado_table_exists($table);

  // if the table does not exist then create it
  if (!$exists) {
    try {
      $ret = db_create_table('chado.' . $table, $schema);
      $created = 1;
    }
    catch (Exception $e) {
      $error = $e->getMessage();
      tripal_report_error('tripal_core', TRIPAL_ERROR,
        "Error adding custom table: @message", array('@message' => $error));
      drupal_set_message("Could not add custom table. $error.", "error");
      return FALSE;
    }
  }

  // if the table exists in Chado and in our custom table and
  // skip creation is turned off then drop and re-create the table
  if ($exists and is_object($centry) and !$skip_creation) {

    // drop the table we'll recreate it with the new schema
    try {
      chado_query('DROP TABLE {' . $table . '}');
      db_create_table('chado.' . $table, $schema);
      $recreated = 1;
    }
    catch (Exception $e) {
      $error = $e->getMessage();
      tripal_report_error('tripal_core', TRIPAL_ERROR,
        "Error adding custom table: @message",
        array('@message' => $error));
      drupal_set_message("Could not add custom table. $error.", "error");
      return FALSE;
    }
  }

  // add an entry in the tripal_custom_table
  $record = new stdClass();
  $record->table_name = $table;
  $record->schema = serialize($schema);

  // if an entry already exists then remove it
  if ($centry) {
    $sql = "DELETE FROM {tripal_custom_tables} WHERE table_name = :table_name";
    db_query($sql, array(':table_name' => $table));
  }
  $success = drupal_write_record('tripal_custom_tables', $record);
  if (!$success) {
    tripal_report_error('tripal_core', TRIPAL_ERROR,
      "Error adding custom table %table_name.",
      array('%table_name' => $table));
    drupal_set_message(t("Could not add custom table %table_name.
      Please check the schema array.", array('%table_name' => $table)), 'error');
    return FALSE;
  }


  // now add any foreign key constraints
  if (!$skip_creation and array_key_exists('foreign keys', $schema)) {

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
        try {
          chado_query($sql);
        }
        catch (Exception $e) {
          $error = $e->getMessage();
          tripal_report_error('tripal_core', TRIPAL_ERROR, "Error, could not add foreign key contraint to custom table: %error",
            array('%error' => $error));
          drupal_set_message("Could not add foreign key contraint to table: $error", 'error');
          return FALSE;
        }
      }
    }
  }

  if ($created) {
    drupal_set_message("Custom table created successfully.", 'status');
  }
  elseif ($recreated) {
    drupal_set_message("Custom table re-created successfully.", 'status');
  }
  else {
    drupal_set_message("Custom table already exists. Table structure not changed, but definition array has been saved.", 'status');
  }
  return TRUE;
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
