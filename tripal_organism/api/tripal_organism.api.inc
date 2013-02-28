<?php

/**
 * @file
 * Provides an API to tripal organisms
 */

/**
 * Purpose: Create an options array to be used in a form element
 *   which provides a list of all chado organisms
 *
 * @return an array(organism_id => common_name)
 *   for each organism in the chado organism table
 *
 * @ingroup tripal_organism_api
 */
function tripal_organism_get_organism_options() {

  $result = chado_query(
    "SELECT organism_id, common_name FROM {organism}"
  );

  $options = array();
  while ( $r = db_fetch_object($result) ) {
    $options[$r->organism_id] = $r->common_name;
  }

  return $options;

}

/**
 * Purpose: Return a given organism object using the nid
 *
 * @return organism object created by node load
 *
 * @ingroup tripal_organism_api
 */
function tripal_organism_get_organism_by_nid($nid) {

  return node_load($nid);

}

/**
 * Purpose: Return a given organism object using the organism id
 *
 * @return organism object created by node load
 *
 * @ingroup tripal_organism_api
 */
function tripal_organism_get_organism_by_organism_id($organism_id) {

  $sql = "SELECT nid FROM {chado_organism} WHERE organism_id=%d";
  $r = db_fetch_object(db_query($sql, $organism_id));
  if (!empty($r->nid)) {
    return node_load($r->nid);
  }
  else {
    drupal_set_message(t("Function: tripal_organism_get_organism_by_organism_id() -no organism with that organism id sync'd with drupal"), 'error');
  }

  return 0;

}

/**
 * @section Chado Table Descriptions
 * There should be a default table description for all chado tables included
 * in core.
 */

/**
 * Implements hook_chado_schema_v1_11_table()
 *
 * Purpose: To add descriptions and foreign keys to default table description
 * Note: This array will be merged with the array from all other implementations
 *
 * @return
 *    Array describing the organism table
 *
 * @ingroup tripal_schema_api
 */
function tripal_organism_chado_schema_v1_11_organism() {
  $description = array();

  $referring_tables = array(
    'biomaterial',
    'feature',
    'library',
    'organism_dbxref',
    'organismprop',
    'phylonode_organism',
    'stock',
    'wwwuser_organism'
  );
  $description['referring_tables'] = $referring_tables;

  return $description;
}

/**
 * Implements hook_chado_schema_v1_2_table()
 *
 * Purpose: To add descriptions and foreign keys to default table description
 * Note: This array will be merged with the array from all other implementations
 *
 * @return
 *    Array describing the organism table
 *
 * @ingroup tripal_schema_api
 */
function tripal_organism_chado_schema_v1_2_organism() {
  $description = array();

  $referring_tables = array(
    'biomaterial',
    'feature',
    'library',
    'organism_dbxref',
    'organismprop',
    'phylonode_organism',
    'stock',
    'wwwuser_organism'
  );
  $description['referring_tables'] = $referring_tables;

  return $description;
}

/**
 *  Returns a list of organisms that are currently synced with Drupal
 * @ingroup tripal_organism_api
 */
function tripal_organism_get_synced() {

  // use this SQL for getting synced organisms
  $dsql =  "SELECT * FROM {chado_organism}";
  $orgs = db_query($dsql);

  // use this SQL statement for getting the organisms
  $csql =  "SELECT * FROM {Organism} ".
          "WHERE organism_id = %d";

  $org_list = array();

  // iterate through the organisms and build an array of those that are synced
  while ($org = db_fetch_object($orgs)) {
    $info = db_fetch_object(chado_query($csql, $org->organism_id));
    $org_list[] = $info;
  }
  return $org_list;
}
