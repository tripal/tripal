<?php

/**
 *  @file
 *  This file contains the basic functions for views integration of
 *  chado/tripal db tables. Supplementary functions can be found in
 *  ./views/
 *
 *  Documentation on views integration can be found at
 *  http://views2.logrus.com/doc/html/index.html.
 */

/**
 * @defgroup tripal_db_views External Database Views Integration
 * @ingroup views
 * @ingroup tripal_db
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
 * @ingroup tripal_db_views
 */
function tripal_db_views_data()  {
  $data = array();

  if (module_exists('tripal_views')) {
    // Base Tables
    $tables = array(
      'db',
      'dbxref'
    );
    foreach ($tables as $tablename) {
      if (!tripal_views_is_integrated($tablename, 9)) {
        $table_integration_array = tripal_views_get_integration_array_for_chado_table($tablename, TRUE);

        // add specialty handlers
        if ($tablename == 'db') {
          $table_integration_array['fields']['name']['handlers']['filter']['name'] = 'tripal_views_handler_filter_select_string';
        }

        tripal_views_integration_add_entry($table_integration_array);
      }
    }

    // Additional Tables
    $tables = array(
      'dbxrefprop'
    );
    foreach ($tables as $tablename) {
      if (!tripal_views_is_integrated($tablename, 9)) {
        $table_integration_array = tripal_views_get_integration_array_for_chado_table($tablename, FALSE);
        tripal_views_integration_add_entry($table_integration_array);
      }
    }
  }

  return $data;

}

/**
 *
 * @ingroup tripal_db_views
 */
function tripal_db_views_default_views() {
  $views = array();

  if (!module_exists('tripal_views')) {
    return $views;
  }

  // Main DB default view
  $view = new view;
  $view->name = 'db_listing';
  $view->description = 'A listing of all databases.';
  $view->tag = 'chado default';
  $view->base_table = 'db';
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
      'table' => 'db',
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
      'table' => 'db',
      'field' => 'description',
      'relationship' => 'none',
    ),
    'url' => array(
      'label' => 'Url',
      'alter' => array(
        'alter_text' => 0,
        'text' => '',
        'make_link' => 1,
        'path' => '[url]',
        'absolute' => 1,
        'link_class' => '',
        'alt' => '',
        'rel' => '',
        'prefix' => '',
        'suffix' => '',
        'target' => '_blank',
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
      'id' => 'url',
      'table' => 'db',
      'field' => 'url',
      'relationship' => 'none',
    ),
    'urlprefix' => array(
      'label' => 'Urlprefix',
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
      'id' => 'urlprefix',
      'table' => 'db',
      'field' => 'urlprefix',
      'relationship' => 'none',
    ),
  ));
  $handler->override_option('filters', array(
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
        'remember' => 0,
      ),
      'case' => 0,
      'id' => 'name',
      'table' => 'db',
      'field' => 'name',
      'relationship' => 'none',
      'values_form_type' => 'textfield',
      'multiple' => 0,
      'optional' => 0,
      'agg' => array(
        'records_with' => 1,
        'aggregates_with' => 0,
      ),
    ),
    'description' => array(
      'operator' => 'contains',
      'value' => '',
      'group' => '0',
      'exposed' => TRUE,
      'expose' => array(
        'use_operator' => 0,
        'operator' => 'description_op',
        'identifier' => 'description',
        'label' => 'Description Contains',
        'remember' => 0,
      ),
      'case' => 0,
      'id' => 'description',
      'table' => 'db',
      'field' => 'description',
      'relationship' => 'none',
      'agg' => array(
        'records_with' => 1,
        'aggregates_with' => 0,
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
      'no_results_text' => 'Click "Show" to see a list of all databases matching the entered criteria. If you leave a any of the criteria blank then the databases will be not be filtered based on that field. Furthermore, if you leave all criteria blank then all databases will be listed.',
    ),
  ));
  $handler->override_option('access', array(
    'type' => 'perm',
    'perm' => 'access chado_db content',
  ));
  $handler->override_option('cache', array(
    'type' => 'none',
  ));
  $handler->override_option('title', 'Databases');
  $handler->override_option('header', 'Click "Show" to see a list of all databases matching the entered criteria. If you leave a any of the criteria blank then the databases will be not be filtered based on that field. Furthermore, if you leave all criteria blank then all databases will be listed.');
  $handler->override_option('header_format', '2');
  $handler->override_option('header_empty', 0);
  $handler->override_option('empty', 'No databases match the supplied criteria.');
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
      'url' => 'url',
      'urlprefix' => 'urlprefix',
      'description' => 'description',
    ),
    'info' => array(
      'name' => array(
        'sortable' => 1,
        'separator' => '',
      ),
      'url' => array(
        'sortable' => 1,
        'separator' => '',
      ),
      'urlprefix' => array(
        'sortable' => 0,
        'separator' => '',
      ),
      'description' => array(
        'sortable' => 0,
        'separator' => '',
      ),
    ),
    'default' => 'name',
  ));
  $handler = $view->new_display('page', 'Page', 'page_1');
  $handler->override_option('path', 'admin/tripal/tripal_db/list_dbs');
  $handler->override_option('menu', array(
    'type' => 'normal',
    'title' => 'DB Listing',
    'description' => 'Lists all databases available to create database references for.',
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
  $views[$view->name] = $view;

  // Main dbxref default view
  $view = new view;
  $view->name = 'dbxref_listing';
  $view->description = 'A listing of all database references filtered by database';
  $view->tag = 'chado default';
  $view->base_table = 'dbxref';
  $view->core = 0;
  $view->api_version = '2';
  $view->disabled = FALSE; /* Edit this to true to make a default view disabled initially */
  $handler = $view->new_display('default', 'Defaults', 'default');
  $handler->override_option('fields', array(
    'name' => array(
      'label' => 'Database',
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
      'table' => 'db',
      'field' => 'name',
      'relationship' => 'none',
    ),
    'accession' => array(
      'label' => 'Accession',
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
      'id' => 'accession',
      'table' => 'dbxref',
      'field' => 'accession',
      'relationship' => 'none',
    ),
    'description' => array(
      'label' => 'Description',
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
      'id' => 'description',
      'table' => 'dbxref',
      'field' => 'description',
      'relationship' => 'none',
    ),
    'version' => array(
      'label' => 'Version',
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
      'id' => 'version',
      'table' => 'dbxref',
      'field' => 'version',
      'relationship' => 'none',
    ),
    'urlprefix' => array(
      'label' => 'External Link',
      'alter' => array(
        'alter_text' => 1,
        'text' => '[name]:[accession]',
        'make_link' => 1,
        'path' => '[urlprefix][accession]',
        'absolute' => 1,
        'link_class' => '',
        'alt' => '',
        'rel' => '',
        'prefix' => '',
        'suffix' => '',
        'target' => '_blank',
        'help' => '',
        'trim' => 0,
        'max_length' => '',
        'word_boundary' => 1,
        'ellipsis' => 1,
        'html' => 0,
        'strip_tags' => 0,
      ),
      'empty' => '',
      'hide_empty' => 1,
      'empty_zero' => 0,
      'hide_alter_empty' => 1,
      'type' => 'separator',
      'separator' => ', ',
      'exclude' => 0,
      'id' => 'urlprefix',
      'table' => 'db',
      'field' => 'urlprefix',
      'relationship' => 'none',
    ),
  ));
  $handler->override_option('sorts', array(
    'name' => array(
      'order' => 'ASC',
      'id' => 'name',
      'table' => 'db',
      'field' => 'name',
      'relationship' => 'none',
    ),
    'accession' => array(
      'order' => 'ASC',
      'id' => 'accession',
      'table' => 'dbxref',
      'field' => 'accession',
      'relationship' => 'none',
    ),
  ));
  $handler->override_option('filters', array(
    'name' => array(
      'operator' => '=',
      'value' => array(),
      'group' => '0',
      'exposed' => TRUE,
      'expose' => array(
        'use_operator' => 0,
        'operator' => 'name_op',
        'identifier' => 'db',
        'label' => 'Database',
        'remember' => 0,
      ),
      'case' => 1,
      'id' => 'name',
      'table' => 'db',
      'field' => 'name',
      'relationship' => 'none',
      'values_form_type' => 'select',
      'multiple' => 1,
      'optional' => 0,
      'agg' => array(
        'records_with' => 1,
        'aggregates_with' => 1,
      ),
    ),
    'accession' => array(
      'operator' => 'contains',
      'value' => '',
      'group' => '0',
      'exposed' => TRUE,
      'expose' => array(
        'use_operator' => 0,
        'operator' => 'accession_op',
        'identifier' => 'accession',
        'label' => 'Accession Contains',
        'remember' => 0,
      ),
      'case' => 0,
      'id' => 'accession',
      'table' => 'dbxref',
      'field' => 'accession',
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
      'no_results_text' => 'Click "Show" to see a list of all database references matching the entered criteria. If you leave a any of the criteria blank then the database references will be not be filtered based on that field. Furthermore, if you leave all criteria blank then all database references will be listed.',
    ),
  ));
  $handler->override_option('access', array(
    'type' => 'perm',
    'perm' => 'access chado_db content',
  ));
  $handler->override_option('cache', array(
    'type' => 'none',
  ));
  $handler->override_option('title', 'Database References');
  $handler->override_option('header', 'Click "Show" to see a list of all database references matching the entered criteria. If you leave a any of the criteria blank then the database references will be not be filtered based on that field. Furthermore, if you leave all criteria blank then all database references will be listed.');
  $handler->override_option('header_format', '2');
  $handler->override_option('header_empty', 0);
  $handler->override_option('empty', 'There are no database references matching the above criteria. Please select a database in order to display all references to that database.');
  $handler->override_option('empty_format', '1');
  $handler->override_option('items_per_page', 50);
  $handler->override_option('use_pager', '1');
  $handler->override_option('style_plugin', 'table');
  $handler->override_option('style_options', array(
    'grouping' => '',
    'override' => 1,
    'sticky' => 0,
    'order' => 'asc',
    'columns' => array(
      'name' => 'name',
      'accession' => 'accession',
      'description' => 'description',
      'version' => 'version',
    ),
    'info' => array(
      'name' => array(
        'sortable' => 1,
        'separator' => '',
      ),
      'accession' => array(
        'sortable' => 1,
        'separator' => '',
      ),
      'description' => array(
        'sortable' => 1,
        'separator' => '',
      ),
      'version' => array(
        'sortable' => 1,
        'separator' => '',
      ),
    ),
    'default' => '-1',
  ));
  $handler = $view->new_display('page', 'Page', 'page_1');
  $handler->override_option('path', 'admin/tripal/tripal_db/list_dbxrefs');
  $handler->override_option('menu', array(
    'type' => 'normal',
    'title' => 'DB Reference Listing',
    'description' => 'A listing of all database references associated with a given database',
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
  $views[$view->name] = $view;

  return $views;
}
