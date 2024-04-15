<?php

/**
 * Describe various Tripal Core systems to Views
 *
 * @ingroup tripal
 */
function tripal_chado_views_data() {
  $data = [];

  tripal_chado_views_data_custom_tables($data);
  tripal_chado_views_data_mviews($data);
  return $data;
}


/**
 * Adds the Tripal Custom Tables View
 *
 * @param array $data
 *   A data array for creating a view.
 *
 */
function tripal_chado_views_data_custom_tables(&$data) {
  $data['tripal_custom_tables'] = [];
  $data['tripal_custom_tables']['table'] = [];
  $data['tripal_custom_tables']['table']['group'] = t('Chado Custom Tables');
  $data['tripal_custom_tables']['table']['provider'] = 'tripal';
  $data['tripal_custom_tables']['table']['base'] = [
      'field' => 'table_id',
      'title' => t('Tripal Custom Tables'),
      'help' => t('Provides information about custom tables in Chado.'),
      'weight' => 10,
  ];
  $data['tripal_custom_tables']['table']['join'] = [
    'tripal_mviews' => [
      'left_field' => 'table_id',
      'field' => 'table_id',
    ]
  ];
  $data['tripal_custom_tables']['table']['join']['tripal_mviews'] = [
    'left_table' => 'tripal_mviews',
    'left_field' => 'table_id',
    'field' => 'table_id',
  ];

  // Table ID
  $data['tripal_custom_tables']['table_id'] = [
    'title' => t('Table ID'),
    'help' => t('The custom table primary key.'),
    'field' => [
      'id' => 'numeric',
    ],
    'filter' => [
      'id' => 'numeric',
    ],
    'sort' => [
      'id' => 'standard'
    ],
    'argument' => [
      'id' => 'numeric',
    ],
    'relationship' => [
      'id' => 'standard',
      'base' => 'tripal_mviews',
      'base field' => 'table_id',
      'label' => t('Materialized Views.'),
    ],
  ];

  // Table Name
  $data['tripal_custom_tables']['table_name'] = [
    'title' => t('Table Name'),
    'help' => t('The name of the table.'),
    'field' => [
      'id' => 'standard',
    ],
    'sort' => [
      'id' => 'standard'
    ],
    'filter' => [
      'id' => 'string',
    ],
    'argument' => [
      'id' => 'string',
    ],
  ];

  $data['tripal_custom_tables']['locked'] = [
    'title' => t('Locked'),
    'help' => t('Indicates if the table is locked from end-users.'),
    'field' => [
      'id' => 'boolean',
    ],
    'sort' => [
      'id' => 'standard'
    ],
    'filter' => [
      'id' => 'string',
    ],
    'argument' => [
      'id' => 'string',
    ],
  ];

  $data['tripal_custom_tables']['chado'] = [
    'title' => t('Chado Schema'),
    'help' => t('The Chado schema in which the table is present.'),
    'field' => [
      'id' => 'standard',
    ],
    'sort' => [
      'id' => 'standard'
    ],
    'filter' => [
      'id' => 'string',
    ],
    'argument' => [
      'id' => 'string',
    ],
  ];

  $data['tripal_custom_tables']['edit_link'] = array(
    'title' => t('Edit Table'),
    'help' => t('Clickable link to edit a custom table'),
    'field' => [
      'id' => 'chado_custom_tables_edit_link'
    ],
  );

  $data['tripal_custom_tables']['delete_link'] = array(
    'title' => t('Delete Table'),
    'help' => t('Clickable link to delete a custom table'),
    'field' => [
      'id' => 'chado_custom_tables_delete_link'
    ],
  );
}

/**
 * Adds the Tripal Custom Tables View
 *
 * @param array $data
 *   A data array for creating a view.
 *
 */
function tripal_chado_views_data_mviews(&$data) {
  $data['tripal_mviews'] = [];
  $data['tripal_mviews']['table'] = [];
  $data['tripal_mviews']['table']['group'] = t('Chado Materialized Views');
  $data['tripal_mviews']['table']['provider'] = 'tripal';
  $data['tripal_mviews']['table']['base'] = [
    'field' => 'mview_id',
    'title' => t('Tripal Materialized views'),
    'help' => t('Provides information about materialized views in Chado.'),
    'weight' => 10,
  ];


  // Mview ID
  $data['tripal_mviews']['mview_id'] = [
    'title' => t('Materialized View ID'),
    'help' => t('The materialized view primary key.'),
    'field' => [
      'id' => 'numeric',
    ],
    'filter' => [
      'id' => 'numeric',
    ],
    'sort' => [
      'id' => 'standard'
    ],
    'argument' => [
      'id' => 'numeric',
    ],
  ];

  // Table ID
  $data['tripal_mviews']['table_id'] = [
    'title' => t('Custom Table ID'),
    'help' => t('The custom table foreign key.'),
    'field' => [
      'id' => 'numeric',
    ],
    'filter' => [
      'id' => 'numeric',
    ],
    'sort' => [
      'id' => 'standard'
    ],
    'argument' => [
      'id' => 'numeric',
    ],
  ];

  // Table Name
  $data['tripal_mviews']['name'] = [
    'title' => t('Table Name'),
    'help' => t('The name of the materialized view table.'),
    'field' => [
      'id' => 'standard',
    ],
    'sort' => [
      'id' => 'standard'
    ],
    'filter' => [
      'id' => 'string',
    ],
    'argument' => [
      'id' => 'string',
    ],
  ];

  // SQL Query
  $data['tripal_mviews']['query'] = [
    'title' => t('SQL Query'),
    'help' => t('The SQL query used to populate the view.'),
    'field' => [
      'id' => 'standard',
    ],
    'sort' => [
      'id' => 'standard'
    ],
    'filter' => [
      'id' => 'string',
    ],
    'argument' => [
      'id' => 'string',
    ],
  ];

  // Status
  $data['tripal_mviews']['status'] = [
    'title' => t('Status'),
    'help' => t('The status of the most recent population of the view.'),
    'field' => [
      'id' => 'standard',
    ],
    'sort' => [
      'id' => 'standard'
    ],
    'filter' => [
      'id' => 'string',
    ],
    'argument' => [
      'id' => 'string',
    ],
  ];

  // Comment
  $data['tripal_mviews']['comment'] = [
    'title' => t('Description'),
    'help' => t('A descption of this view.'),
    'field' => [
      'id' => 'standard',
    ],
    'sort' => [
      'id' => 'standard'
    ],
    'filter' => [
      'id' => 'string',
    ],
    'argument' => [
      'id' => 'string',
    ],
  ];

  // Last update
  $data['tripal_mviews']['last_update'] = [
    'title' => t('Last Update'),
    'help' => t('A descption of this view.'),
    'field' => [
      'id' => 'date',
    ],
    'sort' => [
      'id' => 'date'
    ],
    'filter' => [
      'id' => 'date',
    ],
  ];

  $data['tripal_mviews']['mview_edit_link'] = array(
    'title' => t('Edit Materialized View'),
    'help' => t('Clickable link to edit a materialized view'),
    'field' => [
      'id' => 'chado_mviews_edit_link'
    ],
  );

  $data['tripal_mviews']['mview_populate_link'] = array(
    'title' => t('Populate Materialized View'),
    'help' => t('Clickable link to populate a materialized view'),
    'field' => [
      'id' => 'chado_mviews_populate_link'
    ],
  );

  $data['tripal_mviews']['mview_delete_link'] = array(
    'title' => t('Delete Materialized View'),
    'help' => t('Clickable link to delete a materialied view'),
    'field' => [
      'id' => 'chado_mviews_delete_link'
    ],
  );
}