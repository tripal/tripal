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
function tripal_views_default_views() {
  $views = array();

  $view = tripal_admin_defaultview_jobs();
  $views[$view->name] = $view;

  return $views;
}

/**
 * Describes the jobs administration view.
 *
 * @ingroup tripal
 */
function tripal_admin_defaultview_jobs() {

  $view = new view();
  $view->name = 'tripal_admin_jobs';
  $view->description = 'DO NOT DISABLE';
  $view->tag = 'tripal admin';
  $view->base_table = 'tripal_jobs';
  $view->human_name = 'Tripal Jobs (Admin)';
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
