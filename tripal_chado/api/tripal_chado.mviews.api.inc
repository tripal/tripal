<?php
/**
 * @file
 * Provides an application programming interface (API) to manage materialized
 * views in Chado.
 */

/**
 * @defgroup tripal_mviews_api Chado Materalized Views
 * @ingroup tripal_chado_api
 * @{
 * Provides an application programming interface (API) to manage materialized
 * views in Chado The Perl-based chado comes with an interface for managing
 * materialzed views.  This API provides an alternative Drupal-based method.
 * @}
 */

/**
 * Add a materialized view to the chado database.
 *
 * @param $name
 *   The name of the materialized view.
 * @param $modulename
 *   The name of the module submitting the materialized view
 *   (e.g. 'tripal_library').
 * @param $mv_schema
 *   If using the newer Schema API array to define the materialized view then
 *   this variable should contain the array or a string representation of the
 *   array.
 * @param $query
 *   The SQL query that loads the materialized view with data.
 * @param $comment
 *   A string containing a description of the materialized view.
 * @param $redirect
 *   Optional (default: TRUE). By default this function redirects back to
 *   admin pages. However, when called by Drush we don't want to redirect. This
 *   parameter allows this to be used as a true API function.
 *
 * @return
 *   TRUE if the materialized view was successfully added, FALSE otherwise.
 *
 * @ingroup tripal_mviews_api
 */
function chado_add_mview($name, $modulename, $mv_schema, $query, $comment = NULL, $redirect = TRUE) {

  if (!array_key_exists('table', $mv_schema)) {
    tripal_report_error('tripal_chado', TRIPAL_ERROR,
      'Must have a table name when creating an mview.', []);
    return FALSE;
  }

  $mv_table = $mv_schema['table'];

  // See if the mv_table name already exsists.
  $mview_id = db_select('tripal_mviews', 'tm')
    ->fields('tm', ['mview_id'])
    ->condition('name', $name)
    ->execute()
    ->fetchField();

  // Check that the materialized view actually exists and if not,
  // remove the entry from tripal_mviews.
  if ($mview_id AND !chado_table_exists($name)) {
    db_delete('tripal_mviews')
      ->condition('mview_id', $mview_id)
      ->execute();
    $mview_id = FALSE;
  }

  if (!$mview_id) {
    $transaction = db_transaction();
    try {
      // Create a new record.
      $record = new stdClass();
      $record->name = $name;
      $record->modulename = $modulename;
      $record->mv_table = $mv_table;
      $record->query = $query;
      $record->comment = $comment;

      // Convert the schema into a string format.
      $str_schema = var_export($mv_schema, TRUE);
      $str_schema = preg_replace('/=>\s+\n\s+array/', '=> array', $str_schema);
      $record->mv_schema = $str_schema;

      // Add the record to the tripal_mviews table and if successful
      // create the new materialized view in the chado schema.
      if (drupal_write_record('tripal_mviews', $record)) {

        // Drop the table from chado if it exists.
        if (chado_table_exists($mv_table)) {
          $sql = 'DROP TABLE {' . $mv_table . '}';
          chado_query($sql);
        }
        // Create the table.
        chado_create_custom_table($mv_table, $mv_schema, 0, $record->mview_id, $redirect);
      }
    } catch (Exception $e) {
      $transaction->rollback();
      watchdog_exception('tripal_chado', $e);
      $error = _drupal_decode_exception($e);
      tripal_report_error('tripal_chado', TRIPAL_ERROR,
        "Could not create the materialized view %table_name: %message.",
        ['%table_name' => $name, '%message' => $error['!message']]);
      return FALSE;
    }
    drupal_set_message(t("Materialized view '%name' created", ['%name' => $name]));
  }
  else {
    drupal_set_message(t("Materialized view, $name, already exists. Skipping creation.", ['%name' => $name]));
  }
  return TRUE;
}

/**
 * Edits a materialized view to the chado database to help speed data access.
 * This function supports the older style where postgres column specifications
 * are provided using the $mv_table, $mv_specs and $indexed variables. It also
 * supports the newer preferred method where the materialized view is described
 * using the Drupal Schema API array.
 *
 * @param $mview_id
 *   The mview_id of the materialized view to edit.
 * @param $name
 *   The name of the materialized view.
 * @param $modulename
 *   The name of the module submitting the materialized view
 *   (e.g. 'tripal_library').
 * @param $mv_table
 *   The name of the table to add to chado. This is the table that can be
 *   queried.
 * @param $mv_specs
 *   The table definition.
 * @param $indexed
 *   The columns that are to be indexed.
 * @param $query
 *   The SQL query that loads the materialized view with data.
 * @param $special_index
 *   currently not used.
 * @param $comment
 *   A string containing a description of the materialized view.
 * @param $mv_schema
 *   If using the newer Schema API array to define the materialized view then
 *   this variable should contain the array.
 *
 * @ingroup tripal_mviews_api
 */
function chado_edit_mview($mview_id, $name, $modulename, $mv_table, $mv_specs,
                          $indexed, $query, $special_index, $comment = NULL, $mv_schema = NULL) {

  $transaction = db_transaction();
  try {
    // get the table name from the schema array
    $schema_arr = [];
    if ($mv_schema) {
      // get the schema from the mv_specs and use it to add the custom table
      eval("\$schema_arr = $mv_schema;");
      $mv_table = $schema_arr['table'];
    }

    $record = new stdClass();
    $record->mview_id = $mview_id;
    $record->name = $name;
    $record->modulename = $modulename;
    $record->query = $query;
    $record->last_update = 0;
    $record->status = '';
    $record->comment = $comment;
    $record->mv_schema = $mv_schema;
    $record->mv_table = $mv_table;

    // update the record to the tripal_mviews table
    drupal_write_record('tripal_mviews', $record, 'mview_id');

    // get the view before we update and check to see if the table structure has
    // changed. If so, then we want to drop and recreate the table. If not, then
    // just save the updated SQL.
    $create_table = 1;
    $sql = "SELECT * FROM {tripal_mviews} WHERE mview_id = :mview_id";
    $results = db_query($sql, [':mview_id' => $mview_id]);
    $mview = $results->fetchObject();
    if ($mview->mv_schema == $mv_schema and $mview->mv_table == $mv_table) {
      chado_create_custom_table($mv_table, $schema_arr, 0, $record->mview_id);
      drupal_set_message(t("Materialized view '%name' created", ['%name' => $name]));
    }
    else {
      $message = "View '%name' updated.  All records remain. ";
      if ($query != $mview->query) {
        $message .= "Please repopulate the view to use the updated query.";
      }
      drupal_set_message(t($message, ['%name' => $name]));
    }

    // construct the indexes SQL if needed
    $index = '';
    if ($indexed) {
      // add to the array of values
      $vals = preg_split("/[\n,]+/", $indexed);
      $index = '';
      foreach ($vals as $field) {
        $field = trim($field);
        $index .= "CREATE INDEX idx_${mv_table}_${field} ON $mv_table ($field);";
      }
    }

  } catch (Exception $e) {
    $transaction->rollback();
    watchdog_exception('tripal_chado', $e);
    $error = _drupal_decode_exception($e);
    drupal_set_message(t("Could not update materialized view '%table_name': %message.",
      ['%table_name' => $mv_table, '%message' => $error['!message']]), 'error');
    return FALSE;
  }
}

/**
 * Retrieve the materialized view_id given the name.
 *
 * @param $view_name
 *   The name of the materialized view.
 *
 * @return
 *   The unique identifier for the given view.
 *
 * @ingroup tripal_mviews_api
 */
function chado_get_mview_id($view_name) {
  if (db_table_exists('tripal_mviews')) {
    $sql = "SELECT * FROM {tripal_mviews} WHERE name = :name";
    $results = db_query($sql, [':name' => $view_name]);
    $mview = $results->fetchObject();
    if ($mview) {
      return $mview->mview_id;
    }
  }

  return FALSE;
}

/**
 * Retrieves the list of materialized views in this site.
 *
 * @returns
 *   An associative array where the key and value pairs are the table names.
 *
 * @ingroup tripal_mviews_api
 */
function chado_get_mview_table_names() {

  $sql = "SELECT name FROM {tripal_mviews}";
  $resource = db_query($sql);

  $tables = [];
  foreach ($resource as $r) {
    $tables[$r->name] = $r->name;
  }

  asort($tables);
  return $tables;
}

/**
 * Populates the specified Materialized View.
 *
 * @param $mview_id
 *   The unique ID of the materialized view for the action to be performed on.
 *
 * @ingroup tripal_mviews_api
 */
function chado_refresh_mview($mview_id) {
  global $user;

  if (!$mview_id) {
    tripal_report_error('tripal_chado', TRIPAL_ERROR,
      'Must provide an mview_id when refreshing an mview.', []);
    return FALSE;
  }

  // Get this mview details.
  $sql = "SELECT * FROM {tripal_mviews} WHERE mview_id = :mview_id";
  $results = db_query($sql, [':mview_id' => $mview_id]);
  $mview = $results->fetchObject();

  // Add a job to populate the mview.
  $args = ["$mview_id"];
  tripal_add_job("Populate materialized view '$mview->name'", 'tripal_chado',
    'chado_populate_mview', $args, $user->uid);

}

/**
 * Retrieves the list of materialized view IDs and their names.
 *
 * @return
 *   An array of objects with the following properties:  mview_id, name.
 *
 * @ingroup tripal_mviews_api
 *
 */
function chado_get_mviews() {

  $results = db_select('tripal_mviews', 'tm')
    ->fields('tm', ['mview_id', 'name'])
    ->execute();

  $list = [];
  while ($mview = $results->fetchObject()) {
    $list[] = $mview;
  }
  return $list;
}

/**
 * Does the specified action for the specified Materialized View.
 *
 * @param $op
 *   The action to be taken. One of update or delete.
 * @param $mview_id
 *   The unique ID of the materialized view for the action to be performed on.
 *
 * @return
 *   TRUE if the deletion was a success, FALSE on error.
 *
 * @ingroup tripal_mviews_api
 */
function chado_delete_mview($mview_id) {
  global $user;

  if (!$mview_id) {
    tripal_report_error('tripal_chado', TRIPAL_ERROR,
      'Must provide an mview_id when deleting an mview.', []);
    return FALSE;
  }

  try {
    $transaction = db_transaction();

    // Get this mview details.
    $sql = "SELECT * FROM {tripal_mviews} WHERE mview_id = :mview_id";
    $results = db_query($sql, [':mview_id' => $mview_id]);
    $mview = $results->fetchObject();

    // If op is to delete then do so.
    // Remove the mview from the tripal_mviews table.
    $sql = "DELETE FROM {tripal_mviews} WHERE mview_id = $mview_id";
    db_query($sql);

    // Does the table already exist?
    $mview_exists = chado_table_exists($mview->mv_table);

    // Drop the table from chado if it exists.
    if ($mview_exists) {
      $sql = "DROP TABLE {" . $mview->mv_table . "}";
      $success = chado_query($sql);
      if ($success) {
        //unset the variable
        global $databases;
        $default_db = $databases['default']['default']['database'];
        $chado_schema = chado_get_schema_name('chado');
        if (isset($GLOBALS["chado_tables"][$default_db][$chado_schema][$mview->mv_table])) {
          unset($GLOBALS["chado_tables"][$default_db][$chado_schema][$mview->mv_table]);
        }
        drupal_set_message(t("Materialized view, %name, deleted.", ['%name' => $mview->name]));
        return TRUE;
      }
      else {
        drupal_set_message(t("Problem deleting materialized view, %name.", ['%name' => $mview->name]), 'error');
        return FALSE;
      }
    }
    else {
      return TRUE;
    }
  } catch (Exception $e) {
    $transaction->rollback();
    watchdog_exception('tripal_chado', $e);
    $error = _drupal_decode_exception($e);
    tripal_report_error('tripal_chado', TRIPAL_ERROR,
      "Could not delete the materialized view %table_name: %message.",
      ['%table_name' => $name, '%message' => $error['!message']]);
  }
  return FALSE;
}

/**
 * Populate a Materialized View.
 *
 * @param $mview_id
 *   The unique identifier for the materialized view to be updated.
 *
 * @return
 *   True if successful, FALSE otherwise.
 *
 * @ingroup tripal_mviews_api
 */
function chado_populate_mview($mview_id) {
  $sql = "SELECT * FROM {tripal_mviews} WHERE mview_id = :mview_id ";
  $results = db_query($sql, [':mview_id' => $mview_id]);
  $mview = $results->fetchObject();
  if ($mview) {
    // Execute the query inside a transaction so that it doesn't destroy 
    // existing data that may leave parts of the site unfunctional.
    $transaction = db_transaction();
    $previous_db = chado_set_active('chado');  // use chado database
    try {
      $success = chado_query("DELETE FROM {" . $mview->mv_table . "}");
      $success = chado_query("INSERT INTO {" . $mview->mv_table . "} ($mview->query)");
      // If success get the number of results and update the table record.
      if ($success) {
        $sql = "SELECT count(*) as cnt FROM {" . $mview->mv_table . "}";
        $results = chado_query($sql);
        $count = $results->fetchObject();
        $record = new stdClass();
        $record->mview_id = $mview_id;
        $record->last_update = time();
        $record->status = "Populated with " . number_format($count->cnt) . " rows";
        drupal_write_record('tripal_mviews', $record, 'mview_id');
      }
      // If not success then throw an error.
      else {
        throw new Exception("ERROR populating the materialized view " . $mview->mv_table . ". See Drupal's recent log entries for details.");
      }
      chado_set_active($previous_db);
    } catch (Exception $e) {
      $transaction->rollback();
      chado_set_active($previous_db);
      // Print and save the error message.
      $record = new stdClass();
      $record->mview_id = $mview_id;
      $record->status = "ERROR populating $mview->mv_table. See Drupal's recent log entries for details.\n";
      drupal_write_record('tripal_mviews', $record, 'mview_id');
      watchdog_exception('tripal_mviews', $e);
      return FALSE;
    }
    print "Done.\n";
    return TRUE;
  }
}
