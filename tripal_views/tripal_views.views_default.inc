<?php
/**
 * Describes default views for tripal views integrations
 */

/**
 * Implements hook_views_default_views().
 *
 * @ingroup tripal_views
 */
function tripal_views_views_default_views() {
  $views = array();

  $view = tripal_views_defaultview_admin_integrations();
  $views[$view->name] = $view;

  return $views;
}

/**
 * Default integrations administration view
 *
 * @ingroup tripal_views
 */
function tripal_views_defaultview_admin_integrations() {

  $view = new view();
  $view->name = 'tripal_views_admin_integrations';
  $view->description = 'DO NOT DISABLE';
  $view->tag = 'tripal admin';
  $view->base_table = 'tripal_views';
  $view->human_name = 'Tripal Views Admin';
  $view->core = 7;
  $view->api_version = '3.0';
  $view->disabled = FALSE; /* Edit this to true to make a default view disabled initially */

  /* Display: Master */
  $handler = $view->new_display('default', 'Master', 'default');
  $handler->display->display_options['title'] = 'Tripal Views Integrations';
  $handler->display->display_options['use_more_always'] = FALSE;
  $handler->display->display_options['access']['type'] = 'perm';
  $handler->display->display_options['access']['perm'] = 'manage tripal_views_integration';
  $handler->display->display_options['cache']['type'] = 'none';
  $handler->display->display_options['query']['type'] = 'views_query';
  $handler->display->display_options['exposed_form']['type'] = 'basic';
  $handler->display->display_options['exposed_form']['options']['submit_button'] = 'Filter';
  $handler->display->display_options['pager']['type'] = 'full';
  $handler->display->display_options['pager']['options']['items_per_page'] = '25';
  $handler->display->display_options['style_plugin'] = 'table';
  $handler->display->display_options['style_options']['columns'] = array(
    'name' => 'name',
    'table_name' => 'table_name',
    'mv_table' => 'mv_table',
    'comment' => 'comment',
    'base_table' => 'base_table',
    'priority' => 'priority',
  );
  $handler->display->display_options['style_options']['default'] = '-1';
  $handler->display->display_options['style_options']['info'] = array(
    'name' => array(
      'sortable' => 1,
      'default_sort_order' => 'asc',
      'align' => '',
      'separator' => '',
      'empty_column' => 0,
    ),
    'table_name' => array(
      'sortable' => 1,
      'default_sort_order' => 'asc',
      'align' => '',
      'separator' => '',
      'empty_column' => 0,
    ),
    'mv_table' => array(
      'sortable' => 1,
      'default_sort_order' => 'asc',
      'align' => '',
      'separator' => '',
      'empty_column' => 0,
    ),
    'comment' => array(
      'sortable' => 0,
      'default_sort_order' => 'asc',
      'align' => '',
      'separator' => '',
      'empty_column' => 0,
    ),
    'base_table' => array(
      'sortable' => 1,
      'default_sort_order' => 'asc',
      'align' => '',
      'separator' => '',
      'empty_column' => 0,
    ),
    'priority' => array(
      'sortable' => 1,
      'default_sort_order' => 'asc',
      'align' => '',
      'separator' => '',
      'empty_column' => 0,
    ),
  );
  /* Header: Global: Text area */
  $handler->display->display_options['header']['area']['id'] = 'area';
  $handler->display->display_options['header']['area']['table'] = 'views';
  $handler->display->display_options['header']['area']['field'] = 'area';
  $handler->display->display_options['header']['area']['label'] = 'Description';
  $handler->display->display_options['header']['area']['empty'] = TRUE;
  $handler->display->display_options['header']['area']['content'] = '<p>The following tables are available for integration with Drupal Views. If a table is integrated more than once, then the setup with the lightest priority will be used. For example, if you have created a custom setup with a priority of -5 then that will be used instead of the default setup with priority 10. Priorities range from -10 to +10 where a setup with -10 has greater precedent than any other and +10 has the least.</p>';
  $handler->display->display_options['header']['area']['format'] = 'filtered_html';
  /* Header: Global: Action Links */
  $handler->display->display_options['header']['action_links_area']['id'] = 'action_links_area';
  $handler->display->display_options['header']['action_links_area']['table'] = 'views';
  $handler->display->display_options['header']['action_links_area']['field'] = 'action_links_area';
  $handler->display->display_options['header']['action_links_area']['label'] = 'Add Links';
  $handler->display->display_options['header']['action_links_area']['empty'] = TRUE;
  $handler->display->display_options['header']['action_links_area']['link-1'] = array(
    'label-1' => 'Add New Integration',
    'path-1' => 'admin/tripal/views-integration/new',
  );
  $handler->display->display_options['header']['action_links_area']['link-2'] = array(
    'label-2' => 'Import Integration',
    'path-2' => 'admin/tripal/views-integration/import',
  );
  $handler->display->display_options['header']['action_links_area']['link-3'] = array(
    'label-3' => '',
    'path-3' => '',
  );
  $handler->display->display_options['header']['action_links_area']['link-4'] = array(
    'label-4' => '',
    'path-4' => '',
  );
  /* Header: Global: Action Links */
  $handler->display->display_options['header']['action_links_area_1']['id'] = 'action_links_area_1';
  $handler->display->display_options['header']['action_links_area_1']['table'] = 'views';
  $handler->display->display_options['header']['action_links_area_1']['field'] = 'action_links_area';
  $handler->display->display_options['header']['action_links_area_1']['label'] = 'Delete Links';
  $handler->display->display_options['header']['action_links_area_1']['empty'] = TRUE;
  $handler->display->display_options['header']['action_links_area_1']['link-1'] = array(
    'label-1' => 'Delete ALL Integrations',
    'path-1' => 'admin/tripal/views-integration/delete-all/confirm',
  );
  $handler->display->display_options['header']['action_links_area_1']['link-2'] = array(
    'label-2' => '',
    'path-2' => '',
  );
  $handler->display->display_options['header']['action_links_area_1']['link-3'] = array(
    'label-3' => '',
    'path-3' => '',
  );
  $handler->display->display_options['header']['action_links_area_1']['link-4'] = array(
    'label-4' => '',
    'path-4' => '',
  );
  /* Field: Tripal Views Integration: Setup ID */
  $handler->display->display_options['fields']['setup_id']['id'] = 'setup_id';
  $handler->display->display_options['fields']['setup_id']['table'] = 'tripal_views';
  $handler->display->display_options['fields']['setup_id']['field'] = 'setup_id';
  $handler->display->display_options['fields']['setup_id']['exclude'] = TRUE;
  $handler->display->display_options['fields']['setup_id']['separator'] = '';
  /* Field: Tripal Views Integration: Name */
  $handler->display->display_options['fields']['name']['id'] = 'name';
  $handler->display->display_options['fields']['name']['table'] = 'tripal_views';
  $handler->display->display_options['fields']['name']['field'] = 'name';
  /* Field: Tripal Views Integration: Chado Table Name */
  $handler->display->display_options['fields']['table_name']['id'] = 'table_name';
  $handler->display->display_options['fields']['table_name']['table'] = 'tripal_views';
  $handler->display->display_options['fields']['table_name']['field'] = 'table_name';
  /* Field: Tripal Materialized Views: Table */
  $handler->display->display_options['fields']['mv_table']['id'] = 'mv_table';
  $handler->display->display_options['fields']['mv_table']['table'] = 'tripal_mviews';
  $handler->display->display_options['fields']['mv_table']['field'] = 'mv_table';
  $handler->display->display_options['fields']['mv_table']['label'] = 'Materialized View';
  /* Field: Tripal Views Integration: Description */
  $handler->display->display_options['fields']['comment']['id'] = 'comment';
  $handler->display->display_options['fields']['comment']['table'] = 'tripal_views';
  $handler->display->display_options['fields']['comment']['field'] = 'comment';
  /* Field: Tripal Views Integration: Base Table? */
  $handler->display->display_options['fields']['base_table']['id'] = 'base_table';
  $handler->display->display_options['fields']['base_table']['table'] = 'tripal_views';
  $handler->display->display_options['fields']['base_table']['field'] = 'base_table';
  $handler->display->display_options['fields']['base_table']['not'] = 0;
  /* Field: Tripal Views Integration: Priority */
  $handler->display->display_options['fields']['priority']['id'] = 'priority';
  $handler->display->display_options['fields']['priority']['table'] = 'tripal_views';
  $handler->display->display_options['fields']['priority']['field'] = 'priority';
  $handler->display->display_options['fields']['priority']['separator'] = '';
  /* Field: Global: Custom text */
  $handler->display->display_options['fields']['nothing']['id'] = 'nothing';
  $handler->display->display_options['fields']['nothing']['table'] = 'views';
  $handler->display->display_options['fields']['nothing']['field'] = 'nothing';
  $handler->display->display_options['fields']['nothing']['label'] = 'Edit Link';
  $handler->display->display_options['fields']['nothing']['exclude'] = TRUE;
  $handler->display->display_options['fields']['nothing']['alter']['text'] = 'edit';
  $handler->display->display_options['fields']['nothing']['alter']['make_link'] = TRUE;
  $handler->display->display_options['fields']['nothing']['alter']['path'] = 'admin/tripal/views-integration/edit/[setup_id]';
  /* Field: Global: Custom text */
  $handler->display->display_options['fields']['nothing_1']['id'] = 'nothing_1';
  $handler->display->display_options['fields']['nothing_1']['table'] = 'views';
  $handler->display->display_options['fields']['nothing_1']['field'] = 'nothing';
  $handler->display->display_options['fields']['nothing_1']['label'] = 'Delete Link';
  $handler->display->display_options['fields']['nothing_1']['exclude'] = TRUE;
  $handler->display->display_options['fields']['nothing_1']['alter']['text'] = 'delete';
  $handler->display->display_options['fields']['nothing_1']['alter']['make_link'] = TRUE;
  $handler->display->display_options['fields']['nothing_1']['alter']['path'] = 'admin/tripal/views-integration/delete/[setup_id]';
  /* Field: Global: Custom text */
  $handler->display->display_options['fields']['nothing_2']['id'] = 'nothing_2';
  $handler->display->display_options['fields']['nothing_2']['table'] = 'views';
  $handler->display->display_options['fields']['nothing_2']['field'] = 'nothing';
  $handler->display->display_options['fields']['nothing_2']['label'] = 'Export Link';
  $handler->display->display_options['fields']['nothing_2']['exclude'] = TRUE;
  $handler->display->display_options['fields']['nothing_2']['alter']['text'] = 'export';
  $handler->display->display_options['fields']['nothing_2']['alter']['make_link'] = TRUE;
  $handler->display->display_options['fields']['nothing_2']['alter']['path'] = 'admin/tripal/views-integration/export/[setup_id]';
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
  /* Sort criterion: Tripal Views Integration: Chado Table Name */
  $handler->display->display_options['sorts']['table_name']['id'] = 'table_name';
  $handler->display->display_options['sorts']['table_name']['table'] = 'tripal_views';
  $handler->display->display_options['sorts']['table_name']['field'] = 'table_name';
  /* Sort criterion: Tripal Views Integration: Priority */
  $handler->display->display_options['sorts']['priority']['id'] = 'priority';
  $handler->display->display_options['sorts']['priority']['table'] = 'tripal_views';
  $handler->display->display_options['sorts']['priority']['field'] = 'priority';
  /* Filter criterion: Tripal Views Integration: Name */
  $handler->display->display_options['filters']['name']['id'] = 'name';
  $handler->display->display_options['filters']['name']['table'] = 'tripal_views';
  $handler->display->display_options['filters']['name']['field'] = 'name';
  $handler->display->display_options['filters']['name']['operator'] = 'contains';
  $handler->display->display_options['filters']['name']['group'] = 1;
  $handler->display->display_options['filters']['name']['exposed'] = TRUE;
  $handler->display->display_options['filters']['name']['expose']['operator_id'] = 'name_op';
  $handler->display->display_options['filters']['name']['expose']['label'] = 'Integration Name Contains';
  $handler->display->display_options['filters']['name']['expose']['operator'] = 'name_op';
  $handler->display->display_options['filters']['name']['expose']['identifier'] = 'name';
  $handler->display->display_options['filters']['name']['expose']['remember_roles'] = array(
    2 => '2',
    1 => 0,
    3 => 0,
  );
  /* Filter criterion: Tripal Views Integration: Chado Table Name */
  $handler->display->display_options['filters']['table_name']['id'] = 'table_name';
  $handler->display->display_options['filters']['table_name']['table'] = 'tripal_views';
  $handler->display->display_options['filters']['table_name']['field'] = 'table_name';
  $handler->display->display_options['filters']['table_name']['group'] = 1;
  $handler->display->display_options['filters']['table_name']['exposed'] = TRUE;
  $handler->display->display_options['filters']['table_name']['expose']['operator_id'] = 'table_name_op';
  $handler->display->display_options['filters']['table_name']['expose']['label'] = 'Chado Table Name';
  $handler->display->display_options['filters']['table_name']['expose']['operator'] = 'table_name_op';
  $handler->display->display_options['filters']['table_name']['expose']['identifier'] = 'table_name';
  $handler->display->display_options['filters']['table_name']['expose']['remember_roles'] = array(
    2 => '2',
    1 => 0,
    3 => 0,
  );
  /* Filter criterion: Tripal Materialized Views: Table */
  $handler->display->display_options['filters']['mv_table']['id'] = 'mv_table';
  $handler->display->display_options['filters']['mv_table']['table'] = 'tripal_mviews';
  $handler->display->display_options['filters']['mv_table']['field'] = 'mv_table';
  $handler->display->display_options['filters']['mv_table']['group'] = 1;
  $handler->display->display_options['filters']['mv_table']['exposed'] = TRUE;
  $handler->display->display_options['filters']['mv_table']['expose']['operator_id'] = 'mv_table_op';
  $handler->display->display_options['filters']['mv_table']['expose']['label'] = 'Materialized View Table Name';
  $handler->display->display_options['filters']['mv_table']['expose']['operator'] = 'mv_table_op';
  $handler->display->display_options['filters']['mv_table']['expose']['identifier'] = 'mv_table';
  $handler->display->display_options['filters']['mv_table']['expose']['remember_roles'] = array(
    2 => '2',
    1 => 0,
    3 => 0,
  );
  /* Filter criterion: Tripal Views Integration: Priority */
  $handler->display->display_options['filters']['priority']['id'] = 'priority';
  $handler->display->display_options['filters']['priority']['table'] = 'tripal_views';
  $handler->display->display_options['filters']['priority']['field'] = 'priority';
  $handler->display->display_options['filters']['priority']['group'] = 1;
  $handler->display->display_options['filters']['priority']['exposed'] = TRUE;
  $handler->display->display_options['filters']['priority']['expose']['operator_id'] = 'priority_op';
  $handler->display->display_options['filters']['priority']['expose']['label'] = 'Priority';
  $handler->display->display_options['filters']['priority']['expose']['operator'] = 'priority_op';
  $handler->display->display_options['filters']['priority']['expose']['identifier'] = 'priority';
  $handler->display->display_options['filters']['priority']['expose']['remember_roles'] = array(
    2 => '2',
    1 => 0,
    3 => 0,
  );
  /* Filter criterion: Tripal Views Integration: Base Table? */
  $handler->display->display_options['filters']['base_table']['id'] = 'base_table';
  $handler->display->display_options['filters']['base_table']['table'] = 'tripal_views';
  $handler->display->display_options['filters']['base_table']['field'] = 'base_table';
  $handler->display->display_options['filters']['base_table']['value'] = 'All';
  $handler->display->display_options['filters']['base_table']['group'] = 1;
  $handler->display->display_options['filters']['base_table']['exposed'] = TRUE;
  $handler->display->display_options['filters']['base_table']['expose']['operator_id'] = '';
  $handler->display->display_options['filters']['base_table']['expose']['label'] = 'Integrates a Base Table?';
  $handler->display->display_options['filters']['base_table']['expose']['operator'] = 'base_table_op';
  $handler->display->display_options['filters']['base_table']['expose']['identifier'] = 'base_table';
  $handler->display->display_options['filters']['base_table']['expose']['remember_roles'] = array(
    2 => '2',
    1 => 0,
    3 => 0,
  );

  /* Display: Page */
  $handler = $view->new_display('page', 'Page', 'page');
  $handler->display->display_options['path'] = 'admin/tripal/views-integration/integrations';
  $handler->display->display_options['menu']['type'] = 'default tab';
  $handler->display->display_options['menu']['title'] = 'Integrations';
  $handler->display->display_options['menu']['weight'] = '-10';
  $handler->display->display_options['menu']['name'] = 'management';
  $handler->display->display_options['menu']['context'] = 0;
  $handler->display->display_options['menu']['context_only_inline'] = 0;
  $handler->display->display_options['tab_options']['weight'] = '0';

  return $view;
}
