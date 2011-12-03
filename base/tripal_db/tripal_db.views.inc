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

require_once('views/dbxref.views.inc');
require_once('views/db.views.inc'); 
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
   $data = array_merge($data, retrieve_dbxref_views_data());
   $data = array_merge($data, retrieve_db_views_data());
   return $data;

}

/**
 * Implements hook_views_handlers()
 * Purpose: Register all custom handlers with views
 *   where a handler describes either "the type of field", 
 *   "how a field should be filtered", "how a field should be sorted"
 * @return: An array of handler definitions
 *
 * @ingroup tripal_db_views
 */
function tripal_db_views_handlers() {
 return array(
   'info' => array(
     'path' => drupal_get_path('module', 'tripal_db') . '/views/handlers',
   ),
   'handlers' => array(
     'views_handler_field_dbxref_accession_link' => array(
       'parent' => 'views_handler_field',
     ),
   ),
 );
}

/**
 *
 * @ingroup tripal_db_views
 */
function tripal_db_views_default_views () {
  $views = array();
  
  // Main default view
  // List all cvterms based on cv
  $view = new view;
  $view = new view;
  $view->name = 'all_dbxrefs';
  $view->description = 'A listing of all database references filtered by database';
  $view->tag = 'chado';
  $view->view_php = '';
  $view->base_table = 'dbxref';
  $view->is_cacheable = FALSE;
  $view->api_version = 2;
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
    'accession_link' => array(
      'label' => 'External Link',
      'alter' => array(
        'alter_text' => 1,
        'text' => 'link',
        'make_link' => 1,
        'path' => '[accession_link]',
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
      'hide_empty' => 1,
      'empty_zero' => 0,
      'exclude' => 0,
      'id' => 'accession_link',
      'table' => 'dbxref',
      'field' => 'accession_link',
      'relationship' => 'none',
    ),
  ));
  $handler->override_option('filters', array(
    'name' => array(
      'operator' => '=',
      'value' => '<select db>',
      'group' => '0',
      'exposed' => TRUE,
      'expose' => array(
        'use_operator' => 0,
        'operator' => 'name_op',
        'identifier' => 'db',
        'label' => 'Database',
        'optional' => 1,
        'remember' => 0,
      ),
      'case' => 1,
      'id' => 'name',
      'table' => 'db',
      'field' => 'name',
      'relationship' => 'none',
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
        'optional' => 1,
        'remember' => 0,
      ),
      'case' => 0,
      'id' => 'accession',
      'table' => 'dbxref',
      'field' => 'accession',
      'relationship' => 'none',
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
  $views[$view->name] = $view;
  
  return $views;
}
