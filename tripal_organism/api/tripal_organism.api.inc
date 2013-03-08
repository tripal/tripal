<?php

/**
 * @file
 * Provides an application programming interface (API) to manage organisms
 *
 * @defgroup tripal_library_api Organism Module API
 * @ingroup tripal_api
 */

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
