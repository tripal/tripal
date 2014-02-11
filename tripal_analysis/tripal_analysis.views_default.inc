<?php

/**
 * @file
 * Contains the default views for the analysis module.
 *
 * @ingroup tripal_analysis
 */

/**
 * Implements hook_views_default_views().
 *
 * @ingroup tripal_analysis
 */
function tripal_analysis_views_default_views() {
  $views = array();

  $view = tripal_analysis_defaultvalue_admin_analysis();
  $view = tripal_views_make_view_compatible_with_external($view);
  $views[$view->name] = $view;

  return $views;
}

/**
 * Defines the analysis admin default view.
 *
 * @ingroup tripal_analysis
 */
function tripal_analysis_defaultvalue_admin_analysis() {

  $view = new view();
  $view->name = 'tripal_analysis_admin_analyses';
  $view->description = 'DO NOT DISABLE';
  $view->tag = 'tripal admin';
  $view->base_table = 'analysis';
  $view->human_name = 'Analysis Admin';
  $view->core = 0;
  $view->api_version = '3.0';
  $view->disabled = FALSE; /* Edit this to true to make a default view disabled initially */

  /* Display: Defaults */
  $handler = $view->new_display('default', 'Defaults', 'default');
  $handler->display->display_options['title'] = 'Analysis';
  $handler->display->display_options['use_more_always'] = FALSE;
  $handler->display->display_options['access']['type'] = 'perm';
  $handler->display->display_options['access']['perm'] = 'access chado_analysis content';
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
    'name' => 'name',
    'program' => 'program',
    'sourcename' => 'sourcename',
    'num_features' => 'num_features',
    'timeexecuted' => 'timeexecuted',
  );
  $handler->display->display_options['style_options']['default'] = 'name';
  $handler->display->display_options['style_options']['info'] = array(
    'name' => array(
      'sortable' => 1,
      'separator' => '',
    ),
    'program' => array(
      'sortable' => 1,
      'separator' => '',
    ),
    'sourcename' => array(
      'sortable' => 1,
      'separator' => '',
    ),
    'num_features' => array(
      'separator' => '',
    ),
    'timeexecuted' => array(
      'sortable' => 1,
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
    'label-1' => 'Add Analysis',
    'path-1' => 'node/add/tripal-analysis',
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
  $handler->display->display_options['empty']['text']['id'] = 'text';
  $handler->display->display_options['empty']['text']['table'] = 'views';
  $handler->display->display_options['empty']['text']['field'] = 'area';
  $handler->display->display_options['empty']['text']['empty'] = TRUE;
  $handler->display->display_options['empty']['text']['content'] = 'No analyses match the supplied criteria.';
  $handler->display->display_options['empty']['text']['format'] = 'full_html';
  /* Relationship: Analysis: Analysis => Node */
  $handler->display->display_options['relationships']['analysis_id']['id'] = 'analysis_id';
  $handler->display->display_options['relationships']['analysis_id']['table'] = 'chado_analysis';
  $handler->display->display_options['relationships']['analysis_id']['field'] = 'analysis_id';
  /* Field: Content: Nid */
  $handler->display->display_options['fields']['nid']['id'] = 'nid';
  $handler->display->display_options['fields']['nid']['table'] = 'node';
  $handler->display->display_options['fields']['nid']['field'] = 'nid';
  $handler->display->display_options['fields']['nid']['relationship'] = 'analysis_id';
  $handler->display->display_options['fields']['nid']['exclude'] = TRUE;
  /* Field: Chado Analysis: Name */
  $handler->display->display_options['fields']['name']['id'] = 'name';
  $handler->display->display_options['fields']['name']['table'] = 'analysis';
  $handler->display->display_options['fields']['name']['field'] = 'name';
  /* Field: Chado Analysis: Program */
  $handler->display->display_options['fields']['program']['id'] = 'program';
  $handler->display->display_options['fields']['program']['table'] = 'analysis';
  $handler->display->display_options['fields']['program']['field'] = 'program';
  /* Field: Chado Analysis: Sourcename */
  $handler->display->display_options['fields']['sourcename']['id'] = 'sourcename';
  $handler->display->display_options['fields']['sourcename']['table'] = 'analysis';
  $handler->display->display_options['fields']['sourcename']['field'] = 'sourcename';
  $handler->display->display_options['fields']['sourcename']['label'] = 'Source';
  /* Field: Content: Delete link */
  $handler->display->display_options['fields']['delete_node']['id'] = 'delete_node';
  $handler->display->display_options['fields']['delete_node']['table'] = 'views_entity_node';
  $handler->display->display_options['fields']['delete_node']['field'] = 'delete_node';
  $handler->display->display_options['fields']['delete_node']['relationship'] = 'analysis_id';
  $handler->display->display_options['fields']['delete_node']['exclude'] = TRUE;
  /* Field: Content: Edit link */
  $handler->display->display_options['fields']['edit_node']['id'] = 'edit_node';
  $handler->display->display_options['fields']['edit_node']['table'] = 'views_entity_node';
  $handler->display->display_options['fields']['edit_node']['field'] = 'edit_node';
  $handler->display->display_options['fields']['edit_node']['relationship'] = 'analysis_id';
  $handler->display->display_options['fields']['edit_node']['exclude'] = TRUE;
  /* Field: Global: Custom text */
  $handler->display->display_options['fields']['nothing']['id'] = 'nothing';
  $handler->display->display_options['fields']['nothing']['table'] = 'views';
  $handler->display->display_options['fields']['nothing']['field'] = 'nothing';
  $handler->display->display_options['fields']['nothing']['label'] = '';
  $handler->display->display_options['fields']['nothing']['alter']['text'] = '[edit_node]   [delete_node]';
  $handler->display->display_options['fields']['nothing']['element_label_colon'] = FALSE;
  /* Filter criterion: Chado Analysis: Name */
  $handler->display->display_options['filters']['name']['id'] = 'name';
  $handler->display->display_options['filters']['name']['table'] = 'analysis';
  $handler->display->display_options['filters']['name']['field'] = 'name';
  $handler->display->display_options['filters']['name']['operator'] = '~';
  $handler->display->display_options['filters']['name']['group'] = 1;
  $handler->display->display_options['filters']['name']['exposed'] = TRUE;
  $handler->display->display_options['filters']['name']['expose']['operator_id'] = 'name_op';
  $handler->display->display_options['filters']['name']['expose']['label'] = 'Name Contains';
  $handler->display->display_options['filters']['name']['expose']['operator'] = 'name_op';
  $handler->display->display_options['filters']['name']['expose']['identifier'] = 'name';
  /* Filter criterion: Chado Analysis: Program */
  $handler->display->display_options['filters']['program']['id'] = 'program';
  $handler->display->display_options['filters']['program']['table'] = 'analysis';
  $handler->display->display_options['filters']['program']['field'] = 'program';
  $handler->display->display_options['filters']['program']['operator'] = 'contains';
  $handler->display->display_options['filters']['program']['group'] = 1;
  $handler->display->display_options['filters']['program']['exposed'] = TRUE;
  $handler->display->display_options['filters']['program']['expose']['operator_id'] = 'program_op';
  $handler->display->display_options['filters']['program']['expose']['label'] = 'Program Name Contains';
  $handler->display->display_options['filters']['program']['expose']['operator'] = 'program_op';
  $handler->display->display_options['filters']['program']['expose']['identifier'] = 'program';
  $handler->display->display_options['filters']['program']['expose']['remember_roles'] = array(
    2 => '2',
    1 => 0,
    3 => 0,
  );
  /* Filter criterion: Chado Analysis: Sourcename */
  $handler->display->display_options['filters']['sourcename']['id'] = 'sourcename';
  $handler->display->display_options['filters']['sourcename']['table'] = 'analysis';
  $handler->display->display_options['filters']['sourcename']['field'] = 'sourcename';
  $handler->display->display_options['filters']['sourcename']['operator'] = '~';
  $handler->display->display_options['filters']['sourcename']['group'] = 1;
  $handler->display->display_options['filters']['sourcename']['exposed'] = TRUE;
  $handler->display->display_options['filters']['sourcename']['expose']['operator_id'] = 'sourcename_op';
  $handler->display->display_options['filters']['sourcename']['expose']['label'] = 'Source Contains';
  $handler->display->display_options['filters']['sourcename']['expose']['operator'] = 'sourcename_op';
  $handler->display->display_options['filters']['sourcename']['expose']['identifier'] = 'sourcename';

  /* Display: Page */
  $handler = $view->new_display('page', 'Page', 'page_1');
  $handler->display->display_options['path'] = 'admin/tripal/chado/tripal_analysis/analysis';
  $handler->display->display_options['menu']['type'] = 'default tab';
  $handler->display->display_options['menu']['title'] = 'Analyses';
  $handler->display->display_options['menu']['weight'] = '-10';
  $handler->display->display_options['menu']['context'] = 0;
  $handler->display->display_options['menu']['context_only_inline'] = 0;
  $handler->display->display_options['tab_options']['weight'] = '0';

  return $view;
}
