<?php
/**
 * @file
 * Provides functions for managing analysis'.
 *
 * @ingroup tripal_analysis
 */

/**
 * @defgroup tripal_analysis_api Analysis Module API
 * @ingroup tripal_api
 * @{
 * Provides an interface for specialized analysis modules to extend the basic functionality.
 * @}
 */

/**
 * Register tripal_analysis_api sub-modules
 *
 * @param $modulename
 *  The name of the module to be registered as a tripal analysis submodule
 *
 * @ingroup tripal_analysis_api
 */
function tripal_analysis_register_child($modulename) {
  $sql = "SELECT * FROM {tripal_analysis} WHERE modulename = :modname";
  if (!db_query($sql, array(':modname' => $modulename))->fetchField()) {
    $sql = "INSERT INTO {tripal_analysis} (modulename) VALUES (:modname)";
    db_query($sql, array(':modname' => $modulename));
  }
}

/**
 * Un-register a tripal analysis sub-module
 *
 * @param $modulename
 *  The name of the module to un-register
 *
 * @ingroup tripal_analysis_api
 */
function tripal_analysis_unregister_child($modulename) {
  if (db_table_exists('tripal_analysis')) {
      $sql = "DELETE FROM {tripal_analysis} WHERE modulename = :modname";
      db_query($sql, array(':modname' => $modulename));
  }
}

/**
 * Get the analysis node same as node_load would but allowing different arguements
 *
 * @param $itentifier
 *   an array with the key stating what the identifier is. Supported keys (only on of the
 *   following unique keys is required):
 *    - analysis_id: the chado analysis.analysis_id primary key
 *    - nid: the drupal node.nid primary key
 * @return
 *   the analysis node matching the passed in identifier
 */
function chado_get_analysis($identifier) {

  // If the analysis_id is passed in then use it to get the nid
  if (isset($identifier['analysis_id'])) {
    $identifier['nid'] = chado_get_nid_from_id('analysis', $identifier['analysis_id']);
  }

  // Using the nid, get the node
  if (isset($identifier['nid'])) {
    return node_load($identifier['nid']);
  }

  // If there is neither the nid or analysis_id then return FALSE to indicate we failed
  return FALSE;
}