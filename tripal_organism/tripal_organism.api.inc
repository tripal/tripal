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
 * Purpose: Return a given organism object using the nid or organism id
 *
 * @return organism object created by node load
 */
function tripal_organism_get_organism($nid=0, $organism_id=0) {

	if (!empty($nid)) {
		return node_load($nid);
	} else {
		if (!empty($organism_id)) {
			$sql = "SELECT nid FROM {chado_organism} WHERE organism_id=%d";
			$r = db_fetch_object(db_query($sql, $organism_id));
			if (!empty($r->nid)) {
				return node_load($r->nid);
			} else {
				drupal_set_message("Function: tripal_organism_get_organism() -no organism with that organism id sync'd with drupal", 'error');
			}
		} else {
			drupal_set_message("Function: tripal_organism_get_organism() -need to supply at least one of node ID or Organism ID",'error');
		}
	}

	return 0;
	
}
