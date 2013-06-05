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

/**
 * @defgroup tripal_featuremap_views Map Views Integration
 * @ingroup views
 */

/*************************************************************************
 * Implements hook_views_data()
 * Purpose: Describe chado/tripal tables & fields to views
 * @return: a data array which follows the structure outlined in the
 *   views2 documentation for this hook. Essentially, it's an array of table
 *   definitions keyed by chado/tripal table name. Each table definition
 *   includes basic details about the table, fields in that table and
 *   relationships between that table and others (joins)
 *
 * @ingroup tripal_featuremap_views
 */
function tripal_featuremap_views_data()  {
  $data = array();

  if (module_exists('tripal_views')) {
    $tables = array(
      'featuremap'
    );
    foreach ($tables as $tablename) {
      $priority = 9;

      // check to see if the table is integrated. If it is then integrate it's
      // corresponding 'chado_[table]' table.
      if (!tripal_views_is_integrated($tablename, $priority)) {
        $table_integration_array = tripal_views_get_integration_array_for_chado_table($tablename, TRUE, $priority);

        // Add custom handle to handle type_ids
        $table_integration_array['fields']['unittype_id']['handlers']['filter']['name'] = 'tripal_views_handler_filter_select_cvterm';

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
    }

    $tables = array(
      'map_cvterm',
      'map_feature',
      'map_pub',
      'map_synonym',
      'mapprop'
    );
    foreach ($tables as $tablename) {
      $priority = 9;
      if (!tripal_views_is_integrated($tablename, $priority)) {
        $table_integration_array = tripal_views_get_integration_array_for_chado_table($tablename, FALSE, $priority);
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
 * @return: An array of handler definitions
 *
 * @ingroup tripal_featuremap_views
 */
function tripal_featuremap_views_handlers() {
  return array(
   'info' => array(
     'path' => drupal_get_path('module', 'tripal_featuremap') . '/views/handlers',
      ),
   'handlers' => array(
     'views_handler_field_computed_map_nid' => array(
       'parent' => 'views_handler_field_numeric',
      ),
    ),
  );
}

/**
 * Implementation of hook_views_data_alter().
 */
function tripal_featuremap_views_data_alter(&$data) {

  if ( !(is_array($db_url) and array_key_exists('chado', $db_url)) ) {

    // Add featuer relationship to node
    $data['node']['map_chado_nid'] = array(
      'group' => 'Map',
      'title' => 'Map Node',
      'help' => 'Links Chado Map Fields/Data to the Nodes in the current View.',
      'real field' => 'nid',
      'relationship' => array(
        'handler' => 'views_handler_relationship',
        'title' => t('Node => Chado'),
        'label' => t('Node => Chado'),
        'real field' => 'nid',
        'base' => 'chado_map',
        'base field' => 'nid'
      ),
    );
  }

}

/**
 *
 *
 * @ingroup tripal_featuremap_views
 */
function tripal_featuremap_views_default_views() {
  $views = array();

  if (!module_exists('tripal_views')) {
    return $views;
  }

  // Main default view
  $view = new view;
  $view->name = 'featuremap_listing';
  $view->description = 'A listing of all chado feature maps';
  $view->tag = 'chado default';
  $view->base_table = 'featuremap';
  $view->core = 6;
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
        'path' => 'node/[nid]',
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
      'id' => 'name',
      'table' => 'featuremap',
      'field' => 'name',
      'relationship' => 'none',
    ),
    'description' => array(
      'label' => 'Description',
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
      'id' => 'description',
      'table' => 'featuremap',
      'field' => 'description',
      'relationship' => 'none',
    ),
    'name_1' => array(
      'label' => 'Map Units',
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
      'id' => 'name_1',
      'table' => 'cvterm',
      'field' => 'name',
      'relationship' => 'none',
    ),
  ));
  $handler->override_option('sorts', array(
    'name' => array(
      'order' => 'ASC',
      'id' => 'name',
      'table' => 'featuremap',
      'field' => 'name',
      'relationship' => 'none',
    ),
  ));
  $handler->override_option('filters', array(
    'unittype_id' => array(
      'operator' => '=',
      'value' => '',
      'group' => '0',
      'exposed' => TRUE,
      'expose' => array(
        'use_operator' => 0,
        'operator' => 'unittype_id_op',
        'identifier' => 'unittype_id',
        'label' => 'Map Units',
        'remember' => 0,
      ),
      'case' => 1,
      'id' => 'unittype_id',
      'table' => 'featuremap',
      'field' => 'unittype_id',
      'relationship' => 'none',
      'values_form_type' => 'select',
      'multiple' => 1,
      'optional' => 0,
      'show_all' => 0,
      'agg' => array(
        'records_with' => 1,
        'aggregates_with' => 1,
      ),
    ),
    'name_1' => array(
      'operator' => '~',
      'value' => '',
      'group' => '0',
      'exposed' => TRUE,
      'expose' => array(
        'use_operator' => 0,
        'operator' => 'name_1_op',
        'identifier' => 'name_1',
        'label' => 'Name',
        'remember' => 0,
      ),
      'case' => 0,
      'id' => 'name_1',
      'table' => 'featuremap',
      'field' => 'name',
      'relationship' => 'none',
      'agg' => array(
        'records_with' => 1,
        'aggregates_with' => 1,
      ),
    ),
    'description' => array(
      'operator' => '~',
      'value' => '',
      'group' => '0',
      'exposed' => TRUE,
      'expose' => array(
        'use_operator' => 0,
        'operator' => 'description_op',
        'identifier' => 'description',
        'label' => 'Description',
        'remember' => 0,
      ),
      'case' => 0,
      'id' => 'description',
      'table' => 'featuremap',
      'field' => 'description',
      'relationship' => 'none',
      'agg' => array(
        'records_with' => 1,
        'aggregates_with' => 1,
      ),
    ),
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
      'apply_button' => 'Show',
      'no_results_text' => 'Click "Show" to see a list of all feature maps matching the entered criteria. If you leave a any of the criteria blank then the maps will be not be filtered based on that field. Furthermore, if you leave all criteria blank then all maps will be listed.',
    ),
  ));
  $handler->override_option('access', array(
    'type' => 'perm',
    'perm' => 'access chado_featuremap content',
  ));
  $handler->override_option('cache', array(
    'type' => 'none',
  ));
  $handler->override_option('title', 'Maps');
  $handler->override_option('header', 'Click "Show" to see a list of all feature maps matching the entered criteria. If you leave a any of the criteria blank then the maps will be not be filtered based on that field. Furthermore, if you leave all criteria blank then all maps will be listed.');
  $handler->override_option('header_format', '1');
  $handler->override_option('header_empty', 1);
  $handler->override_option('empty', 'No feature maps match the current criteria.');
  $handler->override_option('empty_format', '1');
  $handler->override_option('items_per_page', 25);
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
      'description' => 'description',
      'name_1' => 'name_1',
      'nid' => 'nid',
    ),
    'info' => array(
      'name' => array(
        'sortable' => 1,
        'separator' => '',
      ),
      'description' => array(
        'sortable' => 1,
        'separator' => '',
      ),
      'name_1' => array(
        'sortable' => 1,
        'separator' => '',
      ),
      'nid' => array(
        'separator' => '',
      ),
    ),
    'default' => '-1',
  ));
  $default_handler = $handler;
  $handler = $view->new_display('page', 'Page', 'page_1');
  $handler->override_option('path', 'chado/featuremaps');
  $handler->override_option('menu', array(
    'type' => 'normal',
    'title' => 'Maps',
    'description' => 'A listing of chado feature maps.',
    'weight' => '0',
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
    // Adds feature => Node relationship
    $default_handler->override_option('relationships', array(
      'nid' => array(
        'label' => 'Map to Node',
        'required' => 0,
        'id' => 'nid',
        'table' => 'chado_featuremap',
        'field' => 'nid',
        'relationship' => 'none',
      ),
    ));
    // Change analysis.name to have a link to the node
    $fields['name']['alter']['link_to_node'] = 1;
    $default_handler->override_option('fields', $fields);
    // Only show records with published nodes
    /**
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
    */
  }
  $views[$view->name] = $view;

  return $views;
}
