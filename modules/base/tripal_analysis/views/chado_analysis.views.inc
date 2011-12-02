<?php

/**
 * Purpose: this function returns the portion of the data array 
 *   which describes the chado_analysis drupal table, it's fields and any joins between it and other tables
 * @see tripal_analysis_views_data() --in tripal_analysis.views.inc
 *
 * The main need for description of this table to views is to join chado data with drupal nodes
 *
 *
 * @ingroup tripal_analysis_views
 */
function retrieve_chado_analysis_views_data () {
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
  $data['chado_analysis']['table'] = array(
    'field' => 'nid',
  );
  
  // Note: No joins need to be made from $data['analysis']['table']
  
  // Join the chado analysis table to analysis
  $data['chado_analysis']['table']['join']['analysis'] = array(
  	'left_field' => 'analysis_id',
  	'field' => 'analysis_id',
  );
  
  // Join the node table to chado analysis
  $data['node']['table']['join']['chado_analysis'] = array(
  	'left_field' => 'nid',
  	'field' => 'nid',
  );
  
  // Join the node table to analysis
  $data['node']['table']['join']['analysis'] = array(
  	'left_table' => 'chado_analysis',
  	'left_field' => 'nid',
  	'field' => 'nid',
  );  

  // Add relationship between chado_analysis and analysis
  $data['chado_analysis']['analysis_nid'] = array(
    'group' => 'Analysis',
    'title' => 'Analysis Node',
    'help' => 'Links Chado Analysis Fields/Data to the Nodes in the current View.',
    'real field' => 'analysis_id',
    'relationship' => array(
      'handler' => 'views_handler_relationship',
      'title' => t('Chado => Analysis'),
      'label' => t('Chado => Analysis'),
      'real field' => 'analysis_id',
      'base' => 'analysis',
      'base field' => 'analysis_id'
    ),
  );

  // Add node relationship to analysis
  $data['chado_analysis']['analysis_chado_nid'] = array(
    'group' => 'Analysis',
    'title' => 'Analysis Node',
    'help' => 'Links Chado Analysis Fields/Data to the Nodes in the current View.',
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
