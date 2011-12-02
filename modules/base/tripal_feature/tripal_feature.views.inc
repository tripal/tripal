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
 * @defgroup tripal_feature_views Feature Views Integration
 * @ingroup views
 * @ingroup tripal_feature
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
 * @ingroup tripal_feature_views
 */
require_once('views/feature.views.inc');
require_once('views/chado_feature.views.inc');
require_once('views/misc_tables.views.inc');
function tripal_feature_views_data()  {
  $data = array();
  
  $data = array_merge($data, retrieve_feature_views_data());
  $data = array_merge($data, retrieve_chado_feature_views_data());
  $data = array_merge($data, retrieve_feature_misc_tables_views_data());
  
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
 * @ingroup tripal_feature_views
 */
function tripal_feature_views_handlers() {
 return array(
   'info' => array(
     'path' => drupal_get_path('module', 'tripal_feature') . '/views/handlers',
   ),
   'handlers' => array(
     'views_handler_field_computed_feature_nid' => array(
       'parent' => 'views_handler_field_numeric',
     ),
     'views_handler_field_readable_date' => array(
       'parent' => 'views_handler_field',
     ),
     'views_handler_field_residues' => array(
       'parent' => 'views_handler_field',
     ),
   ),
 );
}

/**
 * Implementation of hook_views_data_alter().
 */
function tripal_feature_views_data_alter(&$data) {

  if( !(is_array($db_url) and array_key_exists('chado',$db_url)) ){

    // Add featuer relationship to node
    $data['node']['feature_chado_nid'] = array(
      'group' => 'Feature',
      'title' => 'Feature Node',
      'help' => 'Links Chado Feature Fields/Data to the Nodes in the current View.',
      'real field' => 'nid',
      'relationship' => array(
        'handler' => 'views_handler_relationship',
        'title' => t('Node => Chado'),
        'label' => t('Node => Chado'),
        'real field' => 'nid',
        'base' => 'chado_feature',
        'base field' => 'nid'
      ),
    );
  }
  
}

/**
 *
 * @ingroup tripal_feature_views
 */
function tripal_feature_views_default_views () {
  $views = array();
  
  // Main default view
  // List all cvterms based on cv
  $view = new view;
  $view->name = 'all_features';
  $view->description = 'A listing of all Sequence FEatures';
  $view->tag = 'chado';
  $view->view_php = '';
  $view->base_table = 'feature';
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
      'table' => 'feature',
      'field' => 'uniquename',
      'relationship' => 'none',
    ),
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
      'link_to_node' => 1,
      'exclude' => 0,
      'id' => 'name',
      'table' => 'feature',
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
    'name_3' => array(
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
      'id' => 'name_3',
      'table' => 'cvterm',
      'field' => 'name',
      'relationship' => 'none',
    ),
    'accession_link' => array(
      'label' => 'External Reference',
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
    'name_1' => array(
      'label' => 'Library',
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
    'name_2' => array(
      'label' => 'Analysis',
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
      'id' => 'name_2',
      'table' => 'analysis',
      'field' => 'name',
      'relationship' => 'none',
    ),
  ));
  $handler->override_option('filters', array(
    'common_name' => array(
      'operator' => '=',
      'value' => '<select organism>',
      'group' => '0',
      'exposed' => TRUE,
      'expose' => array(
        'use_operator' => 0,
        'operator' => 'common_name_op',
        'identifier' => 'organism',
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
        'optional' => 1,
        'remember' => 0,
      ),
      'case' => 0,
      'id' => 'name',
      'table' => 'feature',
      'field' => 'name',
      'relationship' => 'none',
    ),
    'name_1' => array(
      'operator' => 'contains',
      'value' => '',
      'group' => '0',
      'exposed' => TRUE,
      'expose' => array(
        'use_operator' => 0,
        'operator' => 'name_1_op',
        'identifier' => 'library',
        'label' => 'Library Name Contains',
        'optional' => 1,
        'remember' => 0,
      ),
      'case' => 0,
      'id' => 'name_1',
      'table' => 'library',
      'field' => 'name',
      'relationship' => 'none',
    ),
    'name_2' => array(
      'operator' => 'contains',
      'value' => '',
      'group' => '0',
      'exposed' => TRUE,
      'expose' => array(
        'use_operator' => 0,
        'operator' => 'name_2_op',
        'identifier' => 'analysis',
        'label' => 'Analysis Name Contains',
        'optional' => 1,
        'remember' => 0,
      ),
      'case' => 0,
      'id' => 'name_2',
      'table' => 'analysis',
      'field' => 'name',
      'relationship' => 'none',
    ),
  ));
  $handler->override_option('access', array(
    'type' => 'perm',
    'perm' => 'access chado_feature content',
  ));
  $handler->override_option('cache', array(
    'type' => 'none',
  ));
  $handler->override_option('title', 'Sequence Features');
  $handler->override_option('empty', 'There are no features matching that criteria. Please select a different organism above.');
  $handler->override_option('empty_format', '1');
  $handler->override_option('items_per_page', 50);
  $handler->override_option('style_plugin', 'table');
  $handler = $view->new_display('page', 'Page', 'page_1');
  $handler->override_option('path', 'features');
  $handler->override_option('menu', array(
    'type' => 'normal',
    'title' => 'Sequence Features',
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
