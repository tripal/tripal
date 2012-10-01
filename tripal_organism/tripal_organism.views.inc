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
 * @defgroup tripal_organism_views Organism Views Integration
 * @ingroup views
 * @ingroup tripal_organism
 */

/**
 * Implements hook_views_data()
 * Purpose: Describe chado/tripal tables & fields to views
 * @return: a data array which follows the structure outlined in the
 *   views2 documentation for this hook. Essentially, it's an array of table
 *   definitions keyed by chado/tripal table name. Each table definition
 *   includes basic details about the table, fields in that table and
 *   relationships between that table and others (joins)
 *
 * @ingroup tripal_organism_views
 */
function tripal_organism_views_data()  {
  $data = array();

  if (module_exists('tripal_views')) {
    // Base Table
    $tables = array(
      'organism'
    );
    foreach ($tables as $tablename) {
      $priority = 9;
      
      // check to see if the table is integrated. If it is then integrate it's
      // corresponding 'chado_[table]' table.
      if (!tripal_views_is_integrated($tablename, $priority)) {
        $table_integration_array = tripal_views_get_integration_array_for_chado_table($tablename, TRUE, $priority);

        // Add specialty handlers
        $table_integration_array['fields']['common_name']['handlers']['filter']['name'] = 'tripal_views_handler_filter_select_string';
        $table_integration_array['fields']['genus']['handlers']['filter']['name'] = 'tripal_views_handler_filter_select_string';
        $table_integration_array['fields']['species']['handlers']['filter']['name'] = 'tripal_views_handler_filter_select_string';
        $table_integration_array['fields']['abbreviation']['handlers']['filter']['name'] = 'tripal_views_handler_filter_select_string';

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

    // Additional Tables
    $tables = array(
      'organismprop',
      'organism_dbxref'
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

/**
 * Implements hook_views_handlers()
 * Purpose: Register all custom handlers with views
 *   where a handler describes either "the type of field",
 *   "how a field should be filtered", "how a field should be sorted"
 * @return: An array of handler definitions
 *
 * @ingroup tripal_organism_views
 */
function tripal_organism_views_handlers() {
  return array(
    'info' => array(
      'path' => drupal_get_path('module', 'tripal_organism') . '/views/handlers',
    ),
    'handlers' => array(
      'views_handler_field_computed_organism_nid' => array(
        'parent' => 'views_handler_field_numeric',
      ),
      'views_handler_filter_organism_common_name' => array(
      'parent' => 'views_handler_filter_string',
    ),
    ),
  );
}

/**
 * Implementation of hook_views_data_alter().
 */
function tripal_organism_views_data_alter(&$data) {

  if ( !(is_array($db_url) and array_key_exists('chado', $db_url)) ) {

    // Add featuer relationship to node
    $data['node']['organism_chado_nid'] = array(
      'group' => 'Organism',
      'title' => 'Organism Node',
      'help' => 'Links Chado Organism Fields/Data to the Nodes in the current View.',
      'real field' => 'nid',
      'relationship' => array(
        'handler' => 'views_handler_relationship',
        'title' => t('Node => Chado'),
        'label' => t('Node => Chado'),
        'real field' => 'nid',
        'base' => 'chado_organism',
        'base field' => 'nid'
      ),
    );
  }

}

/**
 *
 * @ingroup tripal_organism_views
 */
function tripal_organism_views_default_views() {
  $views = array();

  if (!module_exists('tripal_views')) {
    return $views;
  }

  // Main default view
  $view = new view;
  $view->name = 'organism_listing';
  $view->description = 'A listing of all organism in chado';
  $view->tag = 'chado default';
  $view->base_table = 'organism';
  $view->core = 0;
  $view->api_version = '2';
  $view->disabled = FALSE; /* Edit this to true to make a default view disabled initially */
  $handler = $view->new_display('default', 'Defaults', 'default');
  $handler->override_option('fields', array(
    'common_name' => array(
      'label' => 'Common Name',
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
    'genus' => array(
      'label' => 'Genus',
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
      'id' => 'genus',
      'table' => 'organism',
      'field' => 'genus',
      'relationship' => 'none',
    ),
    'species' => array(
      'label' => 'Species',
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
      'id' => 'species',
      'table' => 'organism',
      'field' => 'species',
      'relationship' => 'none',
    ),
    'abbreviation' => array(
      'label' => 'Abbreviation',
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
      'link_to_node' => 0,
      'exclude' => 0,
      'id' => 'abbreviation',
      'table' => 'organism',
      'field' => 'abbreviation',
      'relationship' => 'none',
    ),
  ));
  $handler->override_option('sorts', array(
    'genus' => array(
      'order' => 'ASC',
      'id' => 'genus',
      'table' => 'organism',
      'field' => 'genus',
      'relationship' => 'none',
    ),
    'species' => array(
      'order' => 'ASC',
      'id' => 'species',
      'table' => 'organism',
      'field' => 'species',
      'relationship' => 'none',
    ),
  ));
  $handler->override_option('access', array(
    'type' => 'perm',
    'perm' => 'access chado_organism content',
  ));
  $handler->override_option('cache', array(
    'type' => 'none',
  ));
  $handler->override_option('title', 'Organisms');
  $handler->override_option('empty', 'No organisms matched the supplied criteria.');
  $handler->override_option('empty_format', '1');
  $handler->override_option('items_per_page', 0);
  $handler->override_option('style_plugin', 'table');
  $handler->override_option('style_options', array(
    'grouping' => '',
    'override' => 1,
    'sticky' => 0,
    'order' => 'asc',
    'summary' => '',
    'columns' => array(
      'common_name' => 'common_name',
      'genus' => 'genus',
      'species' => 'species',
      'abbreviation' => 'abbreviation',
    ),
    'info' => array(
      'common_name' => array(
        'sortable' => 1,
        'separator' => '',
      ),
      'genus' => array(
        'sortable' => 1,
        'separator' => '',
      ),
      'species' => array(
        'sortable' => 1,
        'separator' => '',
      ),
      'abbreviation' => array(
        'sortable' => 1,
        'separator' => '',
      ),
    ),
    'default' => '-1',
  ));
  $default_handler = $handler;
  $handler = $view->new_display('page', 'Page', 'page_1');
  $handler->override_option('path', 'chado/organisms');
  $handler->override_option('menu', array(
    'type' => 'normal',
    'title' => 'Organisms',
    'description' => 'A biological organism.',
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
    // Adds organism => Node relationship
    $default_handler->override_option('relationships', array(
      'nid' => array(
        'label' => 'Organism to Node',
        'required' => 0,
        'id' => 'nid',
        'table' => 'chado_organism',
        'field' => 'nid',
        'relationship' => 'none',
      ),
    ));
    // Change analysis.name to have a link to the node
    $fields['common_name']['alter']['make_link'] = 1;
    $fields['common_name']['alter']['path'] = 'node/[nid]';
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
