<?php

include('api/tripal_views.api.inc');


/**
 * @file
 * Tripal Views Integration
 *
 * @defgroup views Views Integration
 * @{
 * Provide rules for formatting and composition of fields
 * @}
 *
 * @defgroup views_handlers Views Integration Handlers
 * @ingroup views
 * @{
 * Provide rules for formatting and composition of fields
 * @}
 *
 * @defgroup views_field_handlers Views Field Handlers
 * @ingroup views_handlers
 * @{
 * Provide rules for formatting and composition of fields
 * @}
 *
 *
 * @defgroup views_filter_handlers Views Filter Handlers
 * @ingroup views_handlers
 * @{
 * Provide the ability to filter based on specified data
 * @}
 *
 * @defgroup views_sort_handlers Views Sort Handlers
 * @ingroup views_handlers
 * @{
 * Provide methods describing how specific data should be sorted
 * @}
 *
 * @defgroup views_argument_handlers Views Arguement Handlers
 * @ingroup views_handlers
 * @{
 * Provide the ability to filter pased on arguments in the path of the view
 * @}
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

      // Custom Tripal Filter Handlers
      'tripal_views_handler_filter_no_results' => array(
        'parent' => 'views_handler_filter'
      ),
      /** D7 @todo: get handlers working
      'tripal_views_handler_filter_file_upload' => array(
        'parent' => 'views_handler_filter',
      ),
      'tripal_views_handler_filter_textarea' => array(
        'parent' => 'views_handler_filter',
      ),
      'tripal_views_handler_filter_select_cvterm' => array(
        'parent' => 'views_handler_filter_string',
      ),
      'tripal_views_handler_filter_select_string' => array(
        'parent' => 'chado_views_handler_filter_string',
      ),
      'tripal_views_handler_filter_sequence' => array(
        'parent' => 'chado_views_handler_filter_string',
      ),
      */

      // Custom Tripal Field Handlers
      /** D7 @todo: get handlers working
      'tripal_views_handler_field_aggregate' => array(
        'parent' => 'chado_views_handler_field',
      ),
      'tripal_views_handler_field_sequence' => array(
        'parent' => 'chado_views_handler_field',
      ),
      */

      // Join Handlers
      /** D7 @todo: determine if we need these anymore :)
      'views_handler_join_chado_aggregator' => array(
        'parent' => 'views_join',
      ),
      'views_handler_join_chado_through_linking' => array(
        'parent' => 'views_join',
      ),
      */

      // Relationship Handlers
      /** D7 @todo: determine if we need these anymore
      'chado_views_handler_relationship' => array(
        'parent' => 'views_handler_relationship'
      ),
      'chado_views_handler_relationship_to_node' => array(
        'parent' => 'views_handler_relationship'
      ),
      */

      // Wrappers for Default Views Handlers-----
      /** D7 @todo: determine if we need these anymore :)
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
      */
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
function tripal_views_views_pre_render(&$view) {

  // We need to unset the exposed_input for the view so we can repopulate that
  // variable. This is necessary if we're using the file_upload_combo
  // custom form element which adds the file_path variable to the $_GET after the
  // view has populated the $view->exposed_input variable
  unset($view->exposed_input);

  // we want to add to the bottom of the views the form for downloading
  // results in other formats (e.g. Excel, FASTA, CSV, etc.).  The Views Data
  // Export module provides small images at the bottom, but we want to provide
  // a more intutitive interface for getting different file formats
  // $form = drupal_get_form('tripal_views_data_export_download_form', $view, $display_id, $args);
  // $view->attachment_after = $form;
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
function tripal_views_views_data() {
  $data = array();

  // MAKE SURE ALL CHADO TABLES ARE INTEGRATED
  tripal_views_integrate_all_chado_tables();

  // DEFINE GLOBAL FIELDS
  // Filter handler that lets the admin say:
  // "Show no results until they enter search parameters"
  $data['views']['search_results'] = array(
    'title' => t('Search Results'),
    'help' => t('Delay results until Apply/Search is clicked by the user.'),
    'filter' => array(
      'handler' => 'tripal_views_handler_filter_no_results',
    ),
  );

  $tvi_query = db_query('SELECT * FROM {tripal_views}');

  // INTEGRATE THE LIGHTEST SETUP FOR EACH TABLE
  foreach ($tvi_query as $tvi_row) {

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

    // indicated whether the current table is a base table or not
    $is_base_table = $tvi_row->base_table;

    // POPULATE THE BASE TABLE NAME AND FIELDS
    // If an $mview_id is given then get the materialized view info,
    // otherwise get the Chado table info
    if ($mview_id) {

      // get the base table name from the materialized view
      // D7 TODO: Check DBTNG changes work
      $sql = "SELECT name, mv_specs FROM {tripal_mviews} WHERE mview_id = :id";
      $mview_table = db_query($sql, array(':id' => $mview_id));
      $mview_table = $mview_table->fetchObject();
      $base_table = $mview_table->name;

      // get the columns in this materialized view.  They are separated by commas
      // where the first word is the column name and the rest is the type
      $columns = explode(",", $mview_table->mv_specs);
      foreach ($columns as $column) {
        $column = trim($column);  // trim trailing and leading spaces
        preg_match("/^(.*?)\ (.*?)$/", $column, $matches);
        $column_name = $matches[1];
        $column_type = $matches[2];
        $base_fields[$column_name] = array(
          'column_name' => $column_name,
          'type' => $column_type,
        );
      }

      // get the field name and descriptions
      // D7 TODO: Check DBTNG changes work
      $sql = "SELECT * FROM {tripal_views_field} WHERE setup_id=:setup";
      $query = db_query($sql, array(':setup' => $setup_id));
      foreach($query as $field) {
        $base_fields[$field->column_name]['name'] = $field->name;
        $base_fields[$field->column_name]['help'] = $field->description;
      }
    }
    // if this is not a legacy materialized view
    else {
      $base_table = $tvi_row->table_name;

      // get the table description
      $table_desc = tripal_core_get_chado_table_schema($base_table);

      $fields = $table_desc['fields'];
      if (!is_array($fields)) {
        $fields = array();
      }
      foreach ($fields as $column => $attrs) {
        $base_fields[$column] = array(
          'column_name' => $column,
          'type' => $attrs['type'],
        );
      }

      // get the field name and descriptions
      $sql = "SELECT * FROM {tripal_views_field} WHERE setup_id=:setup";
      $query = db_query($sql, array(':setup' => $setup_id));
      foreach ($query as $field) {
        $base_fields[$field->column_name]['name'] = $field->name;
        $base_fields[$field->column_name]['help'] = $field->description;
      }
    }

    // SETUP THE BASE TABLE INFO IN THE DATA ARRAY
    $data[$base_table]['table']['group'] = t("$tvi_row->name");

    if ($is_base_table) {
      $data[$base_table]['table']['base'] = array(
        'group' => "$tvi_row->name",
        'title' => "$tvi_row->name",
        'help'  => $tvi_row->comment,
        'search_path' => 'chado'
      );
    }
    else {
      $data[$base_table]['table'] = array(
        'group' => "$tvi_row->name",
        'title' => "$tvi_row->name",
        'help'  => $tvi_row->comment,
        'search_path' => 'chado'
      );
    }

    // ADD THE FIELDS TO THE DATA ARRAY
    foreach ($base_fields as $column_name => $base_field) {
      $data[$base_table][$column_name] = array(
        'title' => t($base_field['name']),
        'help' => t($base_field['help']),
        'field' => array(
          'click sortable' => TRUE,
        ),
      );


      // now add the handlers
      $sql = "SELECT * FROM {tripal_views_handlers} WHERE setup_id = :setup AND column_name = :column";
      $handlers = db_query($sql, array(':setup' => $setup_id, ':column' => $column_name));
      foreach ($handlers as $handler) {
        $data[$base_table][$column_name][$handler->handler_type]['handler'] = $handler->handler_name;

        // Add in any additional arguments
        // This should be a serialized array including (at a minimum) name => <handler name>
        if ($handler->arguments) {
          $data[$base_table][$column_name][$handler->handler_type] = array_merge($data[$base_table][$column_name][$handler->handler_type], unserialize($handler->arguments));
        }
      };
    }

    // ADD JOINS & RELATIONSHIPS TO DATA ARRAY
    // only add joins if this is a base table
    // this keeps the list of available fields manageable
    // all other tables should be added via relationships
    // D7 @todo: add tripal_views_join field that determines whether a join is added
    if ($is_base_table) {

      $sql = "SELECT * FROM {tripal_views_join} WHERE setup_id = :setup";
      $joins = db_query($sql, array(':setup' => $setup_id));
      if (!isset($joins)) {
        $joins = array();
      }
      foreach ($joins as $join) {
        $left_table = $join->left_table;
        $left_field = $join->left_field;
        $base_table = $join->base_table;
        $base_field = $join->base_field;
        $handler = $join->handler;
        $base_title = ucwords(str_replace('_', ' ', $base_table));
        $left_title = ucwords(str_replace('_', ' ', $left_table));

        // if the 'node' table is in our integrated list then
        // we want to skip it. It shouldn't be there.
        if ($left_table == 'node') {
          continue;
        }

        // add join entry
        if (!$join->relationship_only) {
          $data[$left_table]['table']['join'][$base_table] = array(
            'left_field' => $base_field,
            'field' => $left_table . '_id',
          );
          if ($handler) {
            $data[$left_table]['table']['join'][$base_table]['handler'] = $handler;
          }
          if (!empty($join->arguments)) {
            array_merge($data[$left_table]['table']['join'][$base_table], unserialize($join->arguments));
          }
        }

        // warn if deprecated method of relationship addition was used (ie: through handlers)
        if (isset($data[$base_table][$base_field]['relationship'])) {
          watchdog('tripal_views',
            'DEPRECATED: Currently using tripal_views_handlers to store relationship for %base => %left when you should be using tripal_views_joins.',
            array('%base' => $base_table, '%left' => $left_table),
            WATCHDOG_NOTICE);
        }
        // add relationship entry
        $fake_field = $base_table . '_to_' . $left_table;
        $data[$base_table][$fake_field] = array(
          'title' => "$base_title => $left_title",
          'help' => "Joins $base_title to $left_title",
          'relationship' => array(
            'handler' => $join->relationship_handler,
            'title' => t("$base_title => $left_title"),
            'label' => t("$base_title => $left_title"),
            'real field' => $base_field,
            'base' => $left_table,
            'base field' => $left_field
          )
        );
        if (!empty($join->arguments)) {
          array_merge($data[$base_table][$fake_field]['relationship'], unserialize($join->arguments));
        }
      }
    }
  }

  ddl('cleared view cache');

  return $data;
}

/**
 * Implements hook_views_data_alter().
 * Used to add Chado <-> Node Joins & Relationships
 *   since you need to add to the $data['node'] to do this
 *
 * @ingroup tripal_views
 */
function tripal_views_views_data_alter(&$data) {

    // ADD IN NODE JOINS & RELATIONSHIPS
    // D7 @todo: Create custom handler to allow join from Node => Base (ie: organism)
    //           with the addition of a single relationship
    // D7 @todo: Create custom handler to allow join from Base (ie: organism)
    //           with the addition of a single relationship
    // D7 @todo: Add support for Mview <-> Node joins and relationships
    $tvi_query = db_query('SELECT * FROM {tripal_views} WHERE base_table=1');
    foreach ($tvi_query as $tvi_row) {

      //ids we'll use for queries
      $setup_id = $tvi_row->setup_id;
      $base_table = $tvi_row->table_name;
      $linker_table = 'chado_' . $base_table;
      $base_title = ucwords(str_replace('_', ' ', $base_table));

      // add in joins to the node tables if the Chado schema is local
      if (tripal_core_chado_schema_exists()) {
        // if a node linking table exists then add in the joins
        if (db_table_exists($linker_table)) {

          // Adds content (node) fields to chado base table field lists automatically
          $data['node']['table']['join'][$linker_table] = array(
            'left_field' => 'nid',
            'field' => 'nid',
          );
          $data[$linker_table]['table']['join'][$base_table] = array(
            'left_field' => $base_table . '_id',
            'field' => $base_table . '_id',
          );
          $data['node']['table']['join'][$base_table] = array(
            'left_table' => $linker_table,
            'left_field' => 'nid',
            'field' => 'nid',
          );

          // Adds in a chado base table => node relationship
          // This allows controlled joining to multiple nodes per line
          // Use Case:  link to feature and organism nodes on a feature listing
          // D7 todo: a custom relationship handler to get from feature.organism_id => organism node
          //      without 1st needing to add relationship to organism table
          $base_field = $base_table . '_id';
          $data[$linker_table][$base_field] = array(
			      'group' => $base_title,
			      'title' => $base_title . 'Node',
			      'help' => "Links $base_title to it's node.",
			      'relationship' => array(
			        'handler' => 'views_handler_relationship',
			        'title' => t("$base_title => Node"),
			        'label' => t("$base_title => Node"),
			        'real field' => 'nid',
			        'base' => 'node',
			        'base field' => 'nid'
			      ),
			    );

          // Add Chado fields to a node-based view
          // This will only be done with relationships
          $base_field = $base_table . '_id';
          $data['node'][$base_field] = array(
			      'group' => $base_title,
			      'title' => $base_title,
			      'help' => "Links node to chado $base_title.",
			      'relationship' => array(
			        'handler' => 'views_handler_relationship',
			        'title' => t("Node => $base_title"),
			        'label' => t("Node => $base_title"),
			        'real field' => 'nid',
			        'base' => $linker_table,
			        'base field' => 'nid'
			      ),
			    );

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
*/

/**
 * Implementation of hook_views_pre_view().
 */
function tripal_views_views_pre_view(&$view, &$display_id, &$args) {

  // merge the $_GET and $_POST into the $_GET. This is because
  // Views and Views Data Export modules only uses the $_GET variable but
  // file uploads require $_POST. We need to make sure these two modules
  // have access to everything needed for this view to work properly
  $_GET = array_merge($_GET, $_POST);
}

/**
 * Implementation of hook_views_pre_build().
 */
/* function tripal_views_views_pre_render(&$view, &$display_id, &$args){
  // we want to add to the bottom of the views the form for downloading
  // results in other formats (e.g. Excel, FASTA, CSV, etc.).  The Views Data
  // Export module provides small images at the bottom, but we want to provide
  // a more intutitive interface for getting different file formats
  $form = drupal_get_form('tripal_views_data_export_download_form',$view,$display_id,$args);
  $view->attachment_after = $form;
}*/

/**
 * the Download Views data export form
function tripal_views_data_export_download_form(&$form_state, $view, $display_id, $args) {
  $form = array();
  $urls = array();

  // get any export_data_export displays
  $displays = $view->display;
  $options = array();
  $default = '';
  $current_display = $view->current_display;
  foreach ($displays as $name => $display) {
    if (preg_match("/^views_data_export/", $name)) {

      // only add this display to the form if it is attached
      $display_options = $display->display_options;
      if (strcmp($display_options['displays'][$current_display], $current_display) != 0) {
         continue;
      }

      // set the first item as default
      if (!$default) {
        $default = $display->id;
      }

      $path = $display->display_options['path'];
      $query = $view->get_exposed_input();  // retrieves elements in $_GET array

      $urls[$display->id]['path'] = $path;
      $urls[$display->id]['query'] = $query;

      // add the new item to the options array
      $options[$display->id] = $display->display_title;
    }
  }

  // only generate the form if we have views_data_export displays attached
  // to this view
  if (count($options) > 0) {

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
*/

/**
 * Submit for the Download Views data export form
function tripal_views_data_export_download_form_submit($form, &$form_state) {
  $urls = $form_state['storage']['urls'];
  $display_id = $form_state['values']['file_type'];
  drupal_goto($urls[$display_id]['path'], $urls[$display_id]['query']);
}
*/




