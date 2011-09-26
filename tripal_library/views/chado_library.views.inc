<?php

/**
 * Purpose: this function returns the portion of the data array 
 *   which describes the chado_library drupal table, it's fields and any joins between it and other tables
 * @see tripal_library_views_data() --in tripal_library.views.inc
 *
 * The main need for description of this table to views is to join chado data with drupal nodes
 *
 * @ingroup tripal_library_views
 */
function retrieve_chado_library_views_data () {
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
  $data['chado_library']['table'] = array(
    'field' => 'nid',
  );
  
  // Note: No joins need to be made from $data['library']['table']
  
  // Join the chado library table to library
  $data['chado_library']['table']['join']['library'] = array(
  	'left_field' => 'library_id',
  	'field' => 'library_id',
  );
  
  // Join the node table to chado library
  $data['node']['table']['join']['chado_library'] = array(
  	'left_field' => 'nid',
  	'field' => 'nid',
  );
  
  // Join the node table to library
  $data['node']['table']['join']['library'] = array(
  	'left_table' => 'chado_library',
  	'left_field' => 'nid',
  	'field' => 'nid',
  );  

  // Add relationship between chado_library and library
  $data['chado_library']['library_nid'] = array(
    'group' => 'Library',
    'title' => 'Library Node',
    'help' => 'Links Chado Library Fields/Data to the Nodes in the current View.',
    'real field' => 'library_id',
    'relationship' => array(
      'handler' => 'views_handler_relationship',
      'title' => t('Chado => Library'),
      'label' => t('Chado => Library'),
      'real field' => 'library_id',
      'base' => 'library',
      'base field' => 'library_id'
    ),
  );

  // Add node relationship to library
  $data['chado_library']['library_chado_nid'] = array(
    'group' => 'Library',
    'title' => 'Library Node',
    'help' => 'Links Chado Library Fields/Data to the Nodes in the current View.',
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
