<?php

/**
 * @defgroup views Views Integration
 * @{
 * Provide rules for formatting and composition of fields
 * @}
 */
 
/**
 * @defgroup views_field_handlers Views Field Handlers
 * @{
 * Provide rules for formatting and composition of fields
 * @}
 * @ingroup views
 */

/**
 * @defgroup views_filter_handlers Views Filter Handlers
 * @{
 * Provide the ability to filter based on specified data
 * @}
 * @ingroup views
 */

/**
 * @defgroup views_sort_handlers Views Sort Handlers
 * @{
 * Provide methods describing how specific data should be sorted
 * @}
 * @ingroup views
 */

/**
 * @defgroup views_argument_handlers Views Arguement Handlers
 * @{
 * Provide the ability to filter pased on arguments in the path of the view
 * @}
 * @ingroup views
 */
 
/**
 * Implements hook_views_handlers()
 *
 * Purpose: Register all custom handlers with views
 *   where a handler describes either "the type of field", 
 *   "how a field should be filtered", "how a field should be sorted"
 *
 * @return
 *   An array of handler definitions
 *
 * @ingroup tripal_core
 */
function tripal_core_views_handlers() {
 return array(
   'info' => array(
     'path' => drupal_get_path('module', 'tripal_core') . '/views/handlers',
   ),
   'handlers' => array(
     'views_handler_field_node_optional' => array(
       'parent' => 'views_handler_field_node',
     ),
     'views_handler_field_cvterm_name' => array(
       'parent' => 'views_handler_field',
     ),
     'views_handler_field_chado_tf_boolean' => array(
      'parent' => 'views_handler_field_boolean',
     ),
     'views_handler_field_chado_count' => array(
      'parent' => 'views_handler_field',
     ),
     'views_handler_filter_chado_select_string' => array(
      'parent' => 'views_handler_filter_string',
     ),
     'views_handler_filter_chado_select_cvterm_name' => array(
      'parent' => 'views_handler_filter_string',
     ),
     'views_handler_filter_chado_boolean' => array(
      'parent' => 'views_handler_filter_boolean_operator',
     ),
     'views_handler_field_chado_rel_by_type' => array(
      'parent' => 'views_handler_field_prerender_list',
     ),
   ),
 );
}
    
/**
 * Implements hook_views_pre_render
 *
 * Purpose: Intercepts the view after the query has been executed
 *   All the results are stored in $view->result
 *   Looking up the NID here ensures the query is only executed once
 *   for all stocks in the table.
 *
 * @todo add if !<chado/drupal same db> around NID portion
 *
 * @ingroup tripal_core
 */
function tripal_core_views_pre_render	(&$view) {
		
		//Add Node IDs in to every table that needs them
		// @see file: tripal_core.views.inc
		tripal_core_add_node_ids_to_view ($view);
		
}

/**
 * Adds nid fields to all pertinent view results
 *
 * Purpose: To add basetable_nid fields to all result arrays of a view
 *   only if the basetable_nid field is added. This function will only be
 *   called if chado/drupal are not in the same database (ie: only if 
 *   a join between the base and node table isn't possible.
 *
 * Note: Supports adding Node IDs to analysis, feature, library, organism, stock
 *
 * @param $view
 *   the view object passed to hook_views_pre_render
 *
 * @return the views object with nids added to the result array
 *
 * @ingroup tripal_core
 */
function tripal_core_add_node_ids_to_view (&$view) {
	
	//-----Analysis----------------------------------------------
	if (!empty($view->field['analysis_nid'])) {
		// retrieve the analysis_id for each record in the views current page
		$analysis_ids = array();
		foreach ($view->result as $row_num => $row) {
			if (!empty($row->analysis_id)) {
				//we're looking at analysis id field already in table
				$analysis_ids[$row_num] = $row->analysis_id;
			} else {
				//we're looking at analysis id added by field handler
				$analysis_ids[$row_num] = $row->analysis_analysis_id;
			}
		}
		$unique_analysis_ids = array_filter($analysis_ids);
		$unique_analysis_ids = array_unique($unique_analysis_ids);

		if (!empty($unique_analysis_ids)) {
			// Using the list of analysis_ids from the view
			// lookup the NIDs from drupal
			// and add that to the results of the view
			$sql = "SELECT nid, analysis_id FROM {chado_analysis} WHERE analysis_id IN (".implode(',',$unique_analysis_ids).")";
			$resource = db_query($sql);
			while ($r = db_fetch_object($resource)) {
				$keys = array_keys($analysis_ids, $r->analysis_id);
				foreach ($keys as $k) {
					$view->result[$k]->analysis_nid = $r->nid;
				}
			}
		} // if there are any analysis'
	} //end of case for analysis NID
	
	//-----Feature-----------------------------------------------
	if (!empty($view->field['feature_nid'])) {
		// retrieve the feature_id for each record in the views current page
		$feature_ids = array();
		foreach ($view->result as $row_num => $row) {
			if (!empty($row->feature_id)) {
				//we're looking at feature id field already in table
				$feature_ids[$row_num] = $row->feature_id;
			} else {
				//we're looking at feature id added by field handler
				$feature_ids[$row_num] = $row->feature_feature_id;
			}
		}
		$unique_feature_ids = array_filter($feature_ids);
		$unique_feature_ids = array_unique($unique_feature_ids);

		if (!empty($unique_feature_ids)) {
			// Using the list of feature_ids from the view
			// lookup the NIDs from drupal
			// and add that to the results of the view
			$sql = "SELECT nid, feature_id FROM {chado_feature} WHERE feature_id IN (".implode(',',$unique_feature_ids).")";
			$resource = db_query($sql);
			while ($r = db_fetch_object($resource)) {
				$keys = array_keys($feature_ids, $r->feature_id);
				foreach ($keys as $k) {
					$view->result[$k]->feature_nid = $r->nid;
				}
			}
		} // if there are any features
	} //end of case for feature NID
	
	//-----Library-----------------------------------------------
	if (!empty($view->field['library_nid'])) {
		// retrieve the library_id for each record in the views current page
		$library_ids = array();
		foreach ($view->result as $row_num => $row) {
			if (!empty($row->library_id)) {
				//we're looking at library id field already in table
				$library_ids[$row_num] = $row->library_id;
			} else {
				//we're looking at library id added by field handler
				$library_ids[$row_num] = $row->library_library_id;
			}
		}
		$unique_library_ids = array_filter($library_ids);
		$unique_library_ids = array_unique($unique_library_ids);

		if (!empty($unique_library_ids)) {
			// Using the list of library_ids from the view
			// lookup the NIDs from drupal
			// and add that to the results of the view
			$sql = "SELECT nid, library_id FROM {chado_library} WHERE library_id IN (".implode(',',$unique_library_ids).")";
			$resource = db_query($sql);
			while ($r = db_fetch_object($resource)) {
				$keys = array_keys($library_ids, $r->library_id);
				foreach ($keys as $k) {
					$view->result[$k]->library_nid = $r->nid;
				}
			}
		} // if there are libraries
	} //end of case for library NID
	
	//-----Organism----------------------------------------------
	if (!empty($view->field['organism_nid'])) {
		// retrieve the organism_id for each record in the views current page
		$organism_ids = array();
		foreach ($view->result as $row_num => $row) {
			if (!empty($row->organism_id)) {
				//we're looking at organism id field already in table
				$organism_ids[$row_num] = $row->organism_id;
			} else {
				//we're looking at organism id added by field handler
				$organism_ids[$row_num] = $row->organism_organism_id;
			}
		}
		$unique_organism_ids = array_filter($organism_ids);
		$unique_organism_ids = array_unique($unique_organism_ids);

		if (!empty($unique_organism_ids)) {
			// Using the list of organism_ids from the view
			// lookup the NIDs from drupal
			// and add that to the results of the view
			$sql = "SELECT nid, organism_id FROM {chado_organism} WHERE organism_id IN (".implode(',',$unique_organism_ids).")";
			$resource = db_query($sql);
			while ($r = db_fetch_object($resource)) {
				$keys = array_keys($organism_ids, $r->organism_id);
				foreach ($keys as $k) {
					$view->result[$k]->organism_nid = $r->nid;
				}
			}
		} // if there are organisms
	} //end of case for organism NID
	
	//-----Stock-------------------------------------------------
	if (!empty($view->field['stock_nid'])) {
		// retrieve the stock_id for each record in the views current page
		$stock_ids = array();
		foreach ($view->result as $row_num => $row) {
			if (!empty($row->stock_id)) {
				//we're looking at stock id field already in table
				$stock_ids[$row_num] = $row->stock_id;
			} else {
				//we're looking at stock id added by field handler
				$stock_ids[$row_num] = $row->stock_stock_id;
			}
		}
		$unique_stock_ids = array_filter($stock_ids);
		$unique_stock_ids = array_unique($unique_stock_ids);

		if (!empty($unique_stock_ids)) {
			// Using the list of stock_ids from the view
			// lookup the NIDs from drupal
			// and add that to the results of the view
			$sql = "SELECT nid, stock_id FROM {chado_stock} WHERE stock_id IN (".implode(',',$unique_stock_ids).")";
			$resource = db_query($sql);
			while ($r = db_fetch_object($resource)) {
				$keys = array_keys($stock_ids, $r->stock_id);
				foreach ($keys as $k) {
					$view->result[$k]->stock_nid = $r->nid;
				}
			}
		} //if there are stocks
	} //end of case for stock NID
		
	return $view;
}