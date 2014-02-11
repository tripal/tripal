<?php
/**
 * @file
 * Describes default cv views
 */

/**
 * Implements hook_views_default_views().
 *
 * @ingroup tripal_cv
 */
function tripal_cv_views_default_views() {
  $views = array();

  // Default Tripal Admin View: CVs
  $view = tripal_cv_defaultview_admin_cvs_listing();
  $view = tripal_views_make_view_compatible_with_external($view);
  $views[$view->name] = $view;

  // Default Tripal Admin View: CV Terms
  $view = tripal_cv_defaultview_admin_cvterms_listing();
  $view = tripal_views_make_view_compatible_with_external($view);
  $views[$view->name] = $view;

  return $views;
}

/**
 * Default Admin View for cv management
 *
 * @ingroup tripal_cv
 */
function tripal_cv_defaultview_admin_cvs_listing() {

$view = new view();
$view->name = 'tripal_cv_admin_cvs';
$view->description = 'DO NOT DISABLE';
$view->tag = 'tripal admin';
$view->base_table = 'cv';
$view->human_name = 'CVs Admin';
$view->core = 6;
$view->api_version = '3.0';
$view->disabled = FALSE; /* Edit this to true to make a default view disabled initially */

/* Display: Defaults */
$handler = $view->new_display('default', 'Defaults', 'default');
$handler->display->display_options['title'] = 'Controlled Vocabularies';
$handler->display->display_options['use_more_always'] = FALSE;
$handler->display->display_options['access']['type'] = 'perm';
$handler->display->display_options['access']['perm'] = 'administer controlled vocabularies';
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
  'definition' => 'definition',
  'nothing' => 'nothing',
);
$handler->display->display_options['style_options']['default'] = 'name';
$handler->display->display_options['style_options']['info'] = array(
  'name' => array(
    'sortable' => 1,
    'separator' => '',
  ),
  'definition' => array(
    'sortable' => 0,
    'separator' => '',
  ),
  'nothing' => array(
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
    'label-1' => 'Add Vocabulary',
    'path-1' => 'admin/tripal/chado/tripal_cv/cv/add',
  );
  $handler->display->display_options['header']['action_links_area']['link-2'] = array(
    'label-2' => 'Load Ontology',
    'path-2' => 'admin/tripal/chado/tripal_cv/obo_loader',
  );
/* No results behavior: Global: Text area */
$handler->display->display_options['empty']['text']['id'] = 'area';
$handler->display->display_options['empty']['text']['table'] = 'views';
$handler->display->display_options['empty']['text']['field'] = 'area';
$handler->display->display_options['empty']['text']['content'] = 'No controlled vocabularies match the supplied criteria.';
$handler->display->display_options['empty']['text']['format'] = '2';
/* Field: Chado Cv: Cv Id */
$handler->display->display_options['fields']['cv_id']['id'] = 'cv_id';
$handler->display->display_options['fields']['cv_id']['table'] = 'cv';
$handler->display->display_options['fields']['cv_id']['field'] = 'cv_id';
$handler->display->display_options['fields']['cv_id']['exclude'] = TRUE;
$handler->display->display_options['fields']['cv_id']['separator'] = '';
/* Field: Chado Cv: Name */
$handler->display->display_options['fields']['name']['id'] = 'name';
$handler->display->display_options['fields']['name']['table'] = 'cv';
$handler->display->display_options['fields']['name']['field'] = 'name';
/* Field: Chado Cv: Definition */
$handler->display->display_options['fields']['definition']['id'] = 'definition';
$handler->display->display_options['fields']['definition']['table'] = 'cv';
$handler->display->display_options['fields']['definition']['field'] = 'definition';
/* Field: Global: Custom text */
$handler->display->display_options['fields']['nothing_1']['id'] = 'nothing_1';
$handler->display->display_options['fields']['nothing_1']['table'] = 'views';
$handler->display->display_options['fields']['nothing_1']['field'] = 'nothing';
$handler->display->display_options['fields']['nothing_1']['label'] = 'Edit Link';
$handler->display->display_options['fields']['nothing_1']['exclude'] = TRUE;
$handler->display->display_options['fields']['nothing_1']['alter']['text'] = 'edit';
$handler->display->display_options['fields']['nothing_1']['alter']['make_link'] = TRUE;
$handler->display->display_options['fields']['nothing_1']['alter']['path'] = 'admin/tripal/chado/tripal_cv/cv/edit/[cv_id]';
/* Field: Global: Custom text */
$handler->display->display_options['fields']['nothing']['id'] = 'nothing';
$handler->display->display_options['fields']['nothing']['table'] = 'views';
$handler->display->display_options['fields']['nothing']['field'] = 'nothing';
$handler->display->display_options['fields']['nothing']['label'] = 'View Terms link';
$handler->display->display_options['fields']['nothing']['exclude'] = TRUE;
$handler->display->display_options['fields']['nothing']['alter']['text'] = 'View Terms';
$handler->display->display_options['fields']['nothing']['alter']['make_link'] = TRUE;
$handler->display->display_options['fields']['nothing']['alter']['path'] = 'admin/tripal/chado/tripal_cv/cvterms?cv=[name]';
$handler->display->display_options['fields']['nothing']['hide_alter_empty'] = TRUE;
/* Field: Global: Custom text */
$handler->display->display_options['fields']['nothing_3']['id'] = 'nothing_3';
$handler->display->display_options['fields']['nothing_3']['table'] = 'views';
$handler->display->display_options['fields']['nothing_3']['field'] = 'nothing';
$handler->display->display_options['fields']['nothing_3']['label'] = 'Add term';
$handler->display->display_options['fields']['nothing_3']['exclude'] = TRUE;
$handler->display->display_options['fields']['nothing_3']['alter']['text'] = 'Add Term';
$handler->display->display_options['fields']['nothing_3']['alter']['make_link'] = TRUE;
$handler->display->display_options['fields']['nothing_3']['alter']['path'] = 'admin/tripal/chado/tripal_cv/cv/[cv_id]/cvterm/add';
/* Field: Global: Custom text */
$handler->display->display_options['fields']['nothing_2']['id'] = 'nothing_2';
$handler->display->display_options['fields']['nothing_2']['table'] = 'views';
$handler->display->display_options['fields']['nothing_2']['field'] = 'nothing';
$handler->display->display_options['fields']['nothing_2']['label'] = '';
$handler->display->display_options['fields']['nothing_2']['alter']['text'] = '[nothing_1]<br />
  [nothing]   [nothing_3]';
$handler->display->display_options['fields']['nothing_2']['element_class'] = 'short-column';
$handler->display->display_options['fields']['nothing_2']['element_label_class'] = 'short-column';
$handler->display->display_options['fields']['nothing_2']['element_label_colon'] = FALSE;
/* Sort criterion: Chado Cv: Name */
$handler->display->display_options['sorts']['name']['id'] = 'name';
$handler->display->display_options['sorts']['name']['table'] = 'cv';
$handler->display->display_options['sorts']['name']['field'] = 'name';
/* Filter criterion: Chado Cv: Name */
$handler->display->display_options['filters']['name']['id'] = 'name';
$handler->display->display_options['filters']['name']['table'] = 'cv';
$handler->display->display_options['filters']['name']['field'] = 'name';
$handler->display->display_options['filters']['name']['operator'] = 'contains';
$handler->display->display_options['filters']['name']['group'] = '0';
$handler->display->display_options['filters']['name']['exposed'] = TRUE;
$handler->display->display_options['filters']['name']['expose']['operator_id'] = 'name_op';
$handler->display->display_options['filters']['name']['expose']['label'] = 'Name Contains';
$handler->display->display_options['filters']['name']['expose']['operator'] = 'name_op';
$handler->display->display_options['filters']['name']['expose']['identifier'] = 'name';
$handler->display->display_options['filters']['name']['expose']['remember_roles'] = array(
  2 => '2',
  1 => 0,
  3 => 0,
);
/* Filter criterion: Chado Cv: Definition */
$handler->display->display_options['filters']['definition']['id'] = 'definition';
$handler->display->display_options['filters']['definition']['table'] = 'cv';
$handler->display->display_options['filters']['definition']['field'] = 'definition';
$handler->display->display_options['filters']['definition']['operator'] = 'contains';
$handler->display->display_options['filters']['definition']['group'] = '0';
$handler->display->display_options['filters']['definition']['exposed'] = TRUE;
$handler->display->display_options['filters']['definition']['expose']['operator_id'] = 'definition_op';
$handler->display->display_options['filters']['definition']['expose']['label'] = 'Definition Contains';
$handler->display->display_options['filters']['definition']['expose']['operator'] = 'definition_op';
$handler->display->display_options['filters']['definition']['expose']['identifier'] = 'definition';
$handler->display->display_options['filters']['definition']['expose']['remember_roles'] = array(
  2 => '2',
  1 => 0,
  3 => 0,
);

  /** MANUALLY ADD since filter handler no exporting correctly */
  $handler->display->display_options['filters']['name']['expose']['values_form_type'] = 'textfield';
  $handler->display->display_options['filters']['name']['expose']['select_multiple'] = FALSE;
  $handler->display->display_options['filters']['name']['expose']['select_optional'] = FALSE;
  $handler->display->display_options['filters']['name']['expose']['max_length'] = 40;

/* Display: Page */
$handler = $view->new_display('page', 'Page', 'page_1');
$handler->display->display_options['path'] = 'admin/tripal/chado/tripal_cv/cvs';
$handler->display->display_options['menu']['type'] = 'default tab';
$handler->display->display_options['menu']['title'] = 'Vocabularies';
$handler->display->display_options['menu']['description'] = 'A listing of all controlled vocabularies';
$handler->display->display_options['menu']['weight'] = '-10';
$handler->display->display_options['menu']['name'] = 'management';
$handler->display->display_options['menu']['context'] = 0;
$handler->display->display_options['menu']['context_only_inline'] = 0;
$handler->display->display_options['tab_options']['weight'] = '0';


  /** MANUALLY ADD since filter handler no exporting correctly
  $handler->display->display_options['filters']['name']['expose']['values_form_type'] = 'textfield';
  $handler->display->display_options['filters']['name']['expose']['select_multiple'] = FALSE;
  $handler->display->display_options['filters']['name']['expose']['select_optional'] = FALSE;
  $handler->display->display_options['filters']['name']['expose']['max_length'] = 40;
  */

  return $view;
}

/**
 * Default Admin View for cvterm management
 *
 * @ingroup tripal_cv
 */
function tripal_cv_defaultview_admin_cvterms_listing() {

$view = new view();
$view->name = 'tripal_cv_admin_cvterms';
$view->description = 'DO NOT DISABLE';
$view->tag = 'tripal admin';
$view->base_table = 'cvterm';
$view->human_name = 'CV Terms Admin';
$view->core = 0;
$view->api_version = '3.0';
$view->disabled = FALSE; /* Edit this to true to make a default view disabled initially */

/* Display: Defaults */
$handler = $view->new_display('default', 'Defaults', 'default');
$handler->display->display_options['title'] = 'Controlled Vocabulary Terms';
$handler->display->display_options['use_more_always'] = FALSE;
$handler->display->display_options['access']['type'] = 'perm';
$handler->display->display_options['access']['perm'] = 'administer controlled vocabularies';
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
  'name_1' => 'name_1',
  'name' => 'name',
  'definition' => 'definition',
  'is_obsolete' => 'is_obsolete',
  'is_relationshiptype' => 'is_relationshiptype',
);
$handler->display->display_options['style_options']['default'] = '-1';
$handler->display->display_options['style_options']['info'] = array(
  'name_1' => array(
    'sortable' => 1,
    'separator' => '',
  ),
  'name' => array(
    'sortable' => 1,
    'separator' => '',
  ),
  'definition' => array(
    'sortable' => 0,
    'separator' => '',
  ),
  'is_obsolete' => array(
    'sortable' => 1,
    'separator' => '',
  ),
  'is_relationshiptype' => array(
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
    'label-1' => 'Add Term',
    'path-1' => 'admin/tripal/chado/tripal_cv/cvterm/add',
  );
/* No results behavior: Global: Text area */
$handler->display->display_options['empty']['text']['id'] = 'area';
$handler->display->display_options['empty']['text']['table'] = 'views';
$handler->display->display_options['empty']['text']['field'] = 'area';
$handler->display->display_options['empty']['text']['content'] = 'There are no terms associated with the selected controlled vocabulary. Please select a different vocabulary from the list above.';
$handler->display->display_options['empty']['text']['format'] = '1';
/* Field: Chado Cv: Cv Id */
$handler->display->display_options['fields']['cv_id']['id'] = 'cv_id';
$handler->display->display_options['fields']['cv_id']['table'] = 'cv';
$handler->display->display_options['fields']['cv_id']['field'] = 'cv_id';
$handler->display->display_options['fields']['cv_id']['exclude'] = TRUE;
$handler->display->display_options['fields']['cv_id']['separator'] = '';
/* Field: Chado Cv: Name */
$handler->display->display_options['fields']['name_1']['id'] = 'name_1';
$handler->display->display_options['fields']['name_1']['table'] = 'cv';
$handler->display->display_options['fields']['name_1']['field'] = 'name';
$handler->display->display_options['fields']['name_1']['label'] = 'Vocabulary';
$handler->display->display_options['fields']['name_1']['alter']['make_link'] = TRUE;
$handler->display->display_options['fields']['name_1']['alter']['path'] = 'admin/tripal/chado/tripal_cv/cvs?name=[name_1]';
/* Field: Chado Cvterm: Cvterm Id */
$handler->display->display_options['fields']['cvterm_id']['id'] = 'cvterm_id';
$handler->display->display_options['fields']['cvterm_id']['table'] = 'cvterm';
$handler->display->display_options['fields']['cvterm_id']['field'] = 'cvterm_id';
$handler->display->display_options['fields']['cvterm_id']['exclude'] = TRUE;
$handler->display->display_options['fields']['cvterm_id']['separator'] = '';
/* Field: Chado Cvterm: Name */
$handler->display->display_options['fields']['name']['id'] = 'name';
$handler->display->display_options['fields']['name']['table'] = 'cvterm';
$handler->display->display_options['fields']['name']['field'] = 'name';
/* Field: Chado Cvterm: Definition */
$handler->display->display_options['fields']['definition']['id'] = 'definition';
$handler->display->display_options['fields']['definition']['table'] = 'cvterm';
$handler->display->display_options['fields']['definition']['field'] = 'definition';
$handler->display->display_options['fields']['definition']['element_class'] = 'wide-column';
$handler->display->display_options['fields']['definition']['element_label_class'] = 'wide-column';
/* Field: Chado Cvterm: Is Obsolete */
$handler->display->display_options['fields']['is_obsolete']['id'] = 'is_obsolete';
$handler->display->display_options['fields']['is_obsolete']['table'] = 'cvterm';
$handler->display->display_options['fields']['is_obsolete']['field'] = 'is_obsolete';
$handler->display->display_options['fields']['is_obsolete']['label'] = 'Obsolete?';
$handler->display->display_options['fields']['is_obsolete']['alter']['alter_text'] = TRUE;
$handler->display->display_options['fields']['is_obsolete']['alter']['text'] = 'Yes';
$handler->display->display_options['fields']['is_obsolete']['element_class'] = 'short-column';
$handler->display->display_options['fields']['is_obsolete']['element_label_class'] = 'short-column';
$handler->display->display_options['fields']['is_obsolete']['empty'] = 'No';
$handler->display->display_options['fields']['is_obsolete']['empty_zero'] = TRUE;
$handler->display->display_options['fields']['is_obsolete']['separator'] = '';
/* Field: Chado Cvterm: Is Relationshiptype */
$handler->display->display_options['fields']['is_relationshiptype']['id'] = 'is_relationshiptype';
$handler->display->display_options['fields']['is_relationshiptype']['table'] = 'cvterm';
$handler->display->display_options['fields']['is_relationshiptype']['field'] = 'is_relationshiptype';
$handler->display->display_options['fields']['is_relationshiptype']['label'] = 'Relation-ship?';
$handler->display->display_options['fields']['is_relationshiptype']['alter']['alter_text'] = TRUE;
$handler->display->display_options['fields']['is_relationshiptype']['alter']['text'] = 'Yes';
$handler->display->display_options['fields']['is_relationshiptype']['element_class'] = 'short-column';
$handler->display->display_options['fields']['is_relationshiptype']['element_label_class'] = 'short-column';
$handler->display->display_options['fields']['is_relationshiptype']['empty'] = 'No';
$handler->display->display_options['fields']['is_relationshiptype']['empty_zero'] = TRUE;
$handler->display->display_options['fields']['is_relationshiptype']['separator'] = '';
/* Field: Global: Custom text */
$handler->display->display_options['fields']['nothing']['id'] = 'nothing';
$handler->display->display_options['fields']['nothing']['table'] = 'views';
$handler->display->display_options['fields']['nothing']['field'] = 'nothing';
$handler->display->display_options['fields']['nothing']['label'] = 'Edit Link';
$handler->display->display_options['fields']['nothing']['exclude'] = TRUE;
$handler->display->display_options['fields']['nothing']['alter']['text'] = 'edit';
$handler->display->display_options['fields']['nothing']['alter']['make_link'] = TRUE;
$handler->display->display_options['fields']['nothing']['alter']['path'] = 'admin/tripal/chado/tripal_cv/cv/[cv_id]/cvterm/edit/[cvterm_id]';
/* Field: Global: Custom text */
$handler->display->display_options['fields']['nothing_1']['id'] = 'nothing_1';
$handler->display->display_options['fields']['nothing_1']['table'] = 'views';
$handler->display->display_options['fields']['nothing_1']['field'] = 'nothing';
$handler->display->display_options['fields']['nothing_1']['label'] = '';
$handler->display->display_options['fields']['nothing_1']['alter']['text'] = '[nothing]';
$handler->display->display_options['fields']['nothing_1']['element_class'] = 'short-column';
$handler->display->display_options['fields']['nothing_1']['element_label_class'] = 'short-column';
$handler->display->display_options['fields']['nothing_1']['element_label_colon'] = FALSE;
/* Sort criterion: Chado Cv: Name */
$handler->display->display_options['sorts']['name']['id'] = 'name';
$handler->display->display_options['sorts']['name']['table'] = 'cv';
$handler->display->display_options['sorts']['name']['field'] = 'name';
/* Sort criterion: Chado Cvterm: Name */
$handler->display->display_options['sorts']['name_1']['id'] = 'name_1';
$handler->display->display_options['sorts']['name_1']['table'] = 'cvterm';
$handler->display->display_options['sorts']['name_1']['field'] = 'name';
/* Filter criterion: Chado Cv: Name */
$handler->display->display_options['filters']['name']['id'] = 'name';
$handler->display->display_options['filters']['name']['table'] = 'cv';
$handler->display->display_options['filters']['name']['field'] = 'name';
$handler->display->display_options['filters']['name']['value'] = 'All';
$handler->display->display_options['filters']['name']['group'] = '0';
$handler->display->display_options['filters']['name']['exposed'] = TRUE;
$handler->display->display_options['filters']['name']['expose']['operator_id'] = 'name_op';
$handler->display->display_options['filters']['name']['expose']['label'] = 'Vocabulary';
$handler->display->display_options['filters']['name']['expose']['operator'] = 'name_op';
$handler->display->display_options['filters']['name']['expose']['identifier'] = 'cv';
$handler->display->display_options['filters']['name']['expose']['remember_roles'] = array(
  2 => '2',
  1 => 0,
  3 => 0,
);
/* Filter criterion: Chado Cvterm: Name */
$handler->display->display_options['filters']['name_1']['id'] = 'name_1';
$handler->display->display_options['filters']['name_1']['table'] = 'cvterm';
$handler->display->display_options['filters']['name_1']['field'] = 'name';
$handler->display->display_options['filters']['name_1']['operator'] = 'contains';
$handler->display->display_options['filters']['name_1']['group'] = '0';
$handler->display->display_options['filters']['name_1']['exposed'] = TRUE;
$handler->display->display_options['filters']['name_1']['expose']['operator_id'] = '';
$handler->display->display_options['filters']['name_1']['expose']['label'] = 'Name Contains';
$handler->display->display_options['filters']['name_1']['expose']['identifier'] = 'name';
$handler->display->display_options['filters']['name_1']['expose']['remember_roles'] = array(
  2 => '2',
  1 => 0,
  3 => 0,
);
/* Filter criterion: Chado Cvterm: Definition */
$handler->display->display_options['filters']['definition']['id'] = 'definition';
$handler->display->display_options['filters']['definition']['table'] = 'cvterm';
$handler->display->display_options['filters']['definition']['field'] = 'definition';
$handler->display->display_options['filters']['definition']['operator'] = 'contains';
$handler->display->display_options['filters']['definition']['group'] = '0';
$handler->display->display_options['filters']['definition']['exposed'] = TRUE;
$handler->display->display_options['filters']['definition']['expose']['operator_id'] = 'definition_op';
$handler->display->display_options['filters']['definition']['expose']['label'] = 'Definition Contains';
$handler->display->display_options['filters']['definition']['expose']['operator'] = 'definition_op';
$handler->display->display_options['filters']['definition']['expose']['identifier'] = 'definition';
$handler->display->display_options['filters']['definition']['expose']['remember_roles'] = array(
  2 => '2',
  1 => 0,
  3 => 0,
);

/** MANUALLY ADDED since filter handler no exporting correctly */
  $handler->display->display_options['filters']['name']['expose']['values_form_type'] = 'select';
  $handler->display->display_options['filters']['name']['expose']['select_multiple'] = FALSE;
  $handler->display->display_options['filters']['name']['expose']['select_optional'] = TRUE;
  $handler->display->display_options['filters']['name']['expose']['max_length'] = 40;

  $handler->display->display_options['filters']['name_1']['expose']['values_form_type'] = 'textfield';
  $handler->display->display_options['filters']['name_1']['expose']['select_multiple'] = FALSE;
  $handler->display->display_options['filters']['name_1']['expose']['select_optional'] = FALSE;
  $handler->display->display_options['filters']['name_1']['expose']['max_length'] = 40;

/* Display: Page */
$handler = $view->new_display('page', 'Page', 'page_1');
$handler->display->display_options['path'] = 'admin/tripal/chado/tripal_cv/cvterms';
$handler->display->display_options['menu']['type'] = 'tab';
$handler->display->display_options['menu']['title'] = 'Terms';
$handler->display->display_options['menu']['description'] = 'A listing of a controlled vocabulary terms for a given vocabulary';
$handler->display->display_options['menu']['weight'] = '-8';
$handler->display->display_options['menu']['name'] = 'management';
$handler->display->display_options['menu']['context'] = 0;
$handler->display->display_options['menu']['context_only_inline'] = 0;

  /** MANUALLY ADD since filter handler no exporting correctly
  $handler->display->display_options['filters']['name']['expose']['values_form_type'] = 'select';
  $handler->display->display_options['filters']['name']['expose']['select_multiple'] = FALSE;
  $handler->display->display_options['filters']['name']['expose']['select_optional'] = TRUE;
  $handler->display->display_options['filters']['name']['expose']['max_length'] = 40;

  $handler->display->display_options['filters']['name_1']['expose']['values_form_type'] = 'textfield';
  $handler->display->display_options['filters']['name_1']['expose']['select_multiple'] = FALSE;
  $handler->display->display_options['filters']['name_1']['expose']['select_optional'] = FALSE;
  $handler->display->display_options['filters']['name_1']['expose']['max_length'] = 40;
  */

  return $view;
}
