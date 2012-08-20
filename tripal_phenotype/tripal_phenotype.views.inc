<?php

/**
 *  @file
 *  This file contains the basic functions for views integration of
 *  chado/tripal_phenotype tables. Supplementary functions can be found in
 *  ./views/
 *
 *  Documentation on views integration can be found at
 *  http://views2.logrus.com/doc/html/index.html.
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
function tripal_phenotype_views_data()  {
  $data = array();

  if (module_exists('tripal_views')) {
    $tables = array(
      'phenotype'
    );
    foreach ($tables as $tablename) {
      if (!tripal_views_is_integrated($tablename, 9)) {
        $table_integration_array = tripal_views_get_integration_array_for_chado_table($tablename, TRUE);

        // Add specialty handlers
        switch ($tablename) {
          case 'phenotype':
            $table_integration_array['fields']['attr_id']['handlers']['filter']['name'] = 'tripal_views_handler_filter_select_cvterm';
            $table_integration_array['fields']['assay_id']['handlers']['filter']['name'] = 'tripal_views_handler_filter_select_cvterm';
            $table_integration_array['fields']['cvalue_id']['handlers']['filter']['name'] = 'tripal_views_handler_filter_select_cvterm';
            $table_integration_array['fields']['observable_id']['handlers']['filter']['name'] = 'tripal_views_handler_filter_select_cvterm';
            break;
        }

        tripal_views_integration_add_entry($table_integration_array);
      }
    }

    $tables = array(
      'feature_phenotype',
      'phenotype_cvterm'
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

/*************************************************************************
 * Implements hook_views_handlers()
 * Purpose: Register all custom handlers with views
 *   where a handler describes either "the type of field",
 *   "how a field should be filtered", "how a field should be sorted"
 *
 * @return: An array of handler definitions
 */
function tripal_phenotype_views_handlers() {
  return array(
   'info' => array(
     'path' => drupal_get_path('module', 'tripal_phenotype') . '/views/handlers',
  ),
   'handlers' => array(

  ),
  );
}

/**
 *
 * @ingroup tripal_feature_views
 */
function tripal_phenotype_views_default_views() {
  $views = array();

  if (!module_exists('tripal_views')) {
    return $views;
  }

  // Main default view
  $view = new view;
  $view->name = 'phenotype_listing';
  $view->description = 'A listing of chado phenotypes';
  $view->tag = 'chado default';
  $view->base_table = 'phenotype';
  $view->core = 6;
  $view->api_version = '2';
  $view->disabled = FALSE; /* Edit this to true to make a default view disabled initially */
  $handler = $view->new_display('default', 'Defaults', 'default');
  $handler->override_option('relationships', array(
    'assay_id' => array(
      'label' => 'assay_id to cvterm',
      'required' => 0,
      'id' => 'assay_id',
      'table' => 'phenotype',
      'field' => 'assay_id',
      'relationship' => 'none',
    ),
    'attr_id' => array(
      'label' => 'attr_id to cvterm',
      'required' => 0,
      'id' => 'attr_id',
      'table' => 'phenotype',
      'field' => 'attr_id',
      'relationship' => 'none',
    ),
    'cvalue_id' => array(
      'label' => 'cvalue_id to cvterm',
      'required' => 0,
      'id' => 'cvalue_id',
      'table' => 'phenotype',
      'field' => 'cvalue_id',
      'relationship' => 'none',
    ),
    'observable_id' => array(
      'label' => 'observable_id to cvterm',
      'required' => 0,
      'id' => 'observable_id',
      'table' => 'phenotype',
      'field' => 'observable_id',
      'relationship' => 'none',
    ),
  ));
  $handler->override_option('fields', array(
    'uniquename' => array(
      'label' => 'Unique Name',
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
      'id' => 'uniquename',
      'table' => 'phenotype',
      'field' => 'uniquename',
      'relationship' => 'none',
    ),
    'name_1' => array(
      'label' => 'Phenotypic Attribute',
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
      'id' => 'name_1',
      'table' => 'cvterm',
      'field' => 'name',
      'relationship' => 'attr_id',
    ),
    'name_3' => array(
      'label' => 'Controlled Value',
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
      'id' => 'name_3',
      'table' => 'cvterm',
      'field' => 'name',
      'relationship' => 'cvalue_id',
    ),
    'value' => array(
      'label' => 'Value',
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
      'table' => 'phenotype',
      'field' => 'value',
      'relationship' => 'none',
    ),
    'name_2' => array(
      'label' => 'Observation Type',
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
      'id' => 'name_2',
      'table' => 'cvterm',
      'field' => 'name',
      'relationship' => 'observable_id',
    ),
    'name' => array(
      'label' => 'Evidence Type',
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
      'table' => 'cvterm',
      'field' => 'name',
      'relationship' => 'assay_id',
    ),
  ));
  $handler->override_option('filters', array(
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
      'no_results_text' => 'Click "Show" to see a list of all phenotypes matching the entered criteria. If you leave a any of the criteria blank then the phenotypes will be not be filtered based on that field. Furthermore, if you leave all criteria blank then all phenotypes will be listed.',
    ),
    'attr_id' => array(
      'operator' => '=',
      'value' => '',
      'group' => '0',
      'exposed' => TRUE,
      'expose' => array(
        'use_operator' => 0,
        'operator' => 'attr_id_op',
        'identifier' => 'attr_id',
        'label' => 'Phenotypic Attribute',
        'remember' => 0,
      ),
      'case' => 1,
      'id' => 'attr_id',
      'table' => 'phenotype',
      'field' => 'attr_id',
      'relationship' => 'none',
      'agg' => array(
        'records_with' => 1,
        'aggregates_with' => 1,
      ),
      'values_form_type' => 'select',
      'multiple' => 1,
      'optional' => 0,
      'show_all' => 0,
    ),
    'cvalue_id' => array(
      'operator' => '=',
      'value' => '',
      'group' => '0',
      'exposed' => TRUE,
      'expose' => array(
        'use_operator' => 0,
        'operator' => 'cvalue_id_op',
        'identifier' => 'cvalue_id',
        'label' => 'Controlled Value',
        'remember' => 0,
      ),
      'case' => 1,
      'id' => 'cvalue_id',
      'table' => 'phenotype',
      'field' => 'cvalue_id',
      'relationship' => 'none',
      'agg' => array(
        'records_with' => 1,
        'aggregates_with' => 1,
      ),
      'values_form_type' => 'select',
      'multiple' => 1,
      'optional' => 0,
      'show_all' => 0,
    ),
    'observable_id' => array(
      'operator' => '=',
      'value' => '',
      'group' => '0',
      'exposed' => TRUE,
      'expose' => array(
        'use_operator' => 0,
        'operator' => 'observable_id_op',
        'identifier' => 'observable_id',
        'label' => 'Observation Type',
        'remember' => 0,
      ),
      'case' => 1,
      'id' => 'observable_id',
      'table' => 'phenotype',
      'field' => 'observable_id',
      'relationship' => 'none',
      'agg' => array(
        'records_with' => 1,
        'aggregates_with' => 1,
      ),
      'values_form_type' => 'select',
      'multiple' => 1,
      'optional' => 0,
      'show_all' => 0,
    ),
    'assay_id' => array(
      'operator' => '=',
      'value' => '',
      'group' => '0',
      'exposed' => TRUE,
      'expose' => array(
        'use_operator' => 0,
        'operator' => 'assay_id_op',
        'identifier' => 'assay_id',
        'label' => 'Evidence Type',
        'remember' => 0,
      ),
      'case' => 1,
      'id' => 'assay_id',
      'table' => 'phenotype',
      'field' => 'assay_id',
      'relationship' => 'none',
      'agg' => array(
        'records_with' => 1,
        'aggregates_with' => 1,
      ),
      'values_form_type' => 'select',
      'multiple' => 1,
      'optional' => 0,
      'show_all' => 0,
    ),
    'uniquename' => array(
      'operator' => 'contains',
      'value' => '',
      'group' => '0',
      'exposed' => TRUE,
      'expose' => array(
        'use_operator' => 0,
        'operator' => 'uniquename_op',
        'identifier' => 'uniquename',
        'label' => 'Unique Name Contains',
        'remember' => 0,
      ),
      'case' => 0,
      'id' => 'uniquename',
      'table' => 'phenotype',
      'field' => 'uniquename',
      'relationship' => 'none',
      'agg' => array(
        'records_with' => 1,
        'aggregates_with' => 1,
      ),
    ),
    'value' => array(
      'operator' => 'contains',
      'value' => '',
      'group' => '0',
      'exposed' => TRUE,
      'expose' => array(
        'use_operator' => 0,
        'operator' => 'value_op',
        'identifier' => 'phen_value',
        'label' => 'Value Contains',
        'remember' => 0,
      ),
      'case' => 0,
      'id' => 'value',
      'table' => 'phenotype',
      'field' => 'value',
      'relationship' => 'none',
      'agg' => array(
        'records_with' => 1,
        'aggregates_with' => 1,
      ),
    ),
  ));
  $handler->override_option('access', array(
    'type' => 'perm',
    'perm' => 'access content',
  ));
  $handler->override_option('cache', array(
    'type' => 'none',
  ));
  $handler->override_option('title', 'Phenotypes');
  $handler->override_option('header', 'Click "Show" to see a list of all phenotypes matching the entered criteria. If you leave a any of the criteria blank then the phenotypes will be not be filtered based on that field. Furthermore, if you leave all criteria blank then all phenotypes will be listed.');
  $handler->override_option('header_format', '2');
  $handler->override_option('header_empty', 0);
  $handler->override_option('empty', 'No phenotypes match the current criteria.');
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
      'uniquename' => 'uniquename',
      'value' => 'value',
    ),
    'info' => array(
      'uniquename' => array(
        'sortable' => 1,
        'separator' => '',
      ),
      'value' => array(
        'sortable' => 1,
        'separator' => '',
      ),
    ),
    'default' => 'uniquename',
  ));
  $handler = $view->new_display('page', 'Page', 'page_1');
  $handler->override_option('path', 'chado/phenotypes');
  $handler->override_option('menu', array(
    'type' => 'normal',
    'title' => 'Phenotypes',
    'description' => 'A phenotypic statement, or a single atomic phenotypic observation, is a controlled sentence describing observable effects of non-wild type function.',
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

