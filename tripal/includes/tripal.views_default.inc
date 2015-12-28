<?php

/**
 * Implements hook_views_default_views().
 * Define default views related primarily to tripal entities.
 */
function tripal_views_default_views() {
  $views = array();

// Vocabulary Search.
$view = new view();
$view->name = 'tripal_vocabularies';
$view->description = '';
$view->tag = 'default';
$view->base_table = 'tripal_vocab';
$view->human_name = 'Search Vocabularies';
$view->core = 7;
$view->api_version = '3.0';
$view->disabled = FALSE; /* Edit this to true to make a default view disabled initially */

/* Display: Master */
$handler = $view->new_display('default', 'Master', 'default');
$handler->display->display_options['title'] = 'Search Vocabularies';
$handler->display->display_options['use_more_always'] = FALSE;
$handler->display->display_options['access']['type'] = 'none';
$handler->display->display_options['cache']['type'] = 'none';
$handler->display->display_options['query']['type'] = 'views_query';
$handler->display->display_options['exposed_form']['type'] = 'input_required';
$handler->display->display_options['exposed_form']['options']['submit_button'] = 'Search';
$handler->display->display_options['exposed_form']['options']['text_input_required'] = 'Use the namespace to search for specific vocabularies or leave it blank to see all vocabularies currently in use on your site.';
$handler->display->display_options['exposed_form']['options']['text_input_required_format'] = 'filtered_html';
$handler->display->display_options['pager']['type'] = 'full';
$handler->display->display_options['pager']['options']['items_per_page'] = '50';
$handler->display->display_options['pager']['options']['offset'] = '0';
$handler->display->display_options['pager']['options']['id'] = '0';
$handler->display->display_options['pager']['options']['quantity'] = '9';
$handler->display->display_options['style_plugin'] = 'table';
/* Field: Controlled Vocabulary: Namespace */
$handler->display->display_options['fields']['namespace']['id'] = 'namespace';
$handler->display->display_options['fields']['namespace']['table'] = 'tripal_vocab';
$handler->display->display_options['fields']['namespace']['field'] = 'namespace';
/* Field: Controlled Vocabulary: Created */
$handler->display->display_options['fields']['created']['id'] = 'created';
$handler->display->display_options['fields']['created']['table'] = 'tripal_vocab';
$handler->display->display_options['fields']['created']['field'] = 'created';
/* Field: Controlled Vocabulary: Changed */
$handler->display->display_options['fields']['changed']['id'] = 'changed';
$handler->display->display_options['fields']['changed']['table'] = 'tripal_vocab';
$handler->display->display_options['fields']['changed']['field'] = 'changed';
/* Filter criterion: Controlled Vocabulary: Namespace */
$handler->display->display_options['filters']['namespace']['id'] = 'namespace';
$handler->display->display_options['filters']['namespace']['table'] = 'tripal_vocab';
$handler->display->display_options['filters']['namespace']['field'] = 'namespace';
$handler->display->display_options['filters']['namespace']['operator'] = 'contains';
$handler->display->display_options['filters']['namespace']['exposed'] = TRUE;
$handler->display->display_options['filters']['namespace']['expose']['operator_id'] = 'namespace_op';
$handler->display->display_options['filters']['namespace']['expose']['label'] = 'Namespace';
$handler->display->display_options['filters']['namespace']['expose']['description'] = 'The vocabulary namespace (partial names accepted).';
$handler->display->display_options['filters']['namespace']['expose']['operator'] = 'namespace_op';
$handler->display->display_options['filters']['namespace']['expose']['identifier'] = 'namespace';
$handler->display->display_options['filters']['namespace']['expose']['remember_roles'] = array(
  2 => '2',
  1 => 0,
  3 => 0,
);

/* Display: Page */
$handler = $view->new_display('page', 'Page', 'page');
$handler->display->display_options['path'] = 'admin/tripal/terms/vocabulary-search';
$handler->display->display_options['menu']['type'] = 'normal';
$handler->display->display_options['menu']['title'] = 'Search Vocabularies';
$handler->display->display_options['menu']['description'] = 'Search the vocabularies currently in use.';
$handler->display->display_options['menu']['weight'] = '0';
$handler->display->display_options['menu']['name'] = 'management';
$handler->display->display_options['menu']['context'] = 0;
$handler->display->display_options['menu']['context_only_inline'] = 0;

$views[$view->name] = $view;

// Term Search.
$view = new view();
$view->name = 'tripal_terms';
$view->description = '';
$view->tag = 'default';
$view->base_table = 'tripal_term';
$view->human_name = 'Search Terms';
$view->core = 7;
$view->api_version = '3.0';
$view->disabled = FALSE; /* Edit this to true to make a default view disabled initially */

/* Display: Master */
$handler = $view->new_display('default', 'Master', 'default');
$handler->display->display_options['title'] = 'Search Terms';
$handler->display->display_options['use_more_always'] = FALSE;
$handler->display->display_options['access']['type'] = 'none';
$handler->display->display_options['cache']['type'] = 'none';
$handler->display->display_options['query']['type'] = 'views_query';
$handler->display->display_options['exposed_form']['type'] = 'input_required';
$handler->display->display_options['exposed_form']['options']['submit_button'] = 'Search';
$handler->display->display_options['exposed_form']['options']['text_input_required'] = 'Use the filters to search for the term you are interested in or leave them blank to see all terms defining Tripal Content Types.';
$handler->display->display_options['exposed_form']['options']['text_input_required_format'] = 'filtered_html';
$handler->display->display_options['pager']['type'] = 'full';
$handler->display->display_options['pager']['options']['items_per_page'] = '50';
$handler->display->display_options['style_plugin'] = 'table';
$handler->display->display_options['style_options']['columns'] = array(
  'namespace' => 'namespace',
  'name' => 'name',
  'accession' => 'accession',
  'created' => 'created',
  'changed' => 'changed',
);
$handler->display->display_options['style_options']['default'] = '-1';
$handler->display->display_options['style_options']['info'] = array(
  'namespace' => array(
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
  'accession' => array(
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
);
/* Field: Controlled Vocabulary: Namespace */
$handler->display->display_options['fields']['namespace']['id'] = 'namespace';
$handler->display->display_options['fields']['namespace']['table'] = 'tripal_vocab';
$handler->display->display_options['fields']['namespace']['field'] = 'namespace';
/* Field: Controlled Vocabulary Term: Name */
$handler->display->display_options['fields']['name']['id'] = 'name';
$handler->display->display_options['fields']['name']['table'] = 'tripal_term';
$handler->display->display_options['fields']['name']['field'] = 'name';
/* Field: Controlled Vocabulary Term: Accession */
$handler->display->display_options['fields']['accession']['id'] = 'accession';
$handler->display->display_options['fields']['accession']['table'] = 'tripal_term';
$handler->display->display_options['fields']['accession']['field'] = 'accession';
/* Field: Controlled Vocabulary Term: Created */
$handler->display->display_options['fields']['created']['id'] = 'created';
$handler->display->display_options['fields']['created']['table'] = 'tripal_term';
$handler->display->display_options['fields']['created']['field'] = 'created';
$handler->display->display_options['fields']['created']['date_format'] = 'short';
$handler->display->display_options['fields']['created']['second_date_format'] = 'long';
/* Field: Controlled Vocabulary Term: Changed */
$handler->display->display_options['fields']['changed']['id'] = 'changed';
$handler->display->display_options['fields']['changed']['table'] = 'tripal_term';
$handler->display->display_options['fields']['changed']['field'] = 'changed';
$handler->display->display_options['fields']['changed']['date_format'] = 'short';
$handler->display->display_options['fields']['changed']['second_date_format'] = 'long';
/* Sort criterion: Controlled Vocabulary: Namespace */
$handler->display->display_options['sorts']['namespace']['id'] = 'namespace';
$handler->display->display_options['sorts']['namespace']['table'] = 'tripal_vocab';
$handler->display->display_options['sorts']['namespace']['field'] = 'namespace';
/* Sort criterion: Controlled Vocabulary Term: Name */
$handler->display->display_options['sorts']['name']['id'] = 'name';
$handler->display->display_options['sorts']['name']['table'] = 'tripal_term';
$handler->display->display_options['sorts']['name']['field'] = 'name';
/* Filter criterion: Controlled Vocabulary: Namespace */
$handler->display->display_options['filters']['namespace']['id'] = 'namespace';
$handler->display->display_options['filters']['namespace']['table'] = 'tripal_vocab';
$handler->display->display_options['filters']['namespace']['field'] = 'namespace';
$handler->display->display_options['filters']['namespace']['operator'] = 'contains';
$handler->display->display_options['filters']['namespace']['exposed'] = TRUE;
$handler->display->display_options['filters']['namespace']['expose']['operator_id'] = 'namespace_op';
$handler->display->display_options['filters']['namespace']['expose']['label'] = 'Vocabulary Namespace';
$handler->display->display_options['filters']['namespace']['expose']['description'] = 'The vocabulary namespace (partial names accepted).';
$handler->display->display_options['filters']['namespace']['expose']['operator'] = 'namespace_op';
$handler->display->display_options['filters']['namespace']['expose']['identifier'] = 'namespace';
$handler->display->display_options['filters']['namespace']['expose']['remember_roles'] = array(
  2 => '2',
  1 => 0,
  3 => 0,
);
/* Filter criterion: Controlled Vocabulary Term: Accession */
$handler->display->display_options['filters']['accession']['id'] = 'accession';
$handler->display->display_options['filters']['accession']['table'] = 'tripal_term';
$handler->display->display_options['filters']['accession']['field'] = 'accession';
$handler->display->display_options['filters']['accession']['operator'] = 'contains';
$handler->display->display_options['filters']['accession']['exposed'] = TRUE;
$handler->display->display_options['filters']['accession']['expose']['operator_id'] = 'accession_op';
$handler->display->display_options['filters']['accession']['expose']['label'] = 'Accession';
$handler->display->display_options['filters']['accession']['expose']['description'] = 'The accession of the term (partial accessions accepted).';
$handler->display->display_options['filters']['accession']['expose']['operator'] = 'accession_op';
$handler->display->display_options['filters']['accession']['expose']['identifier'] = 'accession';
$handler->display->display_options['filters']['accession']['expose']['remember_roles'] = array(
  2 => '2',
  1 => 0,
  3 => 0,
);
/* Filter criterion: Controlled Vocabulary Term: Name */
$handler->display->display_options['filters']['name']['id'] = 'name';
$handler->display->display_options['filters']['name']['table'] = 'tripal_term';
$handler->display->display_options['filters']['name']['field'] = 'name';
$handler->display->display_options['filters']['name']['operator'] = 'contains';
$handler->display->display_options['filters']['name']['exposed'] = TRUE;
$handler->display->display_options['filters']['name']['expose']['operator_id'] = 'name_op';
$handler->display->display_options['filters']['name']['expose']['label'] = 'Name';
$handler->display->display_options['filters']['name']['expose']['description'] = 'The name of the term (partial names accepted).';
$handler->display->display_options['filters']['name']['expose']['operator'] = 'name_op';
$handler->display->display_options['filters']['name']['expose']['identifier'] = 'name';
$handler->display->display_options['filters']['name']['expose']['remember_roles'] = array(
  2 => '2',
  1 => 0,
  3 => 0,
);

/* Display: Page */
$handler = $view->new_display('page', 'Page', 'page');
$handler->display->display_options['path'] = 'admin/tripal/terms/term-search';
$handler->display->display_options['menu']['type'] = 'normal';
$handler->display->display_options['menu']['title'] = 'Search Terms';
$handler->display->display_options['menu']['description'] = 'Search the terms currently defining Tripal Content Types.';
$handler->display->display_options['menu']['weight'] = '0';
$handler->display->display_options['menu']['name'] = 'management';
$handler->display->display_options['menu']['context'] = 0;
$handler->display->display_options['menu']['context_only_inline'] = 0;

$views[$view->name] = $view;

  return $views;
}

