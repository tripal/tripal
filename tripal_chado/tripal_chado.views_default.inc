<?php
/**
 * @file
 * Describes core default views
 */

/**
 * Describes core default views
 *
 * @ingroup tripal
 */
function tripal_chado_views_default_views() {
  $views = [];

  // Default Tripal custom tables view.
  $view = tripal_chado_defaultview_admin_custom_tables();
  $views[$view->name] = $view;

  // Default Trial materialized views view.
  $view = tripal_chado_defaultview_admin_mviews();
  $views[$view->name] = $view;

  // Default Tripal admin view: CVs.
  $view = tripal_chado_defaultview_admin_cvs_listing();
  $view = tripal_make_view_compatible_with_external($view);
  $views[$view->name] = $view;

  // Default Tripal admin view: CV Terms.
  $view = tripal_chado_defaultview_admin_cvterms_listing();
  $view = tripal_make_view_compatible_with_external($view);
  $views[$view->name] = $view;

  // Default Tripal Admin View: DB
  $view = tripal_chado_defaultview_admin_db_listing();
  $view = tripal_make_view_compatible_with_external($view);
  $views[$view->name] = $view;

  // Default Tripal Admin View: DB References
  $view = tripal_chado_defaultview_admin_dbxref_listing();
  $view = tripal_make_view_compatible_with_external($view);
  $views[$view->name] = $view;

  return $views;
}

/**
 * Describes the custom tables administration view.
 *
 * @ingroup tripal
 */
function tripal_chado_defaultview_admin_custom_tables() {

  $view = new view();
  $view->name = 'tripal_admin_custom_table';
  $view->description = 'DO NOT DISABLE';
  $view->tag = 'tripal admin';
  $view->base_table = 'tripal_custom_tables';
  $view->human_name = 'Chado Custom Table (Admin)';
  $view->core = 7;
  $view->api_version = '3.0';
  $view->disabled = FALSE; /* Edit this to true to make a default view disabled initially */

  /* Display: Master */
  $handler = $view->new_display('default', 'Master', 'default');
  $handler->display->display_options['title'] = 'Custom Tables';
  $handler->display->display_options['use_more_always'] = FALSE;
  $handler->display->display_options['access']['type'] = 'none';
  $handler->display->display_options['cache']['type'] = 'none';
  $handler->display->display_options['query']['type'] = 'views_query';
  $handler->display->display_options['exposed_form']['type'] = 'basic';
  $handler->display->display_options['exposed_form']['options']['submit_button'] = 'Filter';
  $handler->display->display_options['pager']['type'] = 'full';
  $handler->display->display_options['pager']['options']['items_per_page'] = '25';
  $handler->display->display_options['style_plugin'] = 'table';
  /* Header: Global: Action Links */
  $handler->display->display_options['header']['action_links_area']['id'] = 'action_links_area';
  $handler->display->display_options['header']['action_links_area']['table'] = 'views';
  $handler->display->display_options['header']['action_links_area']['field'] = 'action_links_area';
  $handler->display->display_options['header']['action_links_area']['label'] = 'Action Links';
  $handler->display->display_options['header']['action_links_area']['empty'] = TRUE;
  $handler->display->display_options['header']['action_links_area']['link-1'] = [
    'label-1' => 'Add Custom Table',
    'path-1' => 'admin/tripal/storage/chado/custom_tables/new',
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
  /* Field: Tripal Custom Tables: Custom Table ID */
  $handler->display->display_options['fields']['table_id']['id'] = 'table_id';
  $handler->display->display_options['fields']['table_id']['table'] = 'tripal_custom_tables';
  $handler->display->display_options['fields']['table_id']['field'] = 'table_id';
  $handler->display->display_options['fields']['table_id']['label'] = 'ID';
  $handler->display->display_options['fields']['table_id']['element_class'] = 'extra-short-column';
  $handler->display->display_options['fields']['table_id']['element_label_class'] = 'extra-short-column';
  $handler->display->display_options['fields']['table_id']['element_label_colon'] = FALSE;
  $handler->display->display_options['fields']['table_id']['separator'] = '';
  /* Field: Tripal Custom Tables: Table Name */
  $handler->display->display_options['fields']['table_name']['id'] = 'table_name';
  $handler->display->display_options['fields']['table_name']['table'] = 'tripal_custom_tables';
  $handler->display->display_options['fields']['table_name']['field'] = 'table_name';
  $handler->display->display_options['fields']['table_name']['label'] = 'Name';
  $handler->display->display_options['fields']['table_name']['alter']['make_link'] = TRUE;
  $handler->display->display_options['fields']['table_name']['alter']['path'] = 'admin/tripal/storage/chado/custom_tables/view/[table_id]';
  /* Field: Tripal Custom Tables: Materialized View ID */
  $handler->display->display_options['fields']['mview_id']['id'] = 'mview_id';
  $handler->display->display_options['fields']['mview_id']['table'] = 'tripal_custom_tables';
  $handler->display->display_options['fields']['mview_id']['field'] = 'mview_id';
  $handler->display->display_options['fields']['mview_id']['label'] = 'Is MView';
  $handler->display->display_options['fields']['mview_id']['alter']['alter_text'] = TRUE;
  $handler->display->display_options['fields']['mview_id']['alter']['text'] = 'Yes';
  $handler->display->display_options['fields']['mview_id']['hide_empty'] = TRUE;
  $handler->display->display_options['fields']['mview_id']['empty_zero'] = TRUE;
  /* Field: Global: Custom text */
  $handler->display->display_options['fields']['nothing']['id'] = 'nothing';
  $handler->display->display_options['fields']['nothing']['table'] = 'views';
  $handler->display->display_options['fields']['nothing']['field'] = 'nothing';
  $handler->display->display_options['fields']['nothing']['label'] = 'Edit Link';
  $handler->display->display_options['fields']['nothing']['exclude'] = TRUE;
  $handler->display->display_options['fields']['nothing']['alter']['text'] = 'Edit';
  $handler->display->display_options['fields']['nothing']['alter']['make_link'] = TRUE;
  $handler->display->display_options['fields']['nothing']['alter']['path'] = 'admin/tripal/storage/chado/custom_tables/edit/[table_id]';
  /* Field: Global: Custom text */
  $handler->display->display_options['fields']['nothing_1']['id'] = 'nothing_1';
  $handler->display->display_options['fields']['nothing_1']['table'] = 'views';
  $handler->display->display_options['fields']['nothing_1']['field'] = 'nothing';
  $handler->display->display_options['fields']['nothing_1']['label'] = 'Delete Link';
  $handler->display->display_options['fields']['nothing_1']['exclude'] = TRUE;
  $handler->display->display_options['fields']['nothing_1']['alter']['text'] = 'Delete';
  $handler->display->display_options['fields']['nothing_1']['alter']['make_link'] = TRUE;
  $handler->display->display_options['fields']['nothing_1']['alter']['path'] = 'admin/tripal/storage/chado/custom_tables/delete/[table_id]';
  /* Field: Global: Custom text */
  $handler->display->display_options['fields']['nothing_2']['id'] = 'nothing_2';
  $handler->display->display_options['fields']['nothing_2']['table'] = 'views';
  $handler->display->display_options['fields']['nothing_2']['field'] = 'nothing';
  $handler->display->display_options['fields']['nothing_2']['label'] = '';
  $handler->display->display_options['fields']['nothing_2']['alter']['text'] = '[nothing]   [nothing_1]';
  $handler->display->display_options['fields']['nothing_2']['element_label_colon'] = FALSE;
  /* Filter criterion: Tripal Custom Tables: Table Name */
  $handler->display->display_options['filters']['table_name']['id'] = 'table_name';
  $handler->display->display_options['filters']['table_name']['table'] = 'tripal_custom_tables';
  $handler->display->display_options['filters']['table_name']['field'] = 'table_name';
  $handler->display->display_options['filters']['table_name']['exposed'] = TRUE;
  $handler->display->display_options['filters']['table_name']['expose']['operator_id'] = 'table_name_op';
  $handler->display->display_options['filters']['table_name']['expose']['label'] = 'Table Name';
  $handler->display->display_options['filters']['table_name']['expose']['operator'] = 'table_name_op';
  $handler->display->display_options['filters']['table_name']['expose']['identifier'] = 'table_name';
  $handler->display->display_options['filters']['table_name']['expose']['remember_roles'] = [
    2 => '2',
    1 => 0,
    3 => 0,
  ];

  /* Display: Page */
  $handler = $view->new_display('page', 'Page', 'page');
  $handler->display->display_options['path'] = 'admin/tripal/storage/chado/custom_tables/tables';
  $handler->display->display_options['menu']['type'] = 'default tab';
  $handler->display->display_options['menu']['title'] = 'Custom Tables';
  $handler->display->display_options['menu']['description'] = 'A list of existing custom tables';
  $handler->display->display_options['menu']['weight'] = '-10';
  $handler->display->display_options['menu']['name'] = 'management';
  $handler->display->display_options['menu']['context'] = 0;
  $handler->display->display_options['menu']['context_only_inline'] = 0;
  $handler->display->display_options['tab_options']['weight'] = '0';

  return $view;
}

/**
 * Describes the materialized views administration view.
 *
 * @ingroup tripal
 */
function tripal_chado_defaultview_admin_mviews() {

  $view = new view();
  $view->name = 'tripal_admin_mviews';
  $view->description = 'DO NOT DISABLE';
  $view->tag = 'tripal admin';
  $view->base_table = 'tripal_mviews';
  $view->human_name = 'Chado Materialized View (Admin)';
  $view->core = 7;
  $view->api_version = '3.0';
  $view->disabled = FALSE; /* Edit this to true to make a default view disabled initially */

  /* Display: Master */
  $handler = $view->new_display('default', 'Master', 'default');
  $handler->display->display_options['title'] = 'Materialized Views';
  $handler->display->display_options['use_more_always'] = FALSE;
  $handler->display->display_options['access']['type'] = 'none';
  $handler->display->display_options['cache']['type'] = 'none';
  $handler->display->display_options['query']['type'] = 'views_query';
  $handler->display->display_options['exposed_form']['type'] = 'basic';
  $handler->display->display_options['exposed_form']['options']['submit_button'] = 'Filter';
  $handler->display->display_options['pager']['type'] = 'full';
  $handler->display->display_options['pager']['options']['items_per_page'] = '25';
  $handler->display->display_options['style_plugin'] = 'table';
  $handler->display->display_options['style_options']['columns'] = [
    'mview_id' => 'mview_id',
    'name' => 'name',
    'mv_table' => 'mv_table',
    'comment' => 'comment',
    'last_update' => 'last_update',
    'status' => 'status',
  ];
  $handler->display->display_options['style_options']['default'] = 'mview_id';
  $handler->display->display_options['style_options']['info'] = [
    'mview_id' => [
      'sortable' => 1,
      'default_sort_order' => 'asc',
      'align' => '',
      'separator' => '',
      'empty_column' => 0,
    ],
    'name' => [
      'sortable' => 1,
      'default_sort_order' => 'asc',
      'align' => '',
      'separator' => '',
      'empty_column' => 0,
    ],
    'mv_table' => [
      'sortable' => 1,
      'default_sort_order' => 'asc',
      'align' => '',
      'separator' => '',
      'empty_column' => 0,
    ],
    'comment' => [
      'sortable' => 0,
      'default_sort_order' => 'asc',
      'align' => '',
      'separator' => '',
      'empty_column' => 0,
    ],
    'last_update' => [
      'sortable' => 1,
      'default_sort_order' => 'asc',
      'align' => '',
      'separator' => '',
      'empty_column' => 0,
    ],
    'status' => [
      'sortable' => 1,
      'default_sort_order' => 'asc',
      'align' => '',
      'separator' => '',
      'empty_column' => 0,
    ],
  ];
  /* Header: Global: Text area */
  $handler->display->display_options['header']['area']['id'] = 'area';
  $handler->display->display_options['header']['area']['table'] = 'views';
  $handler->display->display_options['header']['area']['field'] = 'area';
  $handler->display->display_options['header']['area']['label'] = 'Description';
  $handler->display->display_options['header']['area']['empty'] = TRUE;
  $handler->display->display_options['header']['area']['content'] = '<p>Materialized Views (MViews) are custom tables populated with a defined SQL statement. Because Chado is highly normalized and highly constrained it serves as a wonderful data storage platform, but unfortunately some queries may be slow. MViews alleviate slowness by aggregating data into tables that are more easy to query. Use MViews to create tables for custom search pages or custom Tripal module development.</p>
  <p>MViews behaves in the following way:</p>
  <ul>
  <li>The SQL statement defined for an MVIEW will be used to populate the table</li>
  <li>Altering the table structure of an MView will cause the MView table to be dropped and recreated. All records in the MView will be lost.</li>
  <li>Altering the query of an existing view will not change the MView table. No records will be lost.</li>
  <li>Repopulating an MView that is already populated will result in replacement of all records.</li>
  <li>A database transaction will be used when populating MViews. Therefore replacement of records does not occur until the query completes. Any search forms or pages dependent on the MView will continue to function.</li>
  </ul>';
  $handler->display->display_options['header']['area']['format'] = 'full_html';
  /* Header: Global: Action Links */
  $handler->display->display_options['header']['action_links_area']['id'] = 'action_links_area';
  $handler->display->display_options['header']['action_links_area']['table'] = 'views';
  $handler->display->display_options['header']['action_links_area']['field'] = 'action_links_area';
  $handler->display->display_options['header']['action_links_area']['label'] = 'Action Links';
  $handler->display->display_options['header']['action_links_area']['link-1'] = [
    'label-1' => 'Add Materialized View',
    'path-1' => 'admin/tripal/storage/chado/mviews/new',
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
  /* Field: Tripal Materialized Views: Materialized View ID */
  $handler->display->display_options['fields']['mview_id']['id'] = 'mview_id';
  $handler->display->display_options['fields']['mview_id']['table'] = 'tripal_mviews';
  $handler->display->display_options['fields']['mview_id']['field'] = 'mview_id';
  $handler->display->display_options['fields']['mview_id']['label'] = '';
  $handler->display->display_options['fields']['mview_id']['element_class'] = 'extra-short-column';
  $handler->display->display_options['fields']['mview_id']['element_label_class'] = 'extra-short-column';
  $handler->display->display_options['fields']['mview_id']['element_label_colon'] = FALSE;
  $handler->display->display_options['fields']['mview_id']['separator'] = '';
  /* Field: Tripal Materialized Views: Name */
  $handler->display->display_options['fields']['name']['id'] = 'name';
  $handler->display->display_options['fields']['name']['table'] = 'tripal_mviews';
  $handler->display->display_options['fields']['name']['field'] = 'name';
  $handler->display->display_options['fields']['name']['alter']['make_link'] = TRUE;
  $handler->display->display_options['fields']['name']['alter']['path'] = 'admin/tripal/storage/chado/mviews/report/[mview_id]';
  /* Field: Tripal Materialized Views: Table */
  $handler->display->display_options['fields']['mv_table']['id'] = 'mv_table';
  $handler->display->display_options['fields']['mv_table']['table'] = 'tripal_mviews';
  $handler->display->display_options['fields']['mv_table']['field'] = 'mv_table';
  $handler->display->display_options['fields']['mv_table']['label'] = 'Table Name';
  /* Field: Tripal Materialized Views: Description */
  $handler->display->display_options['fields']['comment']['id'] = 'comment';
  $handler->display->display_options['fields']['comment']['table'] = 'tripal_mviews';
  $handler->display->display_options['fields']['comment']['field'] = 'comment';
  /* Field: Tripal Materialized Views: Updated */
  $handler->display->display_options['fields']['last_update']['id'] = 'last_update';
  $handler->display->display_options['fields']['last_update']['table'] = 'tripal_mviews';
  $handler->display->display_options['fields']['last_update']['field'] = 'last_update';
  $handler->display->display_options['fields']['last_update']['label'] = 'Last Updated';
  $handler->display->display_options['fields']['last_update']['date_format'] = 'long';
  /* Field: Tripal Materialized Views: Status */
  $handler->display->display_options['fields']['status']['id'] = 'status';
  $handler->display->display_options['fields']['status']['table'] = 'tripal_mviews';
  $handler->display->display_options['fields']['status']['field'] = 'status';
  $handler->display->display_options['fields']['status']['empty'] = 'Not yet populated';
  /* Field: Global: Custom text */
  $handler->display->display_options['fields']['nothing']['id'] = 'nothing';
  $handler->display->display_options['fields']['nothing']['table'] = 'views';
  $handler->display->display_options['fields']['nothing']['field'] = 'nothing';
  $handler->display->display_options['fields']['nothing']['label'] = 'Edit Link';
  $handler->display->display_options['fields']['nothing']['exclude'] = TRUE;
  $handler->display->display_options['fields']['nothing']['alter']['text'] = 'Edit';
  $handler->display->display_options['fields']['nothing']['alter']['make_link'] = TRUE;
  $handler->display->display_options['fields']['nothing']['alter']['path'] = 'admin/tripal/storage/chado/mviews/edit/[mview_id]';
  /* Field: Global: Custom text */
  $handler->display->display_options['fields']['nothing_1']['id'] = 'nothing_1';
  $handler->display->display_options['fields']['nothing_1']['table'] = 'views';
  $handler->display->display_options['fields']['nothing_1']['field'] = 'nothing';
  $handler->display->display_options['fields']['nothing_1']['label'] = 'Delete Link';
  $handler->display->display_options['fields']['nothing_1']['exclude'] = TRUE;
  $handler->display->display_options['fields']['nothing_1']['alter']['text'] = 'Delete';
  $handler->display->display_options['fields']['nothing_1']['alter']['make_link'] = TRUE;
  $handler->display->display_options['fields']['nothing_1']['alter']['path'] = 'admin/tripal/storage/chado/mviews/delete/[mview_id]';
  /* Field: Global: Custom text */
  $handler->display->display_options['fields']['nothing_2']['id'] = 'nothing_2';
  $handler->display->display_options['fields']['nothing_2']['table'] = 'views';
  $handler->display->display_options['fields']['nothing_2']['field'] = 'nothing';
  $handler->display->display_options['fields']['nothing_2']['label'] = 'Populate Link';
  $handler->display->display_options['fields']['nothing_2']['exclude'] = TRUE;
  $handler->display->display_options['fields']['nothing_2']['alter']['text'] = 'Populate';
  $handler->display->display_options['fields']['nothing_2']['alter']['make_link'] = TRUE;
  $handler->display->display_options['fields']['nothing_2']['alter']['path'] = 'admin/tripal/storage/chado/mviews/update/[mview_id]';
  /* Field: Global: Custom text */
  $handler->display->display_options['fields']['nothing_3']['id'] = 'nothing_3';
  $handler->display->display_options['fields']['nothing_3']['table'] = 'views';
  $handler->display->display_options['fields']['nothing_3']['field'] = 'nothing';
  $handler->display->display_options['fields']['nothing_3']['label'] = '';
  $handler->display->display_options['fields']['nothing_3']['alter']['text'] = '[nothing]   [nothing_1]<br />
  [nothing_2]';
  $handler->display->display_options['fields']['nothing_3']['element_class'] = 'short-column';
  $handler->display->display_options['fields']['nothing_3']['element_label_class'] = 'short-column';
  $handler->display->display_options['fields']['nothing_3']['element_label_colon'] = FALSE;
  /* Filter criterion: Tripal Materialized Views: Name */
  $handler->display->display_options['filters']['name']['id'] = 'name';
  $handler->display->display_options['filters']['name']['table'] = 'tripal_mviews';
  $handler->display->display_options['filters']['name']['field'] = 'name';
  $handler->display->display_options['filters']['name']['group'] = 1;
  $handler->display->display_options['filters']['name']['exposed'] = TRUE;
  $handler->display->display_options['filters']['name']['expose']['operator_id'] = 'name_op';
  $handler->display->display_options['filters']['name']['expose']['label'] = 'Name';
  $handler->display->display_options['filters']['name']['expose']['operator'] = 'name_op';
  $handler->display->display_options['filters']['name']['expose']['identifier'] = 'name';
  $handler->display->display_options['filters']['name']['expose']['remember_roles'] = [
    2 => '2',
    1 => 0,
    3 => 0,
  ];
  /* Filter criterion: Tripal Materialized Views: Table */
  $handler->display->display_options['filters']['mv_table']['id'] = 'mv_table';
  $handler->display->display_options['filters']['mv_table']['table'] = 'tripal_mviews';
  $handler->display->display_options['filters']['mv_table']['field'] = 'mv_table';
  $handler->display->display_options['filters']['mv_table']['group'] = 1;
  $handler->display->display_options['filters']['mv_table']['exposed'] = TRUE;
  $handler->display->display_options['filters']['mv_table']['expose']['operator_id'] = 'mv_table_op';
  $handler->display->display_options['filters']['mv_table']['expose']['label'] = 'Table Name';
  $handler->display->display_options['filters']['mv_table']['expose']['operator'] = 'mv_table_op';
  $handler->display->display_options['filters']['mv_table']['expose']['identifier'] = 'mv_table';
  $handler->display->display_options['filters']['mv_table']['expose']['remember_roles'] = [
    2 => '2',
    1 => 0,
    3 => 0,
  ];
  /* Filter criterion: Tripal Materialized Views: Module Name */
  $handler->display->display_options['filters']['modulename']['id'] = 'modulename';
  $handler->display->display_options['filters']['modulename']['table'] = 'tripal_mviews';
  $handler->display->display_options['filters']['modulename']['field'] = 'modulename';
  $handler->display->display_options['filters']['modulename']['group'] = 1;
  $handler->display->display_options['filters']['modulename']['exposed'] = TRUE;
  $handler->display->display_options['filters']['modulename']['expose']['operator_id'] = 'modulename_op';
  $handler->display->display_options['filters']['modulename']['expose']['label'] = 'Module';
  $handler->display->display_options['filters']['modulename']['expose']['operator'] = 'modulename_op';
  $handler->display->display_options['filters']['modulename']['expose']['identifier'] = 'modulename';
  $handler->display->display_options['filters']['modulename']['expose']['remember_roles'] = [
    2 => '2',
    1 => 0,
    3 => 0,
  ];
  /* Filter criterion: Tripal Materialized Views: Status */
  $handler->display->display_options['filters']['status']['id'] = 'status';
  $handler->display->display_options['filters']['status']['table'] = 'tripal_mviews';
  $handler->display->display_options['filters']['status']['field'] = 'status';
  $handler->display->display_options['filters']['status']['group'] = 1;
  $handler->display->display_options['filters']['status']['exposed'] = TRUE;
  $handler->display->display_options['filters']['status']['expose']['operator_id'] = 'status_op';
  $handler->display->display_options['filters']['status']['expose']['label'] = 'Status';
  $handler->display->display_options['filters']['status']['expose']['operator'] = 'status_op';
  $handler->display->display_options['filters']['status']['expose']['identifier'] = 'status';
  $handler->display->display_options['filters']['status']['expose']['remember_roles'] = [
    2 => '2',
    1 => 0,
    3 => 0,
  ];

  /* Display: Page */
  $handler = $view->new_display('page', 'Page', 'page');
  $handler->display->display_options['path'] = 'admin/tripal/storage/chado/mviews/mviews';
  $handler->display->display_options['menu']['type'] = 'default tab';
  $handler->display->display_options['menu']['title'] = 'Materialized Views';
  $handler->display->display_options['menu']['description'] = 'a list of mviews';
  $handler->display->display_options['menu']['weight'] = '-10';
  $handler->display->display_options['menu']['name'] = 'management';
  $handler->display->display_options['menu']['context'] = 0;
  $handler->display->display_options['menu']['context_only_inline'] = 0;
  $handler->display->display_options['tab_options']['weight'] = '0';

  return $view;
}

/**
 * Default Admin View for cv management
 *
 * @ingroup tripal_cv
 */
function tripal_chado_defaultview_admin_cvs_listing() {

  $view = new view();
  $view->name = 'tripal_cv_admin_cvs';
  $view->description = 'DO NOT DISABLE';
  $view->tag = 'tripal admin';
  $view->base_table = 'cv';
  $view->human_name = 'CVs Admin';
  $view->core = 6;
  $view->api_version = '3.0';
  $view->disabled = FALSE; /* Edit this to true to make a default view disabled initially */

  /* Display: Defaults */
  $handler = $view->new_display('default', 'Defaults', 'default');
  $handler->display->display_options['title'] = 'Controlled Vocabularies';
  $handler->display->display_options['use_more_always'] = FALSE;
  $handler->display->display_options['access']['type'] = 'perm';
  $handler->display->display_options['access']['perm'] = 'administer tripal';
  $handler->display->display_options['cache']['type'] = 'none';
  $handler->display->display_options['query']['type'] = 'views_query';
  $handler->display->display_options['exposed_form']['type'] = 'input_required';
  $handler->display->display_options['exposed_form']['options']['submit_button'] = 'Search';
  $handler->display->display_options['exposed_form']['options']['text_input_required'] = 'Click search to see a listing of controlled vocabularies that meet the filter requirements. Use the filters to restrict this set to a more reasonable number of vocabularies or to find a specific vocabulary.';
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
    'definition' => 'definition',
    'nothing' => 'nothing',
  ];
  $handler->display->display_options['style_options']['default'] = 'name';
  $handler->display->display_options['style_options']['info'] = [
    'name' => [
      'sortable' => 1,
      'separator' => '',
    ],
    'definition' => [
      'sortable' => 0,
      'separator' => '',
    ],
    'nothing' => [
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
    'label-1' => 'Add Vocabulary',
    'path-1' => 'admin/tripal/loaders/chado_vocabs/chado_cv/add',
  ];
  /* No results behavior: Global: Text area */
  $handler->display->display_options['empty']['text']['id'] = 'area';
  $handler->display->display_options['empty']['text']['table'] = 'views';
  $handler->display->display_options['empty']['text']['field'] = 'area';
  $handler->display->display_options['empty']['text']['content'] = 'No controlled vocabularies match the supplied criteria.';
  $handler->display->display_options['empty']['text']['format'] = '2';
  /* Field: Chado Cv: Cv Id */
  $handler->display->display_options['fields']['cv_id']['id'] = 'cv_id';
  $handler->display->display_options['fields']['cv_id']['table'] = 'cv';
  $handler->display->display_options['fields']['cv_id']['field'] = 'cv_id';
  $handler->display->display_options['fields']['cv_id']['exclude'] = TRUE;
  $handler->display->display_options['fields']['cv_id']['separator'] = '';
  /* Field: Chado Cv: Name */
  $handler->display->display_options['fields']['name']['id'] = 'name';
  $handler->display->display_options['fields']['name']['table'] = 'cv';
  $handler->display->display_options['fields']['name']['field'] = 'name';
  /* Field: Chado Cv: Definition */
  $handler->display->display_options['fields']['definition']['id'] = 'definition';
  $handler->display->display_options['fields']['definition']['table'] = 'cv';
  $handler->display->display_options['fields']['definition']['field'] = 'definition';
  /* Field: Global: Custom text */
  $handler->display->display_options['fields']['nothing_1']['id'] = 'nothing_1';
  $handler->display->display_options['fields']['nothing_1']['table'] = 'views';
  $handler->display->display_options['fields']['nothing_1']['field'] = 'nothing';
  $handler->display->display_options['fields']['nothing_1']['label'] = 'Edit Link';
  $handler->display->display_options['fields']['nothing_1']['exclude'] = TRUE;
  $handler->display->display_options['fields']['nothing_1']['alter']['text'] = 'edit';
  $handler->display->display_options['fields']['nothing_1']['alter']['make_link'] = TRUE;
  $handler->display->display_options['fields']['nothing_1']['alter']['path'] = 'admin/tripal/loaders/chado_vocabs/chado_cv/edit/[cv_id]';
  /* Field: Global: Custom text */
  $handler->display->display_options['fields']['nothing']['id'] = 'nothing';
  $handler->display->display_options['fields']['nothing']['table'] = 'views';
  $handler->display->display_options['fields']['nothing']['field'] = 'nothing';
  $handler->display->display_options['fields']['nothing']['label'] = 'View Terms link';
  $handler->display->display_options['fields']['nothing']['exclude'] = TRUE;
  $handler->display->display_options['fields']['nothing']['alter']['text'] = 'View Terms';
  $handler->display->display_options['fields']['nothing']['alter']['make_link'] = TRUE;
  $handler->display->display_options['fields']['nothing']['alter']['path'] = 'admin/tripal/loaders/chado_vocabs/chado_cvterms?cv=[name]';
  $handler->display->display_options['fields']['nothing']['hide_alter_empty'] = TRUE;
  /* Field: Global: Custom text */
  $handler->display->display_options['fields']['nothing_3']['id'] = 'nothing_3';
  $handler->display->display_options['fields']['nothing_3']['table'] = 'views';
  $handler->display->display_options['fields']['nothing_3']['field'] = 'nothing';
  $handler->display->display_options['fields']['nothing_3']['label'] = 'Add term';
  $handler->display->display_options['fields']['nothing_3']['exclude'] = TRUE;
  $handler->display->display_options['fields']['nothing_3']['alter']['text'] = 'Add Term';
  $handler->display->display_options['fields']['nothing_3']['alter']['make_link'] = TRUE;
  $handler->display->display_options['fields']['nothing_3']['alter']['path'] = 'aadmin/tripal/loaders/chado_vocabs/chado_cv/[cv_id]/cvterm/add';
  /* Field: Global: Custom text */
  $handler->display->display_options['fields']['nothing_2']['id'] = 'nothing_2';
  $handler->display->display_options['fields']['nothing_2']['table'] = 'views';
  $handler->display->display_options['fields']['nothing_2']['field'] = 'nothing';
  $handler->display->display_options['fields']['nothing_2']['label'] = '';
  $handler->display->display_options['fields']['nothing_2']['alter']['text'] = '[nothing_1]<br />
    [nothing]   [nothing_3]';
  $handler->display->display_options['fields']['nothing_2']['element_class'] = 'short-column';
  $handler->display->display_options['fields']['nothing_2']['element_label_class'] = 'short-column';
  $handler->display->display_options['fields']['nothing_2']['element_label_colon'] = FALSE;
  /* Sort criterion: Chado Cv: Name */
  $handler->display->display_options['sorts']['name']['id'] = 'name';
  $handler->display->display_options['sorts']['name']['table'] = 'cv';
  $handler->display->display_options['sorts']['name']['field'] = 'name';
  /* Filter criterion: Chado Cv: Name */
  $handler->display->display_options['filters']['name']['id'] = 'name';
  $handler->display->display_options['filters']['name']['table'] = 'cv';
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
  /* Filter criterion: Chado Cv: Definition */
  $handler->display->display_options['filters']['definition']['id'] = 'definition';
  $handler->display->display_options['filters']['definition']['table'] = 'cv';
  $handler->display->display_options['filters']['definition']['field'] = 'definition';
  $handler->display->display_options['filters']['definition']['operator'] = 'contains';
  $handler->display->display_options['filters']['definition']['group'] = '0';
  $handler->display->display_options['filters']['definition']['exposed'] = TRUE;
  $handler->display->display_options['filters']['definition']['expose']['operator_id'] = 'definition_op';
  $handler->display->display_options['filters']['definition']['expose']['label'] = 'Definition Contains';
  $handler->display->display_options['filters']['definition']['expose']['operator'] = 'definition_op';
  $handler->display->display_options['filters']['definition']['expose']['identifier'] = 'definition';
  $handler->display->display_options['filters']['definition']['expose']['remember_roles'] = [
    2 => '2',
    1 => 0,
    3 => 0,
  ];

  /** MANUALLY ADD since filter handler not exporting correctly */
  $handler->display->display_options['filters']['name']['expose']['values_form_type'] = 'textfield';
  $handler->display->display_options['filters']['name']['expose']['select_multiple'] = FALSE;
  $handler->display->display_options['filters']['name']['expose']['select_optional'] = FALSE;
  $handler->display->display_options['filters']['name']['expose']['max_length'] = 40;

  /* Display: Page */
  $handler = $view->new_display('page', 'Page', 'page_1');
  $handler->display->display_options['path'] = 'admin/tripal/loaders/chado_vocabs/chado_cvs';
  $handler->display->display_options['menu']['type'] = 'default tab';
  $handler->display->display_options['menu']['title'] = 'Chado Controlled Vocabularies';
  $handler->display->display_options['menu']['description'] = 'A listing of all controlled vocabularies';
  $handler->display->display_options['menu']['weight'] = '-10';
  $handler->display->display_options['menu']['name'] = 'management';
  $handler->display->display_options['menu']['context'] = 0;
  $handler->display->display_options['menu']['context_only_inline'] = 0;
  $handler->display->display_options['tab_options']['weight'] = '0';


  /** MANUALLY ADD since filter handler no exporting correctly
   * $handler->display->display_options['filters']['name']['expose']['values_form_type'] = 'textfield';
   * $handler->display->display_options['filters']['name']['expose']['select_multiple'] = FALSE;
   * $handler->display->display_options['filters']['name']['expose']['select_optional'] = FALSE;
   * $handler->display->display_options['filters']['name']['expose']['max_length'] = 40;
   */

  return $view;
}

/**
 * Default Admin View for cvterm management
 *
 * @ingroup tripal_cv
 */
function tripal_chado_defaultview_admin_cvterms_listing() {

  $view = new view();
  $view->name = 'tripal_cv_admin_cvterms';
  $view->description = 'DO NOT DISABLE';
  $view->tag = 'tripal admin';
  $view->base_table = 'cvterm';
  $view->human_name = 'CV Terms Admin';
  $view->core = 0;
  $view->api_version = '3.0';
  $view->disabled = FALSE; /* Edit this to true to make a default view disabled initially */

  /* Display: Defaults */
  $handler = $view->new_display('default', 'Defaults', 'default');
  $handler->display->display_options['title'] = 'Controlled Vocabulary Terms';
  $handler->display->display_options['use_more_always'] = FALSE;
  $handler->display->display_options['access']['type'] = 'perm';
  $handler->display->display_options['access']['perm'] = 'administer tripal';
  $handler->display->display_options['cache']['type'] = 'none';
  $handler->display->display_options['query']['type'] = 'views_query';
  $handler->display->display_options['exposed_form']['type'] = 'input_required';
  $handler->display->display_options['exposed_form']['options']['submit_button'] = 'Search';
  $handler->display->display_options['exposed_form']['options']['text_input_required'] = 'Click search to see a listing of controlled vocabulary terms that meet the filter requirements. Use the filters to restrict this set to a more reasonable number of terms or to find a specific term.';
  $handler->display->display_options['exposed_form']['options']['text_input_required_format'] = 'full_html';
  $handler->display->display_options['pager']['type'] = 'full';
  $handler->display->display_options['pager']['options']['items_per_page'] = '25';
  $handler->display->display_options['pager']['options']['offset'] = '0';
  $handler->display->display_options['pager']['options']['id'] = '0';
  $handler->display->display_options['pager']['options']['quantity'] = '9';
  $handler->display->display_options['style_plugin'] = 'table';
  $handler->display->display_options['style_options']['grouping'] = '';
  $handler->display->display_options['style_options']['columns'] = [
    'name_1' => 'name_1',
    'name' => 'name',
    'definition' => 'definition',
    'is_obsolete' => 'is_obsolete',
    'is_relationshiptype' => 'is_relationshiptype',
  ];
  $handler->display->display_options['style_options']['default'] = '-1';
  $handler->display->display_options['style_options']['info'] = [
    'name_1' => [
      'sortable' => 1,
      'separator' => '',
    ],
    'name' => [
      'sortable' => 1,
      'separator' => '',
    ],
    'definition' => [
      'sortable' => 0,
      'separator' => '',
    ],
    'is_obsolete' => [
      'sortable' => 1,
      'separator' => '',
    ],
    'is_relationshiptype' => [
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
    'label-1' => 'Add Term',
    'path-1' => 'admin/tripal/loaders/chado_vocabs/chado_cv/cvterm/add',
  ];
  /* No results behavior: Global: Text area */
  $handler->display->display_options['empty']['text']['id'] = 'area';
  $handler->display->display_options['empty']['text']['table'] = 'views';
  $handler->display->display_options['empty']['text']['field'] = 'area';
  $handler->display->display_options['empty']['text']['content'] = 'There are no terms associated with the selected controlled vocabulary. Please select a different vocabulary from the list above.';
  $handler->display->display_options['empty']['text']['format'] = '1';
  /* Field: Chado Cv: Cv Id */
  $handler->display->display_options['fields']['cv_id']['id'] = 'cv_id';
  $handler->display->display_options['fields']['cv_id']['table'] = 'cv';
  $handler->display->display_options['fields']['cv_id']['field'] = 'cv_id';
  $handler->display->display_options['fields']['cv_id']['exclude'] = TRUE;
  $handler->display->display_options['fields']['cv_id']['separator'] = '';
  /* Field: Chado Cv: Name */
  $handler->display->display_options['fields']['name_1']['id'] = 'name_1';
  $handler->display->display_options['fields']['name_1']['table'] = 'cv';
  $handler->display->display_options['fields']['name_1']['field'] = 'name';
  $handler->display->display_options['fields']['name_1']['label'] = 'Vocabulary';
  $handler->display->display_options['fields']['name_1']['alter']['make_link'] = TRUE;
  $handler->display->display_options['fields']['name_1']['alter']['path'] = 'admin/tripal/loaders/chado_vocabs/chado_cvs?name=[name_1]';
  /* Field: Chado Cvterm: Cvterm Id */
  $handler->display->display_options['fields']['cvterm_id']['id'] = 'cvterm_id';
  $handler->display->display_options['fields']['cvterm_id']['table'] = 'cvterm';
  $handler->display->display_options['fields']['cvterm_id']['field'] = 'cvterm_id';
  $handler->display->display_options['fields']['cvterm_id']['exclude'] = TRUE;
  $handler->display->display_options['fields']['cvterm_id']['separator'] = '';
  /* Field: Chado Cvterm: Name */
  $handler->display->display_options['fields']['name']['id'] = 'name';
  $handler->display->display_options['fields']['name']['table'] = 'cvterm';
  $handler->display->display_options['fields']['name']['field'] = 'name';
  /* Field: Chado Cvterm: Definition */
  $handler->display->display_options['fields']['definition']['id'] = 'definition';
  $handler->display->display_options['fields']['definition']['table'] = 'cvterm';
  $handler->display->display_options['fields']['definition']['field'] = 'definition';
  $handler->display->display_options['fields']['definition']['element_class'] = 'wide-column';
  $handler->display->display_options['fields']['definition']['element_label_class'] = 'wide-column';
  /* Field: Chado Cvterm: Is Obsolete */
  $handler->display->display_options['fields']['is_obsolete']['id'] = 'is_obsolete';
  $handler->display->display_options['fields']['is_obsolete']['table'] = 'cvterm';
  $handler->display->display_options['fields']['is_obsolete']['field'] = 'is_obsolete';
  $handler->display->display_options['fields']['is_obsolete']['label'] = 'Obsolete?';
  $handler->display->display_options['fields']['is_obsolete']['alter']['alter_text'] = TRUE;
  $handler->display->display_options['fields']['is_obsolete']['alter']['text'] = 'Yes';
  $handler->display->display_options['fields']['is_obsolete']['element_class'] = 'short-column';
  $handler->display->display_options['fields']['is_obsolete']['element_label_class'] = 'short-column';
  $handler->display->display_options['fields']['is_obsolete']['empty'] = 'No';
  $handler->display->display_options['fields']['is_obsolete']['empty_zero'] = TRUE;
  $handler->display->display_options['fields']['is_obsolete']['separator'] = '';
  /* Field: Chado Cvterm: Is Relationshiptype */
  $handler->display->display_options['fields']['is_relationshiptype']['id'] = 'is_relationshiptype';
  $handler->display->display_options['fields']['is_relationshiptype']['table'] = 'cvterm';
  $handler->display->display_options['fields']['is_relationshiptype']['field'] = 'is_relationshiptype';
  $handler->display->display_options['fields']['is_relationshiptype']['label'] = 'Relation-ship?';
  $handler->display->display_options['fields']['is_relationshiptype']['alter']['alter_text'] = TRUE;
  $handler->display->display_options['fields']['is_relationshiptype']['alter']['text'] = 'Yes';
  $handler->display->display_options['fields']['is_relationshiptype']['element_class'] = 'short-column';
  $handler->display->display_options['fields']['is_relationshiptype']['element_label_class'] = 'short-column';
  $handler->display->display_options['fields']['is_relationshiptype']['empty'] = 'No';
  $handler->display->display_options['fields']['is_relationshiptype']['empty_zero'] = TRUE;
  $handler->display->display_options['fields']['is_relationshiptype']['separator'] = '';
  /* Field: Global: Custom text */
  $handler->display->display_options['fields']['nothing']['id'] = 'nothing';
  $handler->display->display_options['fields']['nothing']['table'] = 'views';
  $handler->display->display_options['fields']['nothing']['field'] = 'nothing';
  $handler->display->display_options['fields']['nothing']['label'] = 'Edit Link';
  $handler->display->display_options['fields']['nothing']['exclude'] = TRUE;
  $handler->display->display_options['fields']['nothing']['alter']['text'] = 'edit';
  $handler->display->display_options['fields']['nothing']['alter']['make_link'] = TRUE;
  $handler->display->display_options['fields']['nothing']['alter']['path'] = 'admin/tripal/loaders/chado_vocabs/chado_cv/[cv_id]/cvterm/edit/[cvterm_id]';
  /* Field: Global: Custom text */
  $handler->display->display_options['fields']['nothing_1']['id'] = 'nothing_1';
  $handler->display->display_options['fields']['nothing_1']['table'] = 'views';
  $handler->display->display_options['fields']['nothing_1']['field'] = 'nothing';
  $handler->display->display_options['fields']['nothing_1']['label'] = '';
  $handler->display->display_options['fields']['nothing_1']['alter']['text'] = '[nothing]';
  $handler->display->display_options['fields']['nothing_1']['element_class'] = 'short-column';
  $handler->display->display_options['fields']['nothing_1']['element_label_class'] = 'short-column';
  $handler->display->display_options['fields']['nothing_1']['element_label_colon'] = FALSE;
  /* Sort criterion: Chado Cv: Name */
  $handler->display->display_options['sorts']['name']['id'] = 'name';
  $handler->display->display_options['sorts']['name']['table'] = 'cv';
  $handler->display->display_options['sorts']['name']['field'] = 'name';
  /* Sort criterion: Chado Cvterm: Name */
  $handler->display->display_options['sorts']['name_1']['id'] = 'name_1';
  $handler->display->display_options['sorts']['name_1']['table'] = 'cvterm';
  $handler->display->display_options['sorts']['name_1']['field'] = 'name';
  /* Filter criterion: Chado Cv: Name */
  $handler->display->display_options['filters']['name']['id'] = 'name';
  $handler->display->display_options['filters']['name']['table'] = 'cv';
  $handler->display->display_options['filters']['name']['field'] = 'name';
  $handler->display->display_options['filters']['name']['value'] = 'All';
  $handler->display->display_options['filters']['name']['group'] = '0';
  $handler->display->display_options['filters']['name']['exposed'] = TRUE;
  $handler->display->display_options['filters']['name']['expose']['operator_id'] = 'name_op';
  $handler->display->display_options['filters']['name']['expose']['label'] = 'Vocabulary';
  $handler->display->display_options['filters']['name']['expose']['operator'] = 'name_op';
  $handler->display->display_options['filters']['name']['expose']['identifier'] = 'cv';
  $handler->display->display_options['filters']['name']['expose']['remember_roles'] = [
    2 => '2',
    1 => 0,
    3 => 0,
  ];
  $handler->display->display_options['filters']['name']['values_form_type'] = 'select';
  $handler->display->display_options['filters']['name']['select_optional'] = TRUE;
  /* Filter criterion: Chado Cvterm: Name */
  $handler->display->display_options['filters']['name_1']['id'] = 'name_1';
  $handler->display->display_options['filters']['name_1']['table'] = 'cvterm';
  $handler->display->display_options['filters']['name_1']['field'] = 'name';
  $handler->display->display_options['filters']['name_1']['operator'] = 'contains';
  $handler->display->display_options['filters']['name_1']['group'] = '0';
  $handler->display->display_options['filters']['name_1']['exposed'] = TRUE;
  $handler->display->display_options['filters']['name_1']['expose']['operator_id'] = '';
  $handler->display->display_options['filters']['name_1']['expose']['label'] = 'Name Contains';
  $handler->display->display_options['filters']['name_1']['expose']['identifier'] = 'name';
  $handler->display->display_options['filters']['name_1']['expose']['remember_roles'] = [
    2 => '2',
    1 => 0,
    3 => 0,
  ];
  /* Filter criterion: Chado Cvterm: Definition */
  $handler->display->display_options['filters']['definition']['id'] = 'definition';
  $handler->display->display_options['filters']['definition']['table'] = 'cvterm';
  $handler->display->display_options['filters']['definition']['field'] = 'definition';
  $handler->display->display_options['filters']['definition']['operator'] = 'contains';
  $handler->display->display_options['filters']['definition']['group'] = '0';
  $handler->display->display_options['filters']['definition']['exposed'] = TRUE;
  $handler->display->display_options['filters']['definition']['expose']['operator_id'] = 'definition_op';
  $handler->display->display_options['filters']['definition']['expose']['label'] = 'Definition Contains';
  $handler->display->display_options['filters']['definition']['expose']['operator'] = 'definition_op';
  $handler->display->display_options['filters']['definition']['expose']['identifier'] = 'definition';
  $handler->display->display_options['filters']['definition']['expose']['remember_roles'] = [
    2 => '2',
    1 => 0,
    3 => 0,
  ];

  /* Display: Page */
  $handler = $view->new_display('page', 'Page', 'page_1');
  $handler->display->display_options['path'] = 'admin/tripal/loaders/chado_vocabs/chado_cvterms';
  $handler->display->display_options['menu']['type'] = 'normal';
  $handler->display->display_options['menu']['title'] = 'Manage Chado CV Terms';
  $handler->display->display_options['menu']['description'] = 'View, edit and add controlled vocabulary terms.';
  $handler->display->display_options['menu']['weight'] = '-8';
  $handler->display->display_options['menu']['name'] = 'management';
  $handler->display->display_options['menu']['context'] = 0;
  $handler->display->display_options['menu']['context_only_inline'] = 0;


  return $view;
}

/**
 * Default Admin View for db management
 *
 * @ingroup tripal_db
 */
function tripal_chado_defaultview_admin_db_listing() {

  $view = new view();
  $view->name = 'tripal_db_admin_dbs';
  $view->description = 'DO NOT DISABLE';
  $view->tag = 'tripal admin';
  $view->base_table = 'db';
  $view->human_name = 'DB Admin';
  $view->core = 6;
  $view->api_version = '3.0';
  $view->disabled = FALSE; /* Edit this to true to make a default view disabled initially */

  /* Display: Defaults */
  $handler = $view->new_display('default', 'Defaults', 'default');
  $handler->display->display_options['title'] = 'Databases';
  $handler->display->display_options['use_more_always'] = FALSE;
  $handler->display->display_options['access']['type'] = 'perm';
  $handler->display->display_options['access']['perm'] = 'administer db cross-references';
  $handler->display->display_options['cache']['type'] = 'none';
  $handler->display->display_options['query']['type'] = 'views_query';
  $handler->display->display_options['exposed_form']['type'] = 'input_required';
  $handler->display->display_options['exposed_form']['options']['submit_button'] = 'Search';
  $handler->display->display_options['exposed_form']['options']['text_input_required'] = 'Click search to see a listing of external databases that meet the filter requirements. Use the filters to restrict this set to a more reasonable number of databases or to find a specific database.';
  $handler->display->display_options['exposed_form']['options']['text_input_required_format'] = 'full_html';
  $handler->display->display_options['pager']['type'] = 'full';
  $handler->display->display_options['pager']['options']['items_per_page'] = '50';
  $handler->display->display_options['pager']['options']['offset'] = '0';
  $handler->display->display_options['pager']['options']['id'] = '0';
  $handler->display->display_options['pager']['options']['quantity'] = '9';
  $handler->display->display_options['style_plugin'] = 'table';
  $handler->display->display_options['style_options']['grouping'] = '';
  $handler->display->display_options['style_options']['columns'] = [
    'name' => 'name',
    'url' => 'url',
    'urlprefix' => 'urlprefix',
    'description' => 'description',
  ];
  $handler->display->display_options['style_options']['default'] = 'name';
  $handler->display->display_options['style_options']['info'] = [
    'name' => [
      'sortable' => 1,
      'separator' => '',
    ],
    'description' => [
      'sortable' => 0,
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
    'label-1' => 'Add Database',
    'path-1' => 'admin/tripal/loaders/chado_db/add',
  ];
  /* No results behavior: Global: Text area */
  $handler->display->display_options['empty']['text']['id'] = 'area';
  $handler->display->display_options['empty']['text']['table'] = 'views';
  $handler->display->display_options['empty']['text']['field'] = 'area';
  $handler->display->display_options['empty']['text']['content'] = 'No databases match the supplied criteria.';
  $handler->display->display_options['empty']['text']['format'] = '2';
  /* Field: Chado Db: Db Id */
  $handler->display->display_options['fields']['db_id']['id'] = 'db_id';
  $handler->display->display_options['fields']['db_id']['table'] = 'db';
  $handler->display->display_options['fields']['db_id']['field'] = 'db_id';
  $handler->display->display_options['fields']['db_id']['exclude'] = TRUE;
  $handler->display->display_options['fields']['db_id']['separator'] = '';
  /* Field: Chado Db: Name */
  $handler->display->display_options['fields']['name']['id'] = 'name';
  $handler->display->display_options['fields']['name']['table'] = 'db';
  $handler->display->display_options['fields']['name']['field'] = 'name';
  /* Field: Chado Db: Description */
  $handler->display->display_options['fields']['description']['id'] = 'description';
  $handler->display->display_options['fields']['description']['table'] = 'db';
  $handler->display->display_options['fields']['description']['field'] = 'description';
  /* Field: Global: Custom text */
  $handler->display->display_options['fields']['nothing']['id'] = 'nothing';
  $handler->display->display_options['fields']['nothing']['table'] = 'views';
  $handler->display->display_options['fields']['nothing']['field'] = 'nothing';
  $handler->display->display_options['fields']['nothing']['label'] = 'Edit Link';
  $handler->display->display_options['fields']['nothing']['exclude'] = TRUE;
  $handler->display->display_options['fields']['nothing']['alter']['text'] = 'edit';
  $handler->display->display_options['fields']['nothing']['alter']['make_link'] = TRUE;
  $handler->display->display_options['fields']['nothing']['alter']['path'] = 'admin/tripal/loaders/chado_db/edit/[db_id]';
  /* Field: Global: Custom text */
  $handler->display->display_options['fields']['nothing_1']['id'] = 'nothing_1';
  $handler->display->display_options['fields']['nothing_1']['table'] = 'views';
  $handler->display->display_options['fields']['nothing_1']['field'] = 'nothing';
  $handler->display->display_options['fields']['nothing_1']['label'] = 'View DBxrefs';
  $handler->display->display_options['fields']['nothing_1']['exclude'] = TRUE;
  $handler->display->display_options['fields']['nothing_1']['alter']['text'] = 'items';
  $handler->display->display_options['fields']['nothing_1']['alter']['make_link'] = TRUE;
  $handler->display->display_options['fields']['nothing_1']['alter']['path'] = 'admin/tripal/loaders/chado_db/dbxrefs?db_name=[name]';
  /* Field: Global: Custom text */
  $handler->display->display_options['fields']['nothing_2']['id'] = 'nothing_2';
  $handler->display->display_options['fields']['nothing_2']['table'] = 'views';
  $handler->display->display_options['fields']['nothing_2']['field'] = 'nothing';
  $handler->display->display_options['fields']['nothing_2']['label'] = 'Actions';
  $handler->display->display_options['fields']['nothing_2']['alter']['text'] = '[nothing] | [nothing_1]';
  $handler->display->display_options['fields']['nothing_2']['element_label_colon'] = FALSE;
  /* Filter criterion: Chado Db: Name */
  $handler->display->display_options['filters']['name']['id'] = 'name';
  $handler->display->display_options['filters']['name']['table'] = 'db';
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
  /* Filter criterion: Chado Db: Description */
  $handler->display->display_options['filters']['description']['id'] = 'description';
  $handler->display->display_options['filters']['description']['table'] = 'db';
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
  $handler->display->display_options['path'] = 'admin/tripal/loaders/chado_db/dbs';
  $handler->display->display_options['menu']['type'] = 'default tab';
  $handler->display->display_options['menu']['title'] = 'Databases';
  $handler->display->display_options['menu']['description'] = 'Lists all databases available to create database references for.';
  $handler->display->display_options['menu']['weight'] = '-8';
  $handler->display->display_options['menu']['name'] = 'management';
  $handler->display->display_options['menu']['context'] = 0;
  $handler->display->display_options['menu']['context_only_inline'] = 0;
  $handler->display->display_options['tab_options']['weight'] = '0';

  return $view;
}

/**
 * Default Admin View for dbxref management
 *
 * @ingroup tripal_db
 */
function tripal_chado_defaultview_admin_dbxref_listing() {

  $view = new view();
  $view->name = 'tripal_db_admin_dbxrefs';
  $view->description = 'DO NOT DISABLE';
  $view->tag = 'tripal admin';
  $view->base_table = 'dbxref';
  $view->human_name = 'DB Reference Admin';
  $view->core = 7;
  $view->api_version = '3.0';
  $view->disabled = FALSE; /* Edit this to true to make a default view disabled initially */

  /* Display: Master */
  $handler = $view->new_display('default', 'Master', 'default');
  $handler->display->display_options['title'] = 'DB Reference Admin';
  $handler->display->display_options['use_more_always'] = FALSE;
  $handler->display->display_options['access']['type'] = 'perm';
  $handler->display->display_options['access']['perm'] = 'administer db cross-references';
  $handler->display->display_options['cache']['type'] = 'none';
  $handler->display->display_options['query']['type'] = 'views_query';
  $handler->display->display_options['exposed_form']['type'] = 'input_required';
  $handler->display->display_options['exposed_form']['options']['submit_button'] = 'Search';
  $handler->display->display_options['exposed_form']['options']['text_input_required'] = 'Click search to see a listing of external database references that meet the filter requirements. Use the filters to restrict this set to a more reasonable number of references or to find a specific reference.';
  $handler->display->display_options['exposed_form']['options']['text_input_required_format'] = 'full_html';
  $handler->display->display_options['pager']['type'] = 'full';
  $handler->display->display_options['pager']['options']['items_per_page'] = '50';
  $handler->display->display_options['style_plugin'] = 'table';
  $handler->display->display_options['style_options']['columns'] = [
    'dbxref_id' => 'dbxref_id',
    'name' => 'name',
    'accession' => 'accession',
    'description' => 'description',
    'version' => 'version',
  ];
  $handler->display->display_options['style_options']['default'] = '-1';
  $handler->display->display_options['style_options']['info'] = [
    'dbxref_id' => [
      'sortable' => 0,
      'default_sort_order' => 'asc',
      'align' => '',
      'separator' => '',
      'empty_column' => 0,
    ],
    'name' => [
      'sortable' => 1,
      'default_sort_order' => 'asc',
      'align' => '',
      'separator' => '',
      'empty_column' => 0,
    ],
    'accession' => [
      'sortable' => 1,
      'default_sort_order' => 'asc',
      'align' => '',
      'separator' => '',
      'empty_column' => 0,
    ],
    'description' => [
      'sortable' => 0,
      'default_sort_order' => 'asc',
      'align' => '',
      'separator' => '',
      'empty_column' => 0,
    ],
    'version' => [
      'sortable' => 1,
      'default_sort_order' => 'asc',
      'align' => '',
      'separator' => '',
      'empty_column' => 0,
    ],
  ];
  /* Field: Chado Dbxref: Dbxref Id */
  $handler->display->display_options['fields']['dbxref_id']['id'] = 'dbxref_id';
  $handler->display->display_options['fields']['dbxref_id']['table'] = 'dbxref';
  $handler->display->display_options['fields']['dbxref_id']['field'] = 'dbxref_id';
  $handler->display->display_options['fields']['dbxref_id']['exclude'] = TRUE;
  $handler->display->display_options['fields']['dbxref_id']['separator'] = '';
  /* Field: Chado Db: Name */
  $handler->display->display_options['fields']['name']['id'] = 'name';
  $handler->display->display_options['fields']['name']['table'] = 'db';
  $handler->display->display_options['fields']['name']['field'] = 'name';
  $handler->display->display_options['fields']['name']['label'] = 'Database';
  /* Field: Chado Dbxref: Accession */
  $handler->display->display_options['fields']['accession']['id'] = 'accession';
  $handler->display->display_options['fields']['accession']['table'] = 'dbxref';
  $handler->display->display_options['fields']['accession']['field'] = 'accession';
  /* Field: Chado Dbxref: Description */
  $handler->display->display_options['fields']['description']['id'] = 'description';
  $handler->display->display_options['fields']['description']['table'] = 'dbxref';
  $handler->display->display_options['fields']['description']['field'] = 'description';
  /* Field: Chado Dbxref: Version */
  $handler->display->display_options['fields']['version']['id'] = 'version';
  $handler->display->display_options['fields']['version']['table'] = 'dbxref';
  $handler->display->display_options['fields']['version']['field'] = 'version';
  /* Sort criterion: Chado Db: Name */
  $handler->display->display_options['sorts']['name']['id'] = 'name';
  $handler->display->display_options['sorts']['name']['table'] = 'db';
  $handler->display->display_options['sorts']['name']['field'] = 'name';
  /* Sort criterion: Chado Dbxref: Accession */
  $handler->display->display_options['sorts']['accession']['id'] = 'accession';
  $handler->display->display_options['sorts']['accession']['table'] = 'dbxref';
  $handler->display->display_options['sorts']['accession']['field'] = 'accession';
  /* Sort criterion: Chado Dbxref: Version */
  $handler->display->display_options['sorts']['version']['id'] = 'version';
  $handler->display->display_options['sorts']['version']['table'] = 'dbxref';
  $handler->display->display_options['sorts']['version']['field'] = 'version';
  $handler->display->display_options['sorts']['version']['order'] = 'DESC';
  /* Filter criterion: Chado Db: Name */
  $handler->display->display_options['filters']['name']['id'] = 'name';
  $handler->display->display_options['filters']['name']['table'] = 'db';
  $handler->display->display_options['filters']['name']['field'] = 'name';
  $handler->display->display_options['filters']['name']['value'] = [
    'All' => 'All',
  ];
  $handler->display->display_options['filters']['name']['group'] = 1;
  $handler->display->display_options['filters']['name']['exposed'] = TRUE;
  $handler->display->display_options['filters']['name']['expose']['operator_id'] = '';
  $handler->display->display_options['filters']['name']['expose']['label'] = 'Database';
  $handler->display->display_options['filters']['name']['expose']['identifier'] = 'db_name';
  $handler->display->display_options['filters']['name']['expose']['values_form_type'] = 'select';
  $handler->display->display_options['filters']['name']['expose']['select_multiple'] = TRUE;
  $handler->display->display_options['filters']['name']['expose']['select_optional'] = TRUE;
  $handler->display->display_options['filters']['name']['expose']['max_length'] = 40;
  $handler->display->display_options['filters']['name']['expose']['remember_roles'] = [
    2 => '2',
    1 => 0,
    3 => 0,
  ];
  /* Filter criterion: Chado Dbxref: Accession */
  $handler->display->display_options['filters']['accession']['id'] = 'accession';
  $handler->display->display_options['filters']['accession']['table'] = 'dbxref';
  $handler->display->display_options['filters']['accession']['field'] = 'accession';
  $handler->display->display_options['filters']['accession']['operator'] = 'contains';
  $handler->display->display_options['filters']['accession']['group'] = 1;
  $handler->display->display_options['filters']['accession']['exposed'] = TRUE;
  $handler->display->display_options['filters']['accession']['expose']['operator_id'] = 'accession_op';
  $handler->display->display_options['filters']['accession']['expose']['label'] = 'Accession Contains';
  $handler->display->display_options['filters']['accession']['expose']['operator'] = 'accession_op';
  $handler->display->display_options['filters']['accession']['expose']['identifier'] = 'accession';
  $handler->display->display_options['filters']['accession']['expose']['remember_roles'] = [
    2 => '2',
    1 => 0,
    3 => 0,
  ];
  /* Filter criterion: Chado Dbxref: Description */
  $handler->display->display_options['filters']['description']['id'] = 'description';
  $handler->display->display_options['filters']['description']['table'] = 'dbxref';
  $handler->display->display_options['filters']['description']['field'] = 'description';
  $handler->display->display_options['filters']['description']['operator'] = 'contains';
  $handler->display->display_options['filters']['description']['group'] = 1;
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
  $handler = $view->new_display('page', 'Page', 'page');
  $handler->display->display_options['path'] = 'admin/tripal/loaders/chado_db/dbxrefs';
  $handler->display->display_options['menu']['type'] = 'tab';
  $handler->display->display_options['menu']['title'] = 'References';
  $handler->display->display_options['menu']['weight'] = '0';
  $handler->display->display_options['menu']['name'] = 'management';
  $handler->display->display_options['menu']['context'] = 0;
  $handler->display->display_options['menu']['context_only_inline'] = 0;

  return $view;
}