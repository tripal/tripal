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
//require_once('views/nd_experiment.views.inc');
//require_once('views/nd_experimentprop.views.inc');
//require_once('views/nd_geolocation.views.inc');
//require_once('views/nd_geolocationprop.views.inc');
//require_once('views/nd_reagent.views.inc');
//require_once('views/nd_reagentprop.views.inc');
function tripal_natural_diversity_views_data()  {
  $data = array();
  
  //EXPERIMENT
  //$data = array_merge($data, retrieve_nd_experiment_views_data());
  //$data = array_merge($data, retrieve_nd_experimentprop_views_data());
  
  // GEOLOCATION
  //$data = array_merge($data, retrieve_nd_geolocation_views_data());
  //$data = array_merge($data, retrieve_nd_geolocationprop_views_data());
  
  // REAGENTS
  //$data = array_merge($data, retrieve_nd_reagent_views_data());
  //$data = array_merge($data, retrieve_nd_reagentprop_views_data());

  if (module_exists('tripal_views')) {
    $tables = array(
      'nd_experiment',
      'nd_geolocation',
      'nd_protocol',
      'nd_reagent'
    );
    foreach ($tables as $tablename) {
      if (!tripal_views_is_integrated($tablename, 10)) {
        $table_integration_array = tripal_views_get_integration_array_for_chado_table($tablename,TRUE);
        tripal_views_integration_add_entry($table_integration_array);
      }
    }
        
    $tables = array(
      'nd_experiment_contact',
      'nd_experiment_dbxref',
      'nd_experiment_genotype',
      'nd_experiment_phenotype',
      'nd_experiment_project',
      'nd_experiment_protocol',
      'nd_experiment_pub',
      'nd_experiment_stock',
      'nd_experiment_stock_dbxref',
      'nd_experiment_stockprop',
      'nd_experimentprop',
      'nd_geolocationprop',
      'nd_protocol_reagent',
      'nd_protocolprop',
      'nd_reagent_relationship',
      'nd_reagentprop'
    );
    foreach ($tables as $tablename) {
      if (!tripal_views_is_integrated($tablename, 10)) {
        $table_integration_array = tripal_views_get_integration_array_for_chado_table($tablename,FALSE);
        tripal_views_integration_add_entry($table_integration_array);
      }
    }
  }
  
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