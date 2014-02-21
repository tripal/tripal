<?php
/**
 * @file
 * Describes core default views
 */

/**
 * Describes core default views
 *
 * @ingroup tripal_core
 */
function tripal_core_views_default_views() {
  $views = array();

  $view = tripal_core_admin_defaultview_jobs();
  $views[$view->name] = $view;

  $view = tripal_core_admin_defaultview_custom_tables();
  $views[$view->name] = $view;

  $view = tripal_core_admin_defaultview_mviews();
  $views[$view->name] = $view;

  return $views;
}

/**
 * Describes the jobs administration view.
 *
 * @ingroup tripal_core
 */
function tripal_core_admin_defaultview_jobs() {

  $view = new view();
  $view->name = 'tripal_core_admin_jobs';
  $view->description = 'DO NOT DISABLE';
  $view->tag = 'tripal admin';
  $view->base_table = 'tripal_jobs';
  $view->human_name = 'Jobs Admin';
  $view->core = 7;
  $view->api_version = '3.0';
  $view->disabled = FALSE; /* Edit this to true to make a default view disabled initially */

  /* Display: Master */
  $handler = $view->new_display('default', 'Master', 'default');
  $handler->display->display_options['title'] = 'Jobs';
  $handler->display->display_options['use_more_always'] = FALSE;
  $handler->display->display_options['access']['type'] = 'none';
  $handler->display->display_options['cache']['type'] = 'none';
  $handler->display->display_options['query']['type'] = 'views_query';
  $handler->display->display_options['exposed_form']['type'] = 'basic';
  $handler->display->display_options['exposed_form']['options']['submit_button'] = 'Filter';
  $handler->display->display_options['pager']['type'] = 'full';
  $handler->display->display_options['pager']['options']['items_per_page'] = '25';
  $handler->display->display_options['style_plugin'] = 'table';
  $handler->display->display_options['style_options']['columns'] = array(
    'job_id' => 'job_id',
    'job_name' => 'job_name',
    'submit_date' => 'submit_date',
    'start_time' => 'start_time',
    'end_time' => 'end_time',
    'nothing' => 'nothing',
    'priority' => 'priority',
    'progress' => 'progress',
    'status' => 'status',
    'nothing_2' => 'nothing_2',
    'nothing_3' => 'nothing_3',
    'nothing_1' => 'nothing_1',
  );
  $handler->display->display_options['style_options']['default'] = 'job_id';
  $handler->display->display_options['style_options']['info'] = array(
    'job_id' => array(
      'sortable' => 1,
      'default_sort_order' => 'desc',
      'align' => '',
      'separator' => '',
      'empty_column' => 0,
    ),
    'job_name' => array(
      'sortable' => 1,
      'default_sort_order' => 'asc',
      'align' => '',
      'separator' => '',
      'empty_column' => 0,
    ),
    'submit_date' => array(
      'sortable' => 0,
      'default_sort_order' => 'asc',
      'align' => '',
      'separator' => '',
      'empty_column' => 0,
    ),
    'start_time' => array(
      'sortable' => 0,
      'default_sort_order' => 'asc',
      'align' => '',
      'separator' => '',
      'empty_column' => 0,
    ),
    'end_time' => array(
      'sortable' => 0,
      'default_sort_order' => 'asc',
      'align' => '',
      'separator' => '',
      'empty_column' => 0,
    ),
    'nothing' => array(
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
    'progress' => array(
      'sortable' => 1,
      'default_sort_order' => 'asc',
      'align' => '',
      'separator' => '',
      'empty_column' => 0,
    ),
    'status' => array(
      'sortable' => 1,
      'default_sort_order' => 'asc',
      'align' => '',
      'separator' => '',
      'empty_column' => 0,
    ),
    'nothing_2' => array(
      'align' => '',
      'separator' => '',
      'empty_column' => 0,
    ),
    'nothing_3' => array(
      'align' => '',
      'separator' => '',
      'empty_column' => 0,
    ),
    'nothing_1' => array(
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
  $handler->display->display_options['header']['area']['content'] = '<p>Waiting jobs are executed first by priority level (the lower the number the higher the priority) and second by the order they were entered.</p>';
  $handler->display->display_options['header']['area']['format'] = 'filtered_html';
  /* Field: Tripal Jobs: Job ID */
  $handler->display->display_options['fields']['job_id']['id'] = 'job_id';
  $handler->display->display_options['fields']['job_id']['table'] = 'tripal_jobs';
  $handler->display->display_options['fields']['job_id']['field'] = 'job_id';
  $handler->display->display_options['fields']['job_id']['label'] = '';
  $handler->display->display_options['fields']['job_id']['element_class'] = 'extra-short-column';
  $handler->display->display_options['fields']['job_id']['element_label_class'] = 'extra-short-column';
  $handler->display->display_options['fields']['job_id']['element_label_colon'] = FALSE;
  $handler->display->display_options['fields']['job_id']['separator'] = '';
  /* Field: Tripal Jobs: Job Name */
  $handler->display->display_options['fields']['job_name']['id'] = 'job_name';
  $handler->display->display_options['fields']['job_name']['table'] = 'tripal_jobs';
  $handler->display->display_options['fields']['job_name']['field'] = 'job_name';
  $handler->display->display_options['fields']['job_name']['alter']['make_link'] = TRUE;
  $handler->display->display_options['fields']['job_name']['alter']['path'] = 'admin/tripal/tripal_jobs/view/[job_id]';
  /* Field: Tripal Jobs: Submit Date */
  $handler->display->display_options['fields']['submit_date']['id'] = 'submit_date';
  $handler->display->display_options['fields']['submit_date']['table'] = 'tripal_jobs';
  $handler->display->display_options['fields']['submit_date']['field'] = 'submit_date';
  $handler->display->display_options['fields']['submit_date']['exclude'] = TRUE;
  $handler->display->display_options['fields']['submit_date']['date_format'] = 'long';
  /* Field: Tripal Jobs: Start Time */
  $handler->display->display_options['fields']['start_time']['id'] = 'start_time';
  $handler->display->display_options['fields']['start_time']['table'] = 'tripal_jobs';
  $handler->display->display_options['fields']['start_time']['field'] = 'start_time';
  $handler->display->display_options['fields']['start_time']['exclude'] = TRUE;
  $handler->display->display_options['fields']['start_time']['date_format'] = 'long';
  /* Field: Tripal Jobs: End Time */
  $handler->display->display_options['fields']['end_time']['id'] = 'end_time';
  $handler->display->display_options['fields']['end_time']['table'] = 'tripal_jobs';
  $handler->display->display_options['fields']['end_time']['field'] = 'end_time';
  $handler->display->display_options['fields']['end_time']['exclude'] = TRUE;
  $handler->display->display_options['fields']['end_time']['date_format'] = 'long';
  /* Field: Global: Custom text */
  $handler->display->display_options['fields']['nothing']['id'] = 'nothing';
  $handler->display->display_options['fields']['nothing']['table'] = 'views';
  $handler->display->display_options['fields']['nothing']['field'] = 'nothing';
  $handler->display->display_options['fields']['nothing']['label'] = 'Dates';
  $handler->display->display_options['fields']['nothing']['alter']['text'] = 'Submitted: [submit_date]<br />
  Started: [start_time]<br />
  Ended: [end_time]';
  /* Field: Tripal Jobs: Priority */
  $handler->display->display_options['fields']['priority']['id'] = 'priority';
  $handler->display->display_options['fields']['priority']['table'] = 'tripal_jobs';
  $handler->display->display_options['fields']['priority']['field'] = 'priority';
  $handler->display->display_options['fields']['priority']['element_class'] = 'short-column';
  $handler->display->display_options['fields']['priority']['element_label_class'] = 'short-column';
  $handler->display->display_options['fields']['priority']['separator'] = '';
  /* Field: Tripal Jobs: Progress */
  $handler->display->display_options['fields']['progress']['id'] = 'progress';
  $handler->display->display_options['fields']['progress']['table'] = 'tripal_jobs';
  $handler->display->display_options['fields']['progress']['field'] = 'progress';
  $handler->display->display_options['fields']['progress']['element_class'] = 'short-column';
  $handler->display->display_options['fields']['progress']['element_label_class'] = 'short-column';
  $handler->display->display_options['fields']['progress']['separator'] = '';
  $handler->display->display_options['fields']['progress']['suffix'] = '%';
  /* Field: Tripal Jobs: Status */
  $handler->display->display_options['fields']['status']['id'] = 'status';
  $handler->display->display_options['fields']['status']['table'] = 'tripal_jobs';
  $handler->display->display_options['fields']['status']['field'] = 'status';
  $handler->display->display_options['fields']['status']['element_class'] = 'short-column';
  $handler->display->display_options['fields']['status']['element_label_class'] = 'short-column';
  /* Field: Global: Custom text */
  $handler->display->display_options['fields']['nothing_2']['id'] = 'nothing_2';
  $handler->display->display_options['fields']['nothing_2']['table'] = 'views';
  $handler->display->display_options['fields']['nothing_2']['field'] = 'nothing';
  $handler->display->display_options['fields']['nothing_2']['label'] = 'Re-run Link';
  $handler->display->display_options['fields']['nothing_2']['exclude'] = TRUE;
  $handler->display->display_options['fields']['nothing_2']['alter']['text'] = 'Re-run';
  $handler->display->display_options['fields']['nothing_2']['alter']['make_link'] = TRUE;
  $handler->display->display_options['fields']['nothing_2']['alter']['path'] = 'admin/tripal/tripal_jobs/rerun/[job_id]';
  /* Field: Global: Custom text */
  $handler->display->display_options['fields']['nothing_3']['id'] = 'nothing_3';
  $handler->display->display_options['fields']['nothing_3']['table'] = 'views';
  $handler->display->display_options['fields']['nothing_3']['field'] = 'nothing';
  $handler->display->display_options['fields']['nothing_3']['label'] = 'Cancel link';
  $handler->display->display_options['fields']['nothing_3']['exclude'] = TRUE;
  $handler->display->display_options['fields']['nothing_3']['alter']['text'] = 'Cancel';
  $handler->display->display_options['fields']['nothing_3']['alter']['make_link'] = TRUE;
  $handler->display->display_options['fields']['nothing_3']['alter']['path'] = 'admin/tripal/tripal_jobs/cancel/[job_id]';
  /* Field: Global: Custom text */
  $handler->display->display_options['fields']['nothing_1']['id'] = 'nothing_1';
  $handler->display->display_options['fields']['nothing_1']['table'] = 'views';
  $handler->display->display_options['fields']['nothing_1']['field'] = 'nothing';
  $handler->display->display_options['fields']['nothing_1']['label'] = 'Action';
  $handler->display->display_options['fields']['nothing_1']['alter']['text'] = '[nothing_2]<br />
  [nothing_3]';
  $handler->display->display_options['fields']['nothing_1']['element_class'] = 'short-column';
  $handler->display->display_options['fields']['nothing_1']['element_label_class'] = 'short-column';
  /* Filter criterion: Tripal Jobs: Job Name */
  $handler->display->display_options['filters']['job_name']['id'] = 'job_name';
  $handler->display->display_options['filters']['job_name']['table'] = 'tripal_jobs';
  $handler->display->display_options['filters']['job_name']['field'] = 'job_name';
  $handler->display->display_options['filters']['job_name']['operator'] = 'contains';
  $handler->display->display_options['filters']['job_name']['exposed'] = TRUE;
  $handler->display->display_options['filters']['job_name']['expose']['operator_id'] = 'job_name_op';
  $handler->display->display_options['filters']['job_name']['expose']['label'] = 'Job Name Contains';
  $handler->display->display_options['filters']['job_name']['expose']['operator'] = 'job_name_op';
  $handler->display->display_options['filters']['job_name']['expose']['identifier'] = 'job_name';
  $handler->display->display_options['filters']['job_name']['expose']['remember_roles'] = array(
    2 => '2',
    1 => 0,
    3 => 0,
  );
  /* Filter criterion: Tripal Jobs: Module Name */
  $handler->display->display_options['filters']['modulename']['id'] = 'modulename';
  $handler->display->display_options['filters']['modulename']['table'] = 'tripal_jobs';
  $handler->display->display_options['filters']['modulename']['field'] = 'modulename';
  $handler->display->display_options['filters']['modulename']['exposed'] = TRUE;
  $handler->display->display_options['filters']['modulename']['expose']['operator_id'] = 'modulename_op';
  $handler->display->display_options['filters']['modulename']['expose']['label'] = 'Submitting Module';
  $handler->display->display_options['filters']['modulename']['expose']['operator'] = 'modulename_op';
  $handler->display->display_options['filters']['modulename']['expose']['identifier'] = 'modulename';
  $handler->display->display_options['filters']['modulename']['expose']['remember_roles'] = array(
    2 => '2',
    1 => 0,
    3 => 0,
  );
  /* Filter criterion: Tripal Jobs: Status */
  $handler->display->display_options['filters']['status']['id'] = 'status';
  $handler->display->display_options['filters']['status']['table'] = 'tripal_jobs';
  $handler->display->display_options['filters']['status']['field'] = 'status';
  $handler->display->display_options['filters']['status']['exposed'] = TRUE;
  $handler->display->display_options['filters']['status']['expose']['operator_id'] = 'status_op';
  $handler->display->display_options['filters']['status']['expose']['label'] = 'Job Status';
  $handler->display->display_options['filters']['status']['expose']['operator'] = 'status_op';
  $handler->display->display_options['filters']['status']['expose']['identifier'] = 'status';
  $handler->display->display_options['filters']['status']['expose']['remember_roles'] = array(
    2 => '2',
    1 => 0,
    3 => 0,
  );

  /* Display: Page */
  $handler = $view->new_display('page', 'Page', 'page');
  $handler->display->display_options['path'] = 'admin/tripal/tripal_jobs/jobs';
  $handler->display->display_options['menu']['type'] = 'default tab';
  $handler->display->display_options['menu']['title'] = 'Jobs';
  $handler->display->display_options['menu']['description'] = 'Jobs managed by Tripal';
  $handler->display->display_options['menu']['weight'] = '-10';
  $handler->display->display_options['menu']['context'] = 0;
  $handler->display->display_options['menu']['context_only_inline'] = 0;
  $handler->display->display_options['tab_options']['weight'] = '0';

  return $view;
}

/**
 * Describes the custom tables administration view.
 *
 * @ingroup tripal_core
 */
function tripal_core_admin_defaultview_custom_tables() {

  $view = new view();
  $view->name = 'tripal_core_admin_custom_table';
  $view->description = 'DO NOT DISABLE';
  $view->tag = 'tripal admin';
  $view->base_table = 'tripal_custom_tables';
  $view->human_name = 'Tripal Custom Table Admin';
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
  $handler->display->display_options['header']['action_links_area']['link-1'] = array(
    'label-1' => 'Add Custom Table',
    'path-1' => 'admin/tripal/schema/custom_tables/new',
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
  /* Field: Tripal Custom Tables: Custom Table ID */
  $handler->display->display_options['fields']['table_id']['id'] = 'table_id';
  $handler->display->display_options['fields']['table_id']['table'] = 'tripal_custom_tables';
  $handler->display->display_options['fields']['table_id']['field'] = 'table_id';
  $handler->display->display_options['fields']['table_id']['label'] = '';
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
  $handler->display->display_options['fields']['table_name']['alter']['path'] = 'admin/tripal/schema/custom_tables/view/[table_id]';
  /* Field: Global: Custom text */
  $handler->display->display_options['fields']['nothing']['id'] = 'nothing';
  $handler->display->display_options['fields']['nothing']['table'] = 'views';
  $handler->display->display_options['fields']['nothing']['field'] = 'nothing';
  $handler->display->display_options['fields']['nothing']['label'] = 'Edit Link';
  $handler->display->display_options['fields']['nothing']['exclude'] = TRUE;
  $handler->display->display_options['fields']['nothing']['alter']['text'] = 'Edit';
  $handler->display->display_options['fields']['nothing']['alter']['make_link'] = TRUE;
  $handler->display->display_options['fields']['nothing']['alter']['path'] = 'admin/tripal/schema/custom_tables/edit/[table_id]';
  /* Field: Global: Custom text */
  $handler->display->display_options['fields']['nothing_1']['id'] = 'nothing_1';
  $handler->display->display_options['fields']['nothing_1']['table'] = 'views';
  $handler->display->display_options['fields']['nothing_1']['field'] = 'nothing';
  $handler->display->display_options['fields']['nothing_1']['label'] = 'Delete Link';
  $handler->display->display_options['fields']['nothing_1']['exclude'] = TRUE;
  $handler->display->display_options['fields']['nothing_1']['alter']['text'] = 'Delete';
  $handler->display->display_options['fields']['nothing_1']['alter']['make_link'] = TRUE;
  $handler->display->display_options['fields']['nothing_1']['alter']['path'] = 'admin/tripal/schema/custom_tables/action/delete/[table_id]';
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
  $handler->display->display_options['filters']['table_name']['expose']['remember_roles'] = array(
    2 => '2',
    1 => 0,
    3 => 0,
  );

  /* Display: Page */
  $handler = $view->new_display('page', 'Page', 'page');
  $handler->display->display_options['path'] = 'admin/tripal/schema/custom_tables/tables';
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
 * @ingroup tripal_core
 */
function tripal_core_admin_defaultview_mviews() {

  $view = new view();
  $view->name = 'tripal_core_admin_mviews';
  $view->description = 'DO NOT DISABLE';
  $view->tag = 'tripal admin';
  $view->base_table = 'tripal_mviews';
  $view->human_name = 'MView Admin';
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
  $handler->display->display_options['style_options']['columns'] = array(
    'mview_id' => 'mview_id',
    'name' => 'name',
    'mv_table' => 'mv_table',
    'comment' => 'comment',
    'last_update' => 'last_update',
    'status' => 'status',
  );
  $handler->display->display_options['style_options']['default'] = 'mview_id';
  $handler->display->display_options['style_options']['info'] = array(
    'mview_id' => array(
      'sortable' => 1,
      'default_sort_order' => 'asc',
      'align' => '',
      'separator' => '',
      'empty_column' => 0,
    ),
    'name' => array(
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
    'last_update' => array(
      'sortable' => 1,
      'default_sort_order' => 'asc',
      'align' => '',
      'separator' => '',
      'empty_column' => 0,
    ),
    'status' => array(
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
  $handler->display->display_options['header']['action_links_area']['link-1'] = array(
    'label-1' => 'Add Materialized View',
    'path-1' => 'admin/tripal/schema/mviews/new',
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
  $handler->display->display_options['fields']['name']['alter']['path'] = 'admin/tripal/schema/mviews/report/[mview_id]';
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
  $handler->display->display_options['fields']['nothing']['alter']['path'] = 'admin/tripal/schema/mviews/edit/[mview_id]';
  /* Field: Global: Custom text */
  $handler->display->display_options['fields']['nothing_1']['id'] = 'nothing_1';
  $handler->display->display_options['fields']['nothing_1']['table'] = 'views';
  $handler->display->display_options['fields']['nothing_1']['field'] = 'nothing';
  $handler->display->display_options['fields']['nothing_1']['label'] = 'Delete Link';
  $handler->display->display_options['fields']['nothing_1']['exclude'] = TRUE;
  $handler->display->display_options['fields']['nothing_1']['alter']['text'] = 'Delete';
  $handler->display->display_options['fields']['nothing_1']['alter']['make_link'] = TRUE;
  $handler->display->display_options['fields']['nothing_1']['alter']['path'] = 'admin/tripal/schema/mviews/action/delete/[mview_id]';
  /* Field: Global: Custom text */
  $handler->display->display_options['fields']['nothing_2']['id'] = 'nothing_2';
  $handler->display->display_options['fields']['nothing_2']['table'] = 'views';
  $handler->display->display_options['fields']['nothing_2']['field'] = 'nothing';
  $handler->display->display_options['fields']['nothing_2']['label'] = 'Populate Link';
  $handler->display->display_options['fields']['nothing_2']['exclude'] = TRUE;
  $handler->display->display_options['fields']['nothing_2']['alter']['text'] = 'Populate';
  $handler->display->display_options['fields']['nothing_2']['alter']['make_link'] = TRUE;
  $handler->display->display_options['fields']['nothing_2']['alter']['path'] = 'admin/tripal/schema/mviews/action/update/[mview_id]';
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
  $handler->display->display_options['filters']['name']['expose']['remember_roles'] = array(
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
  $handler->display->display_options['filters']['mv_table']['expose']['label'] = 'Database Table';
  $handler->display->display_options['filters']['mv_table']['expose']['operator'] = 'mv_table_op';
  $handler->display->display_options['filters']['mv_table']['expose']['identifier'] = 'mv_table';
  $handler->display->display_options['filters']['mv_table']['expose']['remember_roles'] = array(
    2 => '2',
    1 => 0,
    3 => 0,
  );
  /* Filter criterion: Tripal Materialized Views: Module Name */
  $handler->display->display_options['filters']['modulename']['id'] = 'modulename';
  $handler->display->display_options['filters']['modulename']['table'] = 'tripal_mviews';
  $handler->display->display_options['filters']['modulename']['field'] = 'modulename';
  $handler->display->display_options['filters']['modulename']['group'] = 1;
  $handler->display->display_options['filters']['modulename']['exposed'] = TRUE;
  $handler->display->display_options['filters']['modulename']['expose']['operator_id'] = 'modulename_op';
  $handler->display->display_options['filters']['modulename']['expose']['label'] = 'Module Created by';
  $handler->display->display_options['filters']['modulename']['expose']['operator'] = 'modulename_op';
  $handler->display->display_options['filters']['modulename']['expose']['identifier'] = 'modulename';
  $handler->display->display_options['filters']['modulename']['expose']['remember_roles'] = array(
    2 => '2',
    1 => 0,
    3 => 0,
  );
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
  $handler->display->display_options['filters']['status']['expose']['remember_roles'] = array(
    2 => '2',
    1 => 0,
    3 => 0,
  );

  /* Display: Page */
  $handler = $view->new_display('page', 'Page', 'page');
  $handler->display->display_options['path'] = 'admin/tripal/schema/mviews/mviews';
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
