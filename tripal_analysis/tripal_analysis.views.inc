<?php

/**
 *  @file
 *  This file contains the basic functions for views integration of
 *  chado/tripal analysis tables. Supplementary functions can be found in
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
require('views/analysis.views.inc');
function tripal_analysis_views_data()  {
  $data = array();

  $data = array_merge($data, retrieve_analysis_views_data());

  return $data;
}

/*************************************************************************
 * Implements hook_views_handlers()
 * Purpose: Register all custom handlers with views
 *   where a handler describes either "the type of field", 
 *   "how a field should be filtered", "how a field should be sorted"
 * @return: An array of handler definitions
 */
function chado_analysis_views_views_handlers() {
  return array(
    'info' => array(
      'path' => drupal_get_path('module', 'tripal_analysis') . '/views/handlers',
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
 *   for all analysis' in the table.
 *
 * @todo add if !<chado/drupal same db> around NID portion
 */
function tripal_analysis_views_pre_render	(&$view) {
	if (preg_match('/analysis/', $view->base_table)) {
		
		// retrieve the analysis_id for each record in the views current page
		$analysis_ids = array();
		foreach ($view->result as $row_num => $row) {
			$analysis_ids[$row_num] = $row->analysis_id;
		}

		// Using the list of analysis_ids from the view
		// lookup the NIDs from drupal
		// and add that to the results of the view
		$sql = "SELECT nid, analysis_id FROM chado_analysis WHERE analysis_id IN (".implode(',',$analysis_ids).")";
		$resource = db_query($sql);
		while ($r = db_fetch_object($resource)) {
			$key = array_search($r->analysis_id, $analysis_ids);
			$view->result[$key]->nid = $r->nid;
		}
	}
}
