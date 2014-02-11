<?php
/**
 * @file
 * Describe publication default views
 */

/**
 * Implements hook_views_default_views().
 *
 * @ingroup tripal_pub
 */
function tripal_pub_views_default_views() {
  $views = array();

  // Main default view
  $view = tripal_pub_defaultview_admin_publications();
  $view = tripal_views_make_view_compatible_with_external($view);
  $views[$view->name] = $view;

  return $views;
}

/**
 * Default publication administration view
 *
 * @ingroup tripal_pub
 */
function tripal_pub_defaultview_admin_publications() {

  $view = new view();
  $view->name = 'tripal_pub_admin_publications';
  $view->description = 'DO NOT DISABLE';
  $view->tag = 'tripal admin';
  $view->base_table = 'pub';
  $view->human_name = 'Publication Admin';
  $view->core = 0;
  $view->api_version = '3.0';
  $view->disabled = FALSE; /* Edit this to true to make a default view disabled initially */

  /* Display: Defaults */
  $handler = $view->new_display('default', 'Defaults', 'default');
  $handler->display->display_options['title'] = 'Publications';
  $handler->display->display_options['use_more_always'] = FALSE;
  $handler->display->display_options['access']['type'] = 'perm';
  $handler->display->display_options['access']['perm'] = 'access chado_pub content';
  $handler->display->display_options['cache']['type'] = 'none';
  $handler->display->display_options['query']['type'] = 'views_query';
  $handler->display->display_options['exposed_form']['type'] = 'basic';
  $handler->display->display_options['pager']['type'] = 'full';
  $handler->display->display_options['pager']['options']['items_per_page'] = '25';
  $handler->display->display_options['pager']['options']['offset'] = '0';
  $handler->display->display_options['pager']['options']['id'] = '0';
  $handler->display->display_options['pager']['options']['quantity'] = '9';
  $handler->display->display_options['style_plugin'] = 'table';
  $handler->display->display_options['style_options']['grouping'] = '';
  $handler->display->display_options['style_options']['columns'] = array(
    'title' => 'title',
    'pyear' => 'pyear',
    'type' => 'type',
    'all_dbxref' => 'all_dbxref',
    'all_properties' => 'all_properties',
    'all_relationships' => 'all_relationships',
  );
  $handler->display->display_options['style_options']['default'] = '-1';
  $handler->display->display_options['style_options']['info'] = array(
    'title' => array(
      'sortable' => 1,
      'separator' => '',
    ),
    'type' => array(
      'sortable' => 1,
      'separator' => '',
    ),
    'pyear' => array(
      'sortable' => 1,
      'separator' => '',
    ),
    'all_dbxref' => array(
      'separator' => '',
    ),
    'all_properties' => array(
      'separator' => '',
    ),
    'all_relationships' => array(
      'separator' => '',
    ),
  );
  /* Header: Global: Action Links */
  $handler->display->display_options['header']['action_links_area']['id'] = 'action_links_area';
  $handler->display->display_options['header']['action_links_area']['table'] = 'views';
  $handler->display->display_options['header']['action_links_area']['field'] = 'action_links_area';
  $handler->display->display_options['header']['action_links_area']['label'] = 'Action Links';
  $handler->display->display_options['header']['action_links_area']['empty'] = TRUE;
  $handler->display->display_options['header']['action_links_area']['link-1'] = array(
    'label-1' => 'Add Publication',
    'path-1' => 'node/add/tripal-pub',
  );
  $handler->display->display_options['header']['action_links_area']['link-2'] = array(
    'label-2' => '',
    'path-2' => '',
  );
  $handler->display->display_options['header']['action_links_area']['link-3'] = array(
    'label-3' => '',
    'path-3' => '',
  );
  $handler->display->display_options['header']['action_links_area']['link-4'] = array(
    'label-4' => '',
    'path-4' => '',
  );
  /* No results behavior: Global: Text area */
  $handler->display->display_options['empty']['text']['id'] = 'area';
  $handler->display->display_options['empty']['text']['table'] = 'views';
  $handler->display->display_options['empty']['text']['field'] = 'area';
  $handler->display->display_options['empty']['text']['content'] = 'No publications match the supplied criteria.';
  $handler->display->display_options['empty']['text']['format'] = '1';
  /* Relationship: Pub: Pub => Node */
  $handler->display->display_options['relationships']['pub_id']['id'] = 'pub_id';
  $handler->display->display_options['relationships']['pub_id']['table'] = 'chado_pub';
  $handler->display->display_options['relationships']['pub_id']['field'] = 'pub_id';
  /* Field: Content: Nid */
  $handler->display->display_options['fields']['nid']['id'] = 'nid';
  $handler->display->display_options['fields']['nid']['table'] = 'node';
  $handler->display->display_options['fields']['nid']['field'] = 'nid';
  $handler->display->display_options['fields']['nid']['exclude'] = TRUE;
  /* Field: Chado Pub: Title */
  $handler->display->display_options['fields']['title']['id'] = 'title';
  $handler->display->display_options['fields']['title']['table'] = 'pub';
  $handler->display->display_options['fields']['title']['field'] = 'title';
  /* Field: Chado Pub: Pyear */
  $handler->display->display_options['fields']['pyear']['id'] = 'name';
  $handler->display->display_options['fields']['pyear']['table'] = 'pub';
  $handler->display->display_options['fields']['pyear']['field'] = 'pyear';
  $handler->display->display_options['fields']['pyear']['label'] = 'Year';
  /* Field: Chado Cvterm: Name */
  $handler->display->display_options['fields']['type']['id'] = 'type';
  $handler->display->display_options['fields']['type']['table'] = 'cvterm';
  $handler->display->display_options['fields']['type']['field'] = 'name';
  $handler->display->display_options['fields']['type']['label'] = 'Type';
  /* Sort criterion: Chado Pub: Pyear */
  $handler->display->display_options['sorts']['pyear']['id'] = 'year';
  $handler->display->display_options['sorts']['pyear']['table'] = 'pub';
  $handler->display->display_options['sorts']['pyear']['field'] = 'pyear';
  /* Sort criterion: Chado Pub: Title */
  $handler->display->display_options['sorts']['title']['id'] = 'title';
  $handler->display->display_options['sorts']['title']['table'] = 'pub';
  $handler->display->display_options['sorts']['title']['field'] = 'title';
  /* Sort criterion: Chado Cvterm: Name */
  $handler->display->display_options['sorts']['type']['id'] = 'type';
  $handler->display->display_options['sorts']['type']['table'] = 'cvterm';
  $handler->display->display_options['sorts']['type']['field'] = 'name';
  /* Filter criterion: Chado Pub: Title */
  $handler->display->display_options['filters']['title']['id'] = 'title';
  $handler->display->display_options['filters']['title']['table'] = 'pub';
  $handler->display->display_options['filters']['title']['field'] = 'title';
  $handler->display->display_options['filters']['title']['operator'] = 'contains';
  $handler->display->display_options['filters']['title']['group'] = '0';
  $handler->display->display_options['filters']['title']['exposed'] = TRUE;
  $handler->display->display_options['filters']['title']['expose']['operator_id'] = 'title_op';
  $handler->display->display_options['filters']['title']['expose']['label'] = 'Title Contains';
  $handler->display->display_options['filters']['title']['expose']['operator'] = 'title_op';
  $handler->display->display_options['filters']['title']['expose']['identifier'] = 'pub_title';
  $handler->display->display_options['filters']['title']['expose']['remember_roles'] = array(
    2 => '2',
    1 => 0,
    3 => 0,
  );
  /* Filter criterion: Chado Pub: Type Id */
  $handler->display->display_options['filters']['type_id']['id'] = 'type_id';
  $handler->display->display_options['filters']['type_id']['table'] = 'pub';
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

  /** MANUALLY ADD since filter handler no exporting correctly */
  $handler->display->display_options['filters']['type_id']['expose']['values_form_type'] = 'select';
  $handler->display->display_options['filters']['type_id']['expose']['select_multiple'] = FALSE;
  $handler->display->display_options['filters']['type_id']['expose']['select_optional'] = TRUE;
  $handler->display->display_options['filters']['type_id']['expose']['max_length'] = 40;

  /* Display: Page */
  $handler = $view->new_display('page', 'Page', 'page_1');
  $handler->display->display_options['path'] = 'admin/tripal/chado/tripal_pub/publications';
  $handler->display->display_options['menu']['type'] = 'default tab';
  $handler->display->display_options['menu']['title'] = 'Publications';
  $handler->display->display_options['menu']['description'] = 'A published article, book, conference proceeding, etc.';
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
