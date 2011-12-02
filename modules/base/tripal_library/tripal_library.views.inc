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
 * @defgroup tripal_library_views Library Views Integration
 * @ingroup views
 * @ingroup tripal_library
 */

require_once('views/library.views.inc');
require_once('views/chado_library.views.inc');
require_once('views/misc_tables.views.inc'); 
/*************************************************************************
 * Implements hook_views_data()
 * Purpose: Describe chado/tripal tables & fields to views
 * @return: a data array which follows the structure outlined in the
 *   views2 documentation for this hook. Essentially, it's an array of table
 *   definitions keyed by chado/tripal table name. Each table definition 
 *   includes basic details about the table, fields in that table and
 *   relationships between that table and others (joins)
 *
 * @ingroup tripal_library_views
 */
function tripal_library_views_data()  {
  $data = array();
  
  $data = array_merge($data, retrieve_library_views_data());
  $data = array_merge($data, retrieve_chado_library_views_data());
  $data = array_merge($data, retrieve_library_misc_tables_views_data());
  
  return $data;
}

/*************************************************************************
 * Implements hook_views_handlers()
 * Purpose: Register all custom handlers with views
 *   where a handler describes either "the type of field", 
 *   "how a field should be filtered", "how a field should be sorted"
 * @return: An array of handler definitions
 *
 * @ingroup tripal_library_views
 */
function tripal_library_views_handlers() {
 return array(
   'info' => array(
     'path' => drupal_get_path('module', 'tripal_library') . '/views/handlers',
   ),
   'handlers' => array(
     'views_handler_field_computed_library_nid' => array(
       'parent' => 'views_handler_field_numeric',
     ),
     'views_handler_field_tf_boolean' => array(
       'parent' => 'views_handler_field',
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
function tripal_library_views_data_alter(&$data) {

  if( !(is_array($db_url) and array_key_exists('chado',$db_url)) ){

    // Add featuer relationship to node
    $data['node']['library_chado_nid'] = array(
      'group' => 'Library',
      'title' => 'Library Node',
      'help' => 'Links Chado Library Fields/Data to the Nodes in the current View.',
      'real field' => 'nid',
      'relationship' => array(
        'handler' => 'views_handler_relationship',
        'title' => t('Node => Chado'),
        'label' => t('Node => Chado'),
        'real field' => 'nid',
        'base' => 'chado_library',
        'base field' => 'nid'
      ),
    );
  }
  
}

/**
 *
 *
 * @ingroup tripal_library_views
 */
function tripal_library_views_default_views () {
  $views = array();
  
  // Main default view
  // List all cvterms based on cv
  $view = new view;
  $view->name = 'all_libraries';
  $view->description = 'A listing of all libraries';
  $view->tag = 'chado';
  $view->view_php = '';
  $view->base_table = 'library';
  $view->is_cacheable = FALSE;
  $view->api_version = 2;
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
      'table' => 'library',
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
      'table' => 'library',
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
      'table' => 'library',
      'field' => 'is_obsolete',
      'relationship' => 'none',
    ),
    'num_features' => array(
      'label' => 'Number of Features',
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
      'id' => 'num_features',
      'table' => 'library',
      'field' => 'num_features',
      'relationship' => 'none',
    ),
  ));
  $handler->override_option('filters', array(
    'common_name' => array(
      'operator' => '=',
      'value' => 'All',
      'group' => '0',
      'exposed' => TRUE,
      'expose' => array(
        'use_operator' => 0,
        'operator' => 'common_name_op',
        'identifier' => 'organism_common_name',
        'label' => 'Organism',
        'optional' => 1,
        'remember' => 0,
      ),
      'case' => 1,
      'id' => 'common_name',
      'table' => 'organism',
      'field' => 'common_name',
      'relationship' => 'none',
    ),
  ));
  $handler->override_option('access', array(
    'type' => 'perm',
    'perm' => 'access chado_library content',
  ));
  $handler->override_option('cache', array(
    'type' => 'none',
  ));
  $handler->override_option('title', 'Libraries');
  $handler->override_option('empty', 'There are no libraries matching the current criteria. If you think there should be, check that all chado libraries are sync\'d with your website.');
  $handler->override_option('empty_format', '1');
  $handler->override_option('use_pager', '1');
  $handler->override_option('style_plugin', 'table');
  $handler->override_option('style_options', array(
    'grouping' => '',
    'override' => 1,
    'sticky' => 0,
    'order' => 'asc',
    'columns' => array(
      'name' => 'name',
      'timeaccessioned' => 'timeaccessioned',
      'timelastmodified' => 'timelastmodified',
      'is_obsolete' => 'is_obsolete',
      'name_1' => 'name_1',
      'uniquename' => 'uniquename',
      'common_name' => 'common_name',
      'num_libraries' => 'num_libraries',
    ),
    'info' => array(
      'name' => array(
        'sortable' => 0,
        'separator' => '',
      ),
      'timeaccessioned' => array(
        'sortable' => 0,
        'separator' => '',
      ),
      'timelastmodified' => array(
        'sortable' => 0,
        'separator' => '',
      ),
      'is_obsolete' => array(
        'sortable' => 0,
        'separator' => '',
      ),
      'name_1' => array(
        'sortable' => 0,
        'separator' => '',
      ),
      'uniquename' => array(
        'sortable' => 0,
        'separator' => '',
      ),
      'common_name' => array(
        'sortable' => 0,
        'separator' => '',
      ),
      'num_libraries' => array(
        'separator' => '',
      ),
    ),
    'default' => '-1',
  ));
  $handler = $view->new_display('page', 'Page', 'page_1');
  $handler->override_option('path', 'libraries');
  $handler->override_option('menu', array(
    'type' => 'normal',
    'title' => 'Sequence Libraries',
    'description' => '',
    'weight' => '0',
    'name' => 'primary-links',
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
