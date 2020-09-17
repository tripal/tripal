<?php
/**
 * @file
 * Default feature map views
 */

/**
 * Implements hook_views_default_views().
 *
 * @ingroup tripal_legacy_featuremap
 */
function tripal_featuremap_views_default_views() {
  $views = [];

  // User View ("Search Biological Content")
  // Remember, if you change the name/path of this view,
  // you also want to change it's description in tripal_featuremap_search_biological_data_views()
  $view = tripal_featuremap_defaultvalue_user_featuremaps();
  $view = tripal_make_view_compatible_with_external($view);
  $views[$view->name] = $view;

  // Admin view
  $view = tripal_featuremap_defaultview_admin_featuremaps();
  $view = tripal_make_view_compatible_with_external($view);
  $views[$view->name] = $view;

  return $views;
}

/**
 * Defines the featuremap user search default view.
 *
 * @ingroup tripal_legacy_featuremap
 */
function tripal_featuremap_defaultvalue_user_featuremaps() {

  // As a shortcut, currently I am copying the admin view and then tweaking it
  // In the future this should be replaced by a views export
  $view = tripal_featuremap_defaultview_admin_featuremaps();

  // change some view admin stuff
  $view->name = 'tripal_featuremap_user_featuremaps';
  $view->human_name = 'Feature Map User Search';
  $view->description = 'Allows users to search feature map data';
  $view->tag = 'tripal search';

  // Change the path
  $view->display['page_1']->display_options['path'] = 'chado/featuremap';
  $view->display['page_1']->display_options['menu'] = [
    'type' => 'normal',
    'title' => 'Maps',
    'description' => 'A map of features from the chado database (e.g. genetic map)',
    'weight' => '-10',
  ];
  unset($view->display['page_1']->display_options['tab_options']);

  // Change the permission
  $view->display['default']->display_options['access']['perm'] = 'access chado_featuremap content';

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
 * Describes the default featuremap admin view
 *
 * @ingroup tripal_legacy_featuremap
 */
function tripal_featuremap_defaultview_admin_featuremaps() {

  $view = new view();
  $view->name = 'tripal_featuremap_admin_featuremaps';
  $view->description = 'DO NOT DISABLE';
  $view->tag = 'tripal admin';
  $view->base_table = 'featuremap';
  $view->human_name = 'Feature Map Admin';
  $view->core = 6;
  $view->api_version = '3.0';
  $view->disabled = FALSE; /* Edit this to true to make a default view disabled initially */

  /* Display: Defaults */
  $handler = $view->new_display('default', 'Defaults', 'default');
  $handler->display->display_options['title'] = 'Maps';
  $handler->display->display_options['use_more_always'] = FALSE;
  $handler->display->display_options['access']['type'] = 'perm';
  $handler->display->display_options['access']['perm'] = 'access chado_featuremap content';
  $handler->display->display_options['cache']['type'] = 'none';
  $handler->display->display_options['query']['type'] = 'views_query';
  $handler->display->display_options['exposed_form']['type'] = 'input_required';
  $handler->display->display_options['exposed_form']['options']['submit_button'] = 'Search';
  $handler->display->display_options['exposed_form']['options']['text_input_required'] = 'Click search to see a listing of feature maps that meet the filter requirements. Use the filters to restrict this set to a more reasonable number of maps or to find a specific map.';
  $handler->display->display_options['exposed_form']['options']['text_input_required_format'] = 'full_html';
  $handler->display->display_options['pager']['type'] = 'full';
  $handler->display->display_options['pager']['options']['items_per_page'] = '25';
  $handler->display->display_options['pager']['options']['offset'] = '0';
  $handler->display->display_options['pager']['options']['id'] = '0';
  $handler->display->display_options['pager']['options']['quantity'] = '9';
  $handler->display->display_options['style_plugin'] = 'table';
  $handler->display->display_options['style_options']['grouping'] = '';
  $handler->display->display_options['style_options']['columns'] = [
    'name' => 'name',
    'description' => 'description',
    'name_1' => 'name_1',
    'nid' => 'nid',
  ];
  $handler->display->display_options['style_options']['default'] = '-1';
  $handler->display->display_options['style_options']['info'] = [
    'name' => [
      'sortable' => 1,
      'separator' => '',
    ],
    'description' => [
      'sortable' => 1,
      'separator' => '',
    ],
    'name_1' => [
      'sortable' => 1,
      'separator' => '',
    ],
    'nid' => [
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
    'label-1' => 'Add Map',
    'path-1' => 'node/add/tripal-featuremap',
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
  $handler->display->display_options['empty']['text']['content'] = 'No feature maps match the current criteria.';
  $handler->display->display_options['empty']['text']['format'] = '1';
  /* Relationship: Featuremap: Featuremap => Node */
  $handler->display->display_options['relationships']['featuremap_id']['id'] = 'featuremap_id';
  $handler->display->display_options['relationships']['featuremap_id']['table'] = 'chado_featuremap';
  $handler->display->display_options['relationships']['featuremap_id']['field'] = 'featuremap_id';
  /* Field: Content: Nid */
  $handler->display->display_options['fields']['nid']['id'] = 'nid';
  $handler->display->display_options['fields']['nid']['table'] = 'node';
  $handler->display->display_options['fields']['nid']['field'] = 'nid';
  $handler->display->display_options['fields']['nid']['relationship'] = 'featuremap_id';
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
  /* Field: Chado Featuremap: Name */
  $handler->display->display_options['fields']['name']['id'] = 'name';
  $handler->display->display_options['fields']['name']['table'] = 'featuremap';
  $handler->display->display_options['fields']['name']['field'] = 'name';
  $handler->display->display_options['fields']['name']['alter']['make_link'] = TRUE;
  $handler->display->display_options['fields']['name']['alter']['path'] = '[nid_1]';
  /* Field: Chado Featuremap: Description */
  $handler->display->display_options['fields']['description']['id'] = 'description';
  $handler->display->display_options['fields']['description']['table'] = 'featuremap';
  $handler->display->display_options['fields']['description']['field'] = 'description';
  /* Field: Chado Cvterm: Name */
  $handler->display->display_options['fields']['name_1']['id'] = 'name_1';
  $handler->display->display_options['fields']['name_1']['table'] = 'cvterm';
  $handler->display->display_options['fields']['name_1']['field'] = 'name';
  $handler->display->display_options['fields']['name_1']['label'] = 'Map Units';
  /* Field: Content: Delete link */
  $handler->display->display_options['fields']['delete_node']['id'] = 'delete_node';
  $handler->display->display_options['fields']['delete_node']['table'] = 'views_entity_node';
  $handler->display->display_options['fields']['delete_node']['field'] = 'delete_node';
  $handler->display->display_options['fields']['delete_node']['relationship'] = 'featuremap_id';
  $handler->display->display_options['fields']['delete_node']['exclude'] = TRUE;
  /* Field: Content: Edit link */
  $handler->display->display_options['fields']['edit_node']['id'] = 'edit_node';
  $handler->display->display_options['fields']['edit_node']['table'] = 'views_entity_node';
  $handler->display->display_options['fields']['edit_node']['field'] = 'edit_node';
  $handler->display->display_options['fields']['edit_node']['relationship'] = 'featuremap_id';
  $handler->display->display_options['fields']['edit_node']['exclude'] = TRUE;
  /* Field: Global: Custom text */
  $handler->display->display_options['fields']['nothing']['id'] = 'nothing';
  $handler->display->display_options['fields']['nothing']['table'] = 'views';
  $handler->display->display_options['fields']['nothing']['field'] = 'nothing';
  $handler->display->display_options['fields']['nothing']['label'] = '';
  $handler->display->display_options['fields']['nothing']['alter']['text'] = '[edit_node]   [delete_node]';
  $handler->display->display_options['fields']['nothing']['element_label_colon'] = FALSE;
  /* Sort criterion: Chado Featuremap: Id */
  $handler->display->display_options['sorts']['featuremap_id']['id'] = 'featuremap_id';
  $handler->display->display_options['sorts']['featuremap_id']['table'] = 'featuremap';
  $handler->display->display_options['sorts']['featuremap_id']['field'] = 'featuremap_id';
  $handler->display->display_options['sorts']['featuremap_id']['order'] = 'DESC';
  /* Filter criterion: Chado Featuremap: Name */
  $handler->display->display_options['filters']['name_1']['id'] = 'name_1';
  $handler->display->display_options['filters']['name_1']['table'] = 'featuremap';
  $handler->display->display_options['filters']['name_1']['field'] = 'name';
  $handler->display->display_options['filters']['name_1']['operator'] = 'contains';
  $handler->display->display_options['filters']['name_1']['group'] = '0';
  $handler->display->display_options['filters']['name_1']['exposed'] = TRUE;
  $handler->display->display_options['filters']['name_1']['expose']['operator_id'] = 'name_1_op';
  $handler->display->display_options['filters']['name_1']['expose']['label'] = 'Name Contains';
  $handler->display->display_options['filters']['name_1']['expose']['operator'] = 'name_1_op';
  $handler->display->display_options['filters']['name_1']['expose']['identifier'] = 'name_1';
  $handler->display->display_options['filters']['name_1']['expose']['remember_roles'] = [
    2 => '2',
    1 => 0,
    3 => 0,
  ];
  /* Filter criterion: Chado Featuremap: Description */
  $handler->display->display_options['filters']['description']['id'] = 'description';
  $handler->display->display_options['filters']['description']['table'] = 'featuremap';
  $handler->display->display_options['filters']['description']['field'] = 'description';
  $handler->display->display_options['filters']['description']['operator'] = 'contains';
  $handler->display->display_options['filters']['description']['group'] = '0';
  $handler->display->display_options['filters']['description']['exposed'] = TRUE;
  $handler->display->display_options['filters']['description']['expose']['operator_id'] = 'description_op';
  $handler->display->display_options['filters']['description']['expose']['label'] = 'Description Contains';
  $handler->display->display_options['filters']['description']['expose']['operator'] = 'description_op';
  $handler->display->display_options['filters']['description']['expose']['identifier'] = 'description';
  $handler->display->display_options['filters']['description']['expose']['remember_roles'] = [
    2 => '2',
    1 => 0,
    3 => 0,
  ];

  /* Display: Page */
  $handler = $view->new_display('page', 'Page', 'page_1');
  $handler->display->display_options['path'] = 'admin/tripal/legacy/tripal_featuremap/featuremaps';
  $handler->display->display_options['menu']['type'] = 'default tab';
  $handler->display->display_options['menu']['title'] = 'Maps';
  $handler->display->display_options['menu']['description'] = 'A listing of chado feature maps.';
  $handler->display->display_options['menu']['weight'] = '-10';
  $handler->display->display_options['menu']['context'] = 0;
  $handler->display->display_options['menu']['context_only_inline'] = 0;
  $handler->display->display_options['tab_options']['weight'] = '0';

  return $view;
}
