<?php

/**
 *  @file
 *  This file contains the basic functions for views integration of
 *  chado/tripal_natural_diversity tables. Supplementary functions can be found in
 *  ./views/
 *
 *  Documentation on views integration can be found at 
 *  http://views2.logrus.com/doc/html/index.html.
 */
 
/*************************************************************************
 * Implements hook_views_data()
 * Purpose: Describe chado/tripal tables & fields to views
 *
 * @return: a data array which follows the structure outlined in the
 *   views2 documentation for this hook. Essentially, it's an array of table
 *   definitions keyed by chado/tripal table name. Each table definition 
 *   includes basic details about the table, fields in that table and
 *   relationships between that table and others (joins)
 */
require_once('views/nd_experiment.views.inc');
require_once('views/nd_experimentprop.views.inc');
require_once('views/nd_geolocation.views.inc');
require_once('views/nd_geolocationprop.views.inc');
require_once('views/nd_reagent.views.inc');
require_once('views/nd_reagentprop.views.inc');
function tripal_natural_diversity_views_data()  {
  $data = array();
  
  //EXPERIMENT
  $data = array_merge($data, retrieve_nd_experiment_views_data());
  $data = array_merge($data, retrieve_nd_experimentprop_views_data());
  
  // GEOLOCATION
  $data = array_merge($data, retrieve_nd_geolocation_views_data());
  $data = array_merge($data, retrieve_nd_geolocationprop_views_data());
  
  // REAGENTS
  $data = array_merge($data, retrieve_nd_reagent_views_data());
  $data = array_merge($data, retrieve_nd_reagentprop_views_data());
  
  return $data;
}

/*************************************************************************
 * Implements hook_views_handlers()
 * Purpose: Register all custom handlers with views
 *   where a handler describes either "the type of field", 
 *   "how a field should be filtered", "how a field should be sorted"
 *
 * @return: An array of handler definitions
 */
function tripal_natural_diversity_views_handlers() {
 return array(
   'info' => array(
     'path' => drupal_get_path('module', 'tripal_natural_diversity') . '/views/handlers',
   ),
   'handlers' => array(
		'views_handler_field_chado_relationship_all' => array(
       'parent' => 'views_handler_field_prerender_list',
     ),
		'views_handler_field_chado_relationship_by_type' => array(
       'parent' => 'views_handler_field_prerender_list',
     ),
   ),
 );
}