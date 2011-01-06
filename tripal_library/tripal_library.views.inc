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
 * @return: a data array which follows the structure outlined in the
 *   views2 documentation for this hook. Essentially, it's an array of table
 *   definitions keyed by chado/tripal table name. Each table definition 
 *   includes basic details about the table, fields in that table and
 *   relationships between that table and others (joins)
 */
require_once('views/library.views.inc');
function tripal_library_views_data()  {
  $data = array();
  
  $data = array_merge($data, retrieve_library_views_data());
  
  return $data;
}

/*************************************************************************
 * Implements hook_views_handlers()
 * Purpose: Register all custom handlers with views
 *   where a handler describes either "the type of field", 
 *   "how a field should be filtered", "how a field should be sorted"
 * @return: An array of handler definitions
 */
function tripal_library_views_handlers() {
 return array(
   'info' => array(
     'path' => drupal_get_path('module', 'tripal_library') . '/views/handlers',
   ),
   'handlers' => array(
     'views_handler_field_library_nid' => array(
       'parent' => 'views_handler_field_numeric',
     ),
     'views_handler_field_tf_boolean' => array(
       'parent' => 'views_handler_field',
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
 */
function tripal_library_views_pre_render	(&$view) {
	if (preg_match('/library/', $view->base_table)) {
		
		// retrieve the library_id for each record in the views current page
		$library_ids = array();
		foreach ($view->result as $row_num => $row) {
			$library_ids[$row_num] = $row->library_id;
		}

		// Using the list of library_ids from the view
		// lookup the NIDs from drupal
		// and add that to the results of the view
		$sql = "SELECT nid, library_id FROM chado_library WHERE library_id IN (".implode(',',$library_ids).")";
		$resource = db_query($sql);
		while ($r = db_fetch_object($resource)) {
			$key = array_search($r->library_id, $library_ids);
			$view->result[$key]->nid = $r->nid;
		}
	}
}
