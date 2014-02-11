<?php
/**
 * @file
 * Describe db default views
 */

/**
 * Implements hook_views_default_views().
 *
 * @ingroup tripal_db
 */
function tripal_db_views_default_views() {
  $views = array();

  // Default Tripal Admin View: DB
  $view = tripal_db_defaultview_admin_db_listing();
  $view = tripal_views_make_view_compatible_with_external($view);
  $views[$view->name] = $view;

  // Default Tripal Admin View: DB References
  $view = tripal_db_defaultview_admin_dbxref_listing();
  $view = tripal_views_make_view_compatible_with_external($view);
  $views[$view->name] = $view;

  return $views;
}

/**
 * Default Admin View for db management
 *
 * @ingroup tripal_db
 */
function tripal_db_defaultview_admin_db_listing() {

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
  $handler->display->display_options['access']['perm'] = 'access chado_db content';
  $handler->display->display_options['cache']['type'] = 'none';
  $handler->display->display_options['query']['type'] = 'views_query';
  $handler->display->display_options['exposed_form']['type'] = 'basic';
  $handler->display->display_options['pager']['type'] = 'full';
  $handler->display->display_options['pager']['options']['items_per_page'] = '50';
  $handler->display->display_options['pager']['options']['offset'] = '0';
  $handler->display->display_options['pager']['options']['id'] = '0';
  $handler->display->display_options['pager']['options']['quantity'] = '9';
  $handler->display->display_options['style_plugin'] = 'table';
  $handler->display->display_options['style_options']['grouping'] = '';
  $handler->display->display_options['style_options']['columns'] = array(
    'name' => 'name',
    'url' => 'url',
    'urlprefix' => 'urlprefix',
    'description' => 'description',
  );
  $handler->display->display_options['style_options']['default'] = 'name';
  $handler->display->display_options['style_options']['info'] = array(
    'name' => array(
      'sortable' => 1,
      'separator' => '',
    ),
    'url' => array(
      'sortable' => 1,
      'separator' => '',
    ),
    'urlprefix' => array(
      'sortable' => 0,
      'separator' => '',
    ),
    'description' => array(
      'sortable' => 0,
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
    'label-1' => 'Add Database',
    'path-1' => 'admin/tripal/chado/tripal_db/add',
  );
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
  /* Field: Chado Db: Urlprefix */
  $handler->display->display_options['fields']['urlprefix']['id'] = 'urlprefix';
  $handler->display->display_options['fields']['urlprefix']['table'] = 'db';
  $handler->display->display_options['fields']['urlprefix']['field'] = 'urlprefix';
  $handler->display->display_options['fields']['urlprefix']['exclude'] = TRUE;
  /* Field: Chado Db: Url */
  $handler->display->display_options['fields']['url']['id'] = 'url';
  $handler->display->display_options['fields']['url']['table'] = 'db';
  $handler->display->display_options['fields']['url']['field'] = 'url';
  $handler->display->display_options['fields']['url']['alter']['alter_text'] = TRUE;
  $handler->display->display_options['fields']['url']['alter']['text'] = '[url]<br />[urlprefix]';
  $handler->display->display_options['fields']['url']['alter']['make_link'] = TRUE;
  $handler->display->display_options['fields']['url']['alter']['path'] = '[url]';
  $handler->display->display_options['fields']['url']['alter']['absolute'] = TRUE;
  $handler->display->display_options['fields']['url']['alter']['target'] = '_blank';
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
  $handler->display->display_options['fields']['nothing']['alter']['path'] = 'admin/tripal/chado/tripal_db/edit/[db_id]';
  /* Field: Global: Custom text */
  $handler->display->display_options['fields']['nothing_1']['id'] = 'nothing_1';
  $handler->display->display_options['fields']['nothing_1']['table'] = 'views';
  $handler->display->display_options['fields']['nothing_1']['field'] = 'nothing';
  $handler->display->display_options['fields']['nothing_1']['label'] = 'View DBxrefs';
  $handler->display->display_options['fields']['nothing_1']['exclude'] = TRUE;
  $handler->display->display_options['fields']['nothing_1']['alter']['text'] = 'References';
  $handler->display->display_options['fields']['nothing_1']['alter']['make_link'] = TRUE;
  $handler->display->display_options['fields']['nothing_1']['alter']['path'] = 'admin/tripal/chado/tripal_db/dbxrefs?db_name=[name]';
  /* Field: Global: Custom text */
  $handler->display->display_options['fields']['nothing_2']['id'] = 'nothing_2';
  $handler->display->display_options['fields']['nothing_2']['table'] = 'views';
  $handler->display->display_options['fields']['nothing_2']['field'] = 'nothing';
  $handler->display->display_options['fields']['nothing_2']['label'] = '';
  $handler->display->display_options['fields']['nothing_2']['alter']['text'] = '[nothing]<br />
    [nothing_1]';
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
  $handler->display->display_options['filters']['name']['expose']['values_form_type'] = 'textfield';
  $handler->display->display_options['filters']['name']['expose']['select_multiple'] = FALSE;
  $handler->display->display_options['filters']['name']['expose']['select_optional'] = FALSE;
  $handler->display->display_options['filters']['name']['expose']['max_length'] = 40;
  $handler->display->display_options['filters']['name']['expose']['remember_roles'] = array(
    2 => '2',
    1 => 0,
    3 => 0,
  );
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
  $handler->display->display_options['filters']['description']['expose']['remember_roles'] = array(
    2 => '2',
    1 => 0,
    3 => 0,
  );

  /* Display: Page */
  $handler = $view->new_display('page', 'Page', 'page_1');
  $handler->display->display_options['path'] = 'admin/tripal/chado/tripal_db/dbs';
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
function tripal_db_defaultview_admin_dbxref_listing() {

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
  $handler->display->display_options['exposed_form']['type'] = 'basic';
  $handler->display->display_options['pager']['type'] = 'full';
  $handler->display->display_options['pager']['options']['items_per_page'] = '50';
  $handler->display->display_options['style_plugin'] = 'table';
  $handler->display->display_options['style_options']['columns'] = array(
    'dbxref_id' => 'dbxref_id',
    'name' => 'name',
    'accession' => 'accession',
    'description' => 'description',
    'version' => 'version',
  );
  $handler->display->display_options['style_options']['default'] = '-1';
  $handler->display->display_options['style_options']['info'] = array(
    'dbxref_id' => array(
      'sortable' => 0,
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
    'accession' => array(
      'sortable' => 1,
      'default_sort_order' => 'asc',
      'align' => '',
      'separator' => '',
      'empty_column' => 0,
    ),
    'description' => array(
      'sortable' => 0,
      'default_sort_order' => 'asc',
      'align' => '',
      'separator' => '',
      'empty_column' => 0,
    ),
    'version' => array(
      'sortable' => 1,
      'default_sort_order' => 'asc',
      'align' => '',
      'separator' => '',
      'empty_column' => 0,
    ),
  );
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
  $handler->display->display_options['filters']['name']['value'] = array(
    'All' => 'All',
  );
  $handler->display->display_options['filters']['name']['group'] = 1;
  $handler->display->display_options['filters']['name']['exposed'] = TRUE;
  $handler->display->display_options['filters']['name']['expose']['operator_id'] = '';
  $handler->display->display_options['filters']['name']['expose']['label'] = 'Database';
  $handler->display->display_options['filters']['name']['expose']['identifier'] = 'db_name';
  $handler->display->display_options['filters']['name']['expose']['values_form_type'] = 'select';
  $handler->display->display_options['filters']['name']['expose']['select_multiple'] = TRUE;
  $handler->display->display_options['filters']['name']['expose']['select_optional'] = TRUE;
  $handler->display->display_options['filters']['name']['expose']['max_length'] = 40;
  $handler->display->display_options['filters']['name']['expose']['remember_roles'] = array(
    2 => '2',
    1 => 0,
    3 => 0,
  );
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
  $handler->display->display_options['filters']['accession']['expose']['remember_roles'] = array(
    2 => '2',
    1 => 0,
    3 => 0,
  );
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
  $handler->display->display_options['filters']['description']['expose']['remember_roles'] = array(
    2 => '2',
    1 => 0,
    3 => 0,
  );

  /* Display: Page */
  $handler = $view->new_display('page', 'Page', 'page');
  $handler->display->display_options['path'] = 'admin/tripal/chado/tripal_db/dbxrefs';
  $handler->display->display_options['menu']['type'] = 'tab';
  $handler->display->display_options['menu']['title'] = 'References';
  $handler->display->display_options['menu']['weight'] = '0';
  $handler->display->display_options['menu']['name'] = 'management';
  $handler->display->display_options['menu']['context'] = 0;
  $handler->display->display_options['menu']['context_only_inline'] = 0;

  return $view;
}
