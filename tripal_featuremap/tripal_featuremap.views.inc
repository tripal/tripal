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
 * @ingroup tripal_featuremap
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
  $view->name = 'map_listing';
  $view->description = 'A listing of all maps';
  $view->tag = 'chado default';
  $view->base_table = 'map';
  $view->core = 0;
  $view->api_version = '2';
  $view->disabled = FALSE; /* Edit this to true to make a default view disabled initially */
  $handler = $view->new_display('default', 'Defaults', 'default');
  $handler->override_option('fields', array(
    'uniquename' => array(
      'label' => 'Unique Name',
      'alter' => array(
        'alter_text' => 0,
        'text' => '',
        'make_link' => 0,
        'path' => '',
        'link_class' => '',
        'alt' => '',
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
      'link_to_node' => 1,
      'exclude' => 0,
      'id' => 'uniquename',
      'table' => 'map',
      'field' => 'uniquename',
      'relationship' => 'none',
    ),
    'name_1' => array(
      'label' => 'Name',
      'alter' => array(
        'alter_text' => 0,
        'text' => '',
        'make_link' => 0,
        'path' => '',
        'link_class' => '',
        'alt' => '',
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
      'link_to_node' => 1,
      'exclude' => 0,
      'id' => 'name_1',
      'table' => 'map',
      'field' => 'name',
      'relationship' => 'none',
    ),
    'common_name' => array(
      'label' => 'Organism',
      'alter' => array(
        'alter_text' => 0,
        'text' => '',
        'make_link' => 0,
        'path' => '',
        'link_class' => '',
        'alt' => '',
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
      'link_to_node' => 1,
      'exclude' => 0,
      'id' => 'common_name',
      'table' => 'organism',
      'field' => 'common_name',
      'relationship' => 'none',
    ),
    'name' => array(
      'label' => 'Type',
      'alter' => array(
        'alter_text' => 0,
        'text' => '',
        'make_link' => 0,
        'path' => '',
        'link_class' => '',
        'alt' => '',
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
      'exclude' => 0,
      'id' => 'name',
      'table' => 'cvterm',
      'field' => 'name',
      'relationship' => 'none',
    ),
    'is_obsolete' => array(
      'label' => 'Is Obsolete?',
      'alter' => array(
        'alter_text' => 0,
        'text' => '',
        'make_link' => 0,
        'path' => '',
        'link_class' => '',
        'alt' => '',
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
      'type' => 'yes-no',
      'not' => 0,
      'exclude' => 0,
      'id' => 'is_obsolete',
      'table' => 'map',
      'field' => 'is_obsolete',
      'relationship' => 'none',
    ),
  ));
  $handler->override_option('sorts', array(
    'name' => array(
      'id' => 'name',
      'table' => 'map',
      'field' => 'name',
    ),
  ));
  $handler->override_option('filters', array(
    'common_name' => array(
      'operator' => '=',
      'value' => array(),
      'group' => '0',
      'exposed' => TRUE,
      'expose' => array(
        'use_operator' => 0,
        'operator' => 'common_name_op',
        'identifier' => 'organism_common_name',
        'label' => 'Organism',
        'remember' => 0,
      ),
      'case' => 1,
      'id' => 'common_name',
      'table' => 'organism',
      'field' => 'common_name',
      'relationship' => 'none',
      'values_form_type' => 'select',
      'multiple' => 1,
      'optional' => 0,
      'agg' => array(
        'records_with' => 1,
        'aggregates_with' => 1,
      ),
    ),
    'type_id' => array(
      'operator' => '=',
      'value' => array(),
      'group' => '0',
      'exposed' => TRUE,
      'expose' => array(
        'use_operator' => 0,
        'operator' => 'type_id_op',
        'identifier' => 'type_id',
        'label' => 'Type',
        'remember' => 0,
      ),
      'case' => 1,
      'id' => 'type_id',
      'table' => 'map',
      'field' => 'type_id',
      'relationship' => 'none',
      'values_form_type' => 'select',
      'multiple' => 1,
      'optional' => 0,
      'show_all' => 0,
      'agg' => array(
        'records_with' => 1,
        'aggregates_with' => 0,
      ),
    ),
  ));
  $handler->override_option('access', array(
    'type' => 'perm',
    'perm' => 'access chado_map content',
  ));
  $handler->override_option('cache', array(
    'type' => 'none',
  ));
  $handler->override_option('title', 'Maps');
  $handler->override_option('header', 'Click "Show" to see a list of all maps matching the entered criteria. If you leave a any of the criteria blank then the maps will be not be filtered based on that field. Furthermore, if you leave all criteria blank then all maps will be listed.');
  $handler->override_option('header_format', '2');
  $handler->override_option('header_empty', 0);
  $handler->override_option('empty', 'No maps match the supplied criteria.');
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
      'uniquename' => 'uniquename',
      'name_1' => 'name_1',
      'common_name' => 'common_name',
      'name' => 'name',
      'is_obsolete' => 'is_obsolete',
    ),
    'info' => array(
      'uniquename' => array(
        'sortable' => 1,
        'separator' => '',
      ),
      'name_1' => array(
        'sortable' => 1,
        'separator' => '',
      ),
      'common_name' => array(
        'sortable' => 1,
        'separator' => '',
      ),
      'name' => array(
        'sortable' => 1,
        'separator' => '',
      ),
      'is_obsolete' => array(
        'sortable' => 1,
        'separator' => '',
      ),
    ),
    'default' => 'uniquename',
  ));
  $default_handler = $handler;
  $handler = $view->new_display('page', 'Page', 'page_1');
  $handler->override_option('path', 'chado/maps');
  $handler->override_option('menu', array(
    'type' => 'normal',
    'title' => 'Maps',
    'description' => 'A map is a collection of features of a given type. For example, a cDNA or BAC clone map.',
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
    // Adds map => Node relationship
    $default_handler->override_option('relationships', array(
      'nid' => array(
        'label' => 'Map to Node',
        'required' => 0,
        'id' => 'nid',
        'table' => 'chado_map',
        'field' => 'nid',
        'relationship' => 'none',
      ),
    ));
    // Change analysis.name to have a link to the node
    $fields['name_1']['alter']['make_link'] = 1;
    $fields['name_1']['alter']['path'] = 'node/[nid]';
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
