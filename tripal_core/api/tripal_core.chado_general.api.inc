<?php

require_once "tripal_core.schema_v1.2.api.inc";
require_once "tripal_core.schema_v1.11.api.inc";

/**
 * @defgroup tripal_chado_api Chado API
 * @ingroup tripal_core_api
 * @{
 * Provides an application programming interface (API) to manage data withing the Chado database.
 * This includes functions for selecting, inserting, updating and deleting records
 * in Chado tables.  The functions will ensure proper integrity contraints are met
 * for inserts and updates.
 *
 * Also, a set of functions is provided for creating template variables.  First,
 * is the tripal_core_generate_chado_vars which is used to select one ore more
 * records from a table and return an array with foreign key relationships fully
 * populated.  For example, if selecting a feature, the organism_id and type_id
 * would be present in the returned array as a nested array with their respective
 * foreign keys also nested.  The only fields that are not included are text
 * fields (which may be very large) or many-to-many foreign key relationships.
 * However, these fields and relationships can be expanded using the
 * tripal_core_expand_chado_vars.
 *
 * When a row from a chado table is selected using these two functions, it provides
 * a way for users who want to cutomize Drupal template files to access all data
 * associate with a specific record.
 *
 * Finally, the property tables in Chado generally follow the same format.  Therefore
 * there is a set of functions for inserting, updating and deleting properties for
 * any table.  This provides quick lookup of properties (provided the CV term is
 * known).
 *
 * @}
 *
 */

// Globals used by Tripals Error catching functions
// Should match those defined by watchdog
define('TRIPAL_CRITICAL',2);
define('TRIPAL_ERROR',3);
define('TRIPAL_WARNING',4);
define('TRIPAL_NOTICE',5);
define('TRIPAL_INFO',6);
define('TRIPAL_DEBUG',7);


/**
 * This function is used to set the global Chado variables
 * @ingroup tripal_chado_api
 */
function tripal_core_set_globals() {
  // these global variables are meant to be accessed by all Tripal
  // modules to find the chado version installed and if Chado is local.
  // these variables are stored as globals rather than using the drupal_set_variable
  // functions because the Drupal functions make databaes queries and for long
  // running loaders we don't want those queries repeatedly.
  $GLOBALS["chado_is_installed"]  = tripal_core_is_chado_installed();
  if ($GLOBALS["chado_is_installed"]) {
    $GLOBALS["chado_is_local"]      = tripal_core_is_chado_local();
    $GLOBALS["chado_version"]       = tripal_core_get_chado_version();
    $GLOBALS["exact_chado_version"] = tripal_core_get_chado_version(TRUE);
  }
}

/**
 * Provide better error notice for Tripal
 * @param $type
 *   The catagory to which this message belongs. Can be any string, but the general
 *   practice is to use the name of the module.
 * @param $message
 *   The message to store in the log. Keep $message translatable by not concatenating
 *   dynamic values into it! Variables in the message should be added by using placeholder
 *   strings alongside the variables argument to declare the value of the placeholders.
 *   See t() for documentation on how $message and $variables interact.
 * @param $variables
 *   Array of variables to replace in the message on display or NULL if message is
 *   already translated or not possible to translate.
 * @param $severity
 *   The severity of the message; one of the following values:
 *     - TRIPAL_CRITICAL: Critical conditions.
 *     - TRIPAL_ERROR: Error conditions.
 *     - TRIPAL_WARNING: Warning conditions.
 *     - TRIPAL_NOTICE: (default) Normal but significant conditions.
 *     - TRIPAL_INFO: Informational messages.
 *     - TRIPAL_DEBUG: Debug-level messages.
 * @param $options
 *   An array of options. Some available options include:
 *     - print: prints the error message to the screen. Useful when display is the command-line
 *
 * @ingroup tripal_chado_api
 */
function tripal_core_report_error($type, $severity, $message, $variables = array(), $options = array()) {

  // Get human-readable severity string
  $severity_string = '';
  switch ($severity) {
    case TRIPAL_CRITICAL:
      $severity_string = 'CRITICAL';
      break;
    case TRIPAL_ERROR:
      $severity_string = 'ERROR';
      break;
    case TRIPAL_WARNING:
      $severity_string = 'WARNING';
      break;
    case TRIPAL_NOTICE:
      $severity_string = 'NOTICE';
      break;
    case TRIPAL_INFO:
      $severity_string = 'INFO';
      break;
    case TRIPAL_DEBUG:
      $severity_string = 'DEBUG';
      break;
  }

  // Send to watchdog
  try {
    watchdog($type, $message, $variables, $severity);
  }
  catch (Exception $e) {
    print "CRITICAL (TRIPAL_CORE): Unable to register error message with watchdog";
    $options['print'] = TRUE;
  }

  // If print option supplied then print directly to the screen
  if (isset($options['print'])) {
    if (sizeof($variables) > 0) {
      $message = str_replace(array_keys($variables), $variables, $message);
    }
    print $severity_string . ' (' . strtoupper($type) . '):' . $message . "\n";
  }
}

/**
 * Get chado id for a node. E.g, if you want to get 'analysis_id' from the
 * 'analysis' table for a synced 'chado_analysis' node, (the same for
 * organisms and features):
 * $analysis_id = chado_get_id_for_node ('analysis', $node->nid)
 * $organism_id = chado_get_id_for_node ('organism', $node->nid)
 * $feature_id  = chado_get_id_for_node ('feature', $node->nid)
 *
 * @param $table
 * @param $nid
 *
 * @ingroup tripal_chado_api
 */
function chado_get_id_for_node($table, $nid) {
  $sql = "SELECT " . $table . "_id as id FROM {chado_$table} WHERE nid = :nid";
  return db_query($sql, array(':nid' => $nid))->fetchField();
}

/**
 *  Get node id for a chado feature/organism/analysis. E.g, if you want to
 *  get the node id for an analysis, use:
 *  $nid = chado_get_node_id ('analysis', $analysis_id)
 *  Likewise,
 *  $nid = chado_get_node_id ('organism', $organism_id)
 *  $nid = chado_get_node_id ('feature', $feature_id)
 *
 *  @ingroup tripal_chado_api
 */
function chado_get_node_id($table, $id) {
  $sql = "SELECT nid FROM {chado_$table} WHERE " . $table . "_id = :" . $table . "_id";
  return db_query($sql, array(":" . $table . "_id" => $id))->fetchField();
}

/**
 * Set the Tripal Database
 *
 * The tripal_db_set_active function is used to prevent namespace collisions
 * when chado and drupal are installed in the same database but in different
 * schemas.  It is also used for backwards compatibility with older versions
 * of tripal or in cases where chado is located outside of the Drupal database.
 * or when using Drupal functions such as db_table_exists()
 *
 * @ingroup tripal_chado_api
 */
function tripal_db_set_active($dbname  = 'default') {
  global $databases, $active_db;

  if ($dbname ) {
    if ($dbname == 'chado') {
      db_query('set search_path to chado,public');
      return 'default';
    }
    else {
      db_query('set search_path to public');
      return 'chado';
    }
  }
  // if the 'chado' database is in the $db_url variable then chado is
  // not in the same Drupal database, so we don't need to set any
  // search_path and can just change the database
  elseif (array_key_exists($dbname, $databases)) {
    return db_set_active($dbname);
  }
}

/**
 * Get max rank for a given set of criteria
 * This function was developed with the many property tables in chado in mind but will
 * work for any table with a rank
 *
 * @params tablename: the name of the chado table you want to select the max rank from
 *    this table must contain a rank column of type integer
 * @params where_options: array(
 *                          <column_name> => array(
 *                            'type' => <type of column: INT/STRING>,
 *                            'value' => <the value you want to filter on>,
 *                            'exact' => <if TRUE use =; if FALSE use ~>,
 *                          )
 *        )
 *     where options should include the id and type for that table to correctly
 *     group a set of records together where the only difference are the value and rank
 * @return the maximum rank
 *
 * @ingroup tripal_chado_api
 */
function tripal_core_get_max_chado_rank($tablename, $where_options) {

  $where_clauses = array();
  $where_args = array();

  //generate the where clause from supplied options
  // the key is the column name
  $i = 0;
  $sql = "
    SELECT max(rank) as max_rank, count(rank) as count
    FROM {".$tablename."}
    WHERE
  ";
  foreach ($where_options as $key => $value) {
    $where_clauses[] = "$key = :$key";
    $where_args[":$key"] = $value;
  }
  $sql .= implode($where_clauses, ' AND ');

  $result = chado_query($sql, $where_args)->fetchObject();
  if ($result->count > 0) {
    return $result->max_rank;
  }
  else {
    return -1;
  }

}

/**
 * Use this function to encapsulate text intended to be 
 * visible only by the site administrator. A small tripal logo
 * appears alongside the text.  Do not call this function directly, but
 * rather, use the theme() function:
 * 
 *   theme('tripal_admin_message', array('message' => $my_message));
 * 
 * @param $message
 *   The message to be displayed to the site administrator
 * 
 * @ingroup tripal_chado_api
 */
function theme_tripal_admin_message($variables) {
  
  $message = $variables['message'];
  
  if (!user_access('access administration pages')) {
    return ''; 
  }
  return "
    <div class=\"tripal-site-admin-only\">
      <div class=\"tripal-site-admin-message\">$message</div>
    </div>";
}