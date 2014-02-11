<?php
/**
 * @file
 * Describe default feature views
 */

/**
 * Implements hook_views_default_views().
 *
 * @ingroup tripal_feature
 */
function tripal_feature_views_default_views() {
  $views = array();

  $view = tripal_feature_defaultview_admin_features();
  $view = tripal_views_make_view_compatible_with_external($view);
  $views[$view->name] = $view;

  return $views;
}

/**
 * The default feature administration view
 *
 * @ingroup tripal_feature
 */
function tripal_feature_defaultview_admin_features() {

  $view = new view();
  $view->name = 'tripal_feature_admin_features';
  $view->description = 'DO NOT DISABLE';
  $view->tag = 'tripal admin';
  $view->base_table = 'feature';
  $view->human_name = 'Feature Admin';
  $view->core = 0;
  $view->api_version = '3.0';
  $view->disabled = FALSE; /* Edit this to true to make a default view disabled initially */

  /* Display: features_all */
  $handler = $view->new_display('default', 'features_all', 'default');
  $handler->display->display_options['title'] = 'Sequence Features';
  $handler->display->display_options['use_more_always'] = FALSE;
  $handler->display->display_options['access']['type'] = 'perm';
  $handler->display->display_options['access']['perm'] = 'access chado_feature content';
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
    'accession' => 'accession',
    'accession_link' => 'accession_link',
    'name' => 'name',
    'uniquename' => 'uniquename',
    'name_1' => 'name_1',
    'common_name' => 'common_name',
    'seqlen' => 'seqlen',
    'is_obsolete' => 'is_obsolete',
    'is_analysis' => 'is_analysis',
    'nid' => 'nid',
  );
  $handler->display->display_options['style_options']['default'] = '-1';
  $handler->display->display_options['style_options']['info'] = array(
    'accession' => array(
      'sortable' => 1,
      'separator' => '',
    ),
    'accession_link' => array(
      'sortable' => 1,
      'separator' => '',
    ),
    'name' => array(
      'sortable' => 1,
      'separator' => '',
    ),
    'uniquename' => array(
      'sortable' => 1,
      'separator' => '',
    ),
    'name_1' => array(
      'sortable' => 1,
      'separator' => '',
    ),
    'common_name' => array(
      'sortable' => 1,
      'separator' => '',
    ),
    'seqlen' => array(
      'sortable' => 1,
      'separator' => '',
    ),
    'is_obsolete' => array(
      'sortable' => 1,
      'separator' => '',
    ),
    'is_analysis' => array(
      'sortable' => 1,
      'separator' => '',
    ),
    'nid' => array(
      'separator' => '',
    ),
  );
  /* Header: Global: Action Links */
  $handler->display->display_options['header']['action_links_area']['id'] = 'action_links_area';
  $handler->display->display_options['header']['action_links_area']['table'] = 'views';
  $handler->display->display_options['header']['action_links_area']['field'] = 'action_links_area';
  $handler->display->display_options['header']['action_links_area']['label'] = 'Add/Import';
  $handler->display->display_options['header']['action_links_area']['empty'] = TRUE;
  $handler->display->display_options['header']['action_links_area']['link-1'] = array(
    'label-1' => 'Add Feature',
    'path-1' => 'node/add/tripal-feature',
  );
  $handler->display->display_options['header']['action_links_area']['link-2'] = array(
    'label-2' => 'Import via FASTA file',
    'path-2' => 'admin/tripal/loaders/fasta_loader',
  );
  $handler->display->display_options['header']['action_links_area']['link-3'] = array(
    'label-3' => 'Import via GFF3 file',
    'path-3' => 'admin/tripal/loaders/gff3_load',
  );
  $handler->display->display_options['header']['action_links_area']['link-4'] = array(
    'label-4' => '',
    'path-4' => '',
  );
  /* Header: Global: Action Links */
  $handler->display->display_options['header']['action_links_area_1']['id'] = 'action_links_area_1';
  $handler->display->display_options['header']['action_links_area_1']['table'] = 'views';
  $handler->display->display_options['header']['action_links_area_1']['field'] = 'action_links_area';
  $handler->display->display_options['header']['action_links_area_1']['label'] = 'Extract';
  $handler->display->display_options['header']['action_links_area_1']['empty'] = TRUE;
  $handler->display->display_options['header']['action_links_area_1']['link-1'] = array(
    'label-1' => 'Retrieve Sequences',
    'path-1' => 'find/sequences',
  );
  $handler->display->display_options['header']['action_links_area_1']['link-2'] = array(
    'label-2' => '',
    'path-2' => '',
  );
  $handler->display->display_options['header']['action_links_area_1']['link-3'] = array(
    'label-3' => '',
    'path-3' => '',
  );
  $handler->display->display_options['header']['action_links_area_1']['link-4'] = array(
    'label-4' => '',
    'path-4' => '',
  );
  /* No results behavior: Global: Text area */
  $handler->display->display_options['empty']['text']['id'] = 'area';
  $handler->display->display_options['empty']['text']['table'] = 'views';
  $handler->display->display_options['empty']['text']['field'] = 'area';
  $handler->display->display_options['empty']['text']['content'] = 'No features matched the supplied criteria.';
  $handler->display->display_options['empty']['text']['format'] = '2';
  /* Relationship: Feature: Feature => Node */
  $handler->display->display_options['relationships']['feature_id']['id'] = 'feature_id';
  $handler->display->display_options['relationships']['feature_id']['table'] = 'chado_feature';
  $handler->display->display_options['relationships']['feature_id']['field'] = 'feature_id';
  /* Field: Content: Nid */
  $handler->display->display_options['fields']['nid']['id'] = 'nid';
  $handler->display->display_options['fields']['nid']['table'] = 'node';
  $handler->display->display_options['fields']['nid']['field'] = 'nid';
  $handler->display->display_options['fields']['nid']['relationship'] = 'feature_id';
  $handler->display->display_options['fields']['nid']['exclude'] = TRUE;
  /* Field: Chado Feature: Uniquename */
  $handler->display->display_options['fields']['uniquename']['id'] = 'uniquename';
  $handler->display->display_options['fields']['uniquename']['table'] = 'feature';
  $handler->display->display_options['fields']['uniquename']['field'] = 'uniquename';
  $handler->display->display_options['fields']['uniquename']['label'] = 'Unique Name';
  $handler->display->display_options['fields']['uniquename']['alter']['path'] = 'node/[nid]';
  /* Field: Chado Feature: Name */
  $handler->display->display_options['fields']['name']['id'] = 'name';
  $handler->display->display_options['fields']['name']['table'] = 'feature';
  $handler->display->display_options['fields']['name']['field'] = 'name';
  /* Field: Chado Cvterm: Name */
  $handler->display->display_options['fields']['name_1']['id'] = 'name_1';
  $handler->display->display_options['fields']['name_1']['table'] = 'cvterm';
  $handler->display->display_options['fields']['name_1']['field'] = 'name';
  $handler->display->display_options['fields']['name_1']['label'] = 'Type';
  /* Field: Chado Organism: Common Name */
  $handler->display->display_options['fields']['common_name']['id'] = 'common_name';
  $handler->display->display_options['fields']['common_name']['table'] = 'organism';
  $handler->display->display_options['fields']['common_name']['field'] = 'common_name';
  /* Field: Chado Feature: Seqlen */
  $handler->display->display_options['fields']['seqlen']['id'] = 'seqlen';
  $handler->display->display_options['fields']['seqlen']['table'] = 'feature';
  $handler->display->display_options['fields']['seqlen']['field'] = 'seqlen';
  $handler->display->display_options['fields']['seqlen']['label'] = 'Sequence Length';
  /* Field: Chado Feature: Is Obsolete */
  $handler->display->display_options['fields']['is_obsolete']['id'] = 'is_obsolete';
  $handler->display->display_options['fields']['is_obsolete']['table'] = 'feature';
  $handler->display->display_options['fields']['is_obsolete']['field'] = 'is_obsolete';
  $handler->display->display_options['fields']['is_obsolete']['type'] = 'separator';
  $handler->display->display_options['fields']['is_obsolete']['not'] = 0;
  /* Field: Content: Delete link */
  $handler->display->display_options['fields']['delete_node']['id'] = 'delete_node';
  $handler->display->display_options['fields']['delete_node']['table'] = 'views_entity_node';
  $handler->display->display_options['fields']['delete_node']['field'] = 'delete_node';
  $handler->display->display_options['fields']['delete_node']['relationship'] = 'feature_id';
  $handler->display->display_options['fields']['delete_node']['exclude'] = TRUE;
  /* Field: Content: Edit link */
  $handler->display->display_options['fields']['edit_node']['id'] = 'edit_node';
  $handler->display->display_options['fields']['edit_node']['table'] = 'views_entity_node';
  $handler->display->display_options['fields']['edit_node']['field'] = 'edit_node';
  $handler->display->display_options['fields']['edit_node']['relationship'] = 'feature_id';
  $handler->display->display_options['fields']['edit_node']['exclude'] = TRUE;
  /* Field: Global: Custom text */
  $handler->display->display_options['fields']['nothing']['id'] = 'nothing';
  $handler->display->display_options['fields']['nothing']['table'] = 'views';
  $handler->display->display_options['fields']['nothing']['field'] = 'nothing';
  $handler->display->display_options['fields']['nothing']['label'] = '';
  $handler->display->display_options['fields']['nothing']['alter']['text'] = '[edit_node]   [delete_node]';
  $handler->display->display_options['fields']['nothing']['element_label_colon'] = FALSE;
  /* Sort criterion: Chado Organism: Common Name */
  $handler->display->display_options['sorts']['common_name']['id'] = 'common_name';
  $handler->display->display_options['sorts']['common_name']['table'] = 'organism';
  $handler->display->display_options['sorts']['common_name']['field'] = 'common_name';
  /* Sort criterion: Chado Cvterm: Name */
  $handler->display->display_options['sorts']['name']['id'] = 'name';
  $handler->display->display_options['sorts']['name']['table'] = 'cvterm';
  $handler->display->display_options['sorts']['name']['field'] = 'name';
  /* Sort criterion: Chado Feature: Name */
  $handler->display->display_options['sorts']['name_1']['id'] = 'name_1';
  $handler->display->display_options['sorts']['name_1']['table'] = 'feature';
  $handler->display->display_options['sorts']['name_1']['field'] = 'name';
  /* Filter criterion: Chado Organism: Common Name */
  $handler->display->display_options['filters']['common_name']['id'] = 'common_name';
  $handler->display->display_options['filters']['common_name']['table'] = 'organism';
  $handler->display->display_options['filters']['common_name']['field'] = 'common_name';
  $handler->display->display_options['filters']['common_name']['value'] = array();
  $handler->display->display_options['filters']['common_name']['group'] = '0';
  $handler->display->display_options['filters']['common_name']['exposed'] = TRUE;
  $handler->display->display_options['filters']['common_name']['expose']['operator_id'] = 'common_name_op';
  $handler->display->display_options['filters']['common_name']['expose']['label'] = 'Organism Common Name';
  $handler->display->display_options['filters']['common_name']['expose']['operator'] = 'common_name_op';
  $handler->display->display_options['filters']['common_name']['expose']['identifier'] = 'organism';
  /* Filter criterion: Chado Feature: Type Id */
  $handler->display->display_options['filters']['type_id']['id'] = 'type_id';
  $handler->display->display_options['filters']['type_id']['table'] = 'feature';
  $handler->display->display_options['filters']['type_id']['field'] = 'type_id';
  $handler->display->display_options['filters']['type_id']['value'] = array();
  $handler->display->display_options['filters']['type_id']['group'] = '0';
  $handler->display->display_options['filters']['type_id']['exposed'] = TRUE;
  $handler->display->display_options['filters']['type_id']['expose']['operator_id'] = 'type_id_op';
  $handler->display->display_options['filters']['type_id']['expose']['label'] = 'Type';
  $handler->display->display_options['filters']['type_id']['expose']['operator'] = 'type_id_op';
  $handler->display->display_options['filters']['type_id']['expose']['identifier'] = 'type_id';
  /* Filter criterion: Chado Feature: Name */
  $handler->display->display_options['filters']['name_1']['id'] = 'name_1';
  $handler->display->display_options['filters']['name_1']['table'] = 'feature';
  $handler->display->display_options['filters']['name_1']['field'] = 'name';
  $handler->display->display_options['filters']['name_1']['operator'] = '~';
  $handler->display->display_options['filters']['name_1']['group'] = '0';
  $handler->display->display_options['filters']['name_1']['exposed'] = TRUE;
  $handler->display->display_options['filters']['name_1']['expose']['operator_id'] = 'name_1_op';
  $handler->display->display_options['filters']['name_1']['expose']['label'] = 'Name Contains';
  $handler->display->display_options['filters']['name_1']['expose']['operator'] = 'name_1_op';
  $handler->display->display_options['filters']['name_1']['expose']['identifier'] = 'name';

  /** MANUALLY ADD since filter handler no exporting correctly */
  $handler->display->display_options['filters']['common_name']['expose']['values_form_type'] = 'select';
  $handler->display->display_options['filters']['common_name']['expose']['select_multiple'] = FALSE;
  $handler->display->display_options['filters']['common_name']['expose']['select_optional'] = TRUE;
  $handler->display->display_options['filters']['common_name']['expose']['max_length'] = 40;
  $handler->display->display_options['filters']['type_id']['expose']['values_form_type'] = 'select';
  $handler->display->display_options['filters']['type_id']['expose']['select_multiple'] = FALSE;
  $handler->display->display_options['filters']['type_id']['expose']['select_optional'] = TRUE;
  $handler->display->display_options['filters']['type_id']['expose']['max_length'] = 40;

  /* Display: Page */
  $handler = $view->new_display('page', 'Page', 'page_1');
  $handler->display->display_options['path'] = 'admin/tripal/chado/tripal_feature/features';
  $handler->display->display_options['menu']['type'] = 'default tab';
  $handler->display->display_options['menu']['title'] = 'Features';
  $handler->display->display_options['menu']['description'] = 'A feature is a biological sequence or a section of a biological sequence, or a collection of such sections.';
  $handler->display->display_options['menu']['weight'] = '-10';
  $handler->display->display_options['menu']['context'] = 0;
  $handler->display->display_options['menu']['context_only_inline'] = 0;
  $handler->display->display_options['tab_options']['weight'] = '0';

  /** MANUALLY ADD since filter handler no exporting correctly
  $handler->display->display_options['filters']['common_name']['expose']['values_form_type'] = 'select';
  $handler->display->display_options['filters']['common_name']['expose']['select_multiple'] = FALSE;
  $handler->display->display_options['filters']['common_name']['expose']['select_optional'] = TRUE;
  $handler->display->display_options['filters']['common_name']['expose']['max_length'] = 40;
  $handler->display->display_options['filters']['type_id']['expose']['values_form_type'] = 'select';
  $handler->display->display_options['filters']['type_id']['expose']['select_multiple'] = FALSE;
  $handler->display->display_options['filters']['type_id']['expose']['select_optional'] = TRUE;
  $handler->display->display_options['filters']['type_id']['expose']['max_length'] = 40;
  */

  return $view;
}
