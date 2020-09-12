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
  $views = [];

  // User View ("Search Biological Content")
  // Remember, if you change the name/path of this view,
  // you also want to change it's description in tripal_genetic_search_biological_data_views()
  $view = tripal_genetic_defaultvalue_user_genotypes();
  $view = tripal_make_view_compatible_with_external($view);
  $views[$view->name] = $view;

  // Admin view
  $view = tripal_genetic_defaultviews_admin_genotypes();
  $view = tripal_make_view_compatible_with_external($view);
  $views[$view->name] = $view;

  return $views;
}

/**
 * Defines the genetic user search default view.
 *
 * @ingroup tripal_genetic
 */
function tripal_genetic_defaultvalue_user_genotypes() {

  // As a shortcut, currently I am copying the admin view and then tweaking it
  // In the future this should be replaced by a views export
  $view = tripal_genetic_defaultviews_admin_genotypes();

  // change some view admin stuff
  $view->name = 'tripal_genetic_user_genotypes';
  $view->human_name = 'Genotype User Search';
  $view->description = 'Allows users to search genotype data';
  $view->tag = 'tripal search';

  // Change the path
  $view->display['page_1']->display_options['path'] = 'chado/genotype';
  $view->display['page_1']->display_options['menu'] = [
    'type' => 'normal',
    'title' => 'Genotypes',
    'description' => 'Genetic variations such as SNPs, MNPs and indels.',
    'weight' => '-10',
  ];
  unset($view->display['page_1']->display_options['tab_options']);

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
  $handler->display->display_options['exposed_form']['type'] = 'input_required';
  $handler->display->display_options['exposed_form']['options']['submit_button'] = 'Search';
  $handler->display->display_options['exposed_form']['options']['text_input_required'] = 'Click search to see a listing of genotypes that meet the filter requirements. Use the filters to restrict this set to a more reasonable number of genotypes or to find a specific genotype.';
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
    'name' => 'name',
    'description' => 'description',
  ];
  $handler->display->display_options['style_options']['default'] = 'uniquename';
  $handler->display->display_options['style_options']['info'] = [
    'uniquename' => [
      'sortable' => 1,
      'separator' => '',
    ],
    'name' => [
      'sortable' => 1,
      'separator' => '',
    ],
    'description' => [
      'sortable' => 1,
      'separator' => '',
    ],
  ];
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
  /* Sort criterion: Chado Genotype: Id */
  $handler->display->display_options['sorts']['genotype_id']['id'] = 'genotype_id';
  $handler->display->display_options['sorts']['genotype_id']['table'] = 'genotype';
  $handler->display->display_options['sorts']['genotype_id']['field'] = 'genotype_id';
  $handler->display->display_options['sorts']['genotype_id']['order'] = 'DESC';
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
  $handler->display->display_options['filters']['uniquename']['expose']['remember_roles'] = [
    2 => '2',
    1 => 0,
    3 => 0,
  ];
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
  $handler->display->display_options['filters']['description']['expose']['remember_roles'] = [
    2 => '2',
    1 => 0,
    3 => 0,
  ];

  /* Display: Page */
  $handler = $view->new_display('page', 'Page', 'page_1');
  $handler->display->display_options['path'] = 'admin/tripal/legacy/tripal_genetic/genotypes';
  $handler->display->display_options['menu']['type'] = 'default tab';
  $handler->display->display_options['menu']['title'] = 'Genotypes';
  $handler->display->display_options['menu']['description'] = 'A genotype is defined by a collection of features, mutations, etc.';
  $handler->display->display_options['menu']['weight'] = '-10';
  $handler->display->display_options['menu']['context'] = 0;
  $handler->display->display_options['menu']['context_only_inline'] = 0;
  $handler->display->display_options['tab_options']['weight'] = '0';

  return $view;
}
