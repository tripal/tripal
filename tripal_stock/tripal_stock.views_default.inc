<?php
/**
 * @file
 * Describes stock default views.
 */

/**
 * Implements hook_views_default_views().
 *
 * @ingroup tripal_stock
 */
function tripal_stock_views_default_views() {
  $views = array();

  $view = tripal_stock_defaultview_admin_stocks();
  $view = tripal_views_make_view_compatible_with_external($view);
  $views[$view->name] = $view;

  return $views;
}

/**
 * Default stock administration view
 *
 * @ingroup tripal_stock
 */
function tripal_stock_defaultview_admin_stocks() {

  $view = new view();
  $view->name = 'tripal_stock_admin_stocks';
  $view->description = 'DO NOT DISABLE';
  $view->tag = 'tripal admin';
  $view->base_table = 'stock';
  $view->human_name = 'Stock Admin';
  $view->core = 0;
  $view->api_version = '3.0';
  $view->disabled = FALSE; /* Edit this to true to make a default view disabled initially */

  /* Display: Defaults */
  $handler = $view->new_display('default', 'Defaults', 'default');
  $handler->display->display_options['title'] = 'Stocks';
  $handler->display->display_options['use_more_always'] = FALSE;
  $handler->display->display_options['access']['type'] = 'perm';
  $handler->display->display_options['access']['perm'] = 'access chado_stock content';
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
    'uniquename' => 'uniquename',
    'name_2' => 'name_2',
    'name' => 'name',
    'common_name' => 'common_name',
    'all_dbxref' => 'all_dbxref',
    'all_properties' => 'all_properties',
    'all_relationships' => 'all_relationships',
  );
  $handler->display->display_options['style_options']['default'] = '-1';
  $handler->display->display_options['style_options']['info'] = array(
    'uniquename' => array(
      'sortable' => 1,
      'separator' => '',
    ),
    'name_2' => array(
      'sortable' => 1,
      'separator' => '',
    ),
    'name' => array(
      'sortable' => 1,
      'separator' => '',
    ),
    'common_name' => array(
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
    'label-1' => 'Add Stock',
    'path-1' => 'node/add/tripal-stock',
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
  $handler->display->display_options['empty']['text']['content'] = 'No stocks match the supplied criteria.';
  $handler->display->display_options['empty']['text']['format'] = '1';
  /* Relationship: Stock: Stock => Node */
  $handler->display->display_options['relationships']['stock_id']['id'] = 'stock_id';
  $handler->display->display_options['relationships']['stock_id']['table'] = 'chado_stock';
  $handler->display->display_options['relationships']['stock_id']['field'] = 'stock_id';
  /* Field: Content: Nid */
  $handler->display->display_options['fields']['nid']['id'] = 'nid';
  $handler->display->display_options['fields']['nid']['table'] = 'node';
  $handler->display->display_options['fields']['nid']['field'] = 'nid';
  $handler->display->display_options['fields']['nid']['exclude'] = TRUE;
  /* Field: Chado Stock: Uniquename */
  $handler->display->display_options['fields']['uniquename']['id'] = 'uniquename';
  $handler->display->display_options['fields']['uniquename']['table'] = 'stock';
  $handler->display->display_options['fields']['uniquename']['field'] = 'uniquename';
  $handler->display->display_options['fields']['uniquename']['label'] = 'Unique Name';
  /* Field: Chado Stock: Name */
  $handler->display->display_options['fields']['name_2']['id'] = 'name_2';
  $handler->display->display_options['fields']['name_2']['table'] = 'stock';
  $handler->display->display_options['fields']['name_2']['field'] = 'name';
  /* Field: Chado Cvterm: Name */
  $handler->display->display_options['fields']['name']['id'] = 'name';
  $handler->display->display_options['fields']['name']['table'] = 'cvterm';
  $handler->display->display_options['fields']['name']['field'] = 'name';
  $handler->display->display_options['fields']['name']['label'] = 'Type';
  /* Field: Chado Organism: Common Name */
  $handler->display->display_options['fields']['common_name']['id'] = 'common_name';
  $handler->display->display_options['fields']['common_name']['table'] = 'organism';
  $handler->display->display_options['fields']['common_name']['field'] = 'common_name';
  $handler->display->display_options['fields']['common_name']['label'] = 'Organism';
  /* Field: Content: Delete link */
  $handler->display->display_options['fields']['delete_node']['id'] = 'delete_node';
  $handler->display->display_options['fields']['delete_node']['table'] = 'views_entity_node';
  $handler->display->display_options['fields']['delete_node']['field'] = 'delete_node';
  $handler->display->display_options['fields']['delete_node']['relationship'] = 'stock_id';
  $handler->display->display_options['fields']['delete_node']['exclude'] = TRUE;
  /* Field: Content: Edit link */
  $handler->display->display_options['fields']['edit_node']['id'] = 'edit_node';
  $handler->display->display_options['fields']['edit_node']['table'] = 'views_entity_node';
  $handler->display->display_options['fields']['edit_node']['field'] = 'edit_node';
  $handler->display->display_options['fields']['edit_node']['relationship'] = 'stock_id';
  $handler->display->display_options['fields']['edit_node']['exclude'] = TRUE;
  /* Field: Global: Custom text */
  $handler->display->display_options['fields']['nothing']['id'] = 'nothing';
  $handler->display->display_options['fields']['nothing']['table'] = 'views';
  $handler->display->display_options['fields']['nothing']['field'] = 'nothing';
  $handler->display->display_options['fields']['nothing']['label'] = '';
  $handler->display->display_options['fields']['nothing']['alter']['text'] = '[edit_node]   [delete_node]';
  $handler->display->display_options['fields']['nothing']['element_label_colon'] = FALSE;
  /* Sort criterion: Chado Organism: Common Name */
  $handler->display->display_options['sorts']['common_name']['id'] = 'common_name';
  $handler->display->display_options['sorts']['common_name']['table'] = 'organism';
  $handler->display->display_options['sorts']['common_name']['field'] = 'common_name';
  /* Sort criterion: Chado Stock: Uniquename */
  $handler->display->display_options['sorts']['uniquename']['id'] = 'uniquename';
  $handler->display->display_options['sorts']['uniquename']['table'] = 'stock';
  $handler->display->display_options['sorts']['uniquename']['field'] = 'uniquename';
  /* Filter criterion: Chado Organism: Common Name */
  $handler->display->display_options['filters']['common_name']['id'] = 'common_name';
  $handler->display->display_options['filters']['common_name']['table'] = 'organism';
  $handler->display->display_options['filters']['common_name']['field'] = 'common_name';
  $handler->display->display_options['filters']['common_name']['value'] = 'All';
  $handler->display->display_options['filters']['common_name']['group'] = '0';
  $handler->display->display_options['filters']['common_name']['exposed'] = TRUE;
  $handler->display->display_options['filters']['common_name']['expose']['operator_id'] = 'common_name_op';
  $handler->display->display_options['filters']['common_name']['expose']['label'] = 'Organism';
  $handler->display->display_options['filters']['common_name']['expose']['operator'] = 'common_name_op';
  $handler->display->display_options['filters']['common_name']['expose']['identifier'] = 'organism_common_name';
  $handler->display->display_options['filters']['common_name']['expose']['remember_roles'] = array(
    2 => '2',
    1 => 0,
    3 => 0,
  );
  /* Filter criterion: Chado Stock: Type Id */
  $handler->display->display_options['filters']['type_id']['id'] = 'type_id';
  $handler->display->display_options['filters']['type_id']['table'] = 'stock';
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
  /* Filter criterion: Chado Stock: Name */
  $handler->display->display_options['filters']['name']['id'] = 'name';
  $handler->display->display_options['filters']['name']['table'] = 'stock';
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
  $handler->display->display_options['filters']['common_name']['expose']['values_form_type'] = 'select';
  $handler->display->display_options['filters']['common_name']['expose']['select_multiple'] = FALSE;
  $handler->display->display_options['filters']['common_name']['expose']['select_optional'] = TRUE;
  $handler->display->display_options['filters']['common_name']['expose']['max_length'] = 40;
  $handler->display->display_options['filters']['type_id']['expose']['values_form_type'] = 'select';
  $handler->display->display_options['filters']['type_id']['expose']['select_multiple'] = FALSE;
  $handler->display->display_options['filters']['type_id']['expose']['select_optional'] = TRUE;
  $handler->display->display_options['filters']['type_id']['expose']['max_length'] = 40;

  /* Display: Page */
  $handler = $view->new_display('page', 'Page', 'page_1');
  $handler->display->display_options['path'] = 'admin/tripal/chado/tripal_stock/stocks';
  $handler->display->display_options['menu']['type'] = 'default tab';
  $handler->display->display_options['menu']['title'] = 'Stocks';
  $handler->display->display_options['menu']['description'] = 'A stock is the physical entities, either living or preserved';
  $handler->display->display_options['menu']['weight'] = '-10';
  $handler->display->display_options['menu']['context'] = 0;
  $handler->display->display_options['menu']['context_only_inline'] = 0;
  $handler->display->display_options['tab_options']['weight'] = '0';

  /** MANUALLY ADD since filter handler no exporting correctly
  $handler->display->display_options['filters']['common_name']['expose']['values_form_type'] = 'select';
  $handler->display->display_options['filters']['common_name']['expose']['select_multiple'] = FALSE;
  $handler->display->display_options['filters']['common_name']['expose']['select_optional'] = TRUE;
  $handler->display->display_options['filters']['common_name']['expose']['max_length'] = 40;
  $handler->display->display_options['filters']['type_id']['expose']['values_form_type'] = 'select';
  $handler->display->display_options['filters']['type_id']['expose']['select_multiple'] = FALSE;
  $handler->display->display_options['filters']['type_id']['expose']['select_optional'] = TRUE;
  $handler->display->display_options['filters']['type_id']['expose']['max_length'] = 40;
  */
  return $view;
}
