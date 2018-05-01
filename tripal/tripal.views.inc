<?php
/**
 * @file
 * Integrates many of the core database tables with drupal views
 */

/**
 * Implements of hook_views_plugins().
 */
function tripal_views_plugins() {
  return [
    'module' => 'tripal',
    'query' => [
      'tripal_views_query' => [
        'title' => t('Tripal Entity Query'),
        'help' => t('Query that allows you to search with Tripal entities.'),
        'handler' => 'tripal_views_query',
        'parent' => 'views_query',
      ],
    ],
  ];
}

/**
 * Describe various Tripal Core systems to Views
 *
 * @ingroup tripal
 */
function tripal_views_data() {
  $data = [];
  // Job Management System
  tripal_views_data_jobs($data);
  tripal_views_data_tripal_entity($data);
  tripal_views_data_fields($data);

  $data['views']['tripal_area_collections'] = [
    'title' => t('Tripal Content Data Collections'),
    'help' => t('Save Tripal content search results into a data collection for downloading or use with other tools.'),
    'area' => [
      'handler' => 'tripal_views_handler_area_collections',
    ],
  ];
  return $data;
}

/**
 * Implements hook views_data_alter()
 *
 * Ensures that all fields attached to TripalEntities use the proper
 * handlers.
 */
function tripal_views_data_alter(&$data) {

  $fields = field_info_fields();
  $tripal_fields = tripal_get_field_types();

  // Iterate through all of the fields.
  foreach ($fields as $field) {

    // Skip fields that aren't attached to TripalEntity entities.
    if (!array_key_exists('TripalEntity', $field['bundles'])) {
      continue;
    }

    // Iterate through the bundles to which this field is attached and
    // if it is a TripalField field then we'll call the viewsData function.
    $bundles = $field['bundles']['TripalEntity'];
    $result = [];
    foreach ($bundles as $bundle_name) {
      $field_name = $field['field_name'];

      // Skip fields that aren't setup for views with this bundle.
      // Fields should be associated with the bundle's term identifier
      // (i.e. [vocab]__[accession].
      $bundle = tripal_load_bundle_entity(['name' => $bundle_name]);
      $term = tripal_load_term_entity(['term_id' => $bundle->term_id]);
      $bundle_term_id = $term->vocab->vocabulary . '__' . $term->accession;
      if (!array_key_exists($bundle_term_id, $data)) {
        continue;
      }

      // Skip fields implemented using a TripalField class. These fields
      // should already have the proper handlers because the class sets
      // them up automatically.
      if (in_array($field_name, $tripal_fields)) {
        continue;
      }

      $data[$bundle_term_id][$field_name]['sort']['handler'] = 'tripal_views_handler_sort';
    }
  }
}

/**
 * Integreates the Tripal fields with Views.
 */
function tripal_views_data_fields(&$data) {

  // Iterate through the fields.
  $fields = field_info_fields();
  foreach ($fields as $field) {

    // Skip fields that aren't attached to TripalEntity entities.
    if (!array_key_exists('TripalEntity', $field['bundles'])) {
      continue;
    }

    // TODO: do we really need this hook? Just substitute the code here
    // for what that hook does... it's only called in one plac.e
    // Call the hook_field_views_data() but only for the tripal module.
    // Otherwise the other modules will expect that this is an SQL-based
    // view.
    $result = (array) module_invoke('tripal', 'field_views_data', $field);

    // Set defaults for the field if no data array was returned.
    if (empty($result)) {
      // Iterate through the bundles to which this field is attached and
      // if it is a TripalField field then we'll call the viewsData function.
      $bundles = $field['bundles']['TripalEntity'];
      $result = [];
      foreach ($bundles as $bundle_name) {
        $field_name = $field['field_name'];

        // Fields should be associated with the bundle's term identifier
        // (i.e. [vocab]__[accession].
        $bundle = tripal_load_bundle_entity(['name' => $bundle_name]);
        $term = tripal_load_term_entity(['term_id' => $bundle->term_id]);
        $view_base_id = $term->vocab->vocabulary . '__' . $term->accession;

        $instance = field_info_instance('TripalEntity', $field['field_name'], $bundle_name);
        $tfield = new TripalField($field, $instance);
        $result += $tfield->viewsData($view_base_id);
      }
    }

    drupal_alter('field_views_data', $result, $field, $module);

    if (is_array($result)) {
      $data = drupal_array_merge_deep($result, $data);
    }
  }
}

/**
 * Integrates the TripalEntity entities with Drupal Views.
 */
function tripal_views_data_tripal_entity(&$data) {

  // Get the list of all of the bundles (entity types) and add them
  // as "base tables" for views.
  $bundles = db_select('tripal_bundle', 'tb')
    ->fields('tb')
    ->execute();

  // Iterate through the bundles.
  while ($bundle = $bundles->fetchObject()) {

    // This isn't really the table name, but because our bundle table
    // names are unique on every Tripal site we must ust a more generic
    // name.  Because we're using our own query class this should be fine.
    $term = tripal_load_term_entity(['term_id' => $bundle->term_id]);
    $table = $term->vocab->vocabulary . '__' . $term->accession;

    // Each bundle gets it's own "table".
    $data[$table]['table']['group'] = t($bundle->label);
    $data[$table]['table']['base'] = [
      'query class' => 'tripal_views_query',
      'title' => t($bundle->label),
      'help' => t('Tripal ' . $bundle->label . ' pages'),
    ];
    $data[$table]['entity_id'] = [
      'title' => t('Entity ID'),
      'help' => t('The unique entity ID for this content type.'),
      'field' => [
        'handler' => 'tripal_views_handler_field_entity',
      ],
      'filter' => [
        'handler' => 'tripal_views_handler_filter',
      ],
      'sort' => [
        'handler' => 'tripal_views_handler_sort',
      ],
    ];
    $data[$table]['link'] = [
      'title' => t('Link'),
      'help' => t('Provide a simple link to the content.'),
      'field' => [
        'handler' => 'tripal_views_handler_field_entity_link',
      ],
    ];
    $data[$table]['edit_link'] = [
      'title' => t('Edit Link'),
      'help' => t('Provide a simple link to edit the content.'),
      'field' => [
        'handler' => 'tripal_views_handler_field_entity_link_edit',
      ],
    ];
    $data[$table]['delete_link'] = [
      'title' => t('Delete Link'),
      'help' => t('Provide a simple link to delete the content.'),
      'field' => [
        'handler' => 'tripal_views_handler_field_entity_link_delete',
      ],
    ];
    $data[$table]['status'] = [
      'title' => t('Published'),
      'help' => t('Whether or not the content is published.'),
      'field' => [
        'handler' => 'tripal_views_handler_field_boolean',
        'click sortable' => TRUE,
        'output formats' => [
          'published-notpublished' => [t('Published'), t('Not published')],
        ],
      ],
      'filter' => [
        'handler' => 'tripal_views_handler_filter_boolean_operator',
        'label' => t('Published'),
        'type' => 'yes-no',
        'use equal' => TRUE,
        // Use status = 1 instead of status <> 0 in WHERE statment
      ],
      'sort' => [
        'handler' => 'tripal_views_handler_sort',
      ],
    ];
  }
}

/**
 * Provides the data array for the tripal job management system
 *
 * @param $data
 *   Previously generated tripal views data array
 * return
 *   $data array with job management system described
 *
 * @ingroup tripal
 */
function tripal_views_data_jobs(&$data) {

  $data['tripal_jobs']['table']['group'] = t('Tripal Jobs');
  $data['tripal_jobs']['table']['base'] = [
    'field' => 'job_id', // This is the identifier field for the view.
    'title' => t('Tripal Jobs'),
    'help' => t('The Job Management system for Tripal.'),
    'weight' => 10,
  ];

  // Job ID
  $data['tripal_jobs']['job_id'] = [
    'title' => t('Job ID'),
    'help' => t('The job primary key.'),
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

  // User ID: Submitter
  $data['tripal_jobs']['uid'] = [
    'title' => t('Job Submitter'),
    'help' => t('The user who submitted the job.'),
    'relationship' => [
      'base' => 'user', // The name of the table to join with.
      'base field' => 'uid', // The name of the field on the joined table.
      'handler' => 'views_handler_relationship',
      'label' => t('Submitting User'),
      'title' => t('Submitting User'),
      'help' => t('The user who submitted the job'),
    ],
  ];

  // Job Name
  $data['tripal_jobs']['job_name'] = [
    'title' => t('Job Name'),
    'help' => t('The name of the job.'),
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

  // Module Name
  $data['tripal_jobs']['modulename'] = [
    'title' => t('Module Name'),
    'help' => t('The name of the module that submitted the job.'),
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

  // Callback
  $data['tripal_jobs']['callback'] = [
    'title' => t('Callback'),
    'help' => t('The callback executed when the job runs.'),
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

  // Arguments
  $data['tripal_jobs']['arguments'] = [
    'title' => t('Arguements'),
    'help' => t('Any arguments passed to the callback.'),
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

  // Progress
  $data['tripal_jobs']['progress'] = [
    'title' => t('Progress'),
    'help' => t('The current progress of the job.'),
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

  // Status
  $data['tripal_jobs']['status'] = [
    'title' => t('Status'),
    'help' => t('The current status of the job.'),
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

  // Submit Data
  $data['tripal_jobs']['submit_date'] = [
    'title' => t('Submit Date'),
    'help' => t('The date the job was submitted.'),
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

  // Start Time
  $data['tripal_jobs']['start_time'] = [
    'title' => t('Start Time'),
    'help' => t('The time the job started.'),
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

  // End Time
  $data['tripal_jobs']['end_time'] = [
    'title' => t('End Time'),
    'help' => t('The time the job ended.'),
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

  // Error Message
  $data['tripal_jobs']['error_msg'] = [
    'title' => t('Error Message '),
    'help' => t('A short description of any error the job might have had.'),
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

  // Unix Pid of the job
  $data['tripal_jobs']['pid'] = [
    'title' => t('Job PID'),
    'help' => t('The Unix PID of the job.'),
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

  // Priority
  $data['tripal_jobs']['priority'] = [
    'title' => t('Priority'),
    'help' => t('The priority of this job.'),
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
}