<?php
/**
 * @file
 * Provides an API to manage custom tables in Chado.
 */

use Drupal\tripal_chado\api\ChadoSchemaExtended;
use Drupal\tripal_chado\ChadoCustomTables\ChadoCustomTable;

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
 * @deprecated
 *
 * Please use the new "tripal_chado.custom_table" service instead.
 *
 * WARNING This function only supports editing of custom tables in the
 * default Chado schema.
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
 *   True if the table was edited successfully, False otherwise.
 *
 * @ingroup tripal_custom_tables_api
 */
function chado_edit_custom_table(int $table_id, string $table_name,
    array $schema, bool $skip_if_exists = True) : bool {

  $logger = \Drupal::service('tripal.logger');
  $custom_tables = \Drupal::service('tripal_chado.custom_tables');
  $mviews = \Drupal::service('tripal_chado.materialized_views');

  $custom_table = $custom_tables->loadById($table_id);
  if (!$custom_table) {
    $logger->error('Cannot find a custom table in this instance of Chado that matches the provided ID.');
    return False;
  }

  $mview_id = $mviews->findByName($table_name);
  if ($mview_id) {
    $logger->error('Cannot edit this custom table as it is a materialized view. '.
      'Use the materialized view API instead.');
    return False;
  }

  // If the table name was not changed.
  if ($custom_table->getTableName() == $table_name) {
    chado_create_custom_table($table_name, $schema, $skip_if_exists);
  }
  // If the table name changed.
  else {
    $custom_table->destory();
    chado_create_custom_table($table_name, $schema);
  }

  return True;
}

/**
 * Add a new table to the Chado schema.
 *
 * @deprecated
 *
 * Please use the new "tripal_chado.custom_table" service instead.
 *
 * WARNING This function only supports creation of custom tables in the default
 * Chado schema. Also, for Tripal v4 the $redirect and $mview_id arguments no
 * longer have meaning.
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
 *   If this custom table is a materialized view, the mview ID.
 * @param bool $redirect
 *   If this form should redirect to a new page.
 * @param Drupal\tripal_chado\Database\ChadoConnection $chado
 *   A ChadoConnection instance. If none is proivded then
 *   the default chado installation is used.
 *
 * @return bool
 *   True if the table was created successfully, False otherwise.
 *
 * @ingroup tripal_custom_tables_api
 */
function chado_create_custom_table(string $table, array $schema, bool $skip_if_exists = TRUE,
    int $mview_id = NULL, bool $redirect = TRUE) {

  $chado = \Drupal::service('tripal_chado.database');
  $custom_tables = \Drupal::service('tripal_chado.custom_tables');

  $table_exists = $chado->schema()->tableExists($table);
  $custom_table = $custom_tables->create($table);

  // Validate the provided schema before we continue.
  $errors = ChadoCustomTable::validateTableSchema($schema);
  if (!empty($errors)) {
    return False;
  }

  // Create the table if it doesn't exist.
  $success = False;
  if (!$table_exists) {
    $success = $custom_table->setTableSchema($schema);
  }

  // Recreate the table if it exists, if it's in the list, and if we aren't
  // skipping existing ones
  if ($table_exists and in_array($table, $custom_tables) and !$skip_if_exists) {
    $errors = ChadoCustomTable::validateTableSchema($schema);
    if (!empty($errors)) {
      return False;
    }
    $success = $custom_table->setTableSchema($schema, True);
  }

  return $success;
}

/**
 * Validate a Drupal Schema API array prior to creating a custom table.
 *
 * @deprecated
 *
 * Please use the
 * \Drupal\tripal_chado\ChadoCustomTables\ChadoCustomTable::validateTableSchema()
 * function instead.
 *
 * This function can be used in a form validate function or whenever a
 * schema is provided by a user and needs validation.
 *
 * @param array $schema_array
 *   An Drupal Schema API compatible array
 *
 * @return string
 *   An empty string for success or a message string for failure
 *
 * @ingroup tripal_custom_tables_api
 */
function chado_validate_custom_table_schema($schema_array) {
  $errors = ChadoCustomTable::validateTableSchema($schema_array);
  return implode('. ', $errors);
}

/**
 * Retrieve the custom table ID of the specified name
 *
 * @deprecated
 *
 * Please use the \Drupal\tripal_chado\ServicesChadoCustomTable::findByName()
 * funct instead.
 *
 * @param string $table_name
 *   The name of the custom table.
 *
 * @return int
 *   The unique identifier for the given table if it exists.
 *
 * @ingroup tripal_custom_tables_api
 */
function chado_get_custom_table_id(string $table_name) : int {
  $custom_tables = \Drupal::service('tripal_chado.custom_tables');
  return $custom_tables->findByName($table_name);
}

/**
 * Retrieve a list of all Chado custom table names.
 *
 * @deprecated
 *
 * Please use the
 * \Drupal\tripal_chado\Services\ChadoCustomTable::getTables() function
 * instead.
 *
 * WARNING: This function only supports geting custom tables in the default
 * Chado schema.
 *
 * @param boolean $include_mview
 *   True if the list of custom tables should include
 *   materialized views.
 * @return array
 *  An associative array where the key and value pairs
 *  are the table name.
 *
 *  @ingroup tripal_custom_tables_api
 */
function chado_get_custom_table_names($include_mview = TRUE) : array {
  $custom_tables = \Drupal::service('tripal_chado.custom_tables');
  $all_tables = $custom_tables->getTables();
  foreach ($all_tables as $table_id => $table_name) {
    $tables[$table_name] = $table_name;
  }
  return $tables;
}

/**
 * Deletes the specified table
 *
 * @deprecated
 *
 * Please use the new "tripal_chado.custom_table" service instead.
 *
 * WARNING: This function only supports deleting custom tables in the
 * default Chado schema.
 *
 * @param int $table_id
 *   The numeric custom table ID.
 * @return bool
 *   True if the table was sucessfully deleted, False otherwise.
 *
 * @ingroup tripal_custom_tables_api
 */
function chado_delete_custom_table(int $table_id) : bool {
  $logger = \Drupal::service('tripal.logger');

  if (!$table_id) {
    $logger->error('Must provide an table_id when deleting a custom table.');
    return FALSE;
  }

  $custom_tables = \Drupal::service('tripal_chado.custom_tables');
  $custom_table = $custom_tables->loadById($table_id);
  if (!$custom_table) {
    $logger->error('Cannot find a custom table in this instance of Chado that matches the provided ID.');
    return False;
  }

  $mviews = \Drupal::service('tripal_chado.custom_tables');
  $mview_id = $mviews->findByName($custom_table->getTableName());
  if ($mview_id) {
    $logger->error('Cannot delete this custom table as it is a materialized view. '.
        'Use the materialized view API instead.');
    return False;
  }

  return $custom_table->delete();
}
