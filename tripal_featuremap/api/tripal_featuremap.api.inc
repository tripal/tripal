<?php

/**
 * @defgroup tripal_featuremap_api Feature Map API
 * @ingroup tripal_api
 * @{
 * Provides an application programming interface (API) to manage chado feature maps
 * @}
 */

/**
 * Retrieve properties of a given type for a given featuremap
 *
 * @param $featuremap_id
 *    The featuremap_id of the properties you would like to retrieve
 * @param $property
 *    The cvterm name of the properties to retrieve
 *
 * @return
 *    An featuremap chado variable with the specified properties expanded
 *
 * @ingroup tripal_featuremap_api
 */
function tripal_featuremap_get_property($featuremap_id, $property) {
  return chado_get_property('featuremap', $featuremap_id, $property, 'featuremap_property');
}

/**
 * Insert a given property
 *
 * @param $featuremap_id
 *   The featuremap_id of the property to insert
 * @param $property
 *   The cvterm name of the property to insert
 * @param $value
 *   The value of the property to insert
 * @param $update_if_present
 *   A boolean indicated whether to update the record if it's already present
 *
 * @return
 *   True of success, False otherwise
 *
 * @ingroup tripal_featuremap_api
 */
function tripal_featuremap_insert_property($featuremap_id, $property, $value, $update_if_present = 0) {
  return chado_insert_property('featuremap', $featuremap_id, $property, 'featuremap_property', $value, $update_if_present);
}

/**
 * Update a given property
 *
 * @param $featuremap_id
 *   The featuremap_id of the property to update
 * @param $property
 *   The cvterm name of the property to update
 * @param $value
 *   The value of the property to update
 * @param $insert_if_missing
 *   A boolean indicated whether to insert the record if it's absent
 *
 * Note: The property will be identified using the unique combination of the $featuremap_id and $property
 * and then it will be updated with the supplied value
 *
 * @return
 *   True of success, False otherwise
 *
 * @ingroup tripal_featuremap_api
 */
function tripal_featuremap_update_property($featuremap_id, $property, $value, $insert_if_missing = 0) {
  return chado_update_property('featuremap', $featuremap_id, $property, 'featuremap_property', $value, $insert_if_missing);
}
/**
 * Delete a given property
 *
 * @param $featuremap_id
 *   The featuremap_id of the property to delete
 * @param $property
 *   The cvterm name of the property to delete
 *
 * Note: The property will be identified using the unique combination of the $featuremap_id and $property
 * and then it will be deleted
 *
 * @return
 *   True of success, False otherwise
 *
 * @ingroup tripal_featuremap_api
 */
function tripal_featuremap_delete_property($featuremap_id, $property) {
  return chado_delete_property('featuremap', $featuremap_id, $property, 'featuremap_property');
}

/*
 *
 */
function tripal_featuremap_add_featuremap_dbxref($featuremap_id, $featuremap_dbxref) {

  // break apart the dbxref
  $dbname = '';
  $accession = '';
  if(preg_match('/^(.*?):(.*?)$/', $featuremap_dbxref, $matches)) {
    $dbname = $matches[1];
    $accession = $matches[2];
  }
  else {
    return FALSE;
  }

  // check to see if the featuremap_dbxref record already exist
  $values = array(
    'dbxref_id' => array(
      'accession' => $accession,
      'db_id' => array(
        'name' => $dbname,
      ),
    ),
    'featuremap_id' => $featuremap_id,
  );
  $options = array('statement_name' => 'sel_featuremapdbxref_dbpu');
  $results = chado_select_record('featuremap_dbxref', array('*'), $values, $options);

  // if the featuremap_dbxref record  exist then we don't need to re-add it.
  if(count($results) > 0) {
    return $results[0];
  }

  // make sure our database already exists
  $db = tripal_db_add_db($dbname);

  // get the database cross-reference
  $dbxvalues = array(
    'accession' => $accession,
    'db_id' => $db->db_id,
  );
  $dbxoptions = array('statement_name' => 'sel_dbxref_acdb');
  $results = chado_select_record('dbxref', array('dbxref_id'), $dbxvalues, $dbxoptions);
  // if the accession doesn't exist then add it
  if(count($results) == 0){
    $dbxref = tripal_db_add_dbxref($db->db_id, $accession);
  }
  else {
    $dbxref = $results[0];
  }

  // now add the record
  $options = array('statement_name' => 'ins_featuremapdbxref_dbpu');
  $results = chado_insert_record('featuremap_dbxref', $values, $options);
  if (!$results) {
    tripal_report_error('t_featuremap', TRIPAL_ERROR, "Cannot add map dbxref: %db:%accession.",
      array('%db' => $dbname, '%accession' => $accession));
    return FALSE;
  }
  return $results;
}