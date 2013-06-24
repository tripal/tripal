<?php

/**
 * @file
 * API functions relating to Analysis'
 *
 * @defgroup tripal_analysis_api Analysis Module API
 * @ingroup tripal_api
 */
/**
 * Register tripal_analysis sub-modules
 *
 * @param $modulename
 *  The name of the module to be registered as a tripal analysis submodule
 *
 * @ingroup tripal_analysis
 */
function tripal_analysis_register_child($modulename) {
  $sql = "SELECT * FROM {tripal_analysis} WHERE modulename = '%s'";
  if(!db_result($sql, $modulename)) {
    $sql = "INSERT INTO {tripal_analysis} (modulename) VALUES ('%s')";
    db_query($sql, $modulename);
  }
}

/**
 * Un-register a tripal analysis sub-module
 *
 * @param $modulename
 *  The name of the module to un-register
 *
 * @ingroup tripal_analysis
 */
function tripal_analysis_unregister_child($modulename) {
  if (db_table_exists('tripal_analysis')) {
      $sql = "DELETE FROM {tripal_analysis} WHERE modulename = '%s'";
      db_query($sql, $modulename);
  }
}
/**
 * Retrieve properties of a given type for a given analysis
 *
 * @param $analysis_id
 *    The analysis_id of the properties you would like to retrieve
 * @param $property
 *    The cvterm name of the properties to retrieve
 * @param $cvname
 *   The name of the vocabulary to which the term belongs. Defaults to 'tripal'.
 *
 * @return
 *    An analysis chado variable with the specified properties expanded
 *
 * @ingroup tripal_analysis_api
 */
function tripal_analysis_get_property($analysis_id, $property, $cvname = 'tripal') {
  return tripal_core_get_property('analysis', $analysis_id, $property, $cvname);
}

/**
 * Insert a given property
 *
 * @param $analysis_id
 *   The analysis_id of the property to insert
 * @param $property
 *   The cvterm name of the property to insert
 * @param $value
 *   The value of the property to insert
 * @param $update_if_present
 *   A boolean indicated whether to update the record if it's already present
 * @param $cvname
 *   The name of the vocabulary to which the term belongs. Defaults to 'tripal'.
 *
 * @return
 *   True of success, False otherwise
 *
 * @ingroup tripal_analysis_api
 */
function tripal_analysis_insert_property($analysis_id, $property, $value, $update_if_present = 0, $cvname = 'tripal') {
    return tripal_core_insert_property('analysis', $analysis_id, $property, $cvname, $value, $update_if_present);
}

/**
 * Update a given property
 *
 * @param $analysis_id
 *   The analysis_id of the property to update
 * @param $property
 *   The cvterm name of the property to update
 * @param $value
 *   The value of the property to update
 * @param $insert_if_missing
 *   A boolean indicated whether to insert the record if it's absent
 * @param $cvname
 *   The name of the vocabulary to which the term belongs. Defaults to 'tripal'.
 *
 * Note: The property will be identified using the unique combination of the $analysis_id and $property
 * and then it will be updated with the supplied value
 *
 * @return
 *   True of success, False otherwise
 *
 * @ingroup tripal_analysis_api
 */
function tripal_analysis_update_property($analysis_id, $property, $value, $insert_if_missing = 0, $cvname = 'tripal') {
  return tripal_core_update_property('analysis', $analysis_id, $property, $cvname, $value, $insert_if_missing);
}

/**
 * Delete a given property
 *
 * @param $analysis_id
 *   The analysis_id of the property to delete
 * @param $property
 *   The cvterm name of the property to delete
 * @param $cvname
 *   The name of the vocabulary to which the term belongs. Defaults to 'tripal'.
 *
 * Note: The property will be identified using the unique combination of the $analysis_id and $property
 * and then it will be deleted
 *
 * @return
 *   True of success, False otherwise
 *
 * @ingroup tripal_analysis_api
 */
function tripal_analysis_delete_property($analysis_id, $property, $cvname = 'tripal') {
  return tripal_core_delete_property('analysis', $analysis_id, $property, $cvname);
}

/**
 * Retreives the node of a sync'ed analysis
 *
 * @param $analysis_id
 *   The analysis_id of the property to delete
 *
 * @return
 *   node of analysis on success, null otherwise
 *
 * @ingroup tripal_analysis_api
 */
function tripal_analysis_get_node($analysis_id) {
  $sql = "SELECT *
           FROM {chado_analysis} CA
              INNER JOIN {node} N on CA.nid = N.nid
           WHERE analysis_id = %d";
  $node = db_fetch_object(db_query($sql, $analysis_id));
  return $node;
}
