<?php
/**
 * @file
 * Defines the tripal bulk loader default views.
 *
 * @ingroup tripal_bulk_loader
 */

/**
 * Implements hook_views_default_views().
 *
 * @ingroup tripal_bulk_loader
 */
function tripal_bulk_loader_views_default_views() {
  $views = array();

  // Default Tripal Admin View: Jobs
  $view = tripal_bulk_loader_defaultview_admin_job_listing();
  $views[$view->name] = $view;

  // Default Tripal Admin View: Templates
  $view = tripal_bulk_loader_defaultview_admin_template_listing();
  $views[$view->name] = $view;

  return $views;
}

/**
 * Default Admin View for job management
 *
 * @ingroup tripal_bulk_loader
 */
function tripal_bulk_loader_defaultview_admin_job_listing() {

  $view = new view();
  $view->name = 'tripal_bulk_loading_jobs';
  $view->description = 'DO NOT DISABLE';
  $view->tag = 'tripal admin';
  $view->base_table = 'tripal_bulk_loader';
  $view->human_name = 'Tripal Bulk Loading Jobs';
  $view->core = 7;
  $view->api_version = '3.0';
  $view->disabled = FALSE; /* Edit this to true to make a default view disabled initially */

  /* Display: Master */
  $handler = $view->new_display('default', 'Master', 'default');
  $handler->display->display_options['title'] = 'Tripal Bulk Loading Jobs';
  $handler->display->display_options['use_more_always'] = FALSE;
  $handler->display->display_options['access']['type'] = 'perm';
  $handler->display->display_options['access']['perm'] = 'administer tripal_bulk_loader';
  $handler->display->display_options['cache']['type'] = 'none';
  $handler->display->display_options['query']['type'] = 'views_query';
  $handler->display->display_options['exposed_form']['type'] = 'basic';
  $handler->display->display_options['pager']['type'] = 'full';
  $handler->display->display_options['pager']['options']['items_per_page'] = '25';
  $handler->display->display_options['style_plugin'] = 'table';
  $handler->display->display_options['style_options']['columns'] = array(
    'template_id' => 'template_id',
    'edit_node' => 'edit_node',
    'loader_name' => 'loader_name',
    'name' => 'name',
    'created' => 'created',
    'changed' => 'changed',
    'job_status' => 'job_status',
  );
  $handler->display->display_options['style_options']['default'] = 'changed';
  $handler->display->display_options['style_options']['info'] = array(
    'template_id' => array(
      'sortable' => 0,
      'default_sort_order' => 'asc',
      'align' => '',
      'separator' => '',
      'empty_column' => 0,
    ),
    'edit_node' => array(
      'align' => '',
      'separator' => '',
      'empty_column' => 0,
    ),
    'loader_name' => array(
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
    'created' => array(
      'sortable' => 1,
      'default_sort_order' => 'asc',
      'align' => '',
      'separator' => '',
      'empty_column' => 0,
    ),
    'changed' => array(
      'sortable' => 1,
      'default_sort_order' => 'asc',
      'align' => '',
      'separator' => '',
      'empty_column' => 0,
    ),
    'job_status' => array(
      'sortable' => 1,
      'default_sort_order' => 'asc',
      'align' => '',
      'separator' => '',
      'empty_column' => 0,
    ),
  );
  /* Header: Global: Action Links */
  $handler->display->display_options['header']['action_links_area']['id'] = 'action_links_area';
  $handler->display->display_options['header']['action_links_area']['table'] = 'views';
  $handler->display->display_options['header']['action_links_area']['field'] = 'action_links_area';
  $handler->display->display_options['header']['action_links_area']['label'] = 'Action Links';
  $handler->display->display_options['header']['action_links_area']['empty'] = TRUE;
  $handler->display->display_options['header']['action_links_area']['link-1'] = array(
    'label-1' => 'Add Bulk Loading Job',
    'path-1' => 'node/add/tripal-bulk-loader',
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
  $handler->display->display_options['empty']['area']['id'] = 'area';
  $handler->display->display_options['empty']['area']['table'] = 'views';
  $handler->display->display_options['empty']['area']['field'] = 'area';
  $handler->display->display_options['empty']['area']['label'] = 'No Results';
  $handler->display->display_options['empty']['area']['empty'] = TRUE;
  $handler->display->display_options['empty']['area']['content'] = '<p>No Tripal Bulk Loading Jobs have been created.</p>';
  $handler->display->display_options['empty']['area']['format'] = 'filtered_html';
  /* Relationship: Tripal Bulk Loading Jobs: Bulk Loading Job => Node */
  $handler->display->display_options['relationships']['nid']['id'] = 'nid';
  $handler->display->display_options['relationships']['nid']['table'] = 'tripal_bulk_loader';
  $handler->display->display_options['relationships']['nid']['field'] = 'nid';
  $handler->display->display_options['relationships']['nid']['label'] = 'TBL Job => Node';
  /* Field: Content: Nid */
  $handler->display->display_options['fields']['nid']['id'] = 'nid';
  $handler->display->display_options['fields']['nid']['table'] = 'node';
  $handler->display->display_options['fields']['nid']['field'] = 'nid';
  $handler->display->display_options['fields']['nid']['relationship'] = 'nid';
  $handler->display->display_options['fields']['nid']['label'] = '';
  $handler->display->display_options['fields']['nid']['exclude'] = TRUE;
  $handler->display->display_options['fields']['nid']['element_label_colon'] = FALSE;
  /* Field: Tripal Bulk Loader Templates: Template ID */
  $handler->display->display_options['fields']['template_id']['id'] = 'template_id';
  $handler->display->display_options['fields']['template_id']['table'] = 'tripal_bulk_loader_template';
  $handler->display->display_options['fields']['template_id']['field'] = 'template_id';
  $handler->display->display_options['fields']['template_id']['exclude'] = TRUE;
  $handler->display->display_options['fields']['template_id']['separator'] = '';
  /* Field: Tripal Bulk Loading Jobs: Name */
  $handler->display->display_options['fields']['loader_name']['id'] = 'loader_name';
  $handler->display->display_options['fields']['loader_name']['table'] = 'tripal_bulk_loader';
  $handler->display->display_options['fields']['loader_name']['field'] = 'loader_name';
  $handler->display->display_options['fields']['loader_name']['alter']['make_link'] = TRUE;
  $handler->display->display_options['fields']['loader_name']['alter']['path'] = 'node/[nid]';
  /* Field: Tripal Bulk Loader Templates: Name */
  $handler->display->display_options['fields']['name']['id'] = 'name';
  $handler->display->display_options['fields']['name']['table'] = 'tripal_bulk_loader_template';
  $handler->display->display_options['fields']['name']['field'] = 'name';
  $handler->display->display_options['fields']['name']['label'] = 'Template';
  $handler->display->display_options['fields']['name']['alter']['make_link'] = TRUE;
  $handler->display->display_options['fields']['name']['alter']['path'] = 'admin/tripal/loaders/bulk/template/[template_id]/edit';
  /* Field: Content: Post date */
  $handler->display->display_options['fields']['created']['id'] = 'created';
  $handler->display->display_options['fields']['created']['table'] = 'node';
  $handler->display->display_options['fields']['created']['field'] = 'created';
  $handler->display->display_options['fields']['created']['relationship'] = 'nid';
  $handler->display->display_options['fields']['created']['label'] = 'Date Created';
  $handler->display->display_options['fields']['created']['date_format'] = 'medium';
  /* Field: Content: Updated date */
  $handler->display->display_options['fields']['changed']['id'] = 'changed';
  $handler->display->display_options['fields']['changed']['table'] = 'node';
  $handler->display->display_options['fields']['changed']['field'] = 'changed';
  $handler->display->display_options['fields']['changed']['relationship'] = 'nid';
  $handler->display->display_options['fields']['changed']['label'] = 'Date Updated';
  $handler->display->display_options['fields']['changed']['date_format'] = 'medium';
  /* Field: Tripal Bulk Loading Jobs: Job Status */
  $handler->display->display_options['fields']['job_status']['id'] = 'job_status';
  $handler->display->display_options['fields']['job_status']['table'] = 'tripal_bulk_loader';
  $handler->display->display_options['fields']['job_status']['field'] = 'job_status';
  $handler->display->display_options['fields']['job_status']['label'] = 'Status';
  /* Field: Content: Edit link */
  $handler->display->display_options['fields']['edit_node']['id'] = 'edit_node';
  $handler->display->display_options['fields']['edit_node']['table'] = 'views_entity_node';
  $handler->display->display_options['fields']['edit_node']['field'] = 'edit_node';
  $handler->display->display_options['fields']['edit_node']['relationship'] = 'nid';
  $handler->display->display_options['fields']['edit_node']['label'] = '';
  $handler->display->display_options['fields']['edit_node']['exclude'] = TRUE;
  $handler->display->display_options['fields']['edit_node']['element_label_colon'] = FALSE;
  $handler->display->display_options['fields']['edit_node']['text'] = 'edit';
  /* Field: Content: Delete link */
  $handler->display->display_options['fields']['delete_node']['id'] = 'delete_node';
  $handler->display->display_options['fields']['delete_node']['table'] = 'views_entity_node';
  $handler->display->display_options['fields']['delete_node']['field'] = 'delete_node';
  $handler->display->display_options['fields']['delete_node']['relationship'] = 'nid';
  $handler->display->display_options['fields']['delete_node']['label'] = '';
  $handler->display->display_options['fields']['delete_node']['exclude'] = TRUE;
  $handler->display->display_options['fields']['delete_node']['element_label_colon'] = FALSE;
  $handler->display->display_options['fields']['delete_node']['text'] = 'delete';
  /* Field: Global: Custom text */
  $handler->display->display_options['fields']['nothing']['id'] = 'nothing';
  $handler->display->display_options['fields']['nothing']['table'] = 'views';
  $handler->display->display_options['fields']['nothing']['field'] = 'nothing';
  $handler->display->display_options['fields']['nothing']['label'] = '';
  $handler->display->display_options['fields']['nothing']['alter']['text'] = '[edit_node]    [delete_node]';
  $handler->display->display_options['fields']['nothing']['element_label_colon'] = FALSE;
  /* Filter criterion: Tripal Bulk Loader Templates: Name */
  $handler->display->display_options['filters']['name']['id'] = 'name';
  $handler->display->display_options['filters']['name']['table'] = 'tripal_bulk_loader_template';
  $handler->display->display_options['filters']['name']['field'] = 'name';
  $handler->display->display_options['filters']['name']['operator'] = 'contains';
  $handler->display->display_options['filters']['name']['exposed'] = TRUE;
  $handler->display->display_options['filters']['name']['expose']['operator_id'] = 'name_op';
  $handler->display->display_options['filters']['name']['expose']['label'] = 'Template Name Contains';
  $handler->display->display_options['filters']['name']['expose']['operator'] = 'name_op';
  $handler->display->display_options['filters']['name']['expose']['identifier'] = 'name';
  $handler->display->display_options['filters']['name']['expose']['remember_roles'] = array(
    2 => '2',
    1 => 0,
    3 => 0,
  );
  /* Filter criterion: Tripal Bulk Loading Jobs: Job Status */
  $handler->display->display_options['filters']['job_status']['id'] = 'job_status';
  $handler->display->display_options['filters']['job_status']['table'] = 'tripal_bulk_loader';
  $handler->display->display_options['filters']['job_status']['field'] = 'job_status';
  $handler->display->display_options['filters']['job_status']['exposed'] = TRUE;
  $handler->display->display_options['filters']['job_status']['expose']['operator_id'] = 'job_status_op';
  $handler->display->display_options['filters']['job_status']['expose']['label'] = 'Job Status';
  $handler->display->display_options['filters']['job_status']['expose']['operator'] = 'job_status_op';
  $handler->display->display_options['filters']['job_status']['expose']['identifier'] = 'job_status';
  $handler->display->display_options['filters']['job_status']['expose']['remember_roles'] = array(
    2 => '2',
    1 => 0,
    3 => 0,
  );
  /* Filter criterion: Tripal Bulk Loading Jobs: Name */
  $handler->display->display_options['filters']['loader_name']['id'] = 'loader_name';
  $handler->display->display_options['filters']['loader_name']['table'] = 'tripal_bulk_loader';
  $handler->display->display_options['filters']['loader_name']['field'] = 'loader_name';
  $handler->display->display_options['filters']['loader_name']['operator'] = 'contains';
  $handler->display->display_options['filters']['loader_name']['exposed'] = TRUE;
  $handler->display->display_options['filters']['loader_name']['expose']['operator_id'] = 'loader_name_op';
  $handler->display->display_options['filters']['loader_name']['expose']['label'] = 'Job Name Contains';
  $handler->display->display_options['filters']['loader_name']['expose']['operator'] = 'loader_name_op';
  $handler->display->display_options['filters']['loader_name']['expose']['identifier'] = 'loader_name';
  $handler->display->display_options['filters']['loader_name']['expose']['remember_roles'] = array(
    2 => '2',
    1 => 0,
    3 => 0,
  );

  /* Display: Page */
  $handler = $view->new_display('page', 'Page', 'page_1');
  $handler->display->display_options['path'] = 'admin/tripal/loaders/bulk/jobs';
  $handler->display->display_options['menu']['type'] = 'default tab';
  $handler->display->display_options['menu']['title'] = 'Jobs';
  $handler->display->display_options['menu']['weight'] = '0';
  $handler->display->display_options['menu']['context'] = 0;
  $handler->display->display_options['menu']['context_only_inline'] = 0;
  $handler->display->display_options['tab_options']['weight'] = '0';


  return $view;
}

/**
 * Default Admin View for template management
 *
 * @ingroup tripal_bulk_loader
 */
function tripal_bulk_loader_defaultview_admin_template_listing() {

  $view = new view();
  $view->name = 'tripal_bulk_loader_templates';
  $view->description = 'DO NOT DISABLE';
  $view->tag = 'tripal admin';
  $view->base_table = 'tripal_bulk_loader_template';
  $view->human_name = 'Tripal Bulk Loader Templates';
  $view->core = 7;
  $view->api_version = '3.0';
  $view->disabled = FALSE; /* Edit this to true to make a default view disabled initially */

  /* Display: Master */
  $handler = $view->new_display('default', 'Master', 'default');
  $handler->display->display_options['title'] = 'Tripal Bulk Loader Templates';
  $handler->display->display_options['use_more_always'] = FALSE;
  $handler->display->display_options['access']['type'] = 'perm';
  $handler->display->display_options['access']['perm'] = 'administer tripal_bulk_loader';
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
    'label-1' => 'Add Template',
    'path-1' => 'admin/tripal/loaders/bulk/template/create',
  );
  $handler->display->display_options['header']['action_links_area']['link-2'] = array(
    'label-2' => 'Import Template',
    'path-2' => 'admin/tripal/loaders/bulk/templates/import',
  );
  /* Field: Tripal Bulk Loader Templates: Name */
  $handler->display->display_options['fields']['name']['id'] = 'name';
  $handler->display->display_options['fields']['name']['table'] = 'tripal_bulk_loader_template';
  $handler->display->display_options['fields']['name']['field'] = 'name';
  /* Field: Tripal Bulk Loader Templates: Created */
  $handler->display->display_options['fields']['created']['id'] = 'created';
  $handler->display->display_options['fields']['created']['table'] = 'tripal_bulk_loader_template';
  $handler->display->display_options['fields']['created']['field'] = 'created';
  $handler->display->display_options['fields']['created']['label'] = 'Created On';
  $handler->display->display_options['fields']['created']['date_format'] = 'long';
  /* Field: Tripal Bulk Loader Templates: Updated */
  $handler->display->display_options['fields']['changed']['id'] = 'changed';
  $handler->display->display_options['fields']['changed']['table'] = 'tripal_bulk_loader_template';
  $handler->display->display_options['fields']['changed']['field'] = 'changed';
  $handler->display->display_options['fields']['changed']['label'] = 'Updated On';
  $handler->display->display_options['fields']['changed']['date_format'] = 'long';
  /* Field: Tripal Bulk Loader Templates: Template ID */
  $handler->display->display_options['fields']['template_id']['id'] = 'template_id';
  $handler->display->display_options['fields']['template_id']['table'] = 'tripal_bulk_loader_template';
  $handler->display->display_options['fields']['template_id']['field'] = 'template_id';
  $handler->display->display_options['fields']['template_id']['label'] = '';
  $handler->display->display_options['fields']['template_id']['exclude'] = TRUE;
  $handler->display->display_options['fields']['template_id']['element_label_colon'] = FALSE;
  $handler->display->display_options['fields']['template_id']['separator'] = '';
  /* Field: Global: Custom text */
  $handler->display->display_options['fields']['nothing']['id'] = 'nothing';
  $handler->display->display_options['fields']['nothing']['table'] = 'views';
  $handler->display->display_options['fields']['nothing']['field'] = 'nothing';
  $handler->display->display_options['fields']['nothing']['label'] = 'Edit Template';
  $handler->display->display_options['fields']['nothing']['exclude'] = TRUE;
  $handler->display->display_options['fields']['nothing']['alter']['text'] = 'edit';
  $handler->display->display_options['fields']['nothing']['alter']['make_link'] = TRUE;
  $handler->display->display_options['fields']['nothing']['alter']['path'] = 'admin/tripal/loaders/bulk/template/[template_id]/edit';
  /* Field: Global: Custom text */
  $handler->display->display_options['fields']['nothing_1']['id'] = 'nothing_1';
  $handler->display->display_options['fields']['nothing_1']['table'] = 'views';
  $handler->display->display_options['fields']['nothing_1']['field'] = 'nothing';
  $handler->display->display_options['fields']['nothing_1']['label'] = 'Delete Template';
  $handler->display->display_options['fields']['nothing_1']['exclude'] = TRUE;
  $handler->display->display_options['fields']['nothing_1']['alter']['text'] = 'delete';
  $handler->display->display_options['fields']['nothing_1']['alter']['make_link'] = TRUE;
  $handler->display->display_options['fields']['nothing_1']['alter']['path'] = 'admin/tripal/loaders/bulk/template/[template_id]/delete?template_name=[name]';
  /* Field: Global: Custom text */
  $handler->display->display_options['fields']['nothing_2']['id'] = 'nothing_2';
  $handler->display->display_options['fields']['nothing_2']['table'] = 'views';
  $handler->display->display_options['fields']['nothing_2']['field'] = 'nothing';
  $handler->display->display_options['fields']['nothing_2']['label'] = 'Export Template';
  $handler->display->display_options['fields']['nothing_2']['exclude'] = TRUE;
  $handler->display->display_options['fields']['nothing_2']['alter']['text'] = 'export';
  $handler->display->display_options['fields']['nothing_2']['alter']['make_link'] = TRUE;
  $handler->display->display_options['fields']['nothing_2']['alter']['path'] = 'admin/tripal/loaders/bulk/template/[template_id]/export';
  /* Field: Global: Custom text */
  $handler->display->display_options['fields']['nothing_3']['id'] = 'nothing_3';
  $handler->display->display_options['fields']['nothing_3']['table'] = 'views';
  $handler->display->display_options['fields']['nothing_3']['field'] = 'nothing';
  $handler->display->display_options['fields']['nothing_3']['label'] = 'View Jobs';
  $handler->display->display_options['fields']['nothing_3']['exclude'] = TRUE;
  $handler->display->display_options['fields']['nothing_3']['alter']['text'] = 'View Jobs';
  $handler->display->display_options['fields']['nothing_3']['alter']['make_link'] = TRUE;
  $handler->display->display_options['fields']['nothing_3']['alter']['path'] = 'admin/tripal/loaders/bulk/jobs?name=[name]';
  /* Field: Global: Custom text */
  $handler->display->display_options['fields']['nothing_4']['id'] = 'nothing_4';
  $handler->display->display_options['fields']['nothing_4']['table'] = 'views';
  $handler->display->display_options['fields']['nothing_4']['field'] = 'nothing';
  $handler->display->display_options['fields']['nothing_4']['label'] = '';
  $handler->display->display_options['fields']['nothing_4']['alter']['text'] = '[nothing]   [nothing_1]   [nothing_2]<br />
      [nothing_3]';
  $handler->display->display_options['fields']['nothing_4']['element_label_colon'] = FALSE;
  $handler->display->display_options['fields']['nothing_4']['element_wrapper_class'] = 'active';
  /* Filter criterion: Tripal Bulk Loader Templates: Name */
  $handler->display->display_options['filters']['name']['id'] = 'name';
  $handler->display->display_options['filters']['name']['table'] = 'tripal_bulk_loader_template';
  $handler->display->display_options['filters']['name']['field'] = 'name';
  $handler->display->display_options['filters']['name']['operator'] = 'contains';
  $handler->display->display_options['filters']['name']['group'] = 1;
  $handler->display->display_options['filters']['name']['exposed'] = TRUE;
  $handler->display->display_options['filters']['name']['expose']['operator_id'] = 'name_op';
  $handler->display->display_options['filters']['name']['expose']['label'] = 'Template Name Contains';
  $handler->display->display_options['filters']['name']['expose']['operator'] = 'name_op';
  $handler->display->display_options['filters']['name']['expose']['identifier'] = 'name';
  $handler->display->display_options['filters']['name']['expose']['remember_roles'] = array(
    2 => '2',
    1 => 0,
    3 => 0,
  );
  /* Filter criterion: Tripal Bulk Loader Templates: Created */
  $handler->display->display_options['filters']['created']['id'] = 'created';
  $handler->display->display_options['filters']['created']['table'] = 'tripal_bulk_loader_template';
  $handler->display->display_options['filters']['created']['field'] = 'created';
  $handler->display->display_options['filters']['created']['group'] = 1;
  $handler->display->display_options['filters']['created']['exposed'] = TRUE;
  $handler->display->display_options['filters']['created']['expose']['operator_id'] = 'created_op';
  $handler->display->display_options['filters']['created']['expose']['label'] = 'Date Created';
  $handler->display->display_options['filters']['created']['expose']['operator'] = 'created_op';
  $handler->display->display_options['filters']['created']['expose']['identifier'] = 'created';
  $handler->display->display_options['filters']['created']['expose']['remember_roles'] = array(
    2 => '2',
    1 => 0,
    3 => 0,
  );

  /* Display: Page */
  $handler = $view->new_display('page', 'Page', 'page_1');
  $handler->display->display_options['path'] = 'admin/tripal/loaders/bulk/templates';
  $handler->display->display_options['menu']['type'] = 'tab';
  $handler->display->display_options['menu']['title'] = 'Templates';
  $handler->display->display_options['menu']['weight'] = '0';
  $handler->display->display_options['menu']['name'] = 'management';
  $handler->display->display_options['menu']['context'] = 0;
  $handler->display->display_options['menu']['context_only_inline'] = 0;

  return $view;
}
