<?php
/**
 * @file
 * Describes stock default views.
 */

/**
 * Implements hook_views_default_views().
 *
 * @ingroup tripal_legacy_stock
 */
function tripal_stock_views_default_views() {
  $views = [];

  // User View ("Search Biological Content")
  // Remember, if you change the name/path of this view,
  // you also want to change it's description in tripal_stock_search_biological_data_views()
  $view = tripal_stock_defaultvalue_user_stocks();
  $view = tripal_make_view_compatible_with_external($view);
  $views[$view->name] = $view;

  // Admin view
  $view = tripal_stock_defaultview_admin_stocks();
  $view = tripal_make_view_compatible_with_external($view);
  $views[$view->name] = $view;

  return $views;
}

/**
 * Defines the stock user search default view.
 *
 * @ingroup tripal_legacy_stock
 */
function tripal_stock_defaultvalue_user_stocks() {

  // As a shortcut, currently I am copying the admin view and then tweaking it
  // In the future this should be replaced by a views export
  $view = tripal_stock_defaultview_admin_stocks();

  // change some view admin stuff
  $view->name = 'tripal_stock_user_stocks';
  $view->human_name = 'Stock User Search';
  $view->description = 'Allows users to search stock data';
  $view->tag = 'tripal search';

  // Change the path
  $view->display['page_1']->display_options['path'] = 'chado/stock';
  $view->display['page_1']->display_options['menu'] = [
    'type' => 'normal',
    'title' => 'Stocks',
    'description' => 'A stock is the physical entities of an organism, either living or preserved.',
    'weight' => '-10',
  ];
  unset($view->display['page_1']->display_options['tab_options']);

  // Change the permission
  $view->display['default']->display_options['access']['perm'] = 'access chado_stock content';

  // Remove edit/delete node
  unset(
    $view->display['default']->display_options['fields']['delete_node'],
    $view->display['default']->display_options['fields']['edit_node'],
    $view->display['default']->display_options['fields']['nothing']
  );

  // Remove the admin action links
  unset($view->display['default']->display_options['header']['action_links_area']);

  return $view;
}

/**
 * Default stock administration view
 *
 * @ingroup tripal_legacy_stock
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
  $handler->display->display_options['exposed_form']['type'] = 'input_required';
  $handler->display->display_options['exposed_form']['options']['submit_button'] = 'Search';
  $handler->display->display_options['exposed_form']['options']['text_input_required'] = 'Click search to see a listing of stocks that meet the filter requirements. Use the filters to restrict this set to a more reasonable number of stocks or to find a specific stock.';
  $handler->display->display_options['exposed_form']['options']['text_input_required_format'] = 'full_html';
  $handler->display->display_options['pager']['type'] = 'full';
  $handler->display->display_options['pager']['options']['items_per_page'] = '25';
  $handler->display->display_options['pager']['options']['offset'] = '0';
  $handler->display->display_options['pager']['options']['id'] = '0';
  $handler->display->display_options['pager']['options']['quantity'] = '9';
  $handler->display->display_options['style_plugin'] = 'table';
  $handler->display->display_options['style_options']['grouping'] = '';
  $handler->display->display_options['style_options']['columns'] = [
    'uniquename' => 'uniquename',
    'name_2' => 'name_2',
    'name' => 'name',
    'common_name' => 'common_name',
    'all_dbxref' => 'all_dbxref',
    'all_properties' => 'all_properties',
    'all_relationships' => 'all_relationships',
  ];
  $handler->display->display_options['style_options']['default'] = '-1';
  $handler->display->display_options['style_options']['info'] = [
    'uniquename' => [
      'sortable' => 1,
      'separator' => '',
    ],
    'name_2' => [
      'sortable' => 1,
      'separator' => '',
    ],
    'name' => [
      'sortable' => 1,
      'separator' => '',
    ],
    'common_name' => [
      'sortable' => 1,
      'separator' => '',
    ],
    'all_dbxref' => [
      'separator' => '',
    ],
    'all_properties' => [
      'separator' => '',
    ],
    'all_relationships' => [
      'separator' => '',
    ],
  ];
  /* Header: Global: Action Links */
  $handler->display->display_options['header']['action_links_area']['id'] = 'action_links_area';
  $handler->display->display_options['header']['action_links_area']['table'] = 'views';
  $handler->display->display_options['header']['action_links_area']['field'] = 'action_links_area';
  $handler->display->display_options['header']['action_links_area']['label'] = 'Action Links';
  $handler->display->display_options['header']['action_links_area']['empty'] = TRUE;
  $handler->display->display_options['header']['action_links_area']['link-1'] = [
    'label-1' => 'Add Stock',
    'path-1' => 'node/add/tripal-stock',
  ];
  $handler->display->display_options['header']['action_links_area']['link-2'] = [
    'label-2' => '',
    'path-2' => '',
  ];
  $handler->display->display_options['header']['action_links_area']['link-3'] = [
    'label-3' => '',
    'path-3' => '',
  ];
  $handler->display->display_options['header']['action_links_area']['link-4'] = [
    'label-4' => '',
    'path-4' => '',
  ];
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
  /* Field: Node URL */
  $handler->display->display_options['fields']['nid_1']['id'] = 'nid_1';
  $handler->display->display_options['fields']['nid_1']['table'] = 'node';
  $handler->display->display_options['fields']['nid_1']['field'] = 'nid';
  $handler->display->display_options['fields']['nid_1']['relationship'] = 'feature_id';
  $handler->display->display_options['fields']['nid_1']['ui_name'] = 'Node URL';
  $handler->display->display_options['fields']['nid_1']['label'] = '';
  $handler->display->display_options['fields']['nid_1']['exclude'] = TRUE;
  $handler->display->display_options['fields']['nid_1']['alter']['alter_text'] = TRUE;
  $handler->display->display_options['fields']['nid_1']['alter']['text'] = 'node/[nid]';
  $handler->display->display_options['fields']['nid_1']['element_label_colon'] = FALSE;
  /* Field: Chado Stock: Uniquename */
  $handler->display->display_options['fields']['uniquename']['id'] = 'uniquename';
  $handler->display->display_options['fields']['uniquename']['table'] = 'stock';
  $handler->display->display_options['fields']['uniquename']['field'] = 'uniquename';
  $handler->display->display_options['fields']['uniquename']['label'] = 'Unique Name';
  $handler->display->display_options['fields']['uniquename']['alter']['make_link'] = TRUE;
  $handler->display->display_options['fields']['uniquename']['alter']['path'] = '[nid_1]';
  /* Field: Chado Stock: Name */
  $handler->display->display_options['fields']['name_2']['id'] = 'name_2';
  $handler->display->display_options['fields']['name_2']['table'] = 'stock';
  $handler->display->display_options['fields']['name_2']['field'] = 'name';
  $handler->display->display_options['fields']['name_2']['alter']['make_link'] = TRUE;
  $handler->display->display_options['fields']['name_2']['alter']['path'] = '[nid_1]';
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
  /* Sort criterion: Chado Stock: Id */
  $handler->display->display_options['sorts']['stock_id']['id'] = 'stock_id';
  $handler->display->display_options['sorts']['stock_id']['table'] = 'stock';
  $handler->display->display_options['sorts']['stock_id']['field'] = 'stock_id';
  $handler->display->display_options['sorts']['stock_id']['order'] = 'DESC';
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
  $handler->display->display_options['filters']['common_name']['expose']['remember_roles'] = [
    2 => '2',
    1 => 0,
    3 => 0,
  ];
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
  $handler->display->display_options['filters']['type_id']['expose']['remember_roles'] = [
    2 => '2',
    1 => 0,
    3 => 0,
  ];
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
  $handler->display->display_options['filters']['name']['expose']['remember_roles'] = [
    2 => '2',
    1 => 0,
    3 => 0,
  ];

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
  $handler->display->display_options['path'] = 'admin/tripal/legacy/tripal_stock/stocks';
  $handler->display->display_options['menu']['type'] = 'default tab';
  $handler->display->display_options['menu']['title'] = 'Stocks';
  $handler->display->display_options['menu']['description'] = 'A stock is the physical entities, either living or preserved';
  $handler->display->display_options['menu']['weight'] = '-10';
  $handler->display->display_options['menu']['context'] = 0;
  $handler->display->display_options['menu']['context_only_inline'] = 0;
  $handler->display->display_options['tab_options']['weight'] = '0';

  /** MANUALLY ADD since filter handler no exporting correctly
   * $handler->display->display_options['filters']['common_name']['expose']['values_form_type'] = 'select';
   * $handler->display->display_options['filters']['common_name']['expose']['select_multiple'] = FALSE;
   * $handler->display->display_options['filters']['common_name']['expose']['select_optional'] = TRUE;
   * $handler->display->display_options['filters']['common_name']['expose']['max_length'] = 40;
   * $handler->display->display_options['filters']['type_id']['expose']['values_form_type'] = 'select';
   * $handler->display->display_options['filters']['type_id']['expose']['select_multiple'] = FALSE;
   * $handler->display->display_options['filters']['type_id']['expose']['select_optional'] = TRUE;
   * $handler->display->display_options['filters']['type_id']['expose']['max_length'] = 40;
   */
  return $view;
}
