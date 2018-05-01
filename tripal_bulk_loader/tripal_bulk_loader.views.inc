<?php
/**
 * @file
 * Provides Drupal views integration excluding default views.
 *
 * @ingroup tripal_bulk_loader
 */

/**
 * Generates a dynamic data array for Views
 *
 * Purpose: This function is a hook used by the Views module. It populates and
 *   returns a data array that specifies for the Views module the base table,
 *   the tables it joins with and handlers.  The data array is populated
 *   using the data stored in the tripal_views tables.
 *
 * @return a data array formatted for the Views module
 *
 * D7 @todo: Add support for materialized views relationships using the new
 *   method
 *
 * @ingroup tripal_bulk_loader
 */
function tripal_bulk_loader_views_data() {
  $data = [];

  // Tripal Bulk Loader *************************
  $data['tripal_bulk_loader']['table']['group'] = t('Tripal Bulk Loading Jobs');
  $data['tripal_bulk_loader']['table']['base'] = [
    'field' => 'nid',
    'title' => t('Tripal Bulk Loading Jobs'),
    'help' => t('Jobs created to load chado data using a Tripal Bulk Loader Template.'),
  ];

  // This table references the {node} table. The declaration below creates an
  // 'implicit' relationship to the node table, so that when 'node' is the base
  // table, the fields are automatically available.
  $data['tripal_bulk_loader']['table']['join'] = [
    'node' => [
      'left_field' => 'nid',
      'field' => 'nid',
    ],
  ];

  $data['tripal_bulk_loader']['table']['join']['tripal_bulk_loader_template'] = [
    'left_field' => 'template_id',
    'field' => 'template_id',
  ];

  $data['tripal_bulk_loader']['table']['join']['tripal_bulk_loader_constants'] = [
    'left_field' => 'nid',
    'field' => 'nid',
  ];

  // Node ID table field.
  $data['tripal_bulk_loader']['nid'] = [
    'title' => t('Tripal Bulk Loading Jobs'),
    'help' => t('Tripal Bulk Loading Jobs.'),

    // Define a relationship to the {node} table, so tripal_bulk_loader views can
    // add a relationship to nodes. If you want to define a relationship the
    // other direction, use hook_views_data_alter(), or use the 'implicit' join
    // method described above.
    'relationship' => [
      'base' => 'node',
      'base field' => 'nid',
      'handler' => 'views_handler_relationship',
      'label' => t('Bulk Loading Job => Node'),
      'title' => t('Bulk Loading Job => Node'),
      'help' => t('Link Tripal Bulk loading jobs to their nodes.'),
    ],
  ];

  $data['tripal_bulk_loader']['loader_name'] = [
    'title' => t('Name'),
    'help' => t('The name of the loading job.'),
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

  $data['tripal_bulk_loader']['template_id'] = [
    'title' => t('Template ID'),
    'help' => t('The template to be used for this bulk loading job.'),
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

  $data['tripal_bulk_loader']['file'] = [
    'title' => t('File'),
    'help' => t('The path to the file to be loaded.'),
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

  $data['tripal_bulk_loader']['job_id'] = [
    'title' => t('Tripal Job ID'),
    'help' => t('The Tripal job ID corresponding to this Tripal Bulk Loading job.'),
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

  $data['tripal_bulk_loader']['job_status'] = [
    'title' => t('Job Status'),
    'help' => t('A description of the status of the loading job.'),
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

  $data['tripal_bulk_loader']['file_has_header'] = [
    'title' => t('File has a Header'),
    'help' => t('Whether the file has a single header line or not.'),
    'field' => [
      'handler' => 'views_handler_field_boolean',
      'click sortable' => TRUE,
    ],
    'filter' => [
      'handler' => 'views_handler_filter_boolean_operator',
      'type' => 'yes-no',
      'use equal' => TRUE,
    ],
    'sort' => [
      'handler' => 'views_handler_sort',
    ],
  ];

  $data['tripal_bulk_loader']['keep_track_inserted'] = [
    'title' => t('Tracking Inserted IDs'),
    'help' => t('Whether the loader is keeping track of inserted IDs to allow reverse of loading.'),
    'field' => [
      'handler' => 'views_handler_field_boolean',
      'click sortable' => TRUE,
    ],
    'filter' => [
      'handler' => 'views_handler_filter_boolean_operator',
      'type' => 'yes-no',
      'use equal' => TRUE,
    ],
    'sort' => [
      'handler' => 'views_handler_sort',
    ],
  ];

  // Tripal Bulk Loader Template ****************
  $data['tripal_bulk_loader_template']['table']['group'] = t('Tripal Bulk Loader Templates');
  $data['tripal_bulk_loader_template']['table']['base'] = [
    'field' => 'template_id',
    'title' => t('Tripal Bulk Loader Templates'),
    'help' => t('Tripal Bulk Loader Templates used to describe where to load data into chado.'),
  ];

  $data['tripal_bulk_loader_template']['table']['join']['tripal_bulk_loader'] = [
    'left_field' => 'template_id',
    'field' => 'template_id',
  ];

  $data['tripal_bulk_loader_template']['template_id'] = [
    'title' => t('Template ID'),
    'help' => t('The unique identifier for tripal bulk loader templates.'),
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
    'relationship' => [
      'base' => 'tripal_bulk_loader',
      'base field' => 'template_id',
      'handler' => 'views_handler_relationship',
      'label' => t('Templates => TBL Jobs'),
      'title' => t('Templates => TBL Jobs'),
      'help' => t('Links each template to the jobs using it.'),
    ],
  ];

  $data['tripal_bulk_loader_template']['name'] = [
    'title' => t('Name'),
    'help' => t('The human-readable name of the template.'),
    'field' => [
      'handler' => 'views_handler_field',
      'click sortable' => TRUE,
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

  $data['tripal_bulk_loader_template']['created'] = [
    'title' => t('Created'),
    'help' => t('Date the template was created on.'),
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

  $data['tripal_bulk_loader_template']['changed'] = [
    'title' => t('Updated'),
    'help' => t('The date the template was last updated on.'),
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

  // Tripal Bulk Loader Constants ***************
  $data['tripal_bulk_loader_constants']['table']['group'] = t('Tripal Bulk Loader Constants');
  $data['tripal_bulk_loader_constants']['table'] = [
    'field' => 'constant_id',
    'title' => t('Tripal Bulk Loading Job Constants'),
    'help' => t('Constants described for a particular Tripal Bulk Loading Job.'),
  ];

  $data['tripal_bulk_loader_constants']['table']['join']['tripal_bulk_loader'] = [
    'left_field' => 'nid',
    'field' => 'nid',
  ];

  $data['tripal_bulk_loader_constants']['group_id'] = [
    'title' => t('Group ID'),
    'help' => t('Constants are grouped together allowing for multiple values for the same field.'),
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

  $data['tripal_bulk_loader_constants']['chado_table'] = [
    'title' => t('Chado Table'),
    'help' => t('The table this constant applies to.'),
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

  $data['tripal_bulk_loader_constants']['chado_field'] = [
    'title' => t('Chado Field'),
    'help' => t('The field in chado this constant applies to.'),
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

  $data['tripal_bulk_loader_constants']['record_id'] = [
    'title' => t('Record ID'),
    'help' => t('Unique identifier of a record within the template.'),
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

  $data['tripal_bulk_loader_constants']['field_id'] = [
    'title' => t('Field Index'),
    'help' => t('Unique identifier of a field within a template record.'),
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

  $data['tripal_bulk_loader_constants']['value'] = [
    'title' => t('Value'),
    'help' => t('The value of the constant.'),
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
