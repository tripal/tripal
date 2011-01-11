<?php

/**
 *  @file
 *  This file contains the basic functions for views integration of
 *  chado/tripal organism tables. Supplementary functions can be found in
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
require_once('views/feature.views.inc');
function tripal_feature_views_data()  {
  $data = array();
  
  $data = array_merge($data, retrieve_feature_views_data());
  
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
function tripal_feature_views_handlers() {
 return array(
   'info' => array(
     'path' => drupal_get_path('module', 'tripal_feature') . '/views/handlers',
   ),
   'handlers' => array(
     'views_handler_field_computed_nid' => array(
       'parent' => 'views_handler_field_numeric',
     ),
     'views_handler_field_readable_date' => array(
       'parent' => 'views_handler_field',
     ),
   ),
 );
}

/**
 * Implements hook_views_pre_render
 * Purpose: Intercepts the view after the query has been executed
 *   All the results are stored in $view->result
 *   Looking up the NID here ensures the query is only executed once
 *   for all features in the table.
 *
 * @todo add if !<chado/drupal same db> around NID portion
 */
function tripal_feature_views_pre_render	(&$view) {
	if (preg_match('/feature/', $view->base_table)) {
		
		// retrieve the feature_id for each record in the views current page
		$feature_ids = array();
		foreach ($view->result as $row_num => $row) {
			$feature_ids[$row_num] = $row->feature_id;
		}

		// Using the list of feature_ids from the view
		// lookup the NIDs from drupal
		// and add that to the results of the view
		$sql = "SELECT nid, feature_id FROM chado_feature WHERE feature_id IN (".implode(',',$feature_ids).")";
		$resource = db_query($sql);
		while ($r = db_fetch_object($resource)) {
			$key = array_search($r->feature_id, $feature_ids);
			$view->result[$key]->nid = $r->nid;
		}
	}
}