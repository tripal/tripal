<?php

/*************************************************************************
 * Purpose: Create an options array to be used in a form element
 *   which provides a list of all chado organisms
 *
 * @return an array(organism_id => common_name) 
 *   for each organism in the chado organism table
 */
function tripal_organism_get_organism_options() {

  $previous_db = tripal_db_set_active('chado');
  $result = db_query(
    "SELECT organism_id, common_name FROM organism"
  );
  tripal_db_set_active($previous_db);

  $options = array();
  while ( $r = db_fetch_object($result) ) {
    $options[$r->organism_id] = $r->common_name;
  }

  return $options;

}

/*************************************************************************
 * Purpose: Return a given organism object using the nid
 *
 * @return organism object created by node load
 */
function tripal_organism_get_organism_by_nid ($nid) {

	return node_load($nid);
	
}

/*************************************************************************
 * Purpose: Return a given organism object using the organism id
 *
 * @return organism object created by node load
 */
function tripal_organism_get_organism_by_organism_id ($organism_id) {

	$sql = "SELECT nid FROM {chado_organism} WHERE organism_id=%d";
	$r = db_fetch_object(db_query($sql, $organism_id));
	if (!empty($r->nid)) {
		return node_load($r->nid);
	} else {
		drupal_set_message("Function: tripal_organism_get_organism_by_organism_id() -no organism with that organism id sync'd with drupal", 'error');
	}
	
	return 0;
	
}

/****************************************************************************
 * @section Chado Table Descriptions
 * There should be a default table description for all chado tables included
 * in core.
 ****************************************************************************/

/**
 * Implements hook_chado_organism_schema()
 *
 * Purpose: To add descriptions and foreign keys to default table description
 * Note: This array will be merged with the array from all other implementations
 *
 * @return
 *    Array describing the organism table
 *
 * @ingroup tripal_schema_api
 */
function tripal_organism_chado_organism_schema() {
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
 * Implements hook_chado_organismprop_schema()
 *
 * Purpose: To add descriptions and foreign keys to default table description
 * Note: This array will be merged with the array from all other implementations
 *
 * @return
 *    Array describing the organismprop table
 *
 * @ingroup tripal_schema_api
 */
function tripal_organism_chado_organismprop_schema() {
  $description = array();
  
  $description['foreign keys']['cvterm'] = array(
        'table' => 'cvterm',
        'columns' => array(
          'type_id' => 'cvterm_id',
        ),
  );
  
  $description['foreign keys']['organism'] = array(
        'table' => 'organism',
        'columns' => array(
          'organism_id' => 'organism_id',
        ),
  );
  
  return $description;
}