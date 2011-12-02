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

require_once('views/organism.views.inc');
require_once('views/chado_organism.views.inc'); 
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
  
  $data = array_merge($data, retrieve_organism_views_data());
  $data = array_merge($data, retrieve_chado_organism_views_data());
  
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

  if( !(is_array($db_url) and array_key_exists('chado',$db_url)) ){

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
function tripal_organism_views_default_views () {
  $views = array();
  
  // Main default view
  // List all cvterms based on cv
  $view = new view;
  $view->name = 'all_organisms';
  $view->description = 'A listing of all organism in chado';
  $view->tag = 'chado';
  $view->view_php = '';
  $view->base_table = 'organism';
  $view->is_cacheable = FALSE;
  $view->api_version = 2;
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
      'table' => 'organism',
      'field' => 'num_features',
      'relationship' => 'none',
    ),
    'num_stocks' => array(
      'label' => 'Number of Stocks',
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
      'id' => 'num_stocks',
      'table' => 'organism',
      'field' => 'num_stocks',
      'relationship' => 'none',
    ),
    'num_libraries' => array(
      'label' => 'Number of Libraries',
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
      'id' => 'num_libraries',
      'table' => 'organism',
      'field' => 'num_libraries',
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
  $handler->override_option('empty', 'No organism currently in your chado database.');
  $handler->override_option('empty_format', '1');
  $handler->override_option('items_per_page', 0);
  $handler->override_option('style_plugin', 'table');
  $handler->override_option('style_options', array(
    'grouping' => '',
    'override' => 1,
    'sticky' => 0,
    'order' => 'asc',
    'columns' => array(
      'abbreviation' => 'abbreviation',
      'common_name' => 'common_name',
      'genus' => 'genus',
      'species' => 'species',
    ),
    'info' => array(
      'abbreviation' => array(
        'sortable' => 0,
        'separator' => '',
      ),
      'common_name' => array(
        'sortable' => 0,
        'separator' => '',
      ),
      'genus' => array(
        'sortable' => 0,
        'separator' => '',
      ),
      'species' => array(
        'sortable' => 0,
        'separator' => '',
      ),
    ),
    'default' => '-1',
  ));
  $handler = $view->new_display('page', 'Page', 'page_1');
  $handler->override_option('path', 'organisms');
  $handler->override_option('menu', array(
    'type' => 'normal',
    'title' => 'Organisms',
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
