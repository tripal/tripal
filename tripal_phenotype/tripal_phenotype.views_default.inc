<?php
/**
 * @file
 * Describe phenotype default views
 */

/**
 * Implements hook_views_default_views().
 *
 * @ingroup tripal_phenotype
 */
function tripal_phenotype_views_default_views() {
  $views = array();

  $view = tripal_phenotype_defaultview_admin_phenotypes();
  $view = tripal_views_make_view_compatible_with_external($view);
  $views[$view->name] = $view;

  return $views;
}

/**
 * Default phenotype administration view
 *
 * @ingroup tripal_phenotype
 */
function tripal_phenotype_defaultview_admin_phenotypes() {

  $view = new view();
  $view->name = 'tripal_phenotype_admin_phenotypes';
  $view->description = 'DO NOT DISABLE';
  $view->tag = 'tripal admin';
  $view->base_table = 'phenotype';
  $view->human_name = 'Phenotype Admin';
  $view->core = 7;
  $view->api_version = '3.0';
  $view->disabled = FALSE; /* Edit this to true to make a default view disabled initially */

  /* Display: Defaults */
  $handler = $view->new_display('default', 'Defaults', 'default');
  $handler->display->display_options['title'] = 'Phenotypes';
  $handler->display->display_options['use_more_always'] = FALSE;
  $handler->display->display_options['access']['type'] = 'perm';
  $handler->display->display_options['access']['perm'] = 'administer tripal';
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
    'value' => 'value',
  );
  $handler->display->display_options['style_options']['default'] = 'uniquename';
  $handler->display->display_options['style_options']['info'] = array(
    'uniquename' => array(
      'sortable' => 1,
      'separator' => '',
    ),
    'value' => array(
      'sortable' => 1,
      'separator' => '',
    ),
  );
  /* No results behavior: Global: Text area */
  $handler->display->display_options['empty']['text']['id'] = 'area';
  $handler->display->display_options['empty']['text']['table'] = 'views';
  $handler->display->display_options['empty']['text']['field'] = 'area';
  $handler->display->display_options['empty']['text']['content'] = 'No phenotypes match the supplied criteria.';
  $handler->display->display_options['empty']['text']['format'] = '2';
  /* Relationship: Chado Phenotype: observable_id => Cvterm */
  $handler->display->display_options['relationships']['observable_id_to_cvterm']['id'] = 'observable_id_to_cvterm';
  $handler->display->display_options['relationships']['observable_id_to_cvterm']['table'] = 'phenotype';
  $handler->display->display_options['relationships']['observable_id_to_cvterm']['field'] = 'observable_id_to_cvterm';
  $handler->display->display_options['relationships']['observable_id_to_cvterm']['label'] = 'Observable ID  => Cvterm';
  /* Relationship: Chado Phenotype: attr_id => Cvterm */
  $handler->display->display_options['relationships']['attr_id_to_cvterm']['id'] = 'attr_id_to_cvterm';
  $handler->display->display_options['relationships']['attr_id_to_cvterm']['table'] = 'phenotype';
  $handler->display->display_options['relationships']['attr_id_to_cvterm']['field'] = 'attr_id_to_cvterm';
  $handler->display->display_options['relationships']['attr_id_to_cvterm']['label'] = 'Attribute id => Cvterm';
  /* Relationship: Chado Phenotype: cvalue_id => Cvterm */
  $handler->display->display_options['relationships']['cvalue_id_to_cvterm']['id'] = 'cvalue_id_to_cvterm';
  $handler->display->display_options['relationships']['cvalue_id_to_cvterm']['table'] = 'phenotype';
  $handler->display->display_options['relationships']['cvalue_id_to_cvterm']['field'] = 'cvalue_id_to_cvterm';
  $handler->display->display_options['relationships']['cvalue_id_to_cvterm']['label'] = 'cvalue id => Cvterm';
  /* Relationship: Chado Phenotype: assay_id => Cvterm */
  $handler->display->display_options['relationships']['assay_id_to_cvterm']['id'] = 'assay_id_to_cvterm';
  $handler->display->display_options['relationships']['assay_id_to_cvterm']['table'] = 'phenotype';
  $handler->display->display_options['relationships']['assay_id_to_cvterm']['field'] = 'assay_id_to_cvterm';
  $handler->display->display_options['relationships']['assay_id_to_cvterm']['label'] = 'assay id => Cvterm';
  /* Field: Chado Phenotype: Uniquename */
  $handler->display->display_options['fields']['uniquename']['id'] = 'uniquename';
  $handler->display->display_options['fields']['uniquename']['table'] = 'phenotype';
  $handler->display->display_options['fields']['uniquename']['field'] = 'uniquename';
  $handler->display->display_options['fields']['uniquename']['label'] = 'Unique Name';
  /* Field: Chado Cvterm: Name */
  $handler->display->display_options['fields']['name_2']['id'] = 'name_2';
  $handler->display->display_options['fields']['name_2']['table'] = 'cvterm';
  $handler->display->display_options['fields']['name_2']['field'] = 'name';
  $handler->display->display_options['fields']['name_2']['relationship'] = 'observable_id_to_cvterm';
  $handler->display->display_options['fields']['name_2']['label'] = 'Observed Unit';
  /* Field: Chado Cvterm: Name */
  $handler->display->display_options['fields']['name_1']['id'] = 'name_1';
  $handler->display->display_options['fields']['name_1']['table'] = 'cvterm';
  $handler->display->display_options['fields']['name_1']['field'] = 'name';
  $handler->display->display_options['fields']['name_1']['relationship'] = 'attr_id_to_cvterm';
  $handler->display->display_options['fields']['name_1']['label'] = 'Phenotypic Attribute';
  /* Field: Chado Cvterm: Name */
  $handler->display->display_options['fields']['name_3']['id'] = 'name_3';
  $handler->display->display_options['fields']['name_3']['table'] = 'cvterm';
  $handler->display->display_options['fields']['name_3']['field'] = 'name';
  $handler->display->display_options['fields']['name_3']['relationship'] = 'cvalue_id_to_cvterm';
  $handler->display->display_options['fields']['name_3']['label'] = 'Controlled Value';
  /* Field: Chado Phenotype: Value */
  $handler->display->display_options['fields']['value']['id'] = 'value';
  $handler->display->display_options['fields']['value']['table'] = 'phenotype';
  $handler->display->display_options['fields']['value']['field'] = 'value';
  /* Field: Chado Cvterm: Name */
  $handler->display->display_options['fields']['name']['id'] = 'name';
  $handler->display->display_options['fields']['name']['table'] = 'cvterm';
  $handler->display->display_options['fields']['name']['field'] = 'name';
  $handler->display->display_options['fields']['name']['relationship'] = 'assay_id_to_cvterm';
  $handler->display->display_options['fields']['name']['label'] = 'Evidence Type';
  /* Filter criterion: Chado Phenotype: Attr Id */
  $handler->display->display_options['filters']['attr_id']['id'] = 'attr_id';
  $handler->display->display_options['filters']['attr_id']['table'] = 'phenotype';
  $handler->display->display_options['filters']['attr_id']['field'] = 'attr_id';
  $handler->display->display_options['filters']['attr_id']['group'] = '0';
  $handler->display->display_options['filters']['attr_id']['exposed'] = TRUE;
  $handler->display->display_options['filters']['attr_id']['expose']['operator_id'] = 'attr_id_op';
  $handler->display->display_options['filters']['attr_id']['expose']['label'] = 'Phenotypic Attribute';
  $handler->display->display_options['filters']['attr_id']['expose']['operator'] = 'attr_id_op';
  $handler->display->display_options['filters']['attr_id']['expose']['identifier'] = 'attr_id';
  /* Filter criterion: Chado Phenotype: Cvalue Id */
  $handler->display->display_options['filters']['cvalue_id']['id'] = 'cvalue_id';
  $handler->display->display_options['filters']['cvalue_id']['table'] = 'phenotype';
  $handler->display->display_options['filters']['cvalue_id']['field'] = 'cvalue_id';
  $handler->display->display_options['filters']['cvalue_id']['group'] = '0';
  $handler->display->display_options['filters']['cvalue_id']['exposed'] = TRUE;
  $handler->display->display_options['filters']['cvalue_id']['expose']['operator_id'] = 'cvalue_id_op';
  $handler->display->display_options['filters']['cvalue_id']['expose']['label'] = 'Controlled Value';
  $handler->display->display_options['filters']['cvalue_id']['expose']['operator'] = 'cvalue_id_op';
  $handler->display->display_options['filters']['cvalue_id']['expose']['identifier'] = 'cvalue_id';
  /* Filter criterion: Chado Phenotype: Observable Id */
  $handler->display->display_options['filters']['observable_id']['id'] = 'observable_id';
  $handler->display->display_options['filters']['observable_id']['table'] = 'phenotype';
  $handler->display->display_options['filters']['observable_id']['field'] = 'observable_id';
  $handler->display->display_options['filters']['observable_id']['group'] = '0';
  $handler->display->display_options['filters']['observable_id']['exposed'] = TRUE;
  $handler->display->display_options['filters']['observable_id']['expose']['operator_id'] = 'observable_id_op';
  $handler->display->display_options['filters']['observable_id']['expose']['label'] = 'Observation Type';
  $handler->display->display_options['filters']['observable_id']['expose']['operator'] = 'observable_id_op';
  $handler->display->display_options['filters']['observable_id']['expose']['identifier'] = 'observable_id';
  /* Filter criterion: Chado Phenotype: Assay Id */
  $handler->display->display_options['filters']['assay_id']['id'] = 'assay_id';
  $handler->display->display_options['filters']['assay_id']['table'] = 'phenotype';
  $handler->display->display_options['filters']['assay_id']['field'] = 'assay_id';
  $handler->display->display_options['filters']['assay_id']['group'] = '0';
  $handler->display->display_options['filters']['assay_id']['exposed'] = TRUE;
  $handler->display->display_options['filters']['assay_id']['expose']['operator_id'] = 'assay_id_op';
  $handler->display->display_options['filters']['assay_id']['expose']['label'] = 'Evidence Type';
  $handler->display->display_options['filters']['assay_id']['expose']['operator'] = 'assay_id_op';
  $handler->display->display_options['filters']['assay_id']['expose']['identifier'] = 'assay_id';
  /* Filter criterion: Chado Phenotype: Uniquename */
  $handler->display->display_options['filters']['uniquename']['id'] = 'uniquename';
  $handler->display->display_options['filters']['uniquename']['table'] = 'phenotype';
  $handler->display->display_options['filters']['uniquename']['field'] = 'uniquename';
  $handler->display->display_options['filters']['uniquename']['operator'] = '~';
  $handler->display->display_options['filters']['uniquename']['group'] = '0';
  $handler->display->display_options['filters']['uniquename']['exposed'] = TRUE;
  $handler->display->display_options['filters']['uniquename']['expose']['operator_id'] = 'uniquename_op';
  $handler->display->display_options['filters']['uniquename']['expose']['label'] = 'Unique Name Contains';
  $handler->display->display_options['filters']['uniquename']['expose']['operator'] = 'uniquename_op';
  $handler->display->display_options['filters']['uniquename']['expose']['identifier'] = 'uniquename';
  /* Filter criterion: Chado Phenotype: Value */
  $handler->display->display_options['filters']['value']['id'] = 'value';
  $handler->display->display_options['filters']['value']['table'] = 'phenotype';
  $handler->display->display_options['filters']['value']['field'] = 'value';
  $handler->display->display_options['filters']['value']['operator'] = 'contains';
  $handler->display->display_options['filters']['value']['group'] = '0';
  $handler->display->display_options['filters']['value']['exposed'] = TRUE;
  $handler->display->display_options['filters']['value']['expose']['operator_id'] = 'value_op';
  $handler->display->display_options['filters']['value']['expose']['label'] = 'Value Contains';
  $handler->display->display_options['filters']['value']['expose']['operator'] = 'value_op';
  $handler->display->display_options['filters']['value']['expose']['identifier'] = 'phen_value';
  $handler->display->display_options['filters']['value']['expose']['remember_roles'] = array(
    2 => '2',
    1 => 0,
    3 => 0,
  );

  /* Display: Page */
  $handler = $view->new_display('page', 'Page', 'page_1');
  $handler->display->display_options['path'] = 'admin/tripal/chado/tripal_phenotype/phenotypes';
  $handler->display->display_options['menu']['type'] = 'default tab';
  $handler->display->display_options['menu']['title'] = 'Phenotypes';
  $handler->display->display_options['menu']['description'] = 'A controlled sentence describing observable effects of non-wild type function';
  $handler->display->display_options['menu']['weight'] = '-10';
  $handler->display->display_options['menu']['context'] = 0;
  $handler->display->display_options['menu']['context_only_inline'] = 0;
  $handler->display->display_options['tab_options']['weight'] = '0';

  return $view;
}
