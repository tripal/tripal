<?php
/**
 * @file
 * Provides an application programming interface (API) to manage materialized
 * views in Chado.
 */

use Drupal\tripal_chado\Database\ChadoConnection;
use \Drupal\tripal_chado\ChadoCustomTables\ChadoCustomTable;
use \Drupal\tripal_chado\ChaodCustomTables\ChadoMview;

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
 * @deprecated
 *
 * Please use the "tripal_chado.materialized_view" service instead.
 *
 * For Tripal v4 the redirect argument no longer works. Also, the function
 * no longer sets a Drupal message. The callee should do this.
 *
 * @param $name
 *   The name of the materialized view.
 * @param $modulename
 *   The name of the module submitting the materialized view
 *   (e.g. 'tripal_library').
 * @param $mv_schema
 *   The Drupal table definition array.
 * @param $query
 *   The SQL query that loads the materialized view with data.
 * @param $comment
 *   A string containing a description of the materialized view.
 * @param $redirect
 *   Non functional.
 *
 * @return
 *   TRUE if the materialized view was successfully added, FALSE otherwise.
 *
 * @ingroup tripal_mviews_api
 */
function chado_add_mview($name, $modulename, $mv_schema, $query,
    $comment = NULL, $redirect = TRUE) {

  $logger = \Drupal::service('tripal.logger');

  if (!array_key_exists('table', $mv_schema)) {
    $logger->error('Must have a table name when creating an mview.');
    return FALSE;
  }

  $mviews = \Drupal::service('tripal_chado.materialized_views');
  $mview = $mviews->create($mv_schema['table']);
  $errors = ChadoCustomTable::validateTableSchema($mv_schema);
  if (!empty($errors)) {
    return False;
  }
  $mview->setTableSchema($mv_schema);
  $mview->setSqlQuery($query);
  $mview->setComment($comment);

  return True;
}

/**
 * Edits a materialized view.
 *
 * @deprecated
 *
 * Please use the "tripal_chado.materialized_view" service instead.
 *
 * @param $mview_id
 *   The mview_id of the materialized view to edit.
 * @param $name
 *   The name of the materialized view.
 * @param $modulename
 *   No longer used.
 * @param $mv_table
 *   The name of the table to add to chado. This is the table that can be
 *   queried.
 * @param $mv_specs
 *   The table definition.
 * @param $indexed
 *   No longer used.
 * @param $query
 *   The SQL query that loads the materialized view with data.
 * @param $special_index
 *   No longer used.
 * @param $comment
 *   A string containing a description of the materialized view.
 * @param $mv_schema
 *   If using the newer Schema API array to define the materialized view then
 *   this variable should contain the array.
 *
 * @ingroup tripal_mviews_api
 */
function chado_edit_mview($mview_id, $name, $modulename, $mv_table, $mv_specs,
    $indexed, $query, $special_index, $comment = NULL,
    $mv_schema = NULL, ChadoConnection $chado = NULL) {

    $logger = \Drupal::service('tripal.logger');

    if (!array_key_exists('table', $mv_schema)) {
      $logger->error('Must have a table name when creating an mview.');
      return FALSE;
    }

    $errors = ChadoCustomTable::validateTableSchema($mv_schema);
    if (!empty($errors)) {
      return False;
    }

    $mviews = \Drupal::service('tripal_chado.materialized_views');
    $mview = $mviews->loadById($mview_id);
    $success = $mview->setTableSchema($mv_schema, True);
    if ($success) {
      $mview->setSqlQuery($query);
      $mview->setComment($comment);
    }

    return True;
}

/**
 * Retrieve the materialized view_id given the name.
 *
 * @deprecated
 *
 * Please use the findByName() function of the Materialized Views Manager
 * Service instead. For example:
 *
 * @code
 *   $mviews = \Drupal::service('tripal_chado.materialized_views');
 *   $mview_id = $mviews->findByName($table_name);
 * @endcode
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
  $mviews = \Drupal::service('tripal_chado.materialized_views');
  return $mviews->findByName($view_name);
}

/**
 * Retrieves the list of materialized views in this site.
 *
 * @deprecated
 *
 * Please use the getAllTables() function of the Materialized Views Manager
 * Service instead. For example:
 *
 * @code
 *   $mviews = \Drupal::service('tripal_chado.materialized_views');
 *   $all_mviews = $mviews->getTables();
 * @endcode
 *
 * @returns
 *   An associative array where the key and value pairs are the table names.
 *
 * @ingroup tripal_mviews_api
 */
function chado_get_mview_table_names() {
  $mviews = \Drupal::service('tripal_chado.materialized_views');
  $all_mviews = $mviews->getTables();
  $tables = [];
  foreach ($all_mviews as $mview_id => $table_name) {
    $tables[] = $table_name;
  }
  return $tables;
}

/**
 * Submits a Tripal job to populate the specified Materialized View.
 *
 * @param $mview_id
 *   The unique ID of the materialized view for the action to be performed on.
 *
 * @ingroup tripal_mviews_api
 */
function chado_refresh_mview($mview_id) {
  $current_user = \Drupal::currentUser();
  $logger = \Drupal::service('tripal.logger');

  if (!$mview_id) {
    $logger->error('Must provide an mview_id when refreshing an mview.');
    return FALSE;
  }

  $mviews = \Drupal::service('tripal_chado.materialized_views');
  $mview = $mviews->loadById($mview_id);

  \Drupal::service('tripal.job')->create([
    'job_name' => t("Populate materialized view: '@table'", ['@table' => $mview->getTableName()]),
    'modulename' => 'tripal_chado',
    'callback' => 'chado_populate_mview',
    'arguments' => [$mview_id],
    'uid' => $current_user->id()
  ]);
}

/**
 * Retrieves the list of materialized view IDs and their names.
 *
 * @deprecated
 *
 * Please use the getAllTables() function of the Materialized Views Manager
 * Service instead. For example:
 *
 * @code
 *   $mviews = \Drupal::service('tripal_chado.materialized_views');
 *   $all_mviews = $mviews->getTables();
 * @endcode
 *
 * @return array
 *   An array of arrays with the following properties:  mview_id, name.
 *
 * @ingroup tripal_mviews_api
 */
function chado_get_mviews() {
  $mviews = [];

  $mviews = \Drupal::service('tripal_chado.materialized_views');
  $all_mviews = $mviews->getTables();
  foreach ($all_mviews as $mview_id => $table_name) {
    $mviews[] = (object) [
      'mview_id' => $mview_id,
      'name' => $table_name,
    ];
  }
  return $mviews;
}

/**
 * Deletes a Materialized View.
 *
 * @deprecated
 *
 * Please use the "tripal_chado.materialized_view" service instead.
 *
 * @param $mview_id
 *   The unique ID of the materialized view for the action to be performed on.
 *
 * @return
 *   TRUE if the deletion was a success, FALSE on error.
 *
 * @ingroup tripal_mviews_api
 */
function chado_delete_mview($mview_id) {

  $logger = \Drupal::service('tripal.logger');
  if (!$mview_id) {
    $logger->error('Must provide an mview_id when deleting an mview.');
    return FALSE;
  }

  $mviews = \Drupal::service('tripal_chado.materialized_views');
  $mview = $mviews->loadById($mview_id);
  if (!$mview) {
    $logger->error('Cannot find a materialized view in this instance of Chado that matches the provided ID.');
    return False;
  }
  return $mview->destroy();
}

/**
 * Populates a Materialized View.
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

  $logger = \Drupal::service('tripal.logger');
  if (!$mview_id) {
    $logger->error('Must provide an mview_id when deleting an mview.');
    return FALSE;
  }

  $mviews = \Drupal::service('tripal_chado.materialized_views');
  $mview = $mviews->loadById($mview_id);
  if (!$mview) {
    $logger->error('Cannot find a materialized view in this instance of Chado that matches the provided ID.');
    return False;
  }
  return $mview->populate();
}