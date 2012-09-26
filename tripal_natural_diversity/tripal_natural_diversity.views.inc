<?php

/**
 *  @file
 *  This file contains the basic functions for views integration of
 *  chado/tripal_natural_diversity tables.
 *
 *  Documentation on views integration can be found at
 *  http://views2.logrus.com/doc/html/index.html.
 */

/**
 * Implements hook_views_data()
 * Purpose: Describe chado/tripal tables & fields to views
 *
 * @return: a data array which follows the structure outlined in the
 *   views2 documentation for this hook. Essentially, it's an array of table
 *   definitions keyed by chado/tripal table name. Each table definition
 *   includes basic details about the table, fields in that table and
 *   relationships between that table and others (joins)
 */
function tripal_natural_diversity_views_data()  {
  $data = array();

  if (module_exists('tripal_views')) {
    $tables = array(
      'nd_experiment',
      'nd_geolocation',
      'nd_protocol',
      'nd_reagent'
    );
    foreach ($tables as $tablename) {
    	// get the setup with the lightest priority. That's the table 
      // that currently integrated with views.
      $priority = tripal_views_get_lightest_priority_setup($tablename);
      
      // check to see if the table is integrated. If it is then integrate it's
      // corresponding 'chado_[table]' table.
      if (!tripal_views_is_integrated($tablename, $priority)) {
        $table_integration_array = tripal_views_get_integration_array_for_chado_table($tablename, TRUE, $priority);

        // Add specialty handlers
        switch ($tablename) {
          case 'nd_geolocation':
            $table_integration_array['fields']['description']['handlers']['filter']['name'] = 'tripal_views_handler_filter_select_string';
            $table_integration_array['fields']['geodetic_datum']['handlers']['filter']['name'] = 'tripal_views_handler_filter_select_string';
            $table_integration_array['fields']['latitude']['handlers']['filter']['name'] = 'chado_views_handler_filter_float';
            $table_integration_array['fields']['longitude']['handlers']['filter']['name'] = 'chado_views_handler_filter_float';
            $table_integration_array['fields']['altitude']['handlers']['filter']['name'] = 'chado_views_handler_filter_float';
            break;
        }

        tripal_views_integration_add_entry($table_integration_array);
      }
    }

    $tables = array(
      'nd_experiment_contact',
      'nd_experiment_dbxref',
      'nd_experiment_genotype',
      'nd_experiment_phenotype',
      'nd_experiment_project',
      'nd_experiment_protocol',
      'nd_experiment_pub',
      'nd_experiment_stock',
      'nd_experiment_stock_dbxref',
      'nd_experiment_stockprop',
      'nd_experimentprop',
      'nd_geolocationprop',
      'nd_protocol_reagent',
      'nd_protocolprop',
      'nd_reagent_relationship',
      'nd_reagentprop'
    );
    foreach ($tables as $tablename) {
    	$priority = tripal_views_get_lightest_priority_setup($tablename);     
      if (!tripal_views_is_integrated($tablename, $priority)) {
        $table_integration_array = tripal_views_get_integration_array_for_chado_table($tablename, FALSE, $priority);
        tripal_views_integration_add_entry($table_integration_array);
      }
    }
  }

  return $data;
}

/**
 * Implements hook_views_handlers()
 * Purpose: Register all custom handlers with views
 *   where a handler describes either "the type of field",
 *   "how a field should be filtered", "how a field should be sorted"
 *
 * @return: An array of handler definitions
 */
function tripal_natural_diversity_views_handlers() {
  return array(
    'info' => array(
      'path' => drupal_get_path('module', 'tripal_natural_diversity') . '/views/handlers',
    ),
    'handlers' => array(
      'views_handler_field_chado_relationship_all' => array(
        'parent' => 'views_handler_field_prerender_list',
      ),
      'views_handler_field_chado_relationship_by_type' => array(
       'parent' => 'views_handler_field_prerender_list',
      ),
    ),
  );
}

/**
 * Created Default views related to the tables integrated by this module
 *
 * @ingroup tripal_feature_views
 */
function tripal_natural_diversity_views_default_views() {
  $views = array();


  if (!module_exists('tripal_views')) {
    return $views;
  }


  // Experiment Listing
  $view = new view;
  $view->name = 'natdiv_experiment_listing';
  $view->description = 'A listing of all natural diversity experiments';
  $view->tag = 'chado default';
  $view->base_table = 'nd_experiment';
  $view->core = 6;
  $view->api_version = '2';
  $view->disabled = FALSE; /* Edit this to true to make a default view disabled initially */
  $handler = $view->new_display('default', 'Defaults', 'default');
  $handler->override_option('fields', array(
    'name' => array(
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
      'id' => 'name',
      'table' => 'cvterm',
      'field' => 'name',
      'relationship' => 'none',
    ),
    'description' => array(
      'label' => 'Location Experiment Performed',
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
      'table' => 'nd_geolocation',
      'field' => 'description',
      'relationship' => 'none',
    ),
  ));
  $handler->override_option('sorts', array(
    'name' => array(
      'order' => 'ASC',
      'id' => 'name',
      'table' => 'cvterm',
      'field' => 'name',
      'relationship' => 'none',
    ),
    'description' => array(
      'order' => 'ASC',
      'id' => 'description',
      'table' => 'nd_geolocation',
      'field' => 'description',
      'relationship' => 'none',
    ),
  ));
  $handler->override_option('filters', array(
    'type_id' => array(
      'operator' => '=',
      'value' => '',
      'group' => '0',
      'exposed' => TRUE,
      'expose' => array(
        'use_operator' => 0,
        'operator' => 'type_id_op',
        'identifier' => 'type_id',
        'label' => 'Experiment Type',
        'remember' => 0,
      ),
      'case' => 1,
      'id' => 'type_id',
      'table' => 'nd_experiment',
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
    'description' => array(
      'operator' => '=',
      'value' => 'University of Saskatchewan',
      'group' => '0',
      'exposed' => TRUE,
      'expose' => array(
        'use_operator' => 0,
        'operator' => 'description_op',
        'identifier' => 'geolocation',
        'label' => 'Location Experiment Performed',
        'remember' => 0,
      ),
      'case' => 1,
      'id' => 'description',
      'table' => 'nd_geolocation',
      'field' => 'description',
      'relationship' => 'none',
      'agg' => array(
        'records_with' => 1,
        'aggregates_with' => 1,
      ),
      'values_form_type' => 'select',
      'multiple' => 1,
      'optional' => 0,
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
      'no_results_text' => 'Click "Show" to see a list of all experiments matching the entered criteria. If you leave a any of the criteria blank then the experiments will be not be filtered based on that field. Furthermore, if you leave all criteria blank then all experiments will be listed.',
    ),
  ));
  $handler->override_option('access', array(
    'type' => 'perm',
    'perm' => 'access content',
  ));
  $handler->override_option('cache', array(
    'type' => 'none',
  ));
  $handler->override_option('title', 'Natural Diversity Experiments');
  $handler->override_option('header', 'Click "Show" to see a list of all experiments matching the entered criteria. If you leave a any of the criteria blank then the experiments will be not be filtered based on that field. Furthermore, if you leave all criteria blank then all experiments will be listed.');
  $handler->override_option('header_format', '2');
  $handler->override_option('header_empty', 0);
  $handler->override_option('empty', 'No natural diversity experiments match the current criteria.');
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
        'sortable' => 1,
        'separator' => '',
      ),
    ),
    'default' => '-1',
  ));
  $handler = $view->new_display('page', 'Page', 'page_1');
  $handler->override_option('path', 'chado/natdiv_experiments');
  $handler->override_option('menu', array(
    'type' => 'normal',
    'title' => 'Natural Diversity Experiments',
    'description' => 'A single assay resulting in a single genotype or phenotype. Multiple assays resulting in an entire experiment should be grouped together as a project.',
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

  // List geolocations
  $view = new view;
  $view->name = 'geolocation_listing';
  $view->description = 'A listing of locations where natural diversity experiments took place.';
  $view->tag = 'chado default';
  $view->base_table = 'nd_geolocation';
  $view->core = 6;
  $view->api_version = '2';
  $view->disabled = FALSE; /* Edit this to true to make a default view disabled initially */
  $handler = $view->new_display('default', 'Defaults', 'default');
  $handler->override_option('fields', array(
    'description' => array(
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
      'id' => 'description',
      'table' => 'nd_geolocation',
      'field' => 'description',
      'relationship' => 'none',
    ),
    'latitude' => array(
      'label' => 'Latitude',
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
      'id' => 'latitude',
      'table' => 'nd_geolocation',
      'field' => 'latitude',
      'relationship' => 'none',
    ),
    'longitude' => array(
      'label' => 'Longitude',
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
      'id' => 'longitude',
      'table' => 'nd_geolocation',
      'field' => 'longitude',
      'relationship' => 'none',
    ),
    'altitude' => array(
      'label' => 'Altitude',
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
      'id' => 'altitude',
      'table' => 'nd_geolocation',
      'field' => 'altitude',
      'relationship' => 'none',
    ),
    'geodetic_datum' => array(
      'label' => 'Geodetic Datum',
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
      'id' => 'geodetic_datum',
      'table' => 'nd_geolocation',
      'field' => 'geodetic_datum',
      'relationship' => 'none',
    ),
  ));
  $handler->override_option('filters', array(
    'geodetic_datum' => array(
      'operator' => '=',
      'value' => array(),
      'group' => '0',
      'exposed' => TRUE,
      'expose' => array(
        'use_operator' => 0,
        'operator' => 'geodetic_datum_op',
        'identifier' => 'geodetic_datum',
        'label' => 'Geodetic Datum',
        'remember' => 0,
      ),
      'case' => 1,
      'id' => 'geodetic_datum',
      'table' => 'nd_geolocation',
      'field' => 'geodetic_datum',
      'relationship' => 'none',
      'agg' => array(
        'records_with' => 1,
        'aggregates_with' => 1,
      ),
      'values_form_type' => 'select',
      'multiple' => 1,
      'optional' => 0,
    ),
    'latitude' => array(
      'operator' => '=',
      'value' => array(
        'value' => '',
        'min' => '',
        'max' => '',
      ),
      'group' => '0',
      'exposed' => TRUE,
      'expose' => array(
        'use_operator' => 1,
        'operator' => 'latitude_op',
        'identifier' => 'latitude',
        'label' => 'Latitude',
        'optional' => 0,
        'remember' => 0,
      ),
      'case' => 1,
      'id' => 'latitude',
      'table' => 'nd_geolocation',
      'field' => 'latitude',
      'relationship' => 'none',
      'agg' => array(
        'records_with' => 1,
        'aggregates_with' => 1,
      ),
    ),
    'longitude' => array(
      'operator' => '=',
      'value' => array(
        'value' => '',
        'min' => '',
        'max' => '',
      ),
      'group' => '0',
      'exposed' => TRUE,
      'expose' => array(
        'use_operator' => 1,
        'operator' => 'longitude_op',
        'identifier' => 'longitude',
        'label' => 'Longitude',
        'optional' => 0,
        'remember' => 0,
      ),
      'case' => 1,
      'id' => 'longitude',
      'table' => 'nd_geolocation',
      'field' => 'longitude',
      'relationship' => 'none',
      'agg' => array(
        'records_with' => 1,
        'aggregates_with' => 1,
      ),
    ),
    'altitude' => array(
      'operator' => '=',
      'value' => array(
        'value' => '',
        'min' => '',
        'max' => '',
      ),
      'group' => '0',
      'exposed' => TRUE,
      'expose' => array(
        'use_operator' => 1,
        'operator' => 'altitude_op',
        'identifier' => 'altitude',
        'label' => 'Altitude',
        'optional' => 0,
        'remember' => 0,
      ),
      'case' => 0,
      'id' => 'altitude',
      'table' => 'nd_geolocation',
      'field' => 'altitude',
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
        'label' => 'Name Contains',
        'remember' => 0,
      ),
      'case' => 0,
      'id' => 'description',
      'table' => 'nd_geolocation',
      'field' => 'description',
      'relationship' => 'none',
      'agg' => array(
        'records_with' => 1,
        'aggregates_with' => 1,
      ),
      'values_form_type' => 'textfield',
      'multiple' => 0,
      'optional' => 0,
    ),
  ));
  $handler->override_option('access', array(
    'type' => 'perm',
    'perm' => 'access content',
  ));
  $handler->override_option('cache', array(
    'type' => 'none',
  ));
  $handler->override_option('title', 'Geographical Locations');
  $handler->override_option('header', 'Use the following criteria to limit the locations listed. If you leave a any of the criteria blank then the locations will be not be filtered based on that field.');
  $handler->override_option('header_format', '2');
  $handler->override_option('header_empty', 0);
  $handler->override_option('empty', 'No locations matched the current criteria.');
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
      'description' => 'description',
      'latitude' => 'latitude',
      'longitude' => 'longitude',
      'altitude' => 'altitude',
      'geodetic_datum' => 'geodetic_datum',
    ),
    'info' => array(
      'description' => array(
        'sortable' => 1,
        'separator' => '',
      ),
      'latitude' => array(
        'sortable' => 1,
        'separator' => '',
      ),
      'longitude' => array(
        'sortable' => 1,
        'separator' => '',
      ),
      'altitude' => array(
        'sortable' => 1,
        'separator' => '',
      ),
      'geodetic_datum' => array(
        'sortable' => 1,
        'separator' => '',
      ),
    ),
    'default' => 'description',
  ));
  $handler = $view->new_display('page', 'Page', 'page_1');
  $handler->override_option('path', 'chado/natdiv_geolocations');
  $handler->override_option('menu', array(
    'type' => 'normal',
    'title' => 'Locations',
    'description' => 'Geographical locations in which natural diversity experiments might be performed.',
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

  // List protocols
  $view = new view;
  $view->name = 'natdiv_protocol_listing';
  $view->description = 'A listing of protocols used for natural diversity experiments';
  $view->tag = 'chado default';
  $view->base_table = 'nd_protocol';
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
      'table' => 'nd_protocol',
      'field' => 'name',
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
      'table' => 'nd_protocol',
      'field' => 'name',
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
      'no_results_text' => 'Click "Show" to see a list of all protocols matching the entered criteria. If you leave a any of the criteria blank then the protocols will be not be filtered based on that field. Furthermore, if you leave all criteria blank then all protocols will be listed.',
    ),
  ));
  $handler->override_option('access', array(
    'type' => 'perm',
    'perm' => 'access content',
  ));
  $handler->override_option('cache', array(
    'type' => 'none',
  ));
  $handler->override_option('title', 'Protocols');
  $handler->override_option('header', 'Click "Show" to see a list of all protocols matching the entered criteria. If you leave a any of the criteria blank then the protocols will be not be filtered based on that field. Furthermore, if you leave all criteria blank then all protocols will be listed.');
  $handler->override_option('header_format', '2');
  $handler->override_option('header_empty', 0);
  $handler->override_option('empty', 'No protocols match the current criteria.');
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
    ),
    'info' => array(
      'name' => array(
        'sortable' => 1,
        'separator' => '',
      ),
    ),
    'default' => 'name',
  ));
  $handler = $view->new_display('page', 'Page', 'page_1');
  $handler->override_option('path', 'chado/natdiv_protocols');
  $handler->override_option('menu', array(
    'type' => 'normal',
    'title' => 'Protocols',
    'description' => 'The protcols used for natural diversity experiments',
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

  // List Reagents
  $view = new view;
  $view->name = 'natdiv_reagent_listing';
  $view->description = 'A listing of reagents used in natural diversity experiments';
  $view->tag = 'chado default';
  $view->base_table = 'nd_reagent';
  $view->core = 6;
  $view->api_version = '2';
  $view->disabled = FALSE; /* Edit this to true to make a default view disabled initially */
  $handler = $view->new_display('default', 'Defaults', 'default');
  $handler->override_option('fields', array(
    'name_1' => array(
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
      'id' => 'name_1',
      'table' => 'nd_reagent',
      'field' => 'name',
      'relationship' => 'none',
    ),
    'name' => array(
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
      'id' => 'name',
      'table' => 'cvterm',
      'field' => 'name',
      'relationship' => 'none',
    ),
    'residues' => array(
      'label' => 'Sequence',
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
      'id' => 'residues',
      'table' => 'feature',
      'field' => 'residues',
      'relationship' => 'none',
    ),
  ));
  $handler->override_option('filters', array(
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
      'table' => 'nd_reagent',
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
      'no_results_text' => 'Click "Show" to see a list of all reagents matching the entered criteria. If you leave a any of the criteria blank then the reagents will be not be filtered based on that field. Furthermore, if you leave all criteria blank then all reagents will be listed.',
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
      'table' => 'nd_reagent',
      'field' => 'name',
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
  $handler->override_option('title', 'Reagents');
  $handler->override_option('header', 'Click "Show" to see a list of all reagents matching the entered criteria. If you leave a any of the criteria blank then the reagents will be not be filtered based on that field. Furthermore, if you leave all criteria blank then all reagents will be listed.');
  $handler->override_option('header_format', '2');
  $handler->override_option('header_empty', 0);
  $handler->override_option('empty', 'No reagents match the current criteria.');
  $handler->override_option('empty_format', '2');
  $handler->override_option('items_per_page', 50);
  $handler->override_option('use_pager', '1');
  $handler->override_option('style_plugin', 'table');
  $handler = $view->new_display('page', 'Page', 'page_1');
  $handler->override_option('path', 'chado/natdiv_reagents');
  $handler->override_option('menu', array(
    'type' => 'normal',
    'title' => 'Reagents',
    'description' => 'Reagents used in natural diversity experiments',
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

