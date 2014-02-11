<?php
/**
 * @file
 * Describe default genetic views
 */

/**
 * Implements hook_views_default_views().
 *
 * @ingroup tripal_genetic
 */
function tripal_genetic_views_default_views() {
  $views = array();

  $view = tripal_genetic_defaultviews_admin_genotypes();
  $view = tripal_views_make_view_compatible_with_external($view);
  $views[$view->name] = $view;

  return $views;
}

/**
 * Describes the default administration genetic view
 *
 * @ingroup tripal_genetic
 */
function tripal_genetic_defaultviews_admin_genotypes() {

  $view = new view();
  $view->name = 'tripal_genetic_admin_genetics';
  $view->description = 'DO NOT DISABLE';
  $view->tag = 'tripal admin';
  $view->base_table = 'genotype';
  $view->human_name = 'Genotype Admin';
  $view->core = 7;
  $view->api_version = '3.0';
  $view->disabled = FALSE; /* Edit this to true to make a default view disabled initially */

  /* Display: Defaults */
  $handler = $view->new_display('default', 'Defaults', 'default');
  $handler->display->display_options['title'] = 'Genotypes Observed';
  $handler->display->display_options['use_more_always'] = FALSE;
  $handler->display->display_options['access']['type'] = 'perm';
  $handler->display->display_options['cache']['type'] = 'none';
  $handler->display->display_options['query']['type'] = 'views_query';
  $handler->display->display_options['exposed_form']['type'] = 'basic';
  $handler->display->display_options['exposed_form']['options']['submit_button'] = 'Filter';
  $handler->display->display_options['pager']['type'] = 'full';
  $handler->display->display_options['pager']['options']['items_per_page'] = '25';
  $handler->display->display_options['pager']['options']['offset'] = '0';
  $handler->display->display_options['pager']['options']['id'] = '0';
  $handler->display->display_options['pager']['options']['quantity'] = '9';
  $handler->display->display_options['style_plugin'] = 'table';
  $handler->display->display_options['style_options']['grouping'] = '';
  $handler->display->display_options['style_options']['columns'] = array(
    'uniquename' => 'uniquename',
    'name' => 'name',
    'description' => 'description',
  );
  $handler->display->display_options['style_options']['default'] = 'uniquename';
  $handler->display->display_options['style_options']['info'] = array(
    'uniquename' => array(
      'sortable' => 1,
      'separator' => '',
    ),
    'name' => array(
      'sortable' => 1,
      'separator' => '',
    ),
    'description' => array(
      'sortable' => 1,
      'separator' => '',
    ),
  );
  /* No results behavior: Global: Text area */
  $handler->display->display_options['empty']['text']['id'] = 'area';
  $handler->display->display_options['empty']['text']['table'] = 'views';
  $handler->display->display_options['empty']['text']['field'] = 'area';
  $handler->display->display_options['empty']['text']['content'] = 'No genotypes match the current criteria.';
  $handler->display->display_options['empty']['text']['format'] = '2';
  /* Field: Chado Genotype: Uniquename */
  $handler->display->display_options['fields']['uniquename']['id'] = 'uniquename';
  $handler->display->display_options['fields']['uniquename']['table'] = 'genotype';
  $handler->display->display_options['fields']['uniquename']['field'] = 'uniquename';
  /* Field: Chado Genotype: Name */
  $handler->display->display_options['fields']['name']['id'] = 'name';
  $handler->display->display_options['fields']['name']['table'] = 'genotype';
  $handler->display->display_options['fields']['name']['field'] = 'name';
  /* Field: Chado Genotype: Description */
  $handler->display->display_options['fields']['description']['id'] = 'description';
  $handler->display->display_options['fields']['description']['table'] = 'genotype';
  $handler->display->display_options['fields']['description']['field'] = 'description';
  /* Filter criterion: Chado Genotype: Uniquename */
  $handler->display->display_options['filters']['uniquename']['id'] = 'uniquename';
  $handler->display->display_options['filters']['uniquename']['table'] = 'genotype';
  $handler->display->display_options['filters']['uniquename']['field'] = 'uniquename';
  $handler->display->display_options['filters']['uniquename']['operator'] = 'contains';
  $handler->display->display_options['filters']['uniquename']['group'] = '0';
  $handler->display->display_options['filters']['uniquename']['exposed'] = TRUE;
  $handler->display->display_options['filters']['uniquename']['expose']['operator_id'] = 'uniquename_op';
  $handler->display->display_options['filters']['uniquename']['expose']['label'] = 'Unique Name Contains';
  $handler->display->display_options['filters']['uniquename']['expose']['operator'] = 'uniquename_op';
  $handler->display->display_options['filters']['uniquename']['expose']['identifier'] = 'uniquename';
  $handler->display->display_options['filters']['uniquename']['expose']['remember_roles'] = array(
    2 => '2',
    1 => 0,
    3 => 0,
  );
  /* Filter criterion: Chado Genotype: Description */
  $handler->display->display_options['filters']['description']['id'] = 'description';
  $handler->display->display_options['filters']['description']['table'] = 'genotype';
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
  $handler->display->display_options['path'] = 'admin/tripal/chado/tripal_genetic/genotypes';
  $handler->display->display_options['menu']['type'] = 'default tab';
  $handler->display->display_options['menu']['title'] = 'Genotypes';
  $handler->display->display_options['menu']['description'] = 'A genotype is defined by a collection of features, mutations, etc.';
  $handler->display->display_options['menu']['weight'] = '-10';
  $handler->display->display_options['menu']['context'] = 0;
  $handler->display->display_options['menu']['context_only_inline'] = 0;
  $handler->display->display_options['tab_options']['weight'] = '0';

  return $view;
}
