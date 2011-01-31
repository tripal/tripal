<?php

/**
 * Purpose: this function returns the portion of the data array 
 *   which describes the chado_organism drupal table, it's fields and any joins between it and other tables
 * @see tripal_organism_views_data() --in tripal_organism.views.inc
 *
 * The main need for description of this table to views is to join chado data with drupal nodes
 *
 */
function retrieve_chado_organism_views_data () {
	global $db_url;
  $data = array();
  
  // if the chado database is not local to the drupal database
  // then we need to set the database name.  This should always
  // be 'chado'.
  if(is_array($db_url) and array_key_exists('chado',$db_url)){
     // return empty data array b/c if chado is external then no join to the nodetable can be made
     return $data;
  }

  // Basic table definition
  $data['chado_organism']['table'] = array(
    'field' => 'nid',
  );
  
  // Note: No joins need to be made from $data['organism']['table']
  
  // Join the chado organism table to organism
  $data['chado_organism']['table']['join']['organism'] = array(
  	'left_field' => 'organism_id',
  	'field' => 'organism_id',
  );
  
  // Join the node table to chado organism
  $data['node']['table']['join']['chado_organism'] = array(
  	'left_field' => 'nid',
  	'field' => 'nid',
  );
  
  // Join the node table to organism
  $data['node']['table']['join']['organism'] = array(
  	'left_table' => 'chado_organism',
  	'left_field' => 'nid',
  	'field' => 'nid',
  );  

	return $data;
}
