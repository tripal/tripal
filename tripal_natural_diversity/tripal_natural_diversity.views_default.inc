<?php
/**
 * @file
 * Describes default natural diversity views
 */

/**
 * Implements hook_views_default_views().
 *
 * @ingroup tripal_natural_diversity
 */
function tripal_natural_diversity_views_default_views() {
  $views = array();

  // Experiment Listing
  $view = tripal_natural_diversity_defaultview_admin_natdiv_exp();
  $view = tripal_views_make_view_compatible_with_external($view);
  $views[$view->name] = $view;

  // List geolocations
  $view = tripal_natural_diversity_defaultview_admin_geolocations();
  $view = tripal_views_make_view_compatible_with_external($view);
  $views[$view->name] = $view;

  // List protocols
  $view = tripal_natural_diversity_defaultview_admin_protocols();
  $view = tripal_views_make_view_compatible_with_external($view);
  $views[$view->name] = $view;

  // List Reagents
  $view = tripal_natural_diversity_defaultview_admin_reagents();
  $view = tripal_views_make_view_compatible_with_external($view);
  $views[$view->name] = $view;

  return $views;
}

/**
 * Describes the default experiment administration view
 *
 * @ingroup tripal_natural_diversity
 */
function tripal_natural_diversity_defaultview_admin_natdiv_exp() {

  $view = new view();
  $view->name = 'tripal_natural_diversity_admin_natdiv_exp';
  $view->description = 'DO NOT DISABLE';
  $view->tag = 'tripal admin';
  $view->base_table = 'nd_experiment';
  $view->human_name = 'NatDiv Experiment Admin';
  $view->core = 7;
  $view->api_version = '3.0';
  $view->disabled = FALSE; /* Edit this to true to make a default view disabled initially */

  /* Display: Defaults */
  $handler = $view->new_display('default', 'Defaults', 'default');
  $handler->display->display_options['title'] = 'Natural Diversity Experiments';
  $handler->display->display_options['use_more_always'] = FALSE;
  $handler->display->display_options['access']['type'] = 'perm';
  $handler->display->display_options['cache']['type'] = 'none';
  $handler->display->display_options['query']['type'] = 'views_query';
  $handler->display->display_options['exposed_form']['type'] = 'basic';
  $handler->display->display_options['exposed_form']['options']['submit_button'] = 'Filter';
  $handler->display->display_options['pager']['type'] = 'full';
  $handler->display->display_options['pager']['options']['items_per_page'] = '25';
  $handler->display->display_options['pager']['options']['offset'] = '0';
  $handler->display->display_options['pager']['options']['id'] = '0';
  $handler->display->display_options['pager']['options']['quantity'] = '9';
  $handler->display->display_options['style_plugin'] = 'table';
  $handler->display->display_options['style_options']['grouping'] = '';
  $handler->display->display_options['style_options']['columns'] = array(
    'name' => 'name',
    'description' => 'description',
  );
  $handler->display->display_options['style_options']['default'] = '-1';
  $handler->display->display_options['style_options']['info'] = array(
    'name' => array(
      'sortable' => 1,
      'separator' => '',
    ),
    'description' => array(
      'sortable' => 1,
      'separator' => '',
    ),
  );
  /* No results behavior: Global: Text area */
  $handler->display->display_options['empty']['text']['id'] = 'area';
  $handler->display->display_options['empty']['text']['table'] = 'views';
  $handler->display->display_options['empty']['text']['field'] = 'area';
  $handler->display->display_options['empty']['text']['content'] = 'No natural diversity experiments match the current criteria.';
  $handler->display->display_options['empty']['text']['format'] = '2';
  /* Field: Chado Cvterm: Name */
  $handler->display->display_options['fields']['name']['id'] = 'name';
  $handler->display->display_options['fields']['name']['table'] = 'cvterm';
  $handler->display->display_options['fields']['name']['field'] = 'name';
  $handler->display->display_options['fields']['name']['label'] = 'Type';
  /* Field: Chado Nd Geolocation: Description */
  $handler->display->display_options['fields']['description']['id'] = 'description';
  $handler->display->display_options['fields']['description']['table'] = 'nd_geolocation';
  $handler->display->display_options['fields']['description']['field'] = 'description';
  $handler->display->display_options['fields']['description']['label'] = 'Location Experiment Performed';
  /* Sort criterion: Chado Cvterm: Name */
  $handler->display->display_options['sorts']['name']['id'] = 'name';
  $handler->display->display_options['sorts']['name']['table'] = 'cvterm';
  $handler->display->display_options['sorts']['name']['field'] = 'name';
  /* Sort criterion: Chado Nd Geolocation: Description */
  $handler->display->display_options['sorts']['description']['id'] = 'description';
  $handler->display->display_options['sorts']['description']['table'] = 'nd_geolocation';
  $handler->display->display_options['sorts']['description']['field'] = 'description';
  /* Filter criterion: Chado Nd Experiment: Type Id */
  $handler->display->display_options['filters']['type_id']['id'] = 'type_id';
  $handler->display->display_options['filters']['type_id']['table'] = 'nd_experiment';
  $handler->display->display_options['filters']['type_id']['field'] = 'type_id';
  $handler->display->display_options['filters']['type_id']['group'] = '0';
  $handler->display->display_options['filters']['type_id']['value'] = 'All';
  $handler->display->display_options['filters']['type_id']['exposed'] = TRUE;
  $handler->display->display_options['filters']['type_id']['expose']['operator_id'] = 'type_id_op';
  $handler->display->display_options['filters']['type_id']['expose']['label'] = 'Experiment Type';
  $handler->display->display_options['filters']['type_id']['expose']['operator'] = 'type_id_op';
  $handler->display->display_options['filters']['type_id']['expose']['identifier'] = 'type_id';
  /* Filter criterion: Chado Nd Geolocation: Description */
  $handler->display->display_options['filters']['description']['id'] = 'description';
  $handler->display->display_options['filters']['description']['table'] = 'nd_geolocation';
  $handler->display->display_options['filters']['description']['field'] = 'description';
  $handler->display->display_options['filters']['description']['value'] = array();
  $handler->display->display_options['filters']['description']['group'] = '0';
  $handler->display->display_options['filters']['description']['exposed'] = TRUE;
  $handler->display->display_options['filters']['description']['expose']['operator_id'] = 'description_op';
  $handler->display->display_options['filters']['description']['expose']['label'] = 'Location Experiment Performed';
  $handler->display->display_options['filters']['description']['expose']['operator'] = 'description_op';
  $handler->display->display_options['filters']['description']['expose']['identifier'] = 'geolocation';

  /** MANUALLY ADD since filter handler no exporting correctly */
  $handler->display->display_options['filters']['type_id']['expose']['values_form_type'] = 'select';
  $handler->display->display_options['filters']['type_id']['expose']['select_multiple'] = FALSE;
  $handler->display->display_options['filters']['type_id']['expose']['select_optional'] = TRUE;
  $handler->display->display_options['filters']['type_id']['expose']['max_length'] = 40;

  /* Display: Page */
  $handler = $view->new_display('page', 'Page', 'page_1');
  $handler->display->display_options['path'] = 'admin/tripal/chado/tripal_natdiv/experiments';
  $handler->display->display_options['menu']['type'] = 'default tab';
  $handler->display->display_options['menu']['title'] = 'Experiments';
  $handler->display->display_options['menu']['description'] = 'A single assay resulting in a single genotype or phenotype.';
  $handler->display->display_options['menu']['weight'] = '-10';
  $handler->display->display_options['menu']['context'] = 0;
  $handler->display->display_options['menu']['context_only_inline'] = 0;
  $handler->display->display_options['tab_options']['weight'] = '0';

  /** MANUALLY ADD since filter handler no exporting correctly
  $handler->display->display_options['filters']['type_id']['expose']['values_form_type'] = 'select';
  $handler->display->display_options['filters']['type_id']['expose']['select_multiple'] = FALSE;
  $handler->display->display_options['filters']['type_id']['expose']['select_optional'] = TRUE;
  $handler->display->display_options['filters']['type_id']['expose']['max_length'] = 40;
  */

  return $view;
}

/**
 * Describes the default geological locations administration view
 *
 * @ingroup tripal_natural_diversity
 */
function tripal_natural_diversity_defaultview_admin_geolocations() {

  $view = new view();
  $view->name = 'tripal_natural_diversity_admin_geolocs';
  $view->description = 'DO NOT DISABLE';
  $view->tag = 'tripal admin';
  $view->base_table = 'nd_geolocation';
  $view->human_name = 'NatDiv Locations Admin';
  $view->core = 7;
  $view->api_version = '3.0';
  $view->disabled = FALSE; /* Edit this to true to make a default view disabled initially */

  /* Display: Defaults */
  $handler = $view->new_display('default', 'Defaults', 'default');
  $handler->display->display_options['title'] = 'Geographical Locations';
  $handler->display->display_options['use_more_always'] = FALSE;
  $handler->display->display_options['access']['type'] = 'perm';
  $handler->display->display_options['cache']['type'] = 'none';
  $handler->display->display_options['query']['type'] = 'views_query';
  $handler->display->display_options['exposed_form']['type'] = 'basic';
  $handler->display->display_options['exposed_form']['options']['submit_button'] = 'Filter';
  $handler->display->display_options['pager']['type'] = 'full';
  $handler->display->display_options['pager']['options']['items_per_page'] = '25';
  $handler->display->display_options['pager']['options']['offset'] = '0';
  $handler->display->display_options['pager']['options']['id'] = '0';
  $handler->display->display_options['pager']['options']['quantity'] = '9';
  $handler->display->display_options['style_plugin'] = 'table';
  $handler->display->display_options['style_options']['grouping'] = '';
  $handler->display->display_options['style_options']['columns'] = array(
    'description' => 'description',
    'latitude' => 'latitude',
    'longitude' => 'longitude',
    'altitude' => 'altitude',
    'geodetic_datum' => 'geodetic_datum',
  );
  $handler->display->display_options['style_options']['default'] = 'description';
  $handler->display->display_options['style_options']['info'] = array(
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
  );
  /* No results behavior: Global: Text area */
  $handler->display->display_options['empty']['text']['id'] = 'area';
  $handler->display->display_options['empty']['text']['table'] = 'views';
  $handler->display->display_options['empty']['text']['field'] = 'area';
  $handler->display->display_options['empty']['text']['content'] = 'No locations matched the current criteria.';
  $handler->display->display_options['empty']['text']['format'] = '2';
  /* Field: Chado Nd Geolocation: Description */
  $handler->display->display_options['fields']['description']['id'] = 'description';
  $handler->display->display_options['fields']['description']['table'] = 'nd_geolocation';
  $handler->display->display_options['fields']['description']['field'] = 'description';
  $handler->display->display_options['fields']['description']['label'] = 'Name';
  /* Field: Chado Nd Geolocation: Latitude */
  $handler->display->display_options['fields']['latitude']['id'] = 'latitude';
  $handler->display->display_options['fields']['latitude']['table'] = 'nd_geolocation';
  $handler->display->display_options['fields']['latitude']['field'] = 'latitude';
  /* Field: Chado Nd Geolocation: Longitude */
  $handler->display->display_options['fields']['longitude']['id'] = 'longitude';
  $handler->display->display_options['fields']['longitude']['table'] = 'nd_geolocation';
  $handler->display->display_options['fields']['longitude']['field'] = 'longitude';
  /* Field: Chado Nd Geolocation: Altitude */
  $handler->display->display_options['fields']['altitude']['id'] = 'altitude';
  $handler->display->display_options['fields']['altitude']['table'] = 'nd_geolocation';
  $handler->display->display_options['fields']['altitude']['field'] = 'altitude';
  /* Field: Chado Nd Geolocation: Geodetic Datum */
  $handler->display->display_options['fields']['geodetic_datum']['id'] = 'geodetic_datum';
  $handler->display->display_options['fields']['geodetic_datum']['table'] = 'nd_geolocation';
  $handler->display->display_options['fields']['geodetic_datum']['field'] = 'geodetic_datum';
  /* Filter criterion: Chado Nd Geolocation: Geodetic Datum */
  $handler->display->display_options['filters']['geodetic_datum']['id'] = 'geodetic_datum';
  $handler->display->display_options['filters']['geodetic_datum']['table'] = 'nd_geolocation';
  $handler->display->display_options['filters']['geodetic_datum']['field'] = 'geodetic_datum';
  $handler->display->display_options['filters']['geodetic_datum']['group'] = '0';
  $handler->display->display_options['filters']['geodetic_datum']['exposed'] = TRUE;
  $handler->display->display_options['filters']['geodetic_datum']['expose']['label'] = 'Geodetic Datum';
  $handler->display->display_options['filters']['geodetic_datum']['expose']['identifier'] = 'geodetic_datum';
  $handler->display->display_options['filters']['geodetic_datum']['expose']['remember_roles'] = array(
    2 => '2',
    1 => 0,
    3 => 0,
  );
  /* Filter criterion: Chado Nd Geolocation: Latitude */
  $handler->display->display_options['filters']['latitude']['id'] = 'latitude';
  $handler->display->display_options['filters']['latitude']['table'] = 'nd_geolocation';
  $handler->display->display_options['filters']['latitude']['field'] = 'latitude';
  $handler->display->display_options['filters']['latitude']['value'] = '';
  $handler->display->display_options['filters']['latitude']['group'] = '0';
  $handler->display->display_options['filters']['latitude']['exposed'] = TRUE;
  $handler->display->display_options['filters']['latitude']['expose']['label'] = 'Latitude';
  $handler->display->display_options['filters']['latitude']['expose']['identifier'] = 'latitude';
  $handler->display->display_options['filters']['latitude']['expose']['remember_roles'] = array(
    2 => '2',
    1 => 0,
    3 => 0,
  );
  /* Filter criterion: Chado Nd Geolocation: Longitude */
  $handler->display->display_options['filters']['longitude']['id'] = 'longitude';
  $handler->display->display_options['filters']['longitude']['table'] = 'nd_geolocation';
  $handler->display->display_options['filters']['longitude']['field'] = 'longitude';
  $handler->display->display_options['filters']['longitude']['value'] = '';
  $handler->display->display_options['filters']['longitude']['group'] = '0';
  $handler->display->display_options['filters']['longitude']['exposed'] = TRUE;
  $handler->display->display_options['filters']['longitude']['expose']['label'] = 'Longitude';
  $handler->display->display_options['filters']['longitude']['expose']['identifier'] = 'longitude';
  $handler->display->display_options['filters']['longitude']['expose']['remember_roles'] = array(
    2 => '2',
    1 => 0,
    3 => 0,
  );
  /* Filter criterion: Chado Nd Geolocation: Altitude */
  $handler->display->display_options['filters']['altitude']['id'] = 'altitude';
  $handler->display->display_options['filters']['altitude']['table'] = 'nd_geolocation';
  $handler->display->display_options['filters']['altitude']['field'] = 'altitude';
  $handler->display->display_options['filters']['altitude']['value'] = '';
  $handler->display->display_options['filters']['altitude']['group'] = '0';
  $handler->display->display_options['filters']['altitude']['exposed'] = TRUE;
  $handler->display->display_options['filters']['altitude']['expose']['label'] = 'Altitude';
  $handler->display->display_options['filters']['altitude']['expose']['identifier'] = 'altitude';
  $handler->display->display_options['filters']['altitude']['expose']['remember_roles'] = array(
    2 => '2',
    1 => 0,
    3 => 0,
  );
  /* Filter criterion: Chado Nd Geolocation: Description */
  $handler->display->display_options['filters']['description']['id'] = 'description';
  $handler->display->display_options['filters']['description']['table'] = 'nd_geolocation';
  $handler->display->display_options['filters']['description']['field'] = 'description';
  $handler->display->display_options['filters']['description']['operator'] = 'contains';
  $handler->display->display_options['filters']['description']['group'] = '0';
  $handler->display->display_options['filters']['description']['exposed'] = TRUE;
  $handler->display->display_options['filters']['description']['expose']['label'] = 'Description Contains';
  $handler->display->display_options['filters']['description']['expose']['identifier'] = 'description';
  $handler->display->display_options['filters']['description']['expose']['remember_roles'] = array(
    2 => '2',
    1 => 0,
    3 => 0,
  );

  /* Display: Page */
  $handler = $view->new_display('page', 'Page', 'page_1');
  $handler->display->display_options['path'] = 'admin/tripal/chado/tripal_natdiv/locations';
  $handler->display->display_options['menu']['type'] = 'tab';
  $handler->display->display_options['menu']['title'] = 'Locations';
  $handler->display->display_options['menu']['description'] = 'Geographical locations in which natural diversity experiments might be performed.';
  $handler->display->display_options['menu']['weight'] = '-8';
  $handler->display->display_options['menu']['name'] = 'management';
  $handler->display->display_options['menu']['context'] = 0;
  $handler->display->display_options['menu']['context_only_inline'] = 0;

  return $view;
}

/**
 * Describes the default reagents administration view
 *
 * @ingroup tripal_natural_diversity
 */
function tripal_natural_diversity_defaultview_admin_reagents() {

  $view = new view();
  $view->name = 'tripal_natural_diversity_admin_reagents';
  $view->description = 'DO NOT DISABLE';
  $view->tag = 'tripal admin';
  $view->base_table = 'nd_reagent';
  $view->human_name = 'NatDiv Reagent Admin';
  $view->core = 6;
  $view->api_version = '3.0';
  $view->disabled = FALSE; /* Edit this to true to make a default view disabled initially */

  /* Display: Defaults */
  $handler = $view->new_display('default', 'Defaults', 'default');
  $handler->display->display_options['title'] = 'Reagents';
  $handler->display->display_options['use_more_always'] = FALSE;
  $handler->display->display_options['access']['type'] = 'perm';
  $handler->display->display_options['cache']['type'] = 'none';
  $handler->display->display_options['query']['type'] = 'views_query';
  $handler->display->display_options['exposed_form']['type'] = 'basic';
  $handler->display->display_options['exposed_form']['options']['submit_button'] = 'Filter';
  $handler->display->display_options['pager']['type'] = 'full';
  $handler->display->display_options['pager']['options']['items_per_page'] = '25';
  $handler->display->display_options['pager']['options']['offset'] = '0';
  $handler->display->display_options['pager']['options']['id'] = '0';
  $handler->display->display_options['pager']['options']['quantity'] = '9';
  $handler->display->display_options['style_plugin'] = 'table';
  /* No results behavior: Global: Text area */
  $handler->display->display_options['empty']['text']['id'] = 'area';
  $handler->display->display_options['empty']['text']['table'] = 'views';
  $handler->display->display_options['empty']['text']['field'] = 'area';
  $handler->display->display_options['empty']['text']['content'] = 'No reagents match the current criteria.';
  $handler->display->display_options['empty']['text']['format'] = '2';
  /* Field: Chado Nd Reagent: Name */
  $handler->display->display_options['fields']['name_1']['id'] = 'name_1';
  $handler->display->display_options['fields']['name_1']['table'] = 'nd_reagent';
  $handler->display->display_options['fields']['name_1']['field'] = 'name';
  /* Field: Chado Cvterm: Name */
  $handler->display->display_options['fields']['name']['id'] = 'name';
  $handler->display->display_options['fields']['name']['table'] = 'cvterm';
  $handler->display->display_options['fields']['name']['field'] = 'name';
  $handler->display->display_options['fields']['name']['label'] = 'Type';
  /* Filter criterion: Chado Nd Reagent: Type Id */
  $handler->display->display_options['filters']['type_id']['id'] = 'type_id';
  $handler->display->display_options['filters']['type_id']['table'] = 'nd_reagent';
  $handler->display->display_options['filters']['type_id']['field'] = 'type_id';
  $handler->display->display_options['filters']['type_id']['value'] = 'All';
  $handler->display->display_options['filters']['type_id']['group'] = '0';
  $handler->display->display_options['filters']['type_id']['exposed'] = TRUE;
  $handler->display->display_options['filters']['type_id']['expose']['operator_id'] = 'type_id_op';
  $handler->display->display_options['filters']['type_id']['expose']['label'] = 'Type';
  $handler->display->display_options['filters']['type_id']['expose']['operator'] = 'type_id_op';
  $handler->display->display_options['filters']['type_id']['expose']['identifier'] = 'type_id';
  $handler->display->display_options['filters']['type_id']['expose']['remember_roles'] = array(
    2 => '2',
    1 => 0,
    3 => 0,
  );
  /* Filter criterion: Chado Nd Reagent: Name */
  $handler->display->display_options['filters']['name']['id'] = 'name';
  $handler->display->display_options['filters']['name']['table'] = 'nd_reagent';
  $handler->display->display_options['filters']['name']['field'] = 'name';
  $handler->display->display_options['filters']['name']['operator'] = 'contains';
  $handler->display->display_options['filters']['name']['group'] = '0';
  $handler->display->display_options['filters']['name']['exposed'] = TRUE;
  $handler->display->display_options['filters']['name']['expose']['operator_id'] = 'name_op';
  $handler->display->display_options['filters']['name']['expose']['label'] = 'Name Contains';
  $handler->display->display_options['filters']['name']['expose']['operator'] = 'name_op';
  $handler->display->display_options['filters']['name']['expose']['identifier'] = 'name';
  $handler->display->display_options['filters']['name']['expose']['remember_roles'] = array(
    2 => '2',
    1 => 0,
    3 => 0,
  );

  /** MANUALLY ADD since filter handler no exporting correctly */
  $handler->display->display_options['filters']['type_id']['expose']['values_form_type'] = 'select';
  $handler->display->display_options['filters']['type_id']['expose']['select_multiple'] = FALSE;
  $handler->display->display_options['filters']['type_id']['expose']['select_optional'] = TRUE;
  $handler->display->display_options['filters']['type_id']['expose']['max_length'] = 40;

  /* Display: Page */
  $handler = $view->new_display('page', 'Page', 'page_1');
  $handler->display->display_options['path'] = 'admin/tripal/chado/tripal_natdiv/reagents';
  $handler->display->display_options['menu']['type'] = 'tab';
  $handler->display->display_options['menu']['title'] = 'Reagents';
  $handler->display->display_options['menu']['description'] = 'Reagents used in natural diversity experiments';
  $handler->display->display_options['menu']['weight'] = '0';
  $handler->display->display_options['menu']['name'] = 'management';
  $handler->display->display_options['menu']['context'] = 0;
  $handler->display->display_options['menu']['context_only_inline'] = 0;

  /** MANUALLY ADD since filter handler no exporting correctly
  $handler->display->display_options['filters']['type_id']['expose']['values_form_type'] = 'select';
  $handler->display->display_options['filters']['type_id']['expose']['select_multiple'] = FALSE;
  $handler->display->display_options['filters']['type_id']['expose']['select_optional'] = TRUE;
  $handler->display->display_options['filters']['type_id']['expose']['max_length'] = 40;
  */

  return $view;
}

/**
 * Describes the default protocols administration view
 *
 * @ingroup tripal_natural_diversity
 */
function tripal_natural_diversity_defaultview_admin_protocols() {

  $view = new view();
  $view->name = 'tripal_natural_diversity_admin_protocols';
  $view->description = 'DO NOT DISABLE';
  $view->tag = 'tripal admin';
  $view->base_table = 'nd_protocol';
  $view->human_name = 'NatDiv Protocol Admin';
  $view->core = 7;
  $view->api_version = '3.0';
  $view->disabled = FALSE; /* Edit this to true to make a default view disabled initially */

  /* Display: Defaults */
  $handler = $view->new_display('default', 'Defaults', 'default');
  $handler->display->display_options['title'] = 'Protocols';
  $handler->display->display_options['use_more_always'] = FALSE;
  $handler->display->display_options['access']['type'] = 'perm';
  $handler->display->display_options['cache']['type'] = 'none';
  $handler->display->display_options['query']['type'] = 'views_query';
  $handler->display->display_options['exposed_form']['type'] = 'basic';
  $handler->display->display_options['exposed_form']['options']['submit_button'] = 'Filter';
  $handler->display->display_options['pager']['type'] = 'none';
  $handler->display->display_options['style_plugin'] = 'table';
  $handler->display->display_options['style_options']['grouping'] = '';
  $handler->display->display_options['style_options']['columns'] = array(
    'name' => 'name',
  );
  $handler->display->display_options['style_options']['default'] = 'name';
  $handler->display->display_options['style_options']['info'] = array(
    'name' => array(
      'sortable' => 1,
      'separator' => '',
    ),
  );
  /* No results behavior: Global: Text area */
  $handler->display->display_options['empty']['text']['id'] = 'area';
  $handler->display->display_options['empty']['text']['table'] = 'views';
  $handler->display->display_options['empty']['text']['field'] = 'area';
  $handler->display->display_options['empty']['text']['content'] = 'No protocols match the current criteria.';
  $handler->display->display_options['empty']['text']['format'] = '2';
  /* Field: Chado Nd Protocol: Name */
  $handler->display->display_options['fields']['name']['id'] = 'name';
  $handler->display->display_options['fields']['name']['table'] = 'nd_protocol';
  $handler->display->display_options['fields']['name']['field'] = 'name';
  /* Filter criterion: Chado Nd Protocol: Name */
  $handler->display->display_options['filters']['name']['id'] = 'name';
  $handler->display->display_options['filters']['name']['table'] = 'nd_protocol';
  $handler->display->display_options['filters']['name']['field'] = 'name';
  $handler->display->display_options['filters']['name']['operator'] = 'contains';
  $handler->display->display_options['filters']['name']['group'] = '0';
  $handler->display->display_options['filters']['name']['exposed'] = TRUE;
  $handler->display->display_options['filters']['name']['expose']['operator_id'] = 'name_op';
  $handler->display->display_options['filters']['name']['expose']['label'] = 'Name Contains';
  $handler->display->display_options['filters']['name']['expose']['operator'] = 'name_op';
  $handler->display->display_options['filters']['name']['expose']['identifier'] = 'name';
  $handler->display->display_options['filters']['name']['expose']['remember_roles'] = array(
    2 => '2',
    1 => 0,
    3 => 0,
  );

  /* Display: Page */
  $handler = $view->new_display('page', 'Page', 'page_1');
  $handler->display->display_options['path'] = 'admin/tripal/chado/tripal_natdiv/protocols';
  $handler->display->display_options['menu']['type'] = 'tab';
  $handler->display->display_options['menu']['title'] = 'Protocols';
  $handler->display->display_options['menu']['description'] = 'The protcols used for natural diversity experiments';
  $handler->display->display_options['menu']['weight'] = '-5';
  $handler->display->display_options['menu']['name'] = 'management';
  $handler->display->display_options['menu']['context'] = 0;
  $handler->display->display_options['menu']['context_only_inline'] = 0;

  return $view;
}
