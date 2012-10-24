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

/**
 * @defgroup tripal_analysis_views Analysis Views Integration
 * @ingroup views
 * @ingroup tripal_analysis
 */

/**
 * Implements hook_views_data()
 *
 * Purpose: Describe chado/tripal tables & fields to views
 *
 * @return: a data array which follows the structure outlined in the
 *   views2 documentation for this hook. Essentially, it's an array of table
 *   definitions keyed by chado/tripal table name. Each table definition
 *   includes basic details about the table, fields in that table and
 *   relationships between that table and others (joins)
 *
 * @ingroup tripal_analysis_views
 */
function tripal_analysis_views_data()  {
  $data = array();

  if (module_exists('tripal_views')) {
    // Base Table: Analysis
    $tablename = 'analysis';
    // get the setup with the lightest priority. That's the table 
    // that currently integrated with views.
    $priority = tripal_views_get_table_lightest_priority($tablename);
      
    // check to see if the table is integrated. If it is then integrate it's
    // corresponding 'chado_[table]' table.
    if (!tripal_views_is_integrated($tablename, $priority)) {
      $table_integration_array = tripal_views_get_integration_array_for_chado_table($tablename, TRUE, $priority);

      // Make table-specific modifications
      $table_integration_array['fields']['program']['handlers']['filter']['name'] = 'tripal_views_handler_filter_select_string';
      $table_integration_array['fields']['algorithm']['handlers']['filter']['name'] = 'tripal_views_handler_filter_select_string';

      // Add in node relationships if chado is in the same db as drupal
      if (tripal_core_chado_schema_exists()) {
        $integrations = tripal_views_add_node_relationship_to_chado_table_integration($table_integration_array);
        foreach ($integrations as $integration) {
          tripal_views_integration_add_entry($integration);
        }
      }
      else {
        tripal_views_integration_add_entry($table_integration_array);
      }
    }


    $tables = array(
      'analysisfeature',
      'analysisprop'
    );
    foreach ($tables as $tablename) {
      $priority = tripal_views_get_table_lightest_priority($tablename);     
      if (!tripal_views_is_integrated($tablename, $priority)) {
        $table_integration_array = tripal_views_get_integration_array_for_chado_table($tablename, FALSE, $priority);
        tripal_views_integration_add_entry($table_integration_array);
      }
    }

  }

  return $data;
}

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
 * @ingroup tripal_analysis_views
 */
function tripal_analysis_views_views_handlers() {
  return array(
    'info' => array(
      'path' => drupal_get_path('module', 'tripal_analysis') . '/views/handlers',
    ),
    'handlers' => array(
      'views_handler_field_computed_analysis_nid' => array(
        'parent' => 'views_handler_field_numeric',
      ),
      'views_handler_field_readable_date' => array(
        'parent' => 'views_handler_field',
      ),
    ),
  );
}

/**
 * Implementation of hook_views_data_alter().
 */
function tripal_analysis_views_data_alter(&$data) {

  if (!(is_array($db_url) and array_key_exists('chado', $db_url)) ) {

    // Add featuer relationship to node
    $data['node']['analysis_chado_nid'] = array(
      'group' => 'Analysis',
      'title' => 'Analysis Node',
      'help' => 'Links Chado analysis Fields/Data to the Nodes in the current View.',
      'real field' => 'nid',
      'relationship' => array(
        'handler' => 'views_handler_relationship',
        'title' => t('Node => Chado'),
        'label' => t('Node => Chado'),
        'real field' => 'nid',
        'base' => 'chado_analysis',
        'base field' => 'nid'
      ),
    );
  }

}

/**
 * Implements hook_views_default_views().
 *
 * @ingroup tripal_analysis_views
 */
function tripal_analysis_views_default_views() {
  $views = array();

  if (!module_exists('tripal_views')) {
    return $views;
  }

  // Main default view
  $view = new view;
  $view->name = 'analysis_listing';
  $view->description = 'A default listing of analyses provided by Tripal';
  $view->tag = 'chado default';
  $view->base_table = 'analysis';
  $view->core = 0;
  $view->api_version = '2';
  $view->disabled = FALSE; /* Edit this to true to make a default view disabled initially */
  $handler = $view->new_display('default', 'Defaults', 'default');
  $handler->override_option('fields', array(
    'name' => array(
      'label' => 'Name',
      'alter' => array(
        'alter_text' => 0,
        'text' => '',
        'make_link' => 0,
        'path' => '',
        'absolute' => 0,
        'link_class' => '',
        'alt' => '',
        'rel' => '',
        'prefix' => '',
        'suffix' => '',
        'target' => '',
        'help' => '',
        'trim' => 0,
        'max_length' => '',
        'word_boundary' => 1,
        'ellipsis' => 1,
        'html' => 0,
        'strip_tags' => 0,
      ),
      'empty' => '',
      'hide_empty' => 0,
      'empty_zero' => 0,
      'hide_alter_empty' => 1,
      'type' => 'separator',
      'separator' => ', ',
      'exclude' => 0,
      'link_to_node' => 1,
      'id' => 'name',
      'table' => 'analysis',
      'field' => 'name',
      'relationship' => 'none',
    ),
    'program' => array(
      'label' => 'Program',
      'alter' => array(
        'alter_text' => 0,
        'text' => '',
        'make_link' => 0,
        'path' => '',
        'absolute' => 0,
        'link_class' => '',
        'alt' => '',
        'rel' => '',
        'prefix' => '',
        'suffix' => '',
        'target' => '',
        'help' => '',
        'trim' => 0,
        'max_length' => '',
        'word_boundary' => 1,
        'ellipsis' => 1,
        'html' => 0,
        'strip_tags' => 0,
      ),
      'empty' => '',
      'hide_empty' => 0,
      'empty_zero' => 0,
      'hide_alter_empty' => 1,
      'exclude' => 0,
      'id' => 'program',
      'table' => 'analysis',
      'field' => 'program',
      'relationship' => 'none',
    ),
    'sourcename' => array(
      'label' => 'Source',
      'alter' => array(
        'alter_text' => 0,
        'text' => '',
        'make_link' => 0,
        'path' => '',
        'absolute' => 0,
        'link_class' => '',
        'alt' => '',
        'rel' => '',
        'prefix' => '',
        'suffix' => '',
        'target' => '',
        'help' => '',
        'trim' => 0,
        'max_length' => '',
        'word_boundary' => 1,
        'ellipsis' => 1,
        'html' => 0,
        'strip_tags' => 0,
      ),
      'empty' => '',
      'hide_empty' => 0,
      'empty_zero' => 0,
      'hide_alter_empty' => 1,
      'exclude' => 0,
      'id' => 'sourcename',
      'table' => 'analysis',
      'field' => 'sourcename',
      'relationship' => 'none',
    ),
    'timeexecuted' => array(
      'label' => 'Time Executed',
      'alter' => array(
        'alter_text' => 0,
        'text' => '',
        'make_link' => 0,
        'path' => '',
        'absolute' => 0,
        'link_class' => '',
        'alt' => '',
        'rel' => '',
        'prefix' => '',
        'suffix' => '',
        'target' => '',
        'help' => '',
        'trim' => 0,
        'max_length' => '',
        'word_boundary' => 1,
        'ellipsis' => 1,
        'html' => 0,
        'strip_tags' => 0,
      ),
      'empty' => '',
      'hide_empty' => 0,
      'empty_zero' => 0,
      'hide_alter_empty' => 1,
      'date_format' => 'custom',
      'custom_date_format' => 'F j, Y',
      'exclude' => 0,
      'id' => 'timeexecuted',
      'table' => 'analysis',
      'field' => 'timeexecuted',
      'override' => array(
        'button' => 'Override',
      ),
      'relationship' => 'none',
    ),
  ));
  $handler->override_option('filters', array(
    'search_results' => array(
      'operator' => '=',
      'value' => '',
      'group' => '0',
      'exposed' => FALSE,
      'expose' => array(
        'operator' => FALSE,
        'label' => '',
      ),
      'id' => 'search_results',
      'table' => 'views',
      'field' => 'search_results',
      'relationship' => 'none',
      'apply_button' => 'Show ',
      'no_results_text' => 'Click "Show" to see a list of all analysis matching the entered criteria. If you leave a any of the criteria blank then the analysis will be not be filtered based on that field. Furthermore, if you leave all criteria blank then all analysis will be listed.',
    ),
    'program' => array(
      'operator' => '=',
      'value' => array(),
      'group' => '0',
      'exposed' => TRUE,
      'expose' => array(
        'use_operator' => 0,
        'operator' => 'program_op',
        'identifier' => 'program',
        'label' => 'Program',
        'remember' => 0,
      ),
      'case' => 1,
      'id' => 'program',
      'table' => 'analysis',
      'field' => 'program',
      'relationship' => 'none',
      'values_form_type' => 'select',
      'multiple' => 1,
      'optional' => 0,
      'agg' => array(
        'records_with' => 1,
        'aggregates_with' => 0,
      ),
    ),
    'timeexecuted' => array(
      'operator' => '>',
      'value' => array(
        'type' => 'date',
        'value' => '',
        'min' => '',
        'max' => '',
      ),
      'group' => '0',
      'exposed' => TRUE,
      'expose' => array(
        'use_operator' => 1,
        'operator' => 'timeexecuted_op',
        'identifier' => 'timeexecuted',
        'label' => 'Time Executed',
        'optional' => 1,
        'remember' => 0,
      ),
      'id' => 'timeexecuted',
      'table' => 'analysis',
      'field' => 'timeexecuted',
      'relationship' => 'none',
      'agg' => array(
        'records_with' => 1,
        'aggregates_with' => 0,
      ),
    ),
    'name' => array(
      'operator' => 'contains',
      'value' => '',
      'group' => '0',
      'exposed' => TRUE,
      'expose' => array(
        'use_operator' => 0,
        'operator' => 'name_op',
        'identifier' => 'name',
        'label' => 'Name Contains',
        'bef_filter_description' => '',
        'remember' => 0,
      ),
      'case' => 0,
      'id' => 'name',
      'table' => 'analysis',
      'field' => 'name',
      'relationship' => 'none',
      'values_form_type' => 'textfield',
      'multiple' => 0,
      'optional' => 0,
    ),
    'sourcename' => array(
      'operator' => 'contains',
      'value' => '',
      'group' => '0',
      'exposed' => TRUE,
      'expose' => array(
        'use_operator' => 0,
        'operator' => 'sourcename_op',
        'identifier' => 'sourcename',
        'label' => 'Source Contains',
        'bef_filter_description' => '',
        'remember' => 0,
      ),
      'case' => 0,
      'id' => 'sourcename',
      'table' => 'analysis',
      'field' => 'sourcename',
      'relationship' => 'none',
    ),
  ));
  $handler->override_option('access', array(
    'type' => 'perm',
    'perm' => 'access chado_analysis content',
  ));
  $handler->override_option('cache', array(
    'type' => 'none',
  ));
  $handler->override_option('title', 'Analysis');
  $handler->override_option('header', 'Click "Show" to see a list of all analysis matching the entered criteria. If you leave a any of the criteria blank then the analysis will be not be filtered based on that field. Furthermore, if you leave all criteria blank then all analysis will be listed.');
  $handler->override_option('header_format', '2');
  $handler->override_option('header_empty', 1);
  $handler->override_option('empty', 'No analyses match the supplied criteria.');
  $handler->override_option('empty_format', '1');
  $handler->override_option('items_per_page', 50);
  $handler->override_option('use_pager', '1');
  $handler->override_option('style_plugin', 'table');
  $handler->override_option('style_options', array(
    'grouping' => '',
    'override' => 1,
    'sticky' => 0,
    'order' => 'asc',
    'summary' => '',
    'columns' => array(
      'name' => 'name',
      'program' => 'program',
      'sourcename' => 'sourcename',
      'num_features' => 'num_features',
      'timeexecuted' => 'timeexecuted',
    ),
    'info' => array(
      'name' => array(
        'sortable' => 1,
        'separator' => '',
      ),
      'program' => array(
        'sortable' => 1,
        'separator' => '',
      ),
      'sourcename' => array(
        'sortable' => 1,
        'separator' => '',
      ),
      'num_features' => array(
        'separator' => '',
      ),
      'timeexecuted' => array(
        'sortable' => 1,
        'separator' => '',
      ),
    ),
    'default' => 'name',
  ));
  // this is needed so that fields/filters can be added to the default display later
  $default_handler = $handler;
  $handler = $view->new_display('page', 'Page', 'page_1');
  $handler->override_option('path', 'chado/analyses');
  $handler->override_option('menu', array(
    'type' => 'normal',
    'title' => 'Analyses',
    'description' => 'An analysis is a particular type of a computational analysis; it may be a blast of one sequence against another, or an all by all blast, or a different kind of analysis altogether. It is a single unit of computation.',
    'weight' => '10',
    'name' => 'navigation',
  ));
  $handler->override_option('tab_options', array(
    'type' => 'none',
    'title' => '',
    'description' => '',
    'weight' => 0,
    'name' => 'navigation',
  ));
  // Add code specific to a local chado installation
  // NOTE: Edit $handler above to $default_handler for the default display
  if (tripal_core_chado_schema_exists()) {
    // Add nid field
    $fields = $view->get_items('field', 'default');
    $new_fields = array(
      'nid' => array(
        'label' => 'Nid',
        'alter' => array(
          'alter_text' => 0,
          'text' => '',
          'make_link' => 0,
          'path' => '',
          'absolute' => 0,
          'link_class' => '',
          'alt' => '',
          'rel' => '',
          'prefix' => '',
          'suffix' => '',
          'target' => '',
          'help' => '',
          'trim' => 0,
          'max_length' => '',
          'word_boundary' => 1,
          'ellipsis' => 1,
          'html' => 0,
          'strip_tags' => 0,
        ),
        'empty' => '',
        'hide_empty' => 0,
        'empty_zero' => 0,
        'hide_alter_empty' => 1,
        'link_to_node' => 0,
        'exclude' => 1,
        'id' => 'nid',
        'table' => 'node',
        'field' => 'nid',
        'relationship' => 'none',
      )
    );
    $fields = $new_fields + $fields;
    // Adds analysis => Node relationship
    $default_handler->override_option('relationships', array(
      'nid' => array(
        'label' => 'Analysis to Node',
        'required' => 0,
        'id' => 'nid',
        'table' => 'chado_analysis',
        'field' => 'nid',
        'relationship' => 'none',
      ),
    ));
    // Change analysis.name to have a link to the node
    $fields['name']['alter']['make_link'] = 1;
    $fields['name']['alter']['path'] = 'node/[nid]';
    $default_handler->override_option('fields', $fields);
    // Only show records with published nodes
    $filters = $view->get_items('filter', 'default');
    $filters['status'] = array(
      'operator' => '=',
      'value' => '1',
      'group' => '0',
      'exposed' => FALSE,
      'expose' => array(
        'operator' => FALSE,
        'label' => '',
      ),
      'id' => 'status',
      'table' => 'node',
      'field' => 'status',
      'relationship' => 'none',
    );
    $default_handler->override_option('filters', $filters);
  }
  $views[$view->name] = $view;

  return $views;
}
