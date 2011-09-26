<?php

/**
 *  @file
 *  This file contains the basic functions for views integration of
 *  chado/tripal analysis tables. Supplementary functions can be found in
 *  ./views/
 *
 *  Documentation on views integration can be found at 
 *  http://views2.logrus.com/doc/html/index.html.
 */

/**
 * @defgroup tripal_analysis_views Analysis Views Integration
 * @ingroup views
 * @ingroup tripal_analysis
 */

require('views/analysis.views.inc');
require('views/chado_analysis.views.inc');
require('views/misc_tables.views.inc'); 
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
 * @ingroup tripal_analysis_views
 */
function tripal_analysis_views_data()  {
  $data = array();

  $data = array_merge($data, retrieve_analysis_views_data());
  $data = array_merge($data, retrieve_chado_analysis_views_data());
  $data = array_merge($data, retrieve_analysis_misc_tables_views_data());

  return $data;
}

/**
 * Implements hook_views_handlers()
 *
 * Purpose: Register all custom handlers with views
 *   where a handler describes either "the type of field", 
 *   "how a field should be filtered", "how a field should be sorted"
 *
 * @return
 *   An array of handler definitions
 *
 * @ingroup tripal_analysis_views
 */
function tripal_analysis_views_views_handlers() {
  return array(
    'info' => array(
      'path' => drupal_get_path('module', 'tripal_analysis') . '/views/handlers',
    ),
    'handlers' => array(
      'views_handler_field_computed_analysis_nid' => array(
        'parent' => 'views_handler_field_numeric',
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
function tripal_analysis_views_data_alter(&$data) {

  if( !(is_array($db_url) and array_key_exists('chado',$db_url)) ){

    // Add featuer relationship to node
    $data['node']['analysis_chado_nid'] = array(
      'group' => 'Analysis',
      'title' => 'Analysis Node',
      'help' => 'Links Chado analysis Fields/Data to the Nodes in the current View.',
      'real field' => 'nid',
      'relationship' => array(
        'handler' => 'views_handler_relationship',
        'title' => t('Node => Chado'),
        'label' => t('Node => Chado'),
        'real field' => 'nid',
        'base' => 'chado_analysis',
        'base field' => 'nid'
      ),
    );
  }
  
}

/**
 *
 *
 * @ingroup tripal_analysis_views
 */
function tripal_analysis_views_default_views () {
  $views = array();
  
  // Main default view
  // List all cvterms based on cv
  $view = new view;
  $view->name = 'all_analysis';
  $view->description = 'A listing of all analysis\'';
  $view->tag = 'chado';
  $view->view_php = '';
  $view->base_table = 'analysis';
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
      'link_to_node' => 1,
      'exclude' => 0,
      'id' => 'name',
      'table' => 'analysis',
      'field' => 'name',
      'relationship' => 'none',
    ),
    'algorithm' => array(
      'label' => 'Algorithm',
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
      'id' => 'algorithm',
      'table' => 'analysis',
      'field' => 'algorithm',
      'relationship' => 'none',
    ),
    'program' => array(
      'label' => 'Program',
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
      'id' => 'program',
      'table' => 'analysis',
      'field' => 'program',
      'relationship' => 'none',
    ),
    'programversion' => array(
      'label' => 'Program Version',
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
      'id' => 'programversion',
      'table' => 'analysis',
      'field' => 'programversion',
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
      'table' => 'analysis',
      'field' => 'description',
      'relationship' => 'none',
    ),
    'sourcename' => array(
      'label' => 'Source Name',
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
      'id' => 'sourcename',
      'table' => 'analysis',
      'field' => 'sourcename',
      'relationship' => 'none',
    ),
    'sourceuri' => array(
      'label' => 'Source URL',
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
      'id' => 'sourceuri',
      'table' => 'analysis',
      'field' => 'sourceuri',
      'relationship' => 'none',
    ),
    'sourceversion' => array(
      'label' => 'Source Version',
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
      'id' => 'sourceversion',
      'table' => 'analysis',
      'field' => 'sourceversion',
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
      'table' => 'analysis',
      'field' => 'num_features',
      'relationship' => 'none',
    ),
    'timeexecuted' => array(
      'label' => 'Time Executed',
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
      'date_format' => 'large',
      'custom_date_format' => '',
      'exclude' => 0,
      'id' => 'timeexecuted',
      'table' => 'analysis',
      'field' => 'timeexecuted',
      'relationship' => 'none',
    ),
  ));
  $handler->override_option('filters', array(
    'program' => array(
      'operator' => '=',
      'value' => 'All',
      'group' => '0',
      'exposed' => TRUE,
      'expose' => array(
        'use_operator' => 0,
        'operator' => 'program_op',
        'identifier' => 'program',
        'label' => 'Program',
        'optional' => 1,
        'remember' => 0,
      ),
      'case' => 0,
      'id' => 'program',
      'table' => 'analysis',
      'field' => 'program',
      'relationship' => 'none',
    ),
    'sourcename' => array(
      'operator' => 'contains',
      'value' => '',
      'group' => '0',
      'exposed' => TRUE,
      'expose' => array(
        'use_operator' => 0,
        'operator' => 'sourcename_op',
        'identifier' => 'sourcename',
        'label' => 'Source Name Contains',
        'optional' => 1,
        'remember' => 0,
      ),
      'case' => 0,
      'id' => 'sourcename',
      'table' => 'analysis',
      'field' => 'sourcename',
      'relationship' => 'none',
    ),
  ));
  $handler->override_option('access', array(
    'type' => 'perm',
    'perm' => 'access chado_analysis content',
  ));
  $handler->override_option('cache', array(
    'type' => 'none',
  ));
  $handler->override_option('title', 'Analysis');
  $handler->override_option('empty', 'No analysis match the supplied criteria.');
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
      'algorithm' => 'algorithm',
      'description' => 'description',
      'name' => 'name',
      'num_features' => 'num_features',
      'program' => 'program',
      'programversion' => 'programversion',
      'sourcename' => 'sourcename',
      'sourceuri' => 'sourceuri',
      'sourceversion' => 'sourceversion',
      'timeexecuted' => 'timeexecuted',
    ),
    'info' => array(
      'algorithm' => array(
        'sortable' => 1,
        'separator' => '',
      ),
      'description' => array(
        'sortable' => 1,
        'separator' => '',
      ),
      'name' => array(
        'sortable' => 1,
        'separator' => '',
      ),
      'num_features' => array(
        'separator' => '',
      ),
      'program' => array(
        'sortable' => 1,
        'separator' => '',
      ),
      'programversion' => array(
        'sortable' => 1,
        'separator' => '',
      ),
      'sourcename' => array(
        'sortable' => 1,
        'separator' => '',
      ),
      'sourceuri' => array(
        'sortable' => 1,
        'separator' => '',
      ),
      'sourceversion' => array(
        'sortable' => 1,
        'separator' => '',
      ),
      'timeexecuted' => array(
        'sortable' => 1,
        'separator' => '',
      ),
    ),
    'default' => 'timeexecuted',
  ));
  $handler = $view->new_display('page', 'Page', 'page_1');
  $handler->override_option('path', 'analyses');
  $handler->override_option('menu', array(
    'type' => 'normal',
    'title' => 'Analyses',
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
