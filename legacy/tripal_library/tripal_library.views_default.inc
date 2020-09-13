<?php
/**
 * @file
 * Describes library default views
 */

/**
 * Implements hook_views_default_views().
 *
 * @ingroup tripal_legacy_library
 */
function tripal_library_views_default_views() {
  $views = [];

  // User View ("Search Biological Content")
  // Remember, if you change the name/path of this view,
  // you also want to change it's description in tripal_library_search_biological_data_views()
  $view = tripal_library_defaultvalue_user_library();
  $view = tripal_make_view_compatible_with_external($view);
  $views[$view->name] = $view;

  // Admin view
  $view = tripal_library_admin_defaultviews_library();
  $view = tripal_make_view_compatible_with_external($view);
  $views[$view->name] = $view;

  return $views;
}

/**
 * Defines the library user search default view.
 *
 * @ingroup tripal_legacy_library
 */
function tripal_library_defaultvalue_user_library() {

  // As a shortcut, currently I am copying the admin view and then tweaking it
  // In the future this should be replaced by a views export
  $view = tripal_library_admin_defaultviews_library();

  // change some view admin stuff
  $view->name = 'tripal_library_user_library';
  $view->human_name = 'Library User Search';
  $view->description = 'Allows users to search library data';
  $view->tag = 'tripal search';

  // Change the path
  $view->display['page_1']->display_options['path'] = 'chado/library';
  $view->display['page_1']->display_options['menu'] = [
    'type' => 'normal',
    'title' => 'Libraries',
    'description' => 'Biological library including BAC, cDNA, FOSMID, etc.',
    'weight' => '-10',
  ];
  unset($view->display['page_1']->display_options['tab_options']);

  // Change the permission
  $view->display['default']->display_options['access']['perm'] = 'access chado_library content';

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
 * Default administration library view
 *
 * @ingroup tripal_legacy_library
 */
function tripal_library_admin_defaultviews_library() {

  $view = new view();
  $view->name = 'tripal_library_admin_libraries';
  $view->description = 'DO NOT DISABLE';
  $view->tag = 'tripal admin';
  $view->base_table = 'library';
  $view->human_name = 'Library Admin';
  $view->core = 0;
  $view->api_version = '3.0';
  $view->disabled = FALSE; /* Edit this to true to make a default view disabled initially */

  /* Display: Defaults */
  $handler = $view->new_display('default', 'Defaults', 'default');
  $handler->display->display_options['title'] = 'Libraries';
  $handler->display->display_options['use_more_always'] = FALSE;
  $handler->display->display_options['access']['type'] = 'perm';
  $handler->display->display_options['access']['perm'] = 'access chado_library content';
  $handler->display->display_options['cache']['type'] = 'none';
  $handler->display->display_options['query']['type'] = 'views_query';
  $handler->display->display_options['exposed_form']['type'] = 'input_required';
  $handler->display->display_options['exposed_form']['options']['submit_button'] = 'Search';
  $handler->display->display_options['exposed_form']['options']['text_input_required'] = 'Click search to see a listing of libraries that meet the filter requirements. Use the filters to restrict this set to a more reasonable number of libraries or to find a specific library.';
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
    'name_1' => 'name_1',
    'common_name' => 'common_name',
    'name' => 'name',
    'is_obsolete' => 'is_obsolete',
  ];
  $handler->display->display_options['style_options']['default'] = 'uniquename';
  $handler->display->display_options['style_options']['info'] = [
    'uniquename' => [
      'sortable' => 1,
      'separator' => '',
    ],
    'name_1' => [
      'sortable' => 1,
      'separator' => '',
    ],
    'common_name' => [
      'sortable' => 1,
      'separator' => '',
    ],
    'name' => [
      'sortable' => 1,
      'separator' => '',
    ],
    'is_obsolete' => [
      'sortable' => 1,
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
    'label-1' => 'Add Library',
    'path-1' => 'node/add/tripal-library',
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
  $handler->display->display_options['empty']['text']['content'] = 'No libraries match the supplied criteria.';
  $handler->display->display_options['empty']['text']['format'] = '1';
  /* Relationship: Library: Library => Node */
  $handler->display->display_options['relationships']['library_id']['id'] = 'library_id';
  $handler->display->display_options['relationships']['library_id']['table'] = 'chado_library';
  $handler->display->display_options['relationships']['library_id']['field'] = 'library_id';
  /* Field: Content: Nid */
  $handler->display->display_options['fields']['nid']['id'] = 'nid';
  $handler->display->display_options['fields']['nid']['table'] = 'node';
  $handler->display->display_options['fields']['nid']['field'] = 'nid';
  $handler->display->display_options['fields']['nid']['relationship'] = 'library_id';
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
  /* Field: Chado Library: Uniquename */
  $handler->display->display_options['fields']['uniquename']['id'] = 'uniquename';
  $handler->display->display_options['fields']['uniquename']['table'] = 'library';
  $handler->display->display_options['fields']['uniquename']['field'] = 'uniquename';
  $handler->display->display_options['fields']['uniquename']['label'] = 'Unique Name';
  $handler->display->display_options['fields']['uniquename']['alter']['make_link'] = TRUE;
  $handler->display->display_options['fields']['uniquename']['alter']['path'] = '[nid_1]';
  /* Field: Chado Library: Name */
  $handler->display->display_options['fields']['name_1']['id'] = 'name_1';
  $handler->display->display_options['fields']['name_1']['table'] = 'library';
  $handler->display->display_options['fields']['name_1']['field'] = 'name';
  $handler->display->display_options['fields']['name']['alter']['make_link'] = TRUE;
  $handler->display->display_options['fields']['name']['alter']['path'] = '[nid_1]';
  /* Field: Chado Organism: Common Name */
  $handler->display->display_options['fields']['common_name']['id'] = 'common_name';
  $handler->display->display_options['fields']['common_name']['table'] = 'organism';
  $handler->display->display_options['fields']['common_name']['field'] = 'common_name';
  $handler->display->display_options['fields']['common_name']['label'] = 'Organism';
  /* Field: Chado Cvterm: Name */
  $handler->display->display_options['fields']['name']['id'] = 'name';
  $handler->display->display_options['fields']['name']['table'] = 'cvterm';
  $handler->display->display_options['fields']['name']['field'] = 'name';
  $handler->display->display_options['fields']['name']['label'] = 'Type';
  /* Field: Chado Library: Is Obsolete */
  $handler->display->display_options['fields']['is_obsolete']['id'] = 'is_obsolete';
  $handler->display->display_options['fields']['is_obsolete']['table'] = 'library';
  $handler->display->display_options['fields']['is_obsolete']['field'] = 'is_obsolete';
  $handler->display->display_options['fields']['is_obsolete']['label'] = 'Obsolete?';
  $handler->display->display_options['fields']['is_obsolete']['alter']['alter_text'] = TRUE;
  $handler->display->display_options['fields']['is_obsolete']['alter']['text'] = 'Yes';
  $handler->display->display_options['fields']['is_obsolete']['empty'] = 'No';
  $handler->display->display_options['fields']['is_obsolete']['empty_zero'] = TRUE;
  $handler->display->display_options['fields']['is_obsolete']['separator'] = '';
  /* Field: Content: Delete link */
  $handler->display->display_options['fields']['delete_node']['id'] = 'delete_node';
  $handler->display->display_options['fields']['delete_node']['table'] = 'views_entity_node';
  $handler->display->display_options['fields']['delete_node']['field'] = 'delete_node';
  $handler->display->display_options['fields']['delete_node']['relationship'] = 'library_id';
  $handler->display->display_options['fields']['delete_node']['exclude'] = TRUE;
  /* Field: Content: Edit link */
  $handler->display->display_options['fields']['edit_node']['id'] = 'edit_node';
  $handler->display->display_options['fields']['edit_node']['table'] = 'views_entity_node';
  $handler->display->display_options['fields']['edit_node']['field'] = 'edit_node';
  $handler->display->display_options['fields']['edit_node']['relationship'] = 'library_id';
  $handler->display->display_options['fields']['edit_node']['exclude'] = TRUE;
  /* Field: Global: Custom text */
  $handler->display->display_options['fields']['nothing']['id'] = 'nothing';
  $handler->display->display_options['fields']['nothing']['table'] = 'views';
  $handler->display->display_options['fields']['nothing']['field'] = 'nothing';
  $handler->display->display_options['fields']['nothing']['label'] = '';
  $handler->display->display_options['fields']['nothing']['alter']['text'] = '[edit_node]   [delete_node]';
  $handler->display->display_options['fields']['nothing']['element_label_colon'] = FALSE;
  /* Sort criterion: Chado Library: Id */
  $handler->display->display_options['sorts']['library_id']['id'] = 'library_id';
  $handler->display->display_options['sorts']['library_id']['table'] = 'library';
  $handler->display->display_options['sorts']['library_id']['field'] = 'library_id';
  $handler->display->display_options['sorts']['library_id']['order'] = 'DESC';
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
  /* Filter criterion: Chado Library: Type Id */
  $handler->display->display_options['filters']['type_id']['id'] = 'type_id';
  $handler->display->display_options['filters']['type_id']['table'] = 'library';
  $handler->display->display_options['filters']['type_id']['field'] = 'type_id';
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

  /** MANUALLY ADD since filter handler no exporting correctly */
  $handler->display->display_options['filters']['common_name']['expose']['values_form_type'] = 'select';
  $handler->display->display_options['filters']['common_name']['expose']['select_multiple'] = FALSE;
  $handler->display->display_options['filters']['common_name']['expose']['select_optional'] = TRUE;
  $handler->display->display_options['filters']['common_name']['expose']['max_length'] = 40;

  /* Display: Page */
  $handler = $view->new_display('page', 'Page', 'page_1');
  $handler->display->display_options['path'] = 'admin/tripal/legacy/tripal_library/libraries';
  $handler->display->display_options['menu']['type'] = 'default tab';
  $handler->display->display_options['menu']['title'] = 'Libraries';
  $handler->display->display_options['menu']['description'] = 'A library is a collection of features of a given type. For example, a cDNA or BAC clone library.';
  $handler->display->display_options['menu']['weight'] = '0';
  $handler->display->display_options['menu']['name'] = 'management';
  $handler->display->display_options['menu']['context'] = 0;
  $handler->display->display_options['menu']['context_only_inline'] = 0;
  $handler->display->display_options['tab_options']['weight'] = '0';

  /** MANUALLY ADD since filter handler no exporting correctly
   * $handler->display->display_options['filters']['common_name']['expose']['values_form_type'] = 'select';
   * $handler->display->display_options['filters']['common_name']['expose']['select_multiple'] = FALSE;
   * $handler->display->display_options['filters']['common_name']['expose']['select_optional'] = TRUE;
   * $handler->display->display_options['filters']['common_name']['expose']['max_length'] = 40;
   */

  return $view;
}
