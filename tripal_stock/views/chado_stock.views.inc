<?php

/**
 * Purpose: this function returns the portion of the data array which describes the chado_stock 
 * drupal table, it's fields and any joins between it and other tables
 *
 * The main need for description of this table to views is to join chado data with drupal nodes
 *
 * @see tripal_stock_views_data() --in tripal_stock.views.inc
 *
 * @ingroup tripal_stock
 */
function retrieve_chado_stock_views_data () {
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
  $data['chado_stock']['table'] = array(
    'field' => 'nid',
  );
  
  // Note: No joins need to be made from $data['stock']['table']
  
  // Join the chado stock table to stock
  $data['chado_stock']['table']['join']['stock'] = array(
  	'left_field' => 'stock_id',
  	'field' => 'stock_id',
  );
  
  // Join the node table to chado stock
  $data['node']['table']['join']['chado_stock'] = array(
  	'left_field' => 'nid',
  	'field' => 'nid',
  );
  
  // Join the node table to stock
  $data['node']['table']['join']['stock'] = array(
  	'left_table' => 'chado_stock',
  	'left_field' => 'nid',
  	'field' => 'nid',
  );  

	return $data;
}