<?php

/**
 *  @file
 *  This file contains the basic functions for views integration of
 *  chado/tripal db tables. Supplementary functions can be found in
 *  ./views/
 *
 *  Documentation on views integration can be found at
 *  http://views2.logrus.com/doc/html/index.html.
 */

/**
 * @defgroup tripal_cv_views Controlled Vocabulary Views Integration
 * @ingroup views
 */

/**
 *
 * @ingroup tripal_cv_views
 */
function tripal_cv_views_default_views() {
  $views = array();

  if (!module_exists('tripal_views')) {
    return $views;
  }

  // Main default view
  // List all cvterms based on cv
  $view = new view;
  $view->name = 'cvterm_listing';
  $view->description = 'A listing of all controlled vocabulary terms filtered by controlled vocabulary';
  $view->tag = 'chado default';
  $view->base_table = 'cvterm';
  $view->core = 0;
  $view->api_version = '2';
  $view->disabled = FALSE; /* Edit this to true to make a default view disabled initially */
  $handler = $view->new_display('default', 'Defaults', 'default');
  $handler->override_option('fields', array(
    'name_1' => array(
      'label' => 'Vocabulary',
      'alter' => array(
        'alter_text' => 0,
        'text' => '',
        'make_link' => 0,
        'path' => '',
        'absolute' => 0,
        'link_class' => '',
        'alt' => '',
        'rel' => '',
        'prefix' => '',
        'suffix' => '',
        'target' => '',
        'help' => '',
        'trim' => 0,
        'max_length' => '',
        'word_boundary' => 1,
        'ellipsis' => 1,
        'html' => 0,
        'strip_tags' => 0,
      ),
      'empty' => '',
      'hide_empty' => 0,
      'empty_zero' => 0,
      'hide_alter_empty' => 1,
      'type' => 'separator',
      'separator' => ', ',
      'exclude' => 0,
      'id' => 'name_1',
      'table' => 'cv',
      'field' => 'name',
      'relationship' => 'none',
    ),
    'name' => array(
      'label' => 'Name',
      'alter' => array(
        'alter_text' => 0,
        'text' => '',
        'make_link' => 0,
        'path' => '',
        'link_class' => '',
        'alt' => '',
        'prefix' => '',
        'suffix' => '',
        'target' => '',
        'help' => '',
        'trim' => 0,
        'max_length' => '',
        'word_boundary' => 1,
        'ellipsis' => 1,
        'html' => 0,
        'strip_tags' => 0,
      ),
      'empty' => '',
      'hide_empty' => 0,
      'empty_zero' => 0,
      'exclude' => 0,
      'id' => 'name',
      'table' => 'cvterm',
      'field' => 'name',
      'relationship' => 'none',
    ),
    'definition' => array(
      'label' => 'Definition',
      'alter' => array(
        'alter_text' => 0,
        'text' => '',
        'make_link' => 0,
        'path' => '',
        'link_class' => '',
        'alt' => '',
        'prefix' => '',
        'suffix' => '',
        'target' => '',
        'help' => '',
        'trim' => 0,
        'max_length' => '',
        'word_boundary' => 1,
        'ellipsis' => 1,
        'html' => 0,
        'strip_tags' => 0,
      ),
      'empty' => '',
      'hide_empty' => 0,
      'empty_zero' => 0,
      'exclude' => 0,
      'id' => 'definition',
      'table' => 'cvterm',
      'field' => 'definition',
      'relationship' => 'none',
    ),
    'is_obsolete' => array(
      'label' => 'Is Obsolete',
      'alter' => array(
        'alter_text' => 0,
        'text' => '',
        'make_link' => 0,
        'path' => '',
        'link_class' => '',
        'alt' => '',
        'prefix' => '',
        'suffix' => '',
        'target' => '',
        'help' => '',
        'trim' => 0,
        'max_length' => '',
        'word_boundary' => 1,
        'ellipsis' => 1,
        'html' => 0,
        'strip_tags' => 0,
      ),
      'empty' => '',
      'hide_empty' => 0,
      'empty_zero' => 0,
      'type' => 'yes-no',
      'not' => 0,
      'exclude' => 0,
      'id' => 'is_obsolete',
      'table' => 'cvterm',
      'field' => 'is_obsolete',
      'relationship' => 'none',
    ),
    'is_relationshiptype' => array(
      'label' => 'Is Relationship',
      'alter' => array(
        'alter_text' => 0,
        'text' => '',
        'make_link' => 0,
        'path' => '',
        'link_class' => '',
        'alt' => '',
        'prefix' => '',
        'suffix' => '',
        'target' => '',
        'help' => '',
        'trim' => 0,
        'max_length' => '',
        'word_boundary' => 1,
        'ellipsis' => 1,
        'html' => 0,
        'strip_tags' => 0,
      ),
      'empty' => '',
      'hide_empty' => 0,
      'empty_zero' => 0,
      'type' => 'yes-no',
      'not' => 0,
      'exclude' => 0,
      'id' => 'is_relationshiptype',
      'table' => 'cvterm',
      'field' => 'is_relationshiptype',
      'relationship' => 'none',
    ),
  ));
  $handler->override_option('sorts', array(
    'name' => array(
      'order' => 'ASC',
      'id' => 'name',
      'table' => 'cv',
      'field' => 'name',
      'relationship' => 'none',
    ),
    'name_1' => array(
      'order' => 'ASC',
      'id' => 'name_1',
      'table' => 'cvterm',
      'field' => 'name',
      'relationship' => 'none',
    ),
  ));
  $handler->override_option('filters', array(
    'name' => array(
      'operator' => '=',
      'value' => array(),
      'group' => '0',
      'exposed' => TRUE,
      'expose' => array(
        'use_operator' => 0,
        'operator' => 'name_op',
        'identifier' => 'cv',
        'label' => 'Vocabulary',
        'remember' => 0,
      ),
      'case' => 1,
      'id' => 'name',
      'table' => 'cv',
      'field' => 'name',
      'relationship' => 'none',
      'values_form_type' => 'select',
      'multiple' => 1,
      'optional' => 0,
      'agg' => array(
        'records_with' => 1,
        'aggregates_with' => 1,
      ),
    ),
    'name_1' => array(
      'operator' => '~',
      'value' => '',
      'group' => '0',
      'exposed' => TRUE,
      'expose' => array(
        'use_operator' => 0,
        'operator' => '',
        'identifier' => 'name',
        'label' => 'Name Contains',
        'remember' => 0,
      ),
      'case' => 0,
      'id' => 'name_1',
      'table' => 'cvterm',
      'field' => 'name',
      'relationship' => 'none',
      'values_form_type' => 'textfield',
      'multiple' => 0,
      'optional' => 0,
      'show_all' => 0,
      'agg' => array(
        'records_with' => 1,
        'aggregates_with' => 1,
      ),
    ),
    'definition' => array(
      'operator' => '~',
      'value' => '',
      'group' => '0',
      'exposed' => TRUE,
      'expose' => array(
        'use_operator' => 0,
        'operator' => 'definition_op',
        'identifier' => 'definition',
        'label' => 'Definition Contains',
        'remember' => 0,
      ),
      'case' => 0,
      'id' => 'definition',
      'table' => 'cvterm',
      'field' => 'definition',
      'relationship' => 'none',
      'agg' => array(
        'records_with' => 1,
        'aggregates_with' => 0,
      ),
    ),
    'search_results' => array(
      'operator' => '=',
      'value' => '',
      'group' => '0',
      'exposed' => FALSE,
      'expose' => array(
        'operator' => FALSE,
        'label' => '',
      ),
      'id' => 'search_results',
      'table' => 'views',
      'field' => 'search_results',
      'relationship' => 'none',
      'apply_button' => 'Show',
      'no_results_text' => 'Click "Show" to see a list of all controlled vocabulary terms matching the entered criteria. If you leave a any of the criteria blank then the controlled vocabulary terms will be not be filtered based on that field. Furthermore, if you leave all criteria blank then all controlled vocabulary terms will be listed.',
    ),
  ));
  $handler->override_option('access', array(
    'type' => 'perm',
    'perm' => 'access chado_cv content',
  ));
  $handler->override_option('cache', array(
    'type' => 'none',
  ));
  $handler->override_option('title', 'Controlled Vocabulary Terms');
  $handler->override_option('header', 'Click "Show" to see a list of all controlled vocabulary terms matching the entered criteria. If you leave a any of the criteria blank then the controlled vocabulary terms will be not be filtered based on that field. Furthermore, if you leave all criteria blank then all controlled vocabulary terms will be listed.');
  $handler->override_option('header_format', '2');
  $handler->override_option('header_empty', 0);
  $handler->override_option('empty', 'There are no terms associated with the selected controlled vocabulary. Please select a different vocabulary from the list above.');
  $handler->override_option('empty_format', '1');
  $handler->override_option('items_per_page', 50);
  $handler->override_option('use_pager', '1');
  $handler->override_option('style_plugin', 'table');
  $handler->override_option('style_options', array(
    'grouping' => '',
    'override' => 1,
    'sticky' => 0,
    'order' => 'asc',
    'summary' => '',
    'columns' => array(
      'name_1' => 'name_1',
      'name' => 'name',
      'definition' => 'definition',
      'is_obsolete' => 'is_obsolete',
      'is_relationshiptype' => 'is_relationshiptype',
    ),
    'info' => array(
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
    ),
    'default' => '-1',
  ));
  $handler = $view->new_display('page', 'Page', 'page_1');
  $handler->override_option('path', 'admin/tripal/tripal_cv/list_cvterms');
  $handler->override_option('menu', array(
    'type' => 'normal',
    'title' => 'Term Listing',
    'description' => 'A listing of a controlled vocabulary terms for a given vocabulary',
    'weight' => '10',
    'name' => 'navigation',
  ));
  $handler->override_option('tab_options', array(
    'type' => 'none',
    'title' => '',
    'description' => '',
    'weight' => 0,
    'name' => 'navigation',
  ));
  $views[$view->name] = $view;

  // Main cv default listing
  $view = new view;
  $view->name = 'cv_listing';
  $view->description = 'A listing of all controlled vocabularies';
  $view->tag = 'chado default';
  $view->base_table = 'cv';
  $view->core = 6;
  $view->api_version = '2';
  $view->disabled = FALSE; /* Edit this to true to make a default view disabled initially */
  $handler = $view->new_display('default', 'Defaults', 'default');
  $handler->override_option('fields', array(
    'name' => array(
      'label' => 'Name',
      'alter' => array(
        'alter_text' => 0,
        'text' => '',
        'make_link' => 0,
        'path' => '',
        'absolute' => 0,
        'link_class' => '',
        'alt' => '',
        'rel' => '',
        'prefix' => '',
        'suffix' => '',
        'target' => '',
        'help' => '',
        'trim' => 0,
        'max_length' => '',
        'word_boundary' => 1,
        'ellipsis' => 1,
        'html' => 0,
        'strip_tags' => 0,
      ),
      'empty' => '',
      'hide_empty' => 0,
      'empty_zero' => 0,
      'hide_alter_empty' => 1,
      'type' => 'separator',
      'separator' => ', ',
      'exclude' => 0,
      'id' => 'name',
      'table' => 'cv',
      'field' => 'name',
      'relationship' => 'none',
    ),
    'definition' => array(
      'label' => 'Definition',
      'alter' => array(
        'alter_text' => 0,
        'text' => '',
        'make_link' => 0,
        'path' => '',
        'absolute' => 0,
        'link_class' => '',
        'alt' => '',
        'rel' => '',
        'prefix' => '',
        'suffix' => '',
        'target' => '',
        'help' => '',
        'trim' => 0,
        'max_length' => '',
        'word_boundary' => 1,
        'ellipsis' => 1,
        'html' => 0,
        'strip_tags' => 0,
      ),
      'empty' => '',
      'hide_empty' => 0,
      'empty_zero' => 0,
      'hide_alter_empty' => 1,
      'type' => 'separator',
      'separator' => ', ',
      'exclude' => 0,
      'id' => 'definition',
      'table' => 'cv',
      'field' => 'definition',
      'relationship' => 'none',
    ),
    'nothing' => array(
      'label' => 'Terms',
      'alter' => array(
        'text' => 'view',
        'make_link' => 1,
        'path' => 'admin/tripal/tripal_cv/list_cvterms?cv%5B%5D=[name]',
        'absolute' => 0,
        'link_class' => '',
        'alt' => '',
        'rel' => '',
        'prefix' => '',
        'suffix' => '',
        'target' => '',
        'help' => '',
        'trim' => 0,
        'max_length' => '',
        'word_boundary' => 1,
        'ellipsis' => 1,
        'html' => 0,
        'strip_tags' => 0,
      ),
      'empty' => '',
      'hide_empty' => 0,
      'empty_zero' => 0,
      'hide_alter_empty' => 1,
      'exclude' => 0,
      'id' => 'nothing',
      'table' => 'views',
      'field' => 'nothing',
      'relationship' => 'none',
    ),
  ));
  $handler->override_option('filters', array(
    'name' => array(
      'operator' => '~',
      'value' => '',
      'group' => '0',
      'exposed' => TRUE,
      'expose' => array(
        'use_operator' => 0,
        'operator' => 'name_op',
        'identifier' => 'name',
        'label' => 'Name Contains',
        'remember' => 0,
      ),
      'case' => 0,
      'id' => 'name',
      'table' => 'cv',
      'field' => 'name',
      'relationship' => 'none',
      'values_form_type' => 'textfield',
      'multiple' => 0,
      'optional' => 0,
      'agg' => array(
        'records_with' => 1,
        'aggregates_with' => 0,
      ),
    ),
    'definition' => array(
      'operator' => '~',
      'value' => '',
      'group' => '0',
      'exposed' => TRUE,
      'expose' => array(
        'use_operator' => 0,
        'operator' => 'definition_op',
        'identifier' => 'definition',
        'label' => 'Definition Contains',
        'remember' => 0,
      ),
      'case' => 0,
      'id' => 'definition',
      'table' => 'cv',
      'field' => 'definition',
      'relationship' => 'none',
      'agg' => array(
        'records_with' => 1,
        'aggregates_with' => 0,
      ),
    ),
    'search_results' => array(
      'operator' => '=',
      'value' => '',
      'group' => '0',
      'exposed' => FALSE,
      'expose' => array(
        'operator' => FALSE,
        'label' => '',
      ),
      'id' => 'search_results',
      'table' => 'views',
      'field' => 'search_results',
      'relationship' => 'none',
      'apply_button' => 'Show',
      'no_results_text' => 'Click "Show" to see a list of all controlled vocabularies matching the entered criteria. If you leave a any of the criteria blank then the controlled vocabularies will be not be filtered based on that field. Furthermore, if you leave all criteria blank then all controlled vocabularies will be listed.',
    ),
  ));
  $handler->override_option('access', array(
    'type' => 'perm',
    'perm' => 'access chado_cv content',
  ));
  $handler->override_option('cache', array(
    'type' => 'none',
  ));
  $handler->override_option('title', 'Controlled Vocabularies');
  $handler->override_option('header', 'Click "Show" to see a list of all controlled vocabularies matching the entered criteria. If you leave a any of the criteria blank then the controlled vocabularies will be not be filtered based on that field. Furthermore, if you leave all criteria blank then all controlled vocabularies will be listed.');
  $handler->override_option('header_format', '2');
  $handler->override_option('header_empty', 0);
  $handler->override_option('empty', 'No controlled vocabularies match the supplied criteria.');
  $handler->override_option('empty_format', '2');
  $handler->override_option('items_per_page', 50);
  $handler->override_option('style_plugin', 'table');
  $handler->override_option('style_options', array(
    'grouping' => '',
    'override' => 1,
    'sticky' => 0,
    'order' => 'asc',
    'summary' => '',
    'columns' => array(
      'name' => 'name',
      'definition' => 'definition',
      'nothing' => 'nothing',
    ),
    'info' => array(
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
    ),
    'default' => 'name',
  ));
  $handler = $view->new_display('page', 'Page', 'page_1');
  $handler->override_option('path', 'admin/tripal/tripal_cv/list_cvs');
  $handler->override_option('menu', array(
    'type' => 'normal',
    'title' => 'CV Listing',
    'description' => 'A listing of all controlled vocabularies',
    'weight' => '10',
    'name' => 'navigation',
  ));
  $handler->override_option('tab_options', array(
    'type' => 'none',
    'title' => '',
    'description' => '',
    'weight' => 0,
    'name' => 'navigation',
  ));
  $views[$view->name] = $view;

  return $views;
}
