<?php

include('views/handlers/views_handler_join_chado_through_linking.inc');
include('views/handlers/views_handler_join_chado_aggregator.inc');
include('tripal_views.api.inc');

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
 * @ingroup tripal_views
 */
function tripal_views_views_handlers() {

 return array(
   'info' => array(
     'path' => drupal_get_path('module', 'tripal_views') . '/views/handlers',
   ),
   'handlers' => array(
   
     // Custom Chado Handlers
     'chado_views_handler_field_aggregate' => array(
      'parent' => 'chado_views_handler_field',
     ),
     'views_handler_filter_chado_select_string' => array(
      'parent' => 'views_handler_filter_string',
     ),
     'views_handler_filter_chado_select_cvterm_name' => array(
      'parent' => 'views_handler_filter_string',
     ),
     
     // Join Handlers
     'views_handler_join_chado_aggregator' => array(
      'parent' => 'views_join',
     ),
     'views_handler_join_chado_through_linking' => array(
      'parent' => 'views_join',
     ),
     
     // Other Custom Handlers
     'views_handler_filter_no_results' => array(
      'parent' => 'views_handler_filter'
     ),
     
     // Old Handlers
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
     'views_handler_filter_chado_boolean' => array(
      'parent' => 'views_handler_filter_boolean_operator',
     ),
     'views_handler_field_chado_rel_by_type' => array(
      'parent' => 'views_handler_field_prerender_list',
     ),
    'views_handler_field_dbxref_accession_link' => array(
       'parent' => 'views_handler_field',
     ),
     'views_handler_field_readable_date' => array(
       'parent' => 'views_handler_field',
     ),

     
     // Wrappers for Default Views Handlers-----
     // Field Handlers
     'chado_views_handler_field' => array(
      'parent' => 'views_handler_field'
     ),
     'chado_views_handler_field_boolean' => array(
      'parent' => 'views_handler_field_boolean'
     ),
     'chado_views_handler_field_counter' => array(
      'parent' => 'views_handler_field_counter'
     ),
     'chado_views_handler_field_custom' => array(
      'parent' => 'views_handler_field_custom'
     ),
     'chado_views_handler_field_date' => array(
      'parent' => 'views_handler_field_date'
     ),
     'chado_views_handler_field_markup' => array(
      'parent' => 'views_handler_field_markup'
     ),
     'chado_views_handler_field_math' => array(
      'parent' => 'views_handler_field_math'
     ),
     'chado_views_handler_field_numeric' => array(
      'parent' => 'views_handler_field_numeric'
     ),
     // Filter Handlers
     'chado_views_handler_filter_string' => array(
      'parent' => 'views_handler_filter_string',
     ), 
     'chado_views_handler_filter_boolean_operator_string' => array(
      'parent' => 'views_handler_filter_boolean_operator_string',
     ),
     'chado_views_handler_filter_boolean_operator' => array(
      'parent' => 'views_handler_filter_boolean_operator',
     ),
     'chado_views_handler_filter_date' => array(
      'parent' => 'views_handler_filter_date',
     ),
     'chado_views_handler_filter_equality' => array(
      'parent' => 'views_handler_filter_equality',
     ),
     'chado_views_handler_filter_float' => array(
      'parent' => 'views_handler_filter_float',
     ),
     'chado_views_handler_filter_numeric' => array(
      'parent' => 'views_handler_filter_numeric',
     ),
     'views_handler_filter_file_upload' => array(
      'parent' => 'views_handler_filter',
     ),
     // Sort Handlers
     'chado_views_handler_sort' => array(
      'parent' => 'views_handler_sort'
     ),     
     'chado_views_handler_sort_date' => array(
      'parent' => 'views_handler_sort_date'
     ), 
     'chado_views_handler_sort_formula' => array(
      'parent' => 'views_handler_sort_formula'
     ), 
     'chado_views_handler_sort_menu_hierarchy' => array(
      'parent' => 'views_handler_sort_menu_hierarchy'
     ), 
     'chado_views_handler_sort_random' => array(
      'parent' => 'views_handler_sort_random'
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
 * @ingroup tripal_views
 */
function tripal_views_views_pre_render	(&$view) {

   //Add Node IDs in to every table that needs them
	// @see file: tripal_views.views.inc
	tripal_views_add_node_ids_to_view ($view);

   // We need to unset the exposed_input for the view so we can repopulate that 
   // variable. This is necessary if we're using the file_upload_combo 
   // custom form element which adds the file_path variable to the $_GET after the
   // view has populated the $view->exposed_input variable 
   unset($view->exposed_input);
   $query = $view->get_exposed_input();  // retrieves elements in $_GET array
		
   // we want to add to the bottom of the views the form for downloading 
   // results in other formats (e.g. Excel, FASTA, CSV, etc.).  The Views Data
   // Export module provides small images at the bottom, but we want to provide
   // a more intutitive interface for getting different file formats
   $form = drupal_get_form('tripal_views_data_export_download_form',$view,$display_id,$args);
   $view->attachment_after = $form;
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
 * @ingroup tripal_views
 */
function tripal_views_add_node_ids_to_view (&$view) {
	
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

/**
 * Generates a dynamic data array for Views
 *
 * Purpose: This function is a hook used by the Views module. It populates and
 *   returns a data array that specifies for the Views module the base table,
 *   the tables it joins with and handlers.  The data array is populated
 *   using the data stored in the tripal_views tables.
 *
 * @return a data array formatted for the Views module
 *
 * @ingroup tripal_views
 */

function tripal_views_views_data(){   
  
   // Define Global Fields 
   // Filter handler that lets the admin say: 
   // "Show no results until they enter search parameters"
   $data['views']['search_results'] = array(
      'title' => t('Search Results'),
      'help' => t('Delay results until Apply/Search is clicked by the user.'),
      'filter' => array(
         'handler' => 'views_handler_filter_no_results',
      ),
   );
  
 
    $tvi_query = db_query('SELECT * FROM {tripal_views}');

    while($tvi_row = db_fetch_object($tvi_query)){
      
      // check to see if this is the lightest (drupal-style) priority setup for this table
      // if not then don't use this definition
      $lightest_priority_setup = tripal_views_is_lightest_priority_setup($tvi_row->setup_id, $tvi_row->table_name);
      if (!$lightest_priority_setup) {
        continue;
      }
      
      // ids we'll use for queries
      $setup_id = $tvi_row->setup_id;
      $mview_id = $tvi_row->mview_id;

      // holds the base table name and fields
      $base_table = '';
      $base_fields = array();
      $type_prefix = '';

      // populate the base table name and fields.  If an $mview_id is given
      // then get the materialized view info, otherwise get the Chado table info
      if($mview_id){
         $type_prefix = 'MView';
         // get the base table name from the materialized view
         $sql = "SELECT name, mv_specs FROM {tripal_mviews} WHERE mview_id = %d";
         $mview_table = db_fetch_object(db_query($sql,$mview_id));
         $base_table = $mview_table->name;

         // get the columns in this materialized view.  They are separated by commas
         // where the first word is the column name and the rest is the type
         $columns = explode(",",$mview_table->mv_specs);
         foreach ($columns as $column){
            $column = trim($column);  // trim trailing and leading spaces
            preg_match("/^(.*?)\ (.*?)$/",$column,$matches);
            $column_name = $matches[1];
            $column_type = $matches[2];
            $base_fields[$column_name] = array(
              'column_name' => $column_name,
              'type' => $column_type,
            );
         }
         
         // get the field name and descriptions
         $sql = "SELECT * FROM {tripal_views_field} WHERE setup_id=%d";
         $query = db_query($sql, $setup_id);
         while ($field = db_fetch_object($query)) {
           $base_fields[$field->column_name]['name'] = $field->name;
           $base_fields[$field->column_name]['help'] = $field->description;
         }
      }
      else {
         $type_prefix = 'Chado Table';
         $base_table = $tvi_row->table_name;
         // The chado table could be a regular Chado table or a custom table
         // in the chado database.  Therefore we'll check both
         $table_desc = module_invoke_all('chado_'.$base_table.'_schema');
         if(!$table_desc){            
            $table_desc = tripal_get_chado_custom_schema($base_table);
         }
         $fields = $table_desc['fields'];
         foreach($fields as $column => $attrs){
            $base_fields[$column] = array(
              'column_name' => $column,
              'type' => $attrs['type'],
            );
         }
         // get the field name and descriptions
         $sql = "SELECT * FROM {tripal_views_field} WHERE setup_id=%d";
         $query = db_query($sql, $setup_id);
         while ($field = db_fetch_object($query)) {
           $base_fields[$field->column_name]['name'] = $field->name;
           $base_fields[$field->column_name]['help'] = $field->description;
         }
      }

      // Setup the base table info in the data array
      $data[$base_table]['table']['group'] = t("$type_prefix: $tvi_row->name");
      $data[$base_table]['table']['base'] = array(
        'group' => "$type_prefix: $tvi_row->name",
        'title' => "$type_prefix: $tvi_row->name",
        'help'  => $tvi_row->comment,
      );
      // first add the fields
      foreach ($base_fields as $column_name => $base_field){
        $data[$base_table][$column_name] = array(
           'title' => t($base_field['name']),
           'help' => t($base_field['help']),
           'field' => array(
              'click sortable' => TRUE,
           ),
        );
        
        
        // now add the handlers
        $sql = "SELECT * FROM {tripal_views_handlers} WHERE setup_id = %d AND column_name = '%s'";
        $handlers = db_query($sql,$setup_id,$column_name);
        while($handler = db_fetch_object($handlers)){
           $data[$base_table][$column_name][$handler->handler_type]['handler'] = $handler->handler_name;
        };
    }

     // now add the joins
     $sql = "SELECT * FROM {tripal_views_join} WHERE setup_id = %d";
     $joins = db_query($sql,$setup_id);
     while($join = db_fetch_object($joins)){               
        $left_table = $join->left_table;
        $left_field = $join->left_field;
        $base_field = $join->base_field; 
        $handler = $join->handler;
      
        // add join entry
        $data[$base_table]['table']['join'][$left_table] = array(
          'left_field' => $left_field,
          'field' => $base_field,
        );
        if ($handler) {
          $data[$base_table]['table']['join'][$left_table]['handler'] = $handler;
        }

        // check to see if the join table is one that correlates with Drupal nodes
        // if so, there will be a chado_<table_name> table in the Drupal database
        // if there is, then we need to add the linking join information
        if(db_table_exists("chado_$left_table")){
           // join the mview to the linker table
           $data[$base_table]['table']['join']["chado_$left_table"] = array(
             'left_field' => $left_field,
             'field' => $base_field,
           );
        }
      }
    }
    return $data;
}
/**
 *
 * @ingroup tripal_views
 */
function tripal_views_views_data_alter(&$data) {
    $tvi_query = db_query('SELECT * FROM {tripal_views}');

    // iterate through the views that we manage
    while($tvi_row = db_fetch_object($tvi_query)){

      //ids we'll use for queries
      $mview_id = $tvi_row->mview_id;
      $setup_id = $tvi_row->setup_id;

      // iterate through the columns and alter the existing data array for
      // joins to other tables
      $sql = "SELECT * FROM {tripal_views_join} WHERE setup_id = %d";
      $joins = db_query($sql,$setup_id);
      while($join = db_fetch_object($joins)){                       
        $left_table = $join->left_table;
        $left_field = $join->left_field;
        $base_field = $join->base_field;  
        $base_table = $join->base_table;    

        // add the recipricol join entries for each column
        if(array_key_exists($left_table,$data)){
           $data[$left_table]['table']['join'][$base_table] = array(
             'left_field' => $base_field,
             'field' => $left_field,
           );
        }

        // check to see if this table is one that correlates with Drupal nodes
        // if so, there will be a chado_<table_name> table in the Drupal database
        // if there is, then we need to add the linking join information.  We did
        // this step in the hook_views_data function above, but now we need 
        // to add the reciprical joins
        if(db_table_exists("chado_$left_table")){

           // join the linker table to the mview
           if(array_key_exists("chado_$left_table",$data)){
              $data["chado_$left_table"]['table']['join'][$base_table] = array(
                'left_field' => $base_field,
                'field' => $left_field,
              );

              // Join the node table to the view by way of the chado linker table
              $data['node']['table']['join'][$base_table] = array(
              	'left_table' => "chado_$left_table",
              	'left_field' => 'nid',
              	'field' => 'nid',
              );           
           }
        }
      }
    }
    return $data;
}
/**
 * Implementation of hook_views_plugins().
 */
function tripal_views_views_plugins() {
  $tc_path = drupal_get_path('module', 'tripal_views');

  $style_defaults = array(
    'path' => $tc_path . '/views_data_export/plugins',
    'parent' => 'views_data_export',
    'theme' => 'views_data_export',
    'theme path' => $tc_path . '/views_data_export/theme',
    'theme file' => 'tripal_views_data_export.theme.inc',
    'uses row plugin' => FALSE,
    'uses fields' => TRUE,
    'uses options' => TRUE,
    'type' => 'data_export',
  );

  // add FASTA format as a new style for the existing views_export_data Display
  return array( 
    'style' => array(   
      'views_data_export_fasta' => array(
        'title' => t('FASTA file'),
        'help' => t('Display results in FASTA format. All fields in results are on the definition line while the feature.residues field must be present .'),
        'handler' => 'tripal_views_plugin_style_export_fasta',
        // Views Data Export element that will be used to set additional headers when serving the feed.
        'export headers' => array('Content-type: text/plain; charset=utf-8'),
        // Views Data Export element mostly used for creating some additional classes and template names.
        'export feed type' => 'fasta',
        'export feed text' => 'FASTA',
        'export feed file' => '%view.fna',
        'export feed icon' => $tc_path . '/views_data_export/images/fasta.png',
        'additional themes' => array(
          'views_data_export_fasta_header' => 'style',
          'views_data_export_fasta_body' => 'style',
          'views_data_export_fasta_footer' => 'style',
        ),
        'additional themes base' => 'views_data_export_fasta',
      ) + $style_defaults,      
    ),
  );
}
/**
 * Implementation of hook_views_pre_view().
 */
function tripal_views_views_pre_view(&$view,&$display_id,&$args){

   // merge the $_GET and $_POST into the $_GET. This is because
   // Views and Views Data Export modules only uses the $_GET variable but
   // file uploads require $_POST. We need to make sure these two modules
   // have access to everything needed for this view to work properlys
   $_GET = array_merge($_GET, $_POST);
}
/**
 * Implementation of hook_views_pre_build().
 */
/*function tripal_views_views_pre_render(&$view, &$display_id, &$args){
   // we want to add to the bottom of the views the form for downloading 
   // results in other formats (e.g. Excel, FASTA, CSV, etc.).  The Views Data
   // Export module provides small images at the bottom, but we want to provide
   // a more intutitive interface for getting different file formats
   $form = drupal_get_form('tripal_views_data_export_download_form',$view,$display_id,$args);
   $view->attachment_after = $form;
}*/
/**
 * 
 */
function tripal_views_data_export_download_form(&$form_state, $view,$display_id,$args){
   $form = array();
   $urls = array();

   // get any export_data_export displays
   $displays = $view->display;
   $options = array();
   $default = '';
   foreach($displays as $name => $display){
      if(preg_match("/^views_data_export/",$name)){
         // set the first item as default
         if(!$default){
            $default = $display->id;
         }

         $path = $display->display_options['path'];
         $urls[$display->id]['path'] = $path;
         $urls[$display->id]['query'] = $query;
         // add the new item to the options array
         $options[$display->id] = $display->display_title;
      }
   }
   // only generate the form if we have views_data_export displays attached
   // to this view
   if(count($options) > 0){

      $form_state['storage']['urls'] = $urls;
      $form['#cache'] = TRUE;

      // we want the form to redirect to a new window
      $form['#attributes']['target'] = "_blank";

      // now build the form elements
      $form['downloads'] = array(
        '#type' => 'fieldset',
        '#title' => 'Download Results',
        '#collapsible' => TRUE,
        '#collapsed' => FALSE
      );
      $form['downloads']['file_type'] = array(
         '#title' => t('File format'),
         '#type' => 'radios',
         '#options' => $options,
         '#required' => TRUE,
         '#default_value' => $default,
         '#description' => t('Please select a file format to download'),
      );
      $form['downloads']['submit'] = array(
         '#value' => t('Download Results'),
         '#type' => 'submit',
      );
   }
   
   return $form;
}
/**
 * 
 */
function tripal_views_data_export_download_form_submit($form, &$form_state){
   $urls = $form_state['storage']['urls'];
   $display_id = $form_state['values']['file_type'];
   drupal_goto($urls[$display_id]['path'],$urls[$display_id]['query']);
}




