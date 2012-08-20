<?php

/**
 *  @file
 *  This file contains the basic functions for views integration of
 *  chado/tripal_contact tables. Supplementary functions can be found in
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
 *
 * @ingroup tripal_contact
 */
function tripal_contact_views_data()  {
  $data = array();

  if (module_exists('tripal_views')) {

    // Base Table: contact
    $tablename = 'contact';
    if (!tripal_views_is_integrated($tablename, 9)) {
      $table_integration_array = tripal_views_get_integration_array_for_chado_table($tablename, TRUE);
      $status = tripal_views_integration_add_entry($table_integration_array);
        if (!$status) {
          drupal_set_message(t('Tripal Contact is unable to integrate %table', array('%table' => $tablename)), 'error');
        }
    }

    // Additional tables
    $tables = array(
      'contact_relationship',
    );
    foreach ($tables as $tablename) {
      if (!tripal_views_is_integrated($tablename, 9)) {
        $table_integration_array = tripal_views_get_integration_array_for_chado_table($tablename, FALSE);
        $status = tripal_views_integration_add_entry($table_integration_array);
        if (!$status) {
          drupal_set_message(t('Tripal Contact is unable to integrate %table', array('%table' => $tablename)), 'error');
        }
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
 *
 * @ingroup tripal_contact
 */
function tripal_contact_views_handlers() {
  return array(
   'info' => array(
     'path' => drupal_get_path('module', 'tripal_contact') . '/views/handlers',
  ),
   'handlers' => array(

  ),
  );
}

/**
 *
 * @ingroup tripal_feature_views
 */
function tripal_contact_views_default_views() {
  $views = array();

  if (!module_exists('tripal_views')) {
    return $views;
  }

  // Main default view
  $view = new view;
  $view->name = 'contact_listing';
  $view->description = 'A listing of chado contacts';
  $view->tag = 'chado default';
  $view->base_table = 'contact';
  $view->core = 6;
  $view->api_version = '2';
  $view->disabled = TRUE; /* Edit this to true to make a default view disabled initially */
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
      'table' => 'contact',
      'field' => 'name',
      'relationship' => 'none',
    ),
    'name_1' => array(
      'label' => 'Type',
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
      'table' => 'contact',
      'field' => 'description',
      'relationship' => 'none',
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
      'no_results_text' => 'Click "Show" to see a list of all contacts matching the entered criteria. If you leave a any of the criteria blank then the contacts will be not be filtered based on that field. Furthermore, if you leave all criteria blank then all contacts will be listed.',
    ),
    'type_id' => array(
      'operator' => '=',
      'value' => '',
      'group' => '0',
      'exposed' => TRUE,
      'expose' => array(
        'use_operator' => 0,
        'operator' => 'type_id_op',
        'identifier' => 'type_id',
        'label' => 'Type',
        'remember' => 0,
      ),
      'case' => 1,
      'id' => 'type_id',
      'table' => 'contact',
      'field' => 'type_id',
      'relationship' => 'none',
      'values_form_type' => 'select',
      'multiple' => 1,
      'optional' => 0,
      'show_all' => 0,
      'agg' => array(
        'records_with' => 1,
        'aggregates_with' => 1,
      ),
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
        'remember' => 0,
      ),
      'case' => 0,
      'id' => 'name',
      'table' => 'contact',
      'field' => 'name',
      'relationship' => 'none',
      'agg' => array(
        'records_with' => 1,
        'aggregates_with' => 1,
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
      'table' => 'contact',
      'field' => 'description',
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
  $handler->override_option('title', 'Contacts');
  $handler->override_option('header', 'Click "Show" to see a list of all contacts matching the entered criteria. If you leave a any of the criteria blank then the contacts will be not be filtered based on that field. Furthermore, if you leave all criteria blank then all contacts will be listed.');
  $handler->override_option('header_format', '2');
  $handler->override_option('header_empty', 0);
  $handler->override_option('empty', 'No contacts match the current criteria.');
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
      'name_1' => 'name_1',
      'description' => 'description',
    ),
    'info' => array(
      'name' => array(
        'sortable' => 1,
        'separator' => '',
      ),
      'name_1' => array(
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
  $handler = $view->new_display('page', 'Page', 'page_1');
  $handler->override_option('path', 'chado/contacts');
  $handler->override_option('menu', array(
    'type' => 'normal',
    'title' => 'Contacts',
    'description' => 'Contacts can be persons, institutes, groups, or organizations.',
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
