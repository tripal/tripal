<?php

/**
 * @defgroup tripal_organism_api Organism API
 * @ingroup tripal_api
 * @{
 * Provides an application programming interface (API) to manage organisms
 * @}
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

  $sql = "SELECT nid FROM {chado_organism} WHERE organism_id = :organism_id";
  $r = db_query($sql, array(':organism_id' => $organism_id))->fetchObject();
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
  $csql =  "SELECT * FROM {Organism} " .
           "WHERE organism_id = :organism_id";

  $org_list = array();

  // iterate through the organisms and build an array of those that are synced
  foreach ($orgs as $org) {
    $args = array(':organism_id' => $org->organism_id);
    $org = chado_query($csql, $args)->fetchObject();
    $org_list[] = $org;
  }
  return $org_list;
}

/**
 *
 * @param $organism
 * @param $nid
 */
function tripal_organism_get_image_url($organism, $nid = NULL) {
  $url = '';

  // first look for an image with the genus/species name.  This is old-style tripal
  // and we keep it for backwards compatibility.  If we don't find that file
  // then look for the image with the node ID in the name. If we don't find that then
  // no image tag is generated
  $base_path = realpath('.');
  $image_dir = tripal_get_files_dir('tripal_organism') . "/images";
  $image_name =  $organism->genus . "_" . $organism->species . ".jpg";

  if (file_exists("$base_path/$image_dir/$image_name")) {
    $url = file_create_url("$image_dir/$image_name");
  }
  else {
     $image_name = $nid . ".jpg";
     if (file_exists("$base_path/$image_dir/$image_name")) {
       $url = file_create_url("$image_dir/$image_name");
     }
  }
  return $url;
}