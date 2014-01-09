<?php

/**
 * Implements hook_views_default_views().
 *
 * @ingroup tripal_analysis_views
 */
function tripal_analysis_views_default_views() {
  $views = array();

  if (!module_exists('tripal_views')) {
    return $views;
  }

  // Main default view
  $view = new view;
  $view->name = 'analysis_listing';
  $view->description = 'A default listing of analyses provided by Tripal';
  $view->tag = 'chado default';
  $view->base_table = 'analysis';
  $view->core = 0;
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
      'link_to_node' => 1,
      'id' => 'name',
      'table' => 'analysis',
      'field' => 'name',
      'relationship' => 'none',
    ),
    'program' => array(
      'label' => 'Program',
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
      'exclude' => 0,
      'id' => 'program',
      'table' => 'analysis',
      'field' => 'program',
      'relationship' => 'none',
    ),
    'sourcename' => array(
      'label' => 'Source',
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
      'exclude' => 0,
      'id' => 'sourcename',
      'table' => 'analysis',
      'field' => 'sourcename',
      'relationship' => 'none',
    ),
    'timeexecuted' => array(
      'label' => 'Time Executed',
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
      'date_format' => 'custom',
      'custom_date_format' => 'F j, Y',
      'exclude' => 0,
      'id' => 'timeexecuted',
      'table' => 'analysis',
      'field' => 'timeexecuted',
      'override' => array(
        'button' => 'Override',
      ),
      'relationship' => 'none',
    ),
  ));
  $handler->override_option('filters', array(
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
      'apply_button' => 'Show ',
      'no_results_text' => 'Click "Show" to see a list of all analysis matching the entered criteria. If you leave a any of the criteria blank then the analysis will be not be filtered based on that field. Furthermore, if you leave all criteria blank then all analysis will be listed.',
    ),
    'program' => array(
      'operator' => '=',
      'value' => '',
      'group' => '0',
      'exposed' => TRUE,
      'expose' => array(
        'use_operator' => 0,
        'operator' => 'program_op',
        'identifier' => 'program',
        'label' => 'Program',
        'remember' => 0,
      ),
      'case' => 1,
      'id' => 'program',
      'table' => 'analysis',
      'field' => 'program',
      'relationship' => 'none',
      'values_form_type' => 'select',
      'multiple' => 1,
      'optional' => 0,
      'agg' => array(
        'records_with' => 1,
        'aggregates_with' => 0,
      ),
    ),
    'timeexecuted' => array(
      'operator' => '>',
      'value' => array(
        'type' => 'date',
        'value' => '',
        'min' => '',
        'max' => '',
      ),
      'group' => '0',
      'exposed' => TRUE,
      'expose' => array(
        'use_operator' => 1,
        'operator' => 'timeexecuted_op',
        'identifier' => 'timeexecuted',
        'label' => 'Time Executed',
        'optional' => 1,
        'remember' => 0,
      ),
      'id' => 'timeexecuted',
      'table' => 'analysis',
      'field' => 'timeexecuted',
      'relationship' => 'none',
      'agg' => array(
        'records_with' => 1,
        'aggregates_with' => 0,
      ),
    ),
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
        'bef_filter_description' => '',
        'remember' => 0,
      ),
      'case' => 0,
      'id' => 'name',
      'table' => 'analysis',
      'field' => 'name',
      'relationship' => 'none',
      'values_form_type' => 'textfield',
      'multiple' => 0,
      'optional' => 0,
    ),
    'sourcename' => array(
      'operator' => '~',
      'value' => '',
      'group' => '0',
      'exposed' => TRUE,
      'expose' => array(
        'use_operator' => 0,
        'operator' => 'sourcename_op',
        'identifier' => 'sourcename',
        'label' => 'Source Contains',
        'bef_filter_description' => '',
        'remember' => 0,
      ),
      'case' => 0,
      'id' => 'sourcename',
      'table' => 'analysis',
      'field' => 'sourcename',
      'relationship' => 'none',
    ),
  ));
  $handler->override_option('access', array(
    'type' => 'perm',
    'perm' => 'access chado_analysis content',
  ));
  $handler->override_option('cache', array(
    'type' => 'none',
  ));
  $handler->override_option('title', 'Analysis');
  $handler->override_option('header', 'Click "Show" to see a list of all analysis matching the entered criteria. If you leave a any of the criteria blank then the analysis will be not be filtered based on that field. Furthermore, if you leave all criteria blank then all analysis will be listed.');
  $handler->override_option('header_format', '2');
  $handler->override_option('header_empty', 1);
  $handler->override_option('empty', 'No analyses match the supplied criteria.');
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
      'name' => 'name',
      'program' => 'program',
      'sourcename' => 'sourcename',
      'num_features' => 'num_features',
      'timeexecuted' => 'timeexecuted',
    ),
    'info' => array(
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
    ),
    'default' => 'name',
  ));
  // this is needed so that fields/filters can be added to the default display later
  $default_handler = $handler;
  $handler = $view->new_display('page', 'Page', 'page_1');
  $handler->override_option('path', 'chado/analyses');
  $handler->override_option('menu', array(
    'type' => 'normal',
    'title' => 'Analyses',
    'description' => 'An analysis is a particular type of a computational analysis; it may be a blast of one sequence against another, or an all by all blast, or a different kind of analysis altogether. It is a single unit of computation.',
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
  // Add code specific to a local chado installation
  // NOTE: Edit $handler above to $default_handler for the default display
  if (tripal_core_chado_schema_exists()) {
    // Add nid field
    $fields = $view->get_items('field', 'default');
    $new_fields = array(
      'nid' => array(
        'label' => 'Nid',
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
        'link_to_node' => 0,
        'exclude' => 1,
        'id' => 'nid',
        'table' => 'node',
        'field' => 'nid',
        'relationship' => 'none',
      )
    );
    $fields = $new_fields + $fields;
    // Adds analysis => Node relationship
    $default_handler->override_option('relationships', array(
      'nid' => array(
        'label' => 'Analysis to Node',
        'required' => 0,
        'id' => 'nid',
        'table' => 'chado_analysis',
        'field' => 'nid',
        'relationship' => 'none',
      ),
    ));
    // Change analysis.name to have a link to the node
    $fields['name']['link_to_node'] = 1;
    $default_handler->override_option('fields', $fields);
    // Only show records with published nodes
    /**
    $filters = $view->get_items('filter', 'default');
    $filters['status'] = array(
      'operator' => '=',
      'value' => '1',
      'group' => '0',
      'exposed' => FALSE,
      'expose' => array(
        'operator' => FALSE,
        'label' => '',
      ),
      'id' => 'status',
      'table' => 'node',
      'field' => 'status',
      'relationship' => 'none',
    );
    $default_handler->override_option('filters', $filters);
    */
  }
  $views[$view->name] = $view;

  return $views;
}
