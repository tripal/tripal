<?php

/**
 * Purpose: this function returns the portion of the data array 
 *   which describes the chado_feature drupal table, it's fields and any joins between it and other tables
 * @see tripal_feature_views_data() --in tripal_feature.views.inc
 *
 * The main need for description of this table to views is to join chado data with drupal nodes
 *
 * @ingroup tripal_feature_views
 */
function retrieve_chado_feature_views_data () {
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
  $data['chado_feature']['table'] = array(
    'field' => 'nid',
  );
  
  // Note: No joins need to be made from $data['feature']['table']
  
  // Join the chado feature table to feature
  $data['chado_feature']['table']['join']['feature'] = array(
  	'left_field' => 'feature_id',
  	'field' => 'feature_id',
  );
  
  // Join the node table to chado feature
  $data['node']['table']['join']['chado_feature'] = array(
  	'left_field' => 'nid',
  	'field' => 'nid',
  );
  
  // Join the node table to feature
  $data['node']['table']['join']['feature'] = array(
  	'left_table' => 'chado_feature',
  	'left_field' => 'nid',
  	'field' => 'nid',
  );
  
  // Add relationship between chado_feature and feature
  $data['chado_feature']['feature_nid'] = array(
    'group' => 'Feature',
    'title' => 'Feature Node',
    'help' => 'Links Chado Feature Fields/Data to the Nodes in the current View.',
    'real field' => 'feature_id',
    'relationship' => array(
      'handler' => 'views_handler_relationship',
      'title' => t('Chado => Feature'),
      'label' => t('Chado => Feature'),
      'real field' => 'feature_id',
      'base' => 'feature',
      'base field' => 'feature_id'
    ),
  );

  // Add node relationship to feature
  $data['chado_feature']['feature_chado_nid'] = array(
    'group' => 'Feature',
    'title' => 'Feature Node',
    'help' => 'Links Chado Feature Fields/Data to the Nodes in the current View.',
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
