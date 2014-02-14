<?php
/**
 * @file
 * Tripal Views Integration
 */

include 'views/handlers/tripal_views_handler_area_action_links.inc';

/**
 * Implements hook_views_handlers().
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

      // Custom Tripal Field Handlers
      /** CANT FIX UNTIL Tripal Feature is working
      'tripal_views_handler_field_sequence' => array(
        'parent' => 'views_handler_field',
      ),
      */

      // Custom area handler
      'tripal_views_handler_area_action_links' => array(
        'parent' => 'views_handler_area',
      ),

      // Custom Tripal Filter Handlers
      'tripal_views_handler_filter_no_results' => array(
        'parent' => 'views_handler_filter'
      ),
      'tripal_views_handler_filter_select_cvterm' => array(
        'parent' => 'tripal_views_handler_filter_select_string',
      ),
      'tripal_views_handler_filter_select_string' => array(
        'parent' => 'views_handler_filter_string',
      ),
      'tripal_views_handler_filter_textarea' => array(
        'parent' => 'views_handler_filter',
      ),
      'tripal_views_handler_filter_file_upload' => array(
        'parent' => 'views_handler_filter',
      ),
      /** D7 @todo: get handlers working

      */
      /** CANT FIX UNTIL Tripal Feature is working
      'tripal_views_handler_filter_sequence' => array(
        'parent' => 'chado_views_handler_filter_string',
      ),
      */
    ),
  );
}

/**
 * Implements hook_views_pre_render().
 *
 * Purpose: Intercepts the view after the query has been executed
 *   All the results are stored in $view->result
 *   Looking up the NID here ensures the query is only executed once
 *   for all stocks in the table.
 *
 * @ingroup tripal_views
 */
function tripal_views_views_pre_render(&$view) {

  // We need to unset the exposed_input for the view so we can repopulate that
  // variable. This is necessary if we're using the file_upload_combo
  // custom form element which adds the file_path variable to the $_GET after the
  // view has populated the $view->exposed_input variable
  unset($view->exposed_input);

  // Add css if tripal admin tagged view
  if ($view->tag == 'tripal admin') {
    $tripal_admin_view_css = drupal_get_path('module', 'tripal_views') . '/theme/css/tripal_views_admin_views.css';
    drupal_add_css($tripal_admin_view_css);
  }

}

/**
 * Implements hook_views_date().
 * Generates a dynamic data array for Views
 *
 * This function is a hook used by the Views module. It populates and
 *   returns a data array that specifies for the Views module the base table,
 *   the tables it joins with and handlers.  The data array is populated
 *   using the data stored in the tripal_views tables.
 *
 * @return a data array formatted for the Views module
 *
 * D7 @todo: Add support for materialized views relationships using the new method
 *
 * @ingroup tripal_views
 */
function tripal_views_views_data() {
  $data = array();

  // Manually integrate the drupal.tripal_views* tables
  $data = tripal_views_views_data_tripal_views_tables($data);

  // MAKE SURE ALL CHADO TABLES ARE INTEGRATED
  tripal_views_integrate_all_chado_tables();

  // DEFINE GLOBAL FIELDS
  // Filter handler that lets the admin say:
  // "Show no results until they enter search parameters"
  $data['views']['search_results'] = array(
    'title' => t('Delay Results'),
    'help' => t('Delay results until Apply/Search is clicked by the user.'),
    'filter' => array(
      'handler' => 'tripal_views_handler_filter_no_results',
    ),
  );

  $data['views']['action_links_area'] = array(
    'title' => t('Action Links'),
    'help' => t('Add action links to the view.'),
    'area' => array(
      'handler' => 'tripal_views_handler_area_action_links',
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
      $table_desc = chado_get_schema($base_table);

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
      if (!isset($base_field['name'])) {
        $data[$base_table][$column_name] = array(
          'title' => t($column_name),
          'help' => t($column_name),
          'field' => array(
            'click sortable' => TRUE,
          ),
        );

        tripal_report_error(
          'tripal_views',
          TRIPAL_WARNING,
          "The name and help were not set for %table.%column. As a consequence the column
            name has been used... You should ensure that the 'name' and 'help' keys for
            this field are set in the integration array (priority = %priority)",
          array(
            '%table'=> $base_table,
            '%column' => $column_name,
            '%priority' => $tvi_row->priority
          )
        );
      }
      else {
        $data[$base_table][$column_name] = array(
          'title' => t($base_field['name']),
          'help' => t($base_field['help']),
          'field' => array(
            'click sortable' => TRUE,
          ),
        );
      }

      // now add the handlers
      $sql = "SELECT * FROM {tripal_views_handlers} WHERE setup_id = :setup AND column_name = :column";
      $handlers = db_query($sql, array(':setup' => $setup_id, ':column' => $column_name));
      $num_handlers = 0;
      foreach ($handlers as $handler) {
        $num_handlers++;

        $data[$base_table][$column_name][$handler->handler_type]['handler'] = $handler->handler_name;

        // Add in any additional arguments
        // This should be a serialized array including (at a minimum) name => <handler name>
        if ($handler->arguments) {
          $data[$base_table][$column_name][$handler->handler_type] = array_merge($data[$base_table][$column_name][$handler->handler_type], unserialize($handler->arguments));
        }
      };

      // If there were no handlers applied to the current field then warn the administrator!
      if ($num_handlers == 0) {
        tripal_report_error(
          'tripal_views',
          TRIPAL_WARNING,
          "No handlers were registered for %table.%column. This means there is no views
            functionality for this column. To register handlers, make sure the 'handlers'
            key for this field is set in the integration array (priority = %priority).
            The supported keys include 'field', 'filter', 'sort', 'relationship'. Look
            at the tripal_views_integration_add_entry() for additional details.",
          array(
            '%table'=> $base_table,
            '%column' => $column_name,
            '%priority' => $tvi_row->priority
          )
        );
      }
    }

    // ADD JOINS & RELATIONSHIPS TO DATA ARRAY
    // only add joins if this is a base table
    // this keeps the list of available fields manageable
    // all other tables should be added via relationships
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
          tripal_report_error('tripal_views', TRIPAL_NOTICE,
            'DEPRECATED: Currently using tripal_views_handlers to store relationship for %base => %left when you should be using tripal_views_joins.',
            array('%base' => $base_table, '%left' => $left_table));
        }
        // add relationship entry
        $fake_field = $base_field . '_to_' . $left_table;
        $data[$base_table][$fake_field] = array(
          'title' => "$base_title.$base_field => $left_title",
          'help' => "Joins $base_title.$base_field to $left_title",
          'relationship' => array(
            'handler' => $join->relationship_handler,
            'title' => t("$base_field => $left_title"),
            'label' => t("$base_field => $left_title"),
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

  return $data;
}

/**
 * Describes the tripal views integration tables to views for the administration views
 *
 * @ingroup tripal_views
 */
function tripal_views_views_data_tripal_views_tables($data) {

  $data['tripal_views']['table']['group'] = t('Tripal Views Integration');
  $data['tripal_views']['table']['base'] = array(
    'field' => 'setup_id', // This is the identifier field for the view.
    'title' => t('Tripal Views Integration'),
    'help' => t('Specifications on how to integrate various tables with Drupal Views'),
    'weight' => -10,
  );

  // Implicit Join to Materialized Views
  $data['tripal_views']['table']['join'] = array(
    'tripal_mviews' => array(
      'left_field' => 'mview_id',
      'field' => 'mview_id',
    ),
  );

  // setup_id
  $data['tripal_views']['setup_id'] = array(
    'title' => t('Setup ID'),
    'help' => t('Primary key of the integration'),
    'field' => array(
      'handler' => 'views_handler_field_numeric',
      'click sortable' => TRUE,
    ),
    'filter' => array(
      'handler' => 'views_handler_filter_numeric',
    ),
    'sort' => array(
      'handler' => 'views_handler_sort',
    ),
  );

  // mview_id
  $data['tripal_views']['mview_id'] = array(
    'title' => t('Materialized View ID'),
    'help' => t('The primary key of the Mview integrated.'),
    'field' => array(
      'handler' => 'views_handler_field_numeric',
      'click sortable' => TRUE,
    ),
    'filter' => array(
      'handler' => 'views_handler_filter_numeric',
    ),
    'sort' => array(
      'handler' => 'views_handler_sort',
    ),
  );

  // base_table
  $data['tripal_views']['base_table'] = array(
    'title' => t('Base Table?'),
    'help' => t('Whether the table being integrated should be considered a base table.'),
    'field' => array(
      'handler' => 'views_handler_field_boolean',
      'click sortable' => TRUE,
    ),
    'filter' => array(
      'handler' => 'views_handler_filter_boolean_operator',
      'label' => t('Base Table?'),
      'type' => 'yes-no',
      'use equal' => TRUE,
    ),
    'sort' => array(
      'handler' => 'views_handler_sort',
    ),
  );

  // table_name
  $data['tripal_views']['table_name'] = array(
    'title' => t('Chado Table Name'),
    'help' => t('The name of the table being integrated'),
    'field' => array(
      'handler' => 'views_handler_field',
      'click sortable' => TRUE, // This is use by the table display plugin.
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

  // priority
  $data['tripal_views']['priority'] = array(
    'title' => t('Priority'),
    'help' => t('The priority of the integration.'),
    'field' => array(
      'handler' => 'views_handler_field_numeric',
      'click sortable' => TRUE,
    ),
    'filter' => array(
      'handler' => 'views_handler_filter_numeric',
    ),
    'sort' => array(
      'handler' => 'views_handler_sort',
    ),
  );

  // name
  $data['tripal_views']['name'] = array(
    'title' => t('Name'),
    'help' => t('The name of the integration'),
    'field' => array(
      'handler' => 'views_handler_field',
      'click sortable' => TRUE, // This is use by the table display plugin.
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

  // comment
  $data['tripal_views']['comment'] = array(
    'title' => t('Description'),
    'help' => t('Short description or comment about this integration.'),
    'field' => array(
      'handler' => 'views_handler_field',
      'click sortable' => TRUE, // This is use by the table display plugin.
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
 * Implementation of hook_views_pre_view().
 *
 * Merge the $_GET and $_POST into the $_GET. This is because
 * Views and Views Data Export modules only uses the $_GET variable but
 * file uploads require $_POST. We need to make sure these two modules
 * have access to everything needed for this view to work properly.
 *
 * @ingroup tripal_views
 */
function tripal_views_views_pre_view(&$view, &$display_id, &$args) {
  $_GET = array_merge($_GET, $_POST);
}