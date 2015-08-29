<?php

function tripal_entities_views_default_views() {
  $views = array();

$view = new view();
$view->name = 'tripal_data';
$view->description = '';
$view->tag = 'default';
$view->base_table = 'tripal_data_type';
$view->human_name = 'Tripal Data';
$view->core = 7;
$view->api_version = '3.0';
$view->disabled = FALSE; /* Edit this to true to make a default view disabled initially */

/* Display: Master */
$handler = $view->new_display('default', 'Master', 'default');
$handler->display->display_options['title'] = 'Biological Data';
$handler->display->display_options['use_more_always'] = FALSE;
$handler->display->display_options['access']['type'] = 'perm';
$handler->display->display_options['access']['perm'] = 'administer blocks';
$handler->display->display_options['cache']['type'] = 'none';
$handler->display->display_options['query']['type'] = 'views_query';
$handler->display->display_options['exposed_form']['type'] = 'basic';
$handler->display->display_options['pager']['type'] = 'full';
$handler->display->display_options['style_plugin'] = 'table';
$handler->display->display_options['style_options']['columns'] = array(
  'id' => 'id',
);
$handler->display->display_options['style_options']['default'] = '-1';
$handler->display->display_options['style_options']['info'] = array(
  'id' => array(
    'sortable' => 0,
    'default_sort_order' => 'asc',
    'align' => '',
    'separator' => '',
    'empty_column' => 0,
  ),
);
/* No results behavior: Global: Text area */
$handler->display->display_options['empty']['area']['id'] = 'area';
$handler->display->display_options['empty']['area']['table'] = 'views';
$handler->display->display_options['empty']['area']['field'] = 'area';
$handler->display->display_options['empty']['area']['label'] = 'No Data';
$handler->display->display_options['empty']['area']['empty'] = TRUE;
$handler->display->display_options['empty']['area']['content'] = 'There is currently no data available.';
$handler->display->display_options['empty']['area']['format'] = 'filtered_html';
/* Field: Tripal Data Type: Internal, numeric tripal data type ID */
$handler->display->display_options['fields']['id']['id'] = 'id';
$handler->display->display_options['fields']['id']['table'] = 'tripal_data_type';
$handler->display->display_options['fields']['id']['field'] = 'id';
$handler->display->display_options['fields']['id']['label'] = 'Data ID';
/* Field: Tripal Data Type: Label */
$handler->display->display_options['fields']['label']['id'] = 'label';
$handler->display->display_options['fields']['label']['table'] = 'tripal_data_type';
$handler->display->display_options['fields']['label']['field'] = 'label';
$handler->display->display_options['fields']['label']['label'] = 'Type ID';
/* Field: Tripal Data Type: Machine-readable name */
$handler->display->display_options['fields']['type']['id'] = 'type';
$handler->display->display_options['fields']['type']['table'] = 'tripal_data_type';
$handler->display->display_options['fields']['type']['field'] = 'type';
$handler->display->display_options['fields']['type']['label'] = 'Type Name';
/* Field: Tripal Data Type: Status */
$handler->display->display_options['fields']['status']['id'] = 'status';
$handler->display->display_options['fields']['status']['table'] = 'tripal_data_type';
$handler->display->display_options['fields']['status']['field'] = 'status';
/* Filter criterion: Tripal Data Type: Machine-readable name */
$handler->display->display_options['filters']['type']['id'] = 'type';
$handler->display->display_options['filters']['type']['table'] = 'tripal_data_type';
$handler->display->display_options['filters']['type']['field'] = 'type';
$handler->display->display_options['filters']['type']['exposed'] = TRUE;
$handler->display->display_options['filters']['type']['expose']['operator_id'] = 'type_op';
$handler->display->display_options['filters']['type']['expose']['label'] = 'Type';
$handler->display->display_options['filters']['type']['expose']['operator'] = 'type_op';
$handler->display->display_options['filters']['type']['expose']['identifier'] = 'type';
$handler->display->display_options['filters']['type']['expose']['remember_roles'] = array(
  2 => '2',
  1 => 0,
  3 => 0,
);
$handler->display->display_options['filters']['type']['group_info']['label'] = 'Machine-readable name';
$handler->display->display_options['filters']['type']['group_info']['identifier'] = 'type';
$handler->display->display_options['filters']['type']['group_info']['remember'] = FALSE;
$handler->display->display_options['filters']['type']['group_info']['group_items'] = array(
  1 => array(),
  2 => array(),
  3 => array(),
);

/* Display: Page */
$handler = $view->new_display('page', 'Page', 'data_admin_page');
$handler->display->display_options['path'] = 'admin/content/data/list';
$handler->display->display_options['menu']['type'] = 'default tab';
$handler->display->display_options['menu']['title'] = 'List';
$handler->display->display_options['menu']['weight'] = '-10';
$handler->display->display_options['menu']['context'] = 0;
$handler->display->display_options['menu']['context_only_inline'] = 0;
$handler->display->display_options['tab_options']['weight'] = '0';


  $views[] = $view;
  return $views;
}
