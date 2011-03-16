<?php
 
/**
 *  @file
 *  This file defines the data array for a given chado table. This array
 *  is merged into a larger array containing definitions of all tables associated
 *  with this module in:
 *  @see tripal_natural_diversity.views.inc --in tripal_natural_diversity_views_data()
 *
 *  Documentation on views integration can be found at 
 *  http://views2.logrus.com/doc/html/index.html.
 */

/*************************************************************************
 * Purpose: this function returns the portion of the data array 
 *   which describes the nd_reagent table, it's fields and any joins between it and other tables
 * @see tripal_natural_diversity_views_data() --in tripal_natural_diversity.views.inc
 *
 * Table: nd_reagent
 * @code
  CREATE TABLE nd_reagent (
    nd_reagent_id serial PRIMARY KEY NOT NULL,
    name character varying(80) NOT NULL,
    type_id integer NOT NULL references cvterm (cvterm_id) on delete cascade INITIALLY DEFERRED,
    feature_id integer
  );
 * @endcode
 */
 function retrieve_nd_reagent_views_data() {
  global $db_url;
  $data = array();

  // if the chado database is not local to the drupal database
  // then we need to set the database name.  This should always
  // be 'chado'.
  if(is_array($db_url) and array_key_exists('chado',$db_url)){
    $database = 'chado';
  }
   
  //Basic table definition-----------------------------------
  $data['nd_reagent']['table']['group'] = t('Chado ND Reagent');
  
  $data['nd_reagent']['table']['base'] = array(
    'field' => 'nd_reagent_id',
    'title' => t('Chado Natural Diversity Reagent'),
    'help' => t('Reagents used in Natural Diversity Experiments.'),
  );
  if($database){
     $data['nd_reagent']['table']['base']['database'] = $database;
  }
   
  //Table Field Definitions----------------------------------
      
  //Field: nd_reagent_id (primary key)
  $data['nd_reagent']['nd_reagent_id'] = array(
    'title' => t('ND Reagent Primary Key'),
    'help' => t('A unique index for every ND Reagent.'),
    'field' => array(
      'handler' => 'views_handler_field_numeric',
      'click sortable' => TRUE,
     ),
    'sort' => array(
      'handler' => 'views_handler_sort',
    ),
    'filter' => array(
      'handler' => 'views_handler_filter_numeric',
    ),
  );

  //Field: name (varchar 255)   
  $data['nd_reagent']['name'] = array(
    'title' => t('Name'),
    'help' => t('The name of the reagent.'),
    'field' => array(
      'handler' => 'views_handler_field',
      'click sortable' => TRUE,
     ),
    'sort' => array(
      'handler' => 'views_handler_sort',
    ),
    'filter' => array(
      'handler' => 'views_handler_filter_string',
    ),
    'argument' => array(
      'handler' => 'views_handler_argument_string',
    ),
  );

  //Field: type_id (foreign key)
  // Join described in cvterm.views.inc
  
  //Field: feature_id (foreign key)
  // Join described in feature.views.inc
 
 	//Calculated Field: stock relationships (ALL)
	// uses a custom field handler which pulls results from the view
	// actual query performed in chado_stock_views_views_pre_render	(&$view) -file:tripal_stock.views.inc
	$data['nd_reagent']['all_relationships'] = array(
  	'title' => t('All Reagent Relationships'),
  	'help' => t('Relationships including the current reagent.'),
  	'field' => array(
    	'title' => t('All Relationships'),
    	'help' => t('Display all relationships including the current reagent.'),
    	'handler' => 'views_handler_field_chado_relationship_all',
  	),
	);

	//Calculated Field: stock relationships
	// uses a custom field handler which pulls results from the view
	// actual query performed in chado_stock_views_views_pre_render	(&$view) -file:tripal_stock.views.inc
	$data['nd_reagent']['relationships'] = array(
    'title' => t('ND Reagent Relationships'),
    'help' => t('Relationships including the current reagent.'),
    'field' => array(
      'title' => t('Relationships'),
      'help' => t('Display a given type of relationships including the current reagent.'),
      'handler' => 'views_handler_field_chado_relationship_by_type',
    ),
  );
  return $data;
}