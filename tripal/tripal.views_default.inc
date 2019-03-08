<?php
/**
 * @file
 * Describes core default views
 */

/**
 * Describes core default views
 *
 * @ingroup tripal
 */
function tripal_views_default_views() {
  $views = [];

  $view = tripal_admin_defaultview_jobs();
  $views[$view->name] = $view;

  // Add in the views for existing content types.
  tripal_bundle_default_views($views);

  return $views;
}

function tripal_bundle_default_views(&$views) {
  // Get the list of all of the bundles (entity types) and add them
  // as "base tables" for views.
  $bundles = db_select('tripal_bundle', 'tb')
    ->fields('tb')
    ->execute();

  // Iterate through the bundles.
  while ($bundle = $bundles->fetchObject()) {

    // The base table for a TripalEntity content type is simply the
    // vocab and the accession for the term.  It's not really a table
    // but we use that nomenclature for views.
    $term = tripal_load_term_entity(['term_id' => $bundle->term_id]);
    $base_table = $term->vocab->vocabulary . '__' . $term->accession;
    $view_name = preg_replace('/[^\w]/', '_', $bundle->label) . '_search';

    // Get the fields that are attached to this bundle type.
    $fields = field_info_instances('TripalEntity', $bundle->name);

    // Initalize the view.
    $view = new view();
    $view->name = $view_name;
    $view->description = 'A search tool for ' . $bundle->label . ' content.';
    $view->tag = $bundle->label . ' search';
    $view->base_table = $base_table;
    $view->human_name = $bundle->label . ' Search';
    $view->core = 7;
    $view->api_version = '3.0';
    $view->disabled = FALSE; /* Edit this to true to make a default view disabled initially */


    /* Display: Defaults */
    $handler = $view->new_display('default', 'Defaults', 'default');
    $handler->display->display_options['title'] = $bundle->label . ' Search';
    $handler->display->display_options['use_more_always'] = FALSE;
    $handler->display->display_options['access']['type'] = 'perm';
    $handler->display->display_options['access']['perm'] = 'view ' . $bundle->name;
    $handler->display->display_options['cache']['type'] = 'none';
    $handler->display->display_options['query']['type'] = 'views_query';
    $handler->display->display_options['exposed_form']['type'] = 'input_required';
    $handler->display->display_options['exposed_form']['options']['submit_button'] = 'Search';
    $handler->display->display_options['exposed_form']['options']['text_input_required_format'] = 'filtered_html';
    $handler->display->display_options['pager']['type'] = 'full';
    $handler->display->display_options['pager']['options']['items_per_page'] = '10';
    $handler->display->display_options['pager']['options']['offset'] = '0';
    $handler->display->display_options['pager']['options']['id'] = '0';
    $handler->display->display_options['pager']['options']['quantity'] = '9';

    // Start the default display style options.
    $handler->display->display_options['style_plugin'] = 'table';
    $handler->display->display_options['style_options']['grouping'] = '';

    // We can't have all fields show up as columns in a table so we have
    // to be selective and choose those that are most likely to be most
    // descriptive of a content type.
    $columns = [];
    $default_fields = [
      'data__image',
      'data__identifier',
      'schema__name',
      'data__accession',
      'rdfs__label',
      'taxrank__genus',
      'taxrank__species',
      'obi__organism',
      'tpub__title',
      'schema__alternate_name',
      'schema__description',
      'tpub__abstract',
    ];
    $selected_fields = [];
    foreach ($default_fields as $field_name) {
      if (in_array($field_name, array_keys($fields))) {
        $selected_fields[] = $field_name;
      }
    }
    $handler->display->display_options['style_options']['default'] = '-1';

    // Add in the entity_id field.
    $handler->display->display_options['style_options']['columns']['entity_id'] = 'entity_id';
    $handler->display->display_options['style_options']['info']['entity_id'] = [
      'sortable' => 1,
      'separator' => '',
    ];
    $handler->display->display_options['fields']['entity_id']['id'] = 'entity_id';
    $handler->display->display_options['fields']['entity_id']['table'] = $base_table;
    $handler->display->display_options['fields']['entity_id']['field'] = 'entity_id';
    $handler->display->display_options['fields']['entity_id']['exclude'] = TRUE;

    // Add in other selected fields to the view.
    foreach ($selected_fields as $field_name) {
      $field = $fields[$field_name];

      // Make sure the table headers are there for this field.
      $handler->display->display_options['style_options']['columns'][$field_name] = $field_name;
      $handler->display->display_options['style_options']['info'][$field_name]['separator'] = '';

      // Add in the current field.
      $handler->display->display_options['fields'][$field_name]['id'] = $field_name;
      $handler->display->display_options['fields'][$field_name]['table'] = $base_table;
      $handler->display->display_options['fields'][$field_name]['field'] = $field_name;
      $handler->display->display_options['fields'][$field_name]['label'] = $field['label'];

      // Only some fields are sortable.
      if (in_array($field_name, [
        'data__identifier',
        'schema__name',
        'data__accession',
        'rdfs__label',
        'taxrank__genus',
        'taxrank__species',
        'obi__organism',
        'schema__alternate_name',
        'tpub__title',
      ])) {
        $handler->display->display_options['style_options']['info'][$field_name]['sortable'] = 1;
        $handler->display->display_options['style_options']['info'][$field_name]['default_sort_order'] = 'asc';
      }

      // The name or identifier fields should link to the record.
      if ($field_name == 'data__identifier' or $field_name == 'schema__name' or
        $field_name == 'taxrank__genus' or $field_name == 'taxrank__species' or
        $field_name == 'tpub__title') {
        $handler->display->display_options['fields'][$field_name]['alter']['make_link'] = TRUE;
        $handler->display->display_options['fields'][$field_name]['alter']['path'] = 'bio_data/[entity_id]';
      }

      // Set a default image width to 100px.
      if ($field_name == 'data__image') {
        $handler->display->display_options['fields']['data__image']['image_width'] = '100';
      }

      // Add a 'read more' link to the description field if it's too big
      if ($field_name == 'schema__description') {
        $handler->display->display_options['fields']['schema__description']['alter']['max_length'] = '512';
        $handler->display->display_options['fields']['schema__description']['alter']['more_link'] = TRUE;
        $handler->display->display_options['fields']['schema__description']['alter']['more_link_text'] = 'read more';
        $handler->display->display_options['fields']['schema__description']['alter']['more_link_path'] = 'bio_data/[entity_id]';
        $handler->display->display_options['fields']['schema__description']['alter']['trim'] = TRUE;
      }

      // Add a filter to show published content only.
      $handler->display->display_options['filters']['status']['id'] = 'status';
      $handler->display->display_options['filters']['status']['table'] = $base_table;
      $handler->display->display_options['filters']['status']['field'] = 'status';
      $handler->display->display_options['filters']['status']['value'] = '1';

      // Filter criterion.
      if (in_array($field_name, [
        'data__identifier',
        'schema__name',
        'data__accession',
        'rdfs__label',
        'taxrank__genus',
        'taxrank__species',
        'obi__organism',
        'tpub__title',
      ])) {
        $handler->display->display_options['filters'][$field_name]['id'] = $field_name;
        $handler->display->display_options['filters'][$field_name]['table'] = $base_table;
        $handler->display->display_options['filters'][$field_name]['field'] = $field_name;
        $handler->display->display_options['filters'][$field_name]['operator'] = 'contains';
        $handler->display->display_options['filters'][$field_name]['group'] = 1;
        $handler->display->display_options['filters'][$field_name]['exposed'] = TRUE;
        $handler->display->display_options['filters'][$field_name]['expose']['operator_id'] = $field_name . '_op';
        $handler->display->display_options['filters'][$field_name]['expose']['label'] = $field['label'];
        $handler->display->display_options['filters'][$field_name]['expose']['use_operator'] = TRUE;
        $handler->display->display_options['filters'][$field_name]['expose']['operator'] = $field_name . '_op';
        $handler->display->display_options['filters'][$field_name]['expose']['identifier'] = $field_name;
        $handler->display->display_options['filters'][$field_name]['expose']['remember_roles'] = [
          2 => '2',
          1 => 0,
          3 => 0,
        ];
        $handler->display->display_options['filters'][$field_name]['select_optional'] = TRUE;
        $handler->display->display_options['filters'][$field_name]['max_length'] = '40';
      }
    }

    // Add the default sorted column.
    if (in_array('data__identifier', $selected_fields)) {
      $handler->display->display_options['sorts']['priority']['id'] = $field_name;
      $handler->display->display_options['sorts']['priority']['table'] = $base_table;
      $handler->display->display_options['sorts']['priority']['field'] = 'data__identifier';
    }
    else {
      if (in_array('schema__name', $selected_fields)) {
        $handler->display->display_options['sorts']['priority']['id'] = $field_name;
        $handler->display->display_options['sorts']['priority']['table'] = $base_table;
        $handler->display->display_options['sorts']['priority']['field'] = 'schema__name';
      }
      else {
        if (in_array('obi__organism', $selected_fields)) {
          $handler->display->display_options['sorts']['priority']['id'] = $field_name;
          $handler->display->display_options['sorts']['priority']['table'] = $base_table;
          $handler->display->display_options['sorts']['priority']['field'] = 'obi__organism';
        }
        else {
          if (in_array('rdfs_label', $selected_fields)) {
            $handler->display->display_options['sorts']['priority']['id'] = $field_name;
            $handler->display->display_options['sorts']['priority']['table'] = $base_table;
            $handler->display->display_options['sorts']['priority']['field'] = 'rdfs_label';
          }
          else {
            if (in_array('taxrank__genus', $selected_fields)) {
              $handler->display->display_options['sorts']['priority']['id'] = $field_name;
              $handler->display->display_options['sorts']['priority']['table'] = $base_table;
              $handler->display->display_options['sorts']['priority']['field'] = 'taxrank__genus';
            }
            else {
              if (in_array('taxrank__species', $selected_fields)) {
                $handler->display->display_options['sorts']['priority']['id'] = $field_name;
                $handler->display->display_options['sorts']['priority']['table'] = $base_table;
                $handler->display->display_options['sorts']['priority']['field'] = 'taxrank__species';
              }
            }
          }
        }
      }
    }

    /* Footer: Global: Tripal Content Data Collections */
    $handler->display->display_options['footer']['tripal_area_collections']['id'] = 'tripal_area_collections';
    $handler->display->display_options['footer']['tripal_area_collections']['table'] = 'views';
    $handler->display->display_options['footer']['tripal_area_collections']['field'] = 'tripal_area_collections';

    // No results behavior: Global: Text area.
    $handler->display->display_options['empty']['text']['id'] = 'text';
    $handler->display->display_options['empty']['text']['table'] = 'views';
    $handler->display->display_options['empty']['text']['field'] = 'area';
    $handler->display->display_options['empty']['text']['empty'] = TRUE;
    $handler->display->display_options['empty']['text']['content'] = 'No ' . strtolower($bundle->label) . ' records matched the supplied criteria.';
    $handler->display->display_options['empty']['text']['format'] = 'filtered_html';

    // Add page and menu.
    $handler = $view->new_display('page', 'Page', 'page_1');
    $handler->display->display_options['path'] = 'data_search/' . strtolower(preg_replace('/[^\w]/', '_', $bundle->label));
    $handler->display->display_options['menu']['type'] = 'normal';
    $handler->display->display_options['menu']['title'] = $bundle->label . ' Search';
    $handler->display->display_options['menu']['description'] = 'A search form for finding ' . $bundle->label . ' records';
    $handler->display->display_options['menu']['weight'] = '-10';
    $handler->display->display_options['menu']['name'] = 'data_search';
    $handler->display->display_options['menu']['context'] = 0;
    $handler->display->display_options['menu']['context_only_inline'] = 0;

    $views[$view_name] = $view;
  }
}

/**
 * Describes the jobs administration view.
 *
 * @ingroup tripal
 */
function tripal_admin_defaultview_jobs() {

  $view = new view();
  $view->name = 'tripal_admin_jobs';
  $view->description = 'DO NOT DISABLE';
  $view->tag = 'tripal admin';
  $view->base_table = 'tripal_jobs';
  $view->human_name = 'Tripal Jobs (Admin)';
  $view->core = 7;
  $view->api_version = '3.0';
  $view->disabled = FALSE; /* Edit this to true to make a default view disabled initially */

  /* Display: Master */
  $handler = $view->new_display('default', 'Master', 'default');
  $handler->display->display_options['title'] = 'Jobs';
  $handler->display->display_options['use_more_always'] = FALSE;
  $handler->display->display_options['access']['type'] = 'perm';
  $handler->display->display_options['access']['perm'] = 'administer tripal';
  $handler->display->display_options['cache']['type'] = 'none';
  $handler->display->display_options['query']['type'] = 'views_query';
  $handler->display->display_options['exposed_form']['type'] = 'basic';
  $handler->display->display_options['exposed_form']['options']['submit_button'] = 'Filter';
  $handler->display->display_options['pager']['type'] = 'full';
  $handler->display->display_options['pager']['options']['items_per_page'] = '25';
  $handler->display->display_options['style_plugin'] = 'table';
  $handler->display->display_options['style_options']['columns'] = [
    'job_id' => 'job_id',
    'job_name' => 'job_name',
    'submit_date' => 'submit_date',
    'start_time' => 'start_time',
    'end_time' => 'end_time',
    'nothing' => 'nothing',
    'priority' => 'priority',
    'progress' => 'progress',
    'status' => 'status',
    'nothing_2' => 'nothing_2',
    'nothing_3' => 'nothing_3',
    'nothing_1' => 'nothing_1',
  ];
  $handler->display->display_options['style_options']['default'] = 'job_id';
  $handler->display->display_options['style_options']['info'] = [
    'job_id' => [
      'sortable' => 1,
      'default_sort_order' => 'desc',
      'align' => '',
      'separator' => '',
      'empty_column' => 0,
    ],
    'job_name' => [
      'sortable' => 1,
      'default_sort_order' => 'asc',
      'align' => '',
      'separator' => '',
      'empty_column' => 0,
    ],
    'submit_date' => [
      'sortable' => 0,
      'default_sort_order' => 'asc',
      'align' => '',
      'separator' => '',
      'empty_column' => 0,
    ],
    'start_time' => [
      'sortable' => 0,
      'default_sort_order' => 'asc',
      'align' => '',
      'separator' => '',
      'empty_column' => 0,
    ],
    'end_time' => [
      'sortable' => 0,
      'default_sort_order' => 'asc',
      'align' => '',
      'separator' => '',
      'empty_column' => 0,
    ],
    'nothing' => [
      'align' => '',
      'separator' => '',
      'empty_column' => 0,
    ],
    'priority' => [
      'sortable' => 1,
      'default_sort_order' => 'asc',
      'align' => '',
      'separator' => '',
      'empty_column' => 0,
    ],
    'progress' => [
      'sortable' => 1,
      'default_sort_order' => 'asc',
      'align' => '',
      'separator' => '',
      'empty_column' => 0,
    ],
    'status' => [
      'sortable' => 1,
      'default_sort_order' => 'asc',
      'align' => '',
      'separator' => '',
      'empty_column' => 0,
    ],
    'nothing_2' => [
      'align' => '',
      'separator' => '',
      'empty_column' => 0,
    ],
    'nothing_3' => [
      'align' => '',
      'separator' => '',
      'empty_column' => 0,
    ],
    'nothing_1' => [
      'align' => '',
      'separator' => '',
      'empty_column' => 0,
    ],
  ];
  /* Header: Global: Text area */
  $handler->display->display_options['header']['area']['id'] = 'area';
  $handler->display->display_options['header']['area']['table'] = 'views';
  $handler->display->display_options['header']['area']['field'] = 'area';
  $handler->display->display_options['header']['area']['label'] = 'Description';
  $handler->display->display_options['header']['area']['empty'] = TRUE;
  $handler->display->display_options['header']['area']['content'] = '<p>Waiting jobs are executed first by priority level (the lower the number the higher the priority) and second by the order they were entered.</p>';
  $handler->display->display_options['header']['area']['format'] = 'filtered_html';
  /* Field: Tripal Jobs: Job ID */
  $handler->display->display_options['fields']['job_id']['id'] = 'job_id';
  $handler->display->display_options['fields']['job_id']['table'] = 'tripal_jobs';
  $handler->display->display_options['fields']['job_id']['field'] = 'job_id';
  $handler->display->display_options['fields']['job_id']['label'] = '';
  $handler->display->display_options['fields']['job_id']['element_class'] = 'extra-short-column';
  $handler->display->display_options['fields']['job_id']['element_label_class'] = 'extra-short-column';
  $handler->display->display_options['fields']['job_id']['element_label_colon'] = FALSE;
  $handler->display->display_options['fields']['job_id']['separator'] = '';
  /* Field: Tripal Jobs: Job Name */
  $handler->display->display_options['fields']['job_name']['id'] = 'job_name';
  $handler->display->display_options['fields']['job_name']['table'] = 'tripal_jobs';
  $handler->display->display_options['fields']['job_name']['field'] = 'job_name';
  $handler->display->display_options['fields']['job_name']['alter']['make_link'] = TRUE;
  $handler->display->display_options['fields']['job_name']['alter']['path'] = 'admin/tripal/tripal_jobs/view/[job_id]';
  /* Field: Tripal Jobs: Submit Date */
  $handler->display->display_options['fields']['submit_date']['id'] = 'submit_date';
  $handler->display->display_options['fields']['submit_date']['table'] = 'tripal_jobs';
  $handler->display->display_options['fields']['submit_date']['field'] = 'submit_date';
  $handler->display->display_options['fields']['submit_date']['exclude'] = TRUE;
  $handler->display->display_options['fields']['submit_date']['date_format'] = 'long';
  /* Field: Tripal Jobs: Start Time */
  $handler->display->display_options['fields']['start_time']['id'] = 'start_time';
  $handler->display->display_options['fields']['start_time']['table'] = 'tripal_jobs';
  $handler->display->display_options['fields']['start_time']['field'] = 'start_time';
  $handler->display->display_options['fields']['start_time']['exclude'] = TRUE;
  $handler->display->display_options['fields']['start_time']['date_format'] = 'long';
  /* Field: Tripal Jobs: End Time */
  $handler->display->display_options['fields']['end_time']['id'] = 'end_time';
  $handler->display->display_options['fields']['end_time']['table'] = 'tripal_jobs';
  $handler->display->display_options['fields']['end_time']['field'] = 'end_time';
  $handler->display->display_options['fields']['end_time']['exclude'] = TRUE;
  $handler->display->display_options['fields']['end_time']['date_format'] = 'long';
  /* Field: Global: Custom text */
  $handler->display->display_options['fields']['nothing']['id'] = 'nothing';
  $handler->display->display_options['fields']['nothing']['table'] = 'views';
  $handler->display->display_options['fields']['nothing']['field'] = 'nothing';
  $handler->display->display_options['fields']['nothing']['label'] = 'Dates';
  $handler->display->display_options['fields']['nothing']['alter']['text'] = 'Submitted: [submit_date]<br />
  Started: [start_time]<br />
  Ended: [end_time]';
  /* Field: Tripal Jobs: Priority */
  $handler->display->display_options['fields']['priority']['id'] = 'priority';
  $handler->display->display_options['fields']['priority']['table'] = 'tripal_jobs';
  $handler->display->display_options['fields']['priority']['field'] = 'priority';
  $handler->display->display_options['fields']['priority']['element_class'] = 'short-column';
  $handler->display->display_options['fields']['priority']['element_label_class'] = 'short-column';
  $handler->display->display_options['fields']['priority']['separator'] = '';
  /* Field: Tripal Jobs: Progress */
  $handler->display->display_options['fields']['progress']['id'] = 'progress';
  $handler->display->display_options['fields']['progress']['table'] = 'tripal_jobs';
  $handler->display->display_options['fields']['progress']['field'] = 'progress';
  $handler->display->display_options['fields']['progress']['element_class'] = 'short-column';
  $handler->display->display_options['fields']['progress']['element_label_class'] = 'short-column';
  $handler->display->display_options['fields']['progress']['separator'] = '';
  $handler->display->display_options['fields']['progress']['suffix'] = '%';
  /* Field: Tripal Jobs: Status */
  $handler->display->display_options['fields']['status']['id'] = 'status';
  $handler->display->display_options['fields']['status']['table'] = 'tripal_jobs';
  $handler->display->display_options['fields']['status']['field'] = 'status';
  $handler->display->display_options['fields']['status']['element_class'] = 'short-column';
  $handler->display->display_options['fields']['status']['element_label_class'] = 'short-column';
  /* Field: Global: Custom text */
  $handler->display->display_options['fields']['nothing_2']['id'] = 'nothing_2';
  $handler->display->display_options['fields']['nothing_2']['table'] = 'views';
  $handler->display->display_options['fields']['nothing_2']['field'] = 'nothing';
  $handler->display->display_options['fields']['nothing_2']['label'] = 'Re-run Link';
  $handler->display->display_options['fields']['nothing_2']['exclude'] = TRUE;
  $handler->display->display_options['fields']['nothing_2']['alter']['text'] = 'Re-run';
  $handler->display->display_options['fields']['nothing_2']['alter']['make_link'] = TRUE;
  $handler->display->display_options['fields']['nothing_2']['alter']['path'] = 'admin/tripal/tripal_jobs/rerun/[job_id]';
  /* Field: Global: Custom text */
  $handler->display->display_options['fields']['nothing_3']['id'] = 'nothing_3';
  $handler->display->display_options['fields']['nothing_3']['table'] = 'views';
  $handler->display->display_options['fields']['nothing_3']['field'] = 'nothing';
  $handler->display->display_options['fields']['nothing_3']['label'] = 'Cancel link';
  $handler->display->display_options['fields']['nothing_3']['exclude'] = TRUE;
  $handler->display->display_options['fields']['nothing_3']['alter']['text'] = 'Cancel';
  $handler->display->display_options['fields']['nothing_3']['alter']['make_link'] = TRUE;
  $handler->display->display_options['fields']['nothing_3']['alter']['path'] = 'admin/tripal/tripal_jobs/cancel/[job_id]';
  /* Field: Global: Custom text */
  $handler->display->display_options['fields']['nothing_4']['id'] = 'nothing_4';
  $handler->display->display_options['fields']['nothing_4']['table'] = 'views';
  $handler->display->display_options['fields']['nothing_4']['field'] = 'nothing';
  $handler->display->display_options['fields']['nothing_4']['label'] = 'Execute link';
  $handler->display->display_options['fields']['nothing_4']['exclude'] = TRUE;
  $handler->display->display_options['fields']['nothing_4']['alter']['text'] = 'Execute';
  $handler->display->display_options['fields']['nothing_4']['alter']['make_link'] = TRUE;
  $handler->display->display_options['fields']['nothing_4']['alter']['path'] = 'admin/tripal/tripal_jobs/execute/[job_id]';
  /* Field: Global: Custom text */
  $handler->display->display_options['fields']['nothing_1']['id'] = 'nothing_1';
  $handler->display->display_options['fields']['nothing_1']['table'] = 'views';
  $handler->display->display_options['fields']['nothing_1']['field'] = 'nothing';
  $handler->display->display_options['fields']['nothing_1']['label'] = 'Action';
  $handler->display->display_options['fields']['nothing_1']['alter']['text'] = '[nothing_4]<br />[nothing_2]<br />[nothing_3]';
  $handler->display->display_options['fields']['nothing_1']['element_class'] = 'short-column';
  $handler->display->display_options['fields']['nothing_1']['element_label_class'] = 'short-column';
  /* Filter criterion: Tripal Jobs: Job Name */
  $handler->display->display_options['filters']['job_name']['id'] = 'job_name';
  $handler->display->display_options['filters']['job_name']['table'] = 'tripal_jobs';
  $handler->display->display_options['filters']['job_name']['field'] = 'job_name';
  $handler->display->display_options['filters']['job_name']['operator'] = 'contains';
  $handler->display->display_options['filters']['job_name']['exposed'] = TRUE;
  $handler->display->display_options['filters']['job_name']['expose']['operator_id'] = 'job_name_op';
  $handler->display->display_options['filters']['job_name']['expose']['label'] = 'Job Name Contains';
  $handler->display->display_options['filters']['job_name']['expose']['operator'] = 'job_name_op';
  $handler->display->display_options['filters']['job_name']['expose']['identifier'] = 'job_name';
  $handler->display->display_options['filters']['job_name']['expose']['remember_roles'] = [
    2 => '2',
    1 => 0,
    3 => 0,
  ];
  /* Filter criterion: Tripal Jobs: Module Name */
  $handler->display->display_options['filters']['modulename']['id'] = 'modulename';
  $handler->display->display_options['filters']['modulename']['table'] = 'tripal_jobs';
  $handler->display->display_options['filters']['modulename']['field'] = 'modulename';
  $handler->display->display_options['filters']['modulename']['exposed'] = TRUE;
  $handler->display->display_options['filters']['modulename']['expose']['operator_id'] = 'modulename_op';
  $handler->display->display_options['filters']['modulename']['expose']['label'] = 'Submitting Module';
  $handler->display->display_options['filters']['modulename']['expose']['operator'] = 'modulename_op';
  $handler->display->display_options['filters']['modulename']['expose']['identifier'] = 'modulename';
  $handler->display->display_options['filters']['modulename']['expose']['remember_roles'] = [
    2 => '2',
    1 => 0,
    3 => 0,
  ];
  /* Filter criterion: Tripal Jobs: Status */
  $handler->display->display_options['filters']['status']['id'] = 'status';
  $handler->display->display_options['filters']['status']['table'] = 'tripal_jobs';
  $handler->display->display_options['filters']['status']['field'] = 'status';
  $handler->display->display_options['filters']['status']['exposed'] = TRUE;
  $handler->display->display_options['filters']['status']['expose']['operator_id'] = 'status_op';
  $handler->display->display_options['filters']['status']['expose']['label'] = 'Job Status';
  $handler->display->display_options['filters']['status']['expose']['operator'] = 'status_op';
  $handler->display->display_options['filters']['status']['expose']['identifier'] = 'status';
  $handler->display->display_options['filters']['status']['expose']['remember_roles'] = [
    2 => '2',
    1 => 0,
    3 => 0,
  ];

  /* Display: Page */
  $handler = $view->new_display('page', 'Page', 'page');
  $handler->display->display_options['path'] = 'admin/tripal/tripal_jobs/jobs';
  $handler->display->display_options['menu']['title'] = 'Jobs';
  $handler->display->display_options['menu']['description'] = 'Jobs managed by Tripal';
  $handler->display->display_options['menu']['weight'] = '-10';
  $handler->display->display_options['menu']['context'] = 0;
  $handler->display->display_options['menu']['context_only_inline'] = 0;
  $handler->display->display_options['tab_options']['weight'] = '0';

  return $view;
}
