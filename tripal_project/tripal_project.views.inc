<?php

/**
 *  @file
 *  This file contains the basic functions for views integration of
 *  chado/tripal_project tables. Supplementary functions can be found in
 *  ./views/
 *
 *  Documentation on views integration can be found at
 *  http://views2.logrus.com/doc/html/index.html.
 */

/**
 * @defgroup tripal_project_views Project Views Integration
 * @ingroup views
 */

/*************************************************************************
 * Implements hook_views_data()
 * Purpose: Describe chado/tripal tables & fields to views
 *
 * @return: a data array which follows the structure outlined in the
 *   views2 documentation for this hook. Essentially, it's an array of table
 *   definitions keyed by chado/tripal table name. Each table definition
 *   includes basic details about the table, fields in that table and
 *   relationships between that table and others (joins)
 */
function tripal_project_views_data()  {
  $data = array();

  if (module_exists('tripal_views')) {
    $tables = array(
      'project'
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
 *
 * @return: An array of handler definitions
 */
function tripal_project_views_handlers() {
  return array(
   'info' => array(
     'path' => drupal_get_path('module', 'tripal_project') . '/views/handlers',
  ),
   'handlers' => array(

  ),
  );
}

/**
 *
 * @ingroup tripal_feature_views
 */
function tripal_project_views_default_views() {
  $views = array();

  if (!module_exists('tripal_views')) {
    return $views;
  }

  $description_type_id = tripal_core_chado_select('cvterm',array('cvterm_id'),array('name' => 'project_description'));
  $description_type_id = $description_type_id[0]->cvterm_id;

  // Main default view
  $view = new view;
  $view->name = 'project_listing';
  $view->description = 'A listing of chado projects';
  $view->tag = 'chado default';
  $view->base_table = 'project';
  $view->core = 6;
  $view->api_version = '2';
  $view->disabled = FALSE;
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
      'id' => 'name',
      'table' => 'project',
      'field' => 'name',
      'relationship' => 'none',
    ),
    'value' => array(
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
      'id' => 'value',
      'table' => 'projectprop',
      'field' => 'value',
      'override' => array(
        'button' => 'Override',
      ),
      'relationship' => 'none',
    ),
  ));
  $handler->override_option('filters', array(
    'name' => array(
      'operator' => '~',
      'value' => '',
      'group' => '0',
      'exposed' => TRUE,
      'expose' => array(
        'use_operator' => 0,
        'operator' => 'name_op',
        'identifier' => 'name',
        'label' => 'Name Contains',
        'remember' => 0,
      ),
      'case' => 0,
      'id' => 'name',
      'table' => 'project',
      'field' => 'name',
      'relationship' => 'none',
      'agg' => array(
        'records_with' => 1,
        'aggregates_with' => 1,
      ),
    ),
    'type_id' => array(
      'operator' => '=',
      'value' => $description_type_id,
      'group' => '0',
      'exposed' => FALSE,
      'expose' => array(
        'operator' => FALSE,
        'label' => '',
        'optional' => FALSE,
      ),
      'case' => 1,
      'id' => 'type_id',
      'table' => 'projectprop',
      'field' => 'type_id',
      'override' => array(
        'button' => 'Override',
      ),
      'relationship' => 'none',
      'values_form_type' => 'select',
      'multiple' => 0,
      'optional' => 0,
      'show_all' => 0,
      'agg' => array(
        'records_with' => 0,
        'aggregates_with' => 1,
      ),
    ),
    'value' => array(
      'operator' => '~',
      'value' => '',
      'group' => '0',
      'exposed' => TRUE,
      'expose' => array(
        'use_operator' => 0,
        'operator' => 'value_op',
        'identifier' => 'description',
        'label' => 'Description Contains',
        'remember' => 0,
      ),
      'case' => 0,
      'id' => 'value',
      'table' => 'projectprop',
      'field' => 'value',
      'override' => array(
        'button' => 'Override',
      ),
      'relationship' => 'none',
      'agg' => array(
        'records_with' => 1,
        'aggregates_with' => 1,
      ),
    ),
  ));
  $handler->override_option('access', array(
    'type' => 'perm',
    'perm' => 'access chado_projects',
  ));
  $handler->override_option('cache', array(
    'type' => 'none',
  ));
  $handler->override_option('title', 'Projects');
  $handler->override_option('empty', 'No projects match the supplied criteria.');
  $handler->override_option('empty_format', '2');
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
      'description' => 'description',
    ),
    'info' => array(
      'name' => array(
        'sortable' => 1,
        'separator' => '',
      ),
      'description' => array(
        'sortable' => 0,
        'separator' => '',
      ),
    ),
    'default' => 'name',
  ));
  $default_handler = $handler;
  $handler = $view->new_display('page', 'Page', 'page_1');
  $handler->override_option('path', 'chado/projects');
  $handler->override_option('menu', array(
    'type' => 'normal',
    'title' => 'Projects',
    'description' => 'A project is a collection of data resulting from a biological experiment.',
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
    // Adds project => Node relationship
    $default_handler->override_option('relationships', array(
      'nid' => array(
        'label' => 'Project to Node',
        'required' => 0,
        'id' => 'nid',
        'table' => 'chado_project',
        'field' => 'nid',
        'relationship' => 'none',
      ),
    ));
    // Change project.name to have a link to the node
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
