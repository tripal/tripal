<?php

//module_load_include('inc','views', 'handlers/views_handler_relationship');

/**
 * Purpose: this function returns the portion of the data array which describes the chado_stock 
 * drupal table, it's fields and any joins between it and other tables
 *
 * The main need for description of this table to views is to join chado data with drupal nodes
 *
 * @see tripal_stock_views_data() --in tripal_stock.views.inc
 *
 * @ingroup tripal_stock_views
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
    'field' => 'stock_id',
    'group' => 'Chado Stock Node',
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
  
  // Add relationship between chado_stock and stock
  $data['chado_stock']['stock_nid'] = array(
    'group' => 'Stock',
    'title' => 'Stock Node',
    'help' => 'Links Chado Stock Fields/Data to the Nodes in the current View.',
    'real field' => 'stock_id',
    'relationship' => array(
      'handler' => 'views_handler_relationship',
      'title' => t('Chado => Stock'),
      'label' => t('Chado => Stock'),
      'real field' => 'stock_id',
      'base' => 'stock',
      'base field' => 'stock_id'
    ),
  );

  // Add node relationship to stock
  $data['chado_stock']['stock_chado_nid'] = array(
    'group' => 'Stock',
    'title' => 'Stock Node',
    'help' => 'Links Chado Stock Fields/Data to the Nodes in the current View.',
    'real field' => 'nid',
    'relationship' => array(
      'handler' => 'views_handler_relationship',
      'title' => t('Chado => Node'),
      'label' => t('Chado => Node'),
      'real field' => 'nid',
      'base' => 'node',
      'base field' => 'nid'
    ),
  );
  
	return $data;
}