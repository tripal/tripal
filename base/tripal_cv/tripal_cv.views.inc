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
 * @defgroup tripal_cv_views Controlled Vocabulary Views Integration
 * @ingroup views
 * @ingroup tripal_cv
 */

require_once('views/cvterm.views.inc');
require_once('views/cv.views.inc'); 
/**
 * Implements hook_views_data()
 *
 * Purpose: Describe chado/tripal tables & fields to views
 *
 * @return
 *   a data array which follows the structure outlined in the
 *   views2 documentation for this hook. Essentially, it's an array of table
 *   definitions keyed by chado/tripal table name. Each table definition 
 *   includes basic details about the table, fields in that table and
 *   relationships between that table and others (joins)
 *
 * @ingroup tripal_cv_views
 */
function tripal_cv_views_data() {
  $data = array();
  
  //Function is contained in includes/cvterm.views.inc
  //Returns the data array for the chado cvterm table
  $data = array_merge($data, retrieve_cvterm_views_data());
  
  //Function is contained in includes/cv.views.inc
  //Returns the data array for the chado cv table
  $data = array_merge($data, retrieve_cv_views_data());
  
  return $data;
}
 
/**
 * Implements hook_views_handlers()
 *
 * Purpose: Register all custom handlers with views
 *   where a handler describes either "the type of field", 
 *   "how a field should be filtered", "how a field should be sorted"
 *
 * @return: An array of handler definitions
 *
 * @ingroup tripal_cv_views
 */
function tripal_cv_views_handlers() {
 return array(
   'info' => array(
     'path' => drupal_get_path('module', 'tripal_cv') . '/views/handlers',
   ),
   'handlers' => array(
     'views_handler_field_tf_boolean' => array(
       'parent' => 'views_handler_field',
     ),
   ),
 );
}

/**
 *
 * @ingroup tripal_cv_views
 */
function tripal_cv_views_default_views () {
  $views = array();
  
  // Main default view
  // List all cvterms based on cv
  $view = new view;
  $view->name = 'all_cvterms';
  $view->description = 'A listing of all controlled vocabulary terms filtered by controlled vocabulary';
  $view->tag = 'chado';
  $view->view_php = '';
  $view->base_table = 'cvterm';
  $view->is_cacheable = FALSE;
  $view->api_version = 2;
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
    'definition' => array(
      'label' => 'Definition',
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
      'id' => 'definition',
      'table' => 'cvterm',
      'field' => 'definition',
      'relationship' => 'none',
    ),
    'is_obsolete' => array(
      'label' => 'Is Obsolete',
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
      'table' => 'cvterm',
      'field' => 'is_obsolete',
      'relationship' => 'none',
    ),
    'is_relationshiptype' => array(
      'label' => 'Is Relationship',
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
      'id' => 'is_relationshiptype',
      'table' => 'cvterm',
      'field' => 'is_relationshiptype',
      'relationship' => 'none',
    ),
    'accession_link' => array(
      'label' => 'Accession Link',
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
      'id' => 'accession_link',
      'table' => 'dbxref',
      'field' => 'accession_link',
      'relationship' => 'none',
    ),
  ));
  $handler->override_option('filters', array(
    'name' => array(
      'operator' => '=',
      'value' => '<select cv>',
      'group' => '0',
      'exposed' => TRUE,
      'expose' => array(
        'use_operator' => 0,
        'operator' => 'name_op',
        'identifier' => 'cv',
        'label' => 'Vocabulary',
        'optional' => 0,
        'remember' => 0,
      ),
      'case' => 1,
      'id' => 'name',
      'table' => 'cv',
      'field' => 'name',
      'relationship' => 'none',
    ),
    'name_1' => array(
      'operator' => '~',
      'value' => '',
      'group' => '0',
      'exposed' => TRUE,
      'expose' => array(
        'use_operator' => 0,
        'operator' => '',
        'identifier' => 'name',
        'label' => 'Name Contains',
        'optional' => 1,
        'remember' => 0,
      ),
      'case' => 0,
      'id' => 'name_1',
      'table' => 'cvterm',
      'field' => 'name',
      'relationship' => 'none',
      'values_form_type' => 'textfield',
    ),
    'definition' => array(
      'operator' => 'contains',
      'value' => '',
      'group' => '0',
      'exposed' => TRUE,
      'expose' => array(
        'use_operator' => 0,
        'operator' => 'definition_op',
        'identifier' => 'definition',
        'label' => 'Definition Contains',
        'optional' => 1,
        'remember' => 0,
      ),
      'case' => 0,
      'id' => 'definition',
      'table' => 'cvterm',
      'field' => 'definition',
      'relationship' => 'none',
    ),
  ));
  $handler->override_option('access', array(
    'type' => 'perm',
    'perm' => 'access chado_cv content',
  ));
  $handler->override_option('cache', array(
    'type' => 'none',
  ));
  $handler->override_option('title', 'Controlled Vocabulary Terms');
  $handler->override_option('empty', 'There are no terms associated with the selected controlled vocabulary. Please select a different vocabulary from the list above.');
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
      'definition' => 'definition',
      'is_obsolete' => 'is_obsolete',
      'is_relationshiptype' => 'is_relationshiptype',
      'name' => 'name',
      'accession_link' => 'accession_link',
    ),
    'info' => array(
      'definition' => array(
        'sortable' => 0,
        'separator' => '',
      ),
      'is_obsolete' => array(
        'sortable' => 1,
        'separator' => '',
      ),
      'is_relationshiptype' => array(
        'sortable' => 1,
        'separator' => '',
      ),
      'name' => array(
        'sortable' => 1,
        'separator' => '',
      ),
      'accession_link' => array(
        'sortable' => 1,
        'separator' => '',
      ),
    ),
    'default' => '-1',
  ));
  $handler = $view->new_display('page', 'Page', 'page_1');
  $handler->override_option('path', 'admin/tripal/tripal_cv/list_cvterms');
  $handler->override_option('menu', array(
    'type' => 'normal',
    'title' => 'Term Listing',
    'description' => 'A listing of a controlled vocabulary terms for a given vocabulary',
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