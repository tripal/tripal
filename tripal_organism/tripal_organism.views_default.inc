<?php
/**
 * @file
 * Organism default views
 */

/**
 * Implements hook_views_default_views().
 *
 * @ingroup tripal_organism
 */
function tripal_organism_views_default_views() {
  $views = array();

  $view = tripal_organism_admin_defaultview_organisms();
  $view = tripal_views_make_view_compatible_with_external($view);
  $views[$view->name] = $view;

  return $views;
}

/**
 * Describe the organism administration view.
 *
 * @ingroup tripal_organism
 */
function tripal_organism_admin_defaultview_organisms() {

  $view = new view();
  $view->name = 'tripal_organism_admin_organisms';
  $view->description = 'DO NOT DISABLE';
  $view->tag = 'tripal admin';
  $view->base_table = 'organism';
  $view->human_name = 'Organism Admin';
  $view->core = 0;
  $view->api_version = '3.0';
  $view->disabled = FALSE; /* Edit this to true to make a default view disabled initially */

  /* Display: Defaults */
  $handler = $view->new_display('default', 'Defaults', 'default');
  $handler->display->display_options['title'] = 'Organisms';
  $handler->display->display_options['use_more_always'] = FALSE;
  $handler->display->display_options['access']['type'] = 'perm';
  $handler->display->display_options['access']['perm'] = 'adminster tripal organism';
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
    'common_name' => 'common_name',
    'genus' => 'genus',
    'species' => 'species',
    'abbreviation' => 'abbreviation',
  );
  $handler->display->display_options['style_options']['default'] = '-1';
  $handler->display->display_options['style_options']['info'] = array(
    'common_name' => array(
      'sortable' => 1,
      'separator' => '',
    ),
    'genus' => array(
      'sortable' => 1,
      'separator' => '',
    ),
    'species' => array(
      'sortable' => 1,
      'separator' => '',
    ),
    'abbreviation' => array(
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
    'label-1' => 'Add Organism',
    'path-1' => 'node/add/tripal_organism',
  );
  /* No results behavior: Global: Text area */
  $handler->display->display_options['empty']['text']['id'] = 'area';
  $handler->display->display_options['empty']['text']['table'] = 'views';
  $handler->display->display_options['empty']['text']['field'] = 'area';
  $handler->display->display_options['empty']['text']['content'] = 'No organisms matched the supplied criteria.';
  $handler->display->display_options['empty']['text']['format'] = '1';
  /* Relationship: Organism: Organism => Node */
  $handler->display->display_options['relationships']['organism_id']['id'] = 'organism_id';
  $handler->display->display_options['relationships']['organism_id']['table'] = 'chado_organism';
  $handler->display->display_options['relationships']['organism_id']['field'] = 'organism_id';
  /* Field: Content: Nid */
  $handler->display->display_options['fields']['nid']['id'] = 'nid';
  $handler->display->display_options['fields']['nid']['table'] = 'node';
  $handler->display->display_options['fields']['nid']['field'] = 'nid';
  $handler->display->display_options['fields']['nid']['exclude'] = TRUE;
  /* Field: Chado Organism: Common Name */
  $handler->display->display_options['fields']['common_name']['id'] = 'common_name';
  $handler->display->display_options['fields']['common_name']['table'] = 'organism';
  $handler->display->display_options['fields']['common_name']['field'] = 'common_name';
  $handler->display->display_options['fields']['common_name']['alter']['make_link'] = TRUE;
  $handler->display->display_options['fields']['common_name']['alter']['path'] = 'node/[nid]';
  /* Field: Chado Organism: Genus */
  $handler->display->display_options['fields']['genus']['id'] = 'genus';
  $handler->display->display_options['fields']['genus']['table'] = 'organism';
  $handler->display->display_options['fields']['genus']['field'] = 'genus';
  /* Field: Chado Organism: Species */
  $handler->display->display_options['fields']['species']['id'] = 'species';
  $handler->display->display_options['fields']['species']['table'] = 'organism';
  $handler->display->display_options['fields']['species']['field'] = 'species';
  /* Field: Chado Organism: Abbreviation */
  $handler->display->display_options['fields']['abbreviation']['id'] = 'abbreviation';
  $handler->display->display_options['fields']['abbreviation']['table'] = 'organism';
  $handler->display->display_options['fields']['abbreviation']['field'] = 'abbreviation';
  /* Field: Content: Delete link */
  $handler->display->display_options['fields']['delete_node']['id'] = 'delete_node';
  $handler->display->display_options['fields']['delete_node']['table'] = 'views_entity_node';
  $handler->display->display_options['fields']['delete_node']['field'] = 'delete_node';
  $handler->display->display_options['fields']['delete_node']['relationship'] = 'organism_id';
  $handler->display->display_options['fields']['delete_node']['exclude'] = TRUE;
  /* Field: Content: Edit link */
  $handler->display->display_options['fields']['edit_node']['id'] = 'edit_node';
  $handler->display->display_options['fields']['edit_node']['table'] = 'views_entity_node';
  $handler->display->display_options['fields']['edit_node']['field'] = 'edit_node';
  $handler->display->display_options['fields']['edit_node']['relationship'] = 'organism_id';
  $handler->display->display_options['fields']['edit_node']['exclude'] = TRUE;
  /* Field: Global: Custom text */
  $handler->display->display_options['fields']['nothing']['id'] = 'nothing';
  $handler->display->display_options['fields']['nothing']['table'] = 'views';
  $handler->display->display_options['fields']['nothing']['field'] = 'nothing';
  $handler->display->display_options['fields']['nothing']['label'] = '';
  $handler->display->display_options['fields']['nothing']['alter']['text'] = '[edit_node]   [delete_node]';
  $handler->display->display_options['fields']['nothing']['element_label_colon'] = FALSE;
  /* Sort criterion: Chado Organism: Genus */
  $handler->display->display_options['sorts']['genus']['id'] = 'genus';
  $handler->display->display_options['sorts']['genus']['table'] = 'organism';
  $handler->display->display_options['sorts']['genus']['field'] = 'genus';
  /* Sort criterion: Chado Organism: Species */
  $handler->display->display_options['sorts']['species']['id'] = 'species';
  $handler->display->display_options['sorts']['species']['table'] = 'organism';
  $handler->display->display_options['sorts']['species']['field'] = 'species';
  /* Filter criterion: Chado Organism: Common Name */
  $handler->display->display_options['filters']['common_name']['id'] = 'common_name';
  $handler->display->display_options['filters']['common_name']['table'] = 'organism';
  $handler->display->display_options['filters']['common_name']['field'] = 'common_name';
  $handler->display->display_options['filters']['common_name']['operator'] = 'contains';
  $handler->display->display_options['filters']['common_name']['exposed'] = TRUE;
  $handler->display->display_options['filters']['common_name']['expose']['operator_id'] = '';
  $handler->display->display_options['filters']['common_name']['expose']['label'] = 'Common Name Contains';
  $handler->display->display_options['filters']['common_name']['expose']['identifier'] = 'common_name';
  $handler->display->display_options['filters']['common_name']['expose']['remember_roles'] = array(
    2 => '2',
    1 => 0,
    3 => 0,
  );
  /* Filter criterion: Chado Organism: Genus */
  $handler->display->display_options['filters']['genus']['id'] = 'genus';
  $handler->display->display_options['filters']['genus']['table'] = 'organism';
  $handler->display->display_options['filters']['genus']['field'] = 'genus';
  $handler->display->display_options['filters']['genus']['exposed'] = TRUE;
  $handler->display->display_options['filters']['genus']['expose']['operator_id'] = 'genus_op';
  $handler->display->display_options['filters']['genus']['expose']['label'] = 'Genus';
  $handler->display->display_options['filters']['genus']['expose']['operator'] = 'genus_op';
  $handler->display->display_options['filters']['genus']['expose']['identifier'] = 'genus';
  $handler->display->display_options['filters']['genus']['expose']['remember_roles'] = array(
    2 => '2',
    1 => 0,
    3 => 0,
  );
  /* Filter criterion: Chado Organism: Species */
  $handler->display->display_options['filters']['species']['id'] = 'species';
  $handler->display->display_options['filters']['species']['table'] = 'organism';
  $handler->display->display_options['filters']['species']['field'] = 'species';
  $handler->display->display_options['filters']['species']['exposed'] = TRUE;
  $handler->display->display_options['filters']['species']['expose']['operator_id'] = 'species_op';
  $handler->display->display_options['filters']['species']['expose']['label'] = 'Species';
  $handler->display->display_options['filters']['species']['expose']['operator'] = 'species_op';
  $handler->display->display_options['filters']['species']['expose']['identifier'] = 'species';
  $handler->display->display_options['filters']['species']['expose']['remember_roles'] = array(
    2 => '2',
    1 => 0,
    3 => 0,
  );

  /** MANUALLY ADD since filter handler no exporting correctly */
  $handler->display->display_options['filters']['common_name']['expose']['values_form_type'] = 'textfield';
  $handler->display->display_options['filters']['common_name']['expose']['select_multiple'] = FALSE;
  $handler->display->display_options['filters']['common_name']['expose']['select_optional'] = FALSE;
  $handler->display->display_options['filters']['common_name']['expose']['max_length'] = 40;

  /* Display: Page */
  $handler = $view->new_display('page', 'Page', 'page_1');
  $handler->display->display_options['path'] = 'admin/tripal/chado/tripal_organism/organisms';
  $handler->display->display_options['menu']['type'] = 'default tab';
  $handler->display->display_options['menu']['title'] = 'Organisms';
  $handler->display->display_options['menu']['description'] = 'A biological organism.';
  $handler->display->display_options['menu']['weight'] = '-10';
  $handler->display->display_options['menu']['name'] = 'management';
  $handler->display->display_options['menu']['context'] = 0;
  $handler->display->display_options['menu']['context_only_inline'] = 0;
  $handler->display->display_options['tab_options']['weight'] = '0';

  /** MANUALLY ADD since filter handler no exporting correctly
  $handler->display->display_options['filters']['common_name']['expose']['values_form_type'] = 'textfield';
  $handler->display->display_options['filters']['common_name']['expose']['select_multiple'] = FALSE;
  $handler->display->display_options['filters']['common_name']['expose']['select_optional'] = FALSE;
  $handler->display->display_options['filters']['common_name']['expose']['max_length'] = 40;
  */

  return $view;
}
