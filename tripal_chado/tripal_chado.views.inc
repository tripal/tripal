<?php
/**
 * @file
 * Integrates many of the core database tables with drupal views
 */

/**
 * Describe various Tripal Core systems to Views for the creation of
 * administrative views.
 *
 * @ingroup tripal
 */
function tripal_chado_views_data() {
  $data = [];

  // Custom Tables Management
  $data = tripal_chado_views_data_custom_tables($data);

  // Materialized Views Management
  $data = tripal_chado_views_data_mviews($data);

  return $data;
}

/**
 * Provides the data array for the tripal custom tables management
 *
 * @param $data
 *   Previously generated tripal views data array
 * return
 *   $data array with custom tables management described
 *
 * @ingroup tripal
 */
function tripal_chado_views_data_custom_tables($data) {

  $data['tripal_custom_tables']['table']['group'] = t('Tripal Custom Tables');
  $data['tripal_custom_tables']['table']['base'] = [
    'field' => 'table_id', // This is the identifier field for the view.
    'title' => t('Tripal Custom Tables'),
    'help' => t('Custom Tables in Chado created by this Tripal Installation.'),
    'weight' => 10,
  ];

  // Table ID
  $data['tripal_custom_tables']['table_id'] = [
    'title' => t('Custom Table ID'),
    'help' => t('Custom table primary key.'),
    'field' => [
      'handler' => 'views_handler_field_numeric',
      'click sortable' => TRUE,
    ],
    'filter' => [
      'handler' => 'views_handler_filter_numeric',
    ],
    'sort' => [
      'handler' => 'views_handler_sort',
    ],
  ];

  // Table Name
  $data['tripal_custom_tables']['table_name'] = [
    'title' => t('Table Name'),
    'help' => t('The name of the table in the database.'),
    'field' => [
      'handler' => 'views_handler_field',
      'click sortable' => TRUE, // This is use by the table display plugin.
    ],
    'sort' => [
      'handler' => 'views_handler_sort',
    ],
    'filter' => [
      'handler' => 'views_handler_filter_string',
    ],
    'argument' => [
      'handler' => 'views_handler_argument_string',
    ],
  ];

  // Schema
  $data['tripal_custom_tables']['schema'] = [
    'title' => t('Table Schema'),
    'help' => t('The schema definition of the table.'),
    'field' => [
      'handler' => 'views_handler_field',
      'click sortable' => TRUE, // This is use by the table display plugin.
    ],
    'sort' => [
      'handler' => 'views_handler_sort',
    ],
    'filter' => [
      'handler' => 'views_handler_filter_string',
    ],
    'argument' => [
      'handler' => 'views_handler_argument_string',
    ],
  ];

  // Table ID
  $data['tripal_custom_tables']['mview_id'] = [
    'title' => t('Materialized View ID'),
    'help' => t('Foreign key to tripal_mviews table for the materialized view.'),
    'field' => [
      'handler' => 'views_handler_field_numeric',
      'click sortable' => TRUE,
    ],
    'filter' => [
      'handler' => 'views_handler_filter_numeric',
    ],
    'sort' => [
      'handler' => 'views_handler_sort',
    ],
  ];


  return $data;
}

/**
 * Provides the data array for the tripal custom tables management
 *
 * @param $data
 *   Previously generated tripal views data array
 * return
 *   $data array with custom tables management described
 *
 * @ingroup tripal
 */
function tripal_chado_views_data_mviews($data) {

  $data['tripal_mviews']['table']['group'] = t('Tripal Materialized Views');
  $data['tripal_mviews']['table']['base'] = [
    'field' => 'mview_id', // This is the identifier field for the view.
    'title' => t('Tripal Materialized Views'),
    'help' => t('Materialized Views in Chado created by this Tripal Installation.'),
    'weight' => 10,
  ];

  // Implicit Join to Tripal Views
  $data['tripal_mviews']['table']['join'] = [
    'tripal_views' => [
      'left_field' => 'mview_id',
      'field' => 'mview_id',
    ],
  ];

  // Mview ID
  $data['tripal_mviews']['mview_id'] = [
    'title' => t('Materialized View ID'),
    'help' => t('The primary key.'),
    'field' => [
      'handler' => 'views_handler_field_numeric',
      'click sortable' => TRUE,
    ],
    'filter' => [
      'handler' => 'views_handler_filter_numeric',
    ],
    'sort' => [
      'handler' => 'views_handler_sort',
    ],
  ];

  // name
  $data['tripal_mviews']['name'] = [
    'title' => t('Name'),
    'help' => t('Human-readable name of the materialized view.'),
    'field' => [
      'handler' => 'views_handler_field',
      'click sortable' => TRUE, // This is use by the table display plugin.
    ],
    'sort' => [
      'handler' => 'views_handler_sort',
    ],
    'filter' => [
      'handler' => 'views_handler_filter_string',
    ],
    'argument' => [
      'handler' => 'views_handler_argument_string',
    ],
  ];

  // modulename
  $data['tripal_mviews']['modulename'] = [
    'title' => t('Module Name'),
    'help' => t('The module that created the materialized view.'),
    'field' => [
      'handler' => 'views_handler_field',
      'click sortable' => TRUE, // This is use by the table display plugin.
    ],
    'sort' => [
      'handler' => 'views_handler_sort',
    ],
    'filter' => [
      'handler' => 'views_handler_filter_string',
    ],
    'argument' => [
      'handler' => 'views_handler_argument_string',
    ],
  ];

  // mv_table
  $data['tripal_mviews']['mv_table'] = [
    'title' => t('Table'),
    'help' => t('The database table the materialized view is stored in.'),
    'field' => [
      'handler' => 'views_handler_field',
      'click sortable' => TRUE, // This is use by the table display plugin.
    ],
    'sort' => [
      'handler' => 'views_handler_sort',
    ],
    'filter' => [
      'handler' => 'views_handler_filter_string',
    ],
    'argument' => [
      'handler' => 'views_handler_argument_string',
    ],
  ];

  // mv_specs
  $data['tripal_mviews']['mv_specs'] = [
    'title' => t('Specification'),
    'help' => t('Materialized View Specification.'),
    'field' => [
      'handler' => 'views_handler_field',
      'click sortable' => TRUE, // This is use by the table display plugin.
    ],
    'sort' => [
      'handler' => 'views_handler_sort',
    ],
    'filter' => [
      'handler' => 'views_handler_filter_string',
    ],
    'argument' => [
      'handler' => 'views_handler_argument_string',
    ],
  ];

  // mv_schema
  $data['tripal_mviews']['mv_schema'] = [
    'title' => t('Schema'),
    'help' => t('Schema definition for the materialized view table.'),
    'field' => [
      'handler' => 'views_handler_field',
      'click sortable' => TRUE, // This is use by the table display plugin.
    ],
    'sort' => [
      'handler' => 'views_handler_sort',
    ],
    'filter' => [
      'handler' => 'views_handler_filter_string',
    ],
    'argument' => [
      'handler' => 'views_handler_argument_string',
    ],
  ];

  // indexed
  $data['tripal_mviews']['indexed'] = [
    'title' => t('Indices'),
    'help' => t('Any indices for this materialized view.'),
    'field' => [
      'handler' => 'views_handler_field',
      'click sortable' => TRUE, // This is use by the table display plugin.
    ],
    'sort' => [
      'handler' => 'views_handler_sort',
    ],
    'filter' => [
      'handler' => 'views_handler_filter_string',
    ],
    'argument' => [
      'handler' => 'views_handler_argument_string',
    ],
  ];

  // query
  $data['tripal_mviews']['query'] = [
    'title' => t('Query'),
    'help' => t('The query used to populate the materialized view.'),
    'field' => [
      'handler' => 'views_handler_field',
      'click sortable' => TRUE, // This is use by the table display plugin.
    ],
    'sort' => [
      'handler' => 'views_handler_sort',
    ],
    'filter' => [
      'handler' => 'views_handler_filter_string',
    ],
    'argument' => [
      'handler' => 'views_handler_argument_string',
    ],
  ];

  // special_index
  $data['tripal_mviews']['special_index'] = [
    'title' => t('Special Index'),
    'help' => t('Any special indices for the materialized view.'),
    'field' => [
      'handler' => 'views_handler_field',
      'click sortable' => TRUE, // This is use by the table display plugin.
    ],
    'sort' => [
      'handler' => 'views_handler_sort',
    ],
    'filter' => [
      'handler' => 'views_handler_filter_string',
    ],
    'argument' => [
      'handler' => 'views_handler_argument_string',
    ],
  ];

  // last_update
  $data['tripal_mviews']['last_update'] = [
    'title' => t('Updated'),
    'help' => t('Date Last Updated.'),
    'field' => [
      'handler' => 'views_handler_field_date',
      'click sortable' => TRUE,
    ],
    'sort' => [
      'handler' => 'views_handler_sort_date',
    ],
    'filter' => [
      'handler' => 'views_handler_filter_date',
    ],
  ];

  // status
  $data['tripal_mviews']['status'] = [
    'title' => t('Status'),
    'help' => t('The status of the materialized view.'),
    'field' => [
      'handler' => 'views_handler_field',
      'click sortable' => TRUE, // This is use by the table display plugin.
    ],
    'sort' => [
      'handler' => 'views_handler_sort',
    ],
    'filter' => [
      'handler' => 'views_handler_filter_string',
    ],
    'argument' => [
      'handler' => 'views_handler_argument_string',
    ],
  ];

  // comment
  $data['tripal_mviews']['comment'] = [
    'title' => t('Description'),
    'help' => t('Human-Readable Admin Description.'),
    'field' => [
      'handler' => 'views_handler_field',
      'click sortable' => TRUE, // This is use by the table display plugin.
    ],
    'sort' => [
      'handler' => 'views_handler_sort',
    ],
    'filter' => [
      'handler' => 'views_handler_filter_string',
    ],
    'argument' => [
      'handler' => 'views_handler_argument_string',
    ],
  ];

  return $data;
}
