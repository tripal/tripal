<?php
/**
 * @file
 * Integrates many of the core database tables with drupal views
 */

/**
 * Implements of hook_views_plugins().
 */
function tripal_views_plugins() {
  return array(
    'module' => 'tripal',
    'query' => array(
      'tripal_views_query' => array(
        'title' => t('Tripal Entity Query'),
        'help' => t('Query that allows you to search with Tripal entities.'),
        'handler' => 'tripal_views_query',
        'parent' => 'views_query',
      ),
    ),
  );
}

/**
 * Describe various Tripal Core systems to Views
 *   for the creation of administrative views.
 *
 * @ingroup tripal
 */
function tripal_views_data() {
  $data = array();
  // Job Management System
  $data = tripal_views_data_jobs($data);

  $data += tripal_entity_views_data();

  return $data;
}

/**
 * Integrates the TripalEntity entities with Drupal Views.
 */
function tripal_entity_views_data() {
  $data = array();

  // Get the list of all of the bundles (entity types) and add them
  // as "base tables" for views.
  $bundles = db_select('tripal_bundle', 'tb')
    ->fields('tb')
    ->execute();

  // Iterate through the bundles.
  while ($bundle = $bundles->fetchObject()) {

    // Each bundle gets it's own "table".
    $data[$bundle->name]['table']['group'] = t('Tripal ' . $bundle->label . ' page');
    $data[$bundle->name]['table']['base'] = array(
      'query class' => 'tripal_views_query',
      'title' => t('Tripal ' . $bundle->label . ' page'),
      'help' => t('Searches Tripal ' . $bundle->label . ' pages'),
    );

    // Now add the fields to the bundle.
    $instances = field_info_instances('TripalEntity', $bundle->name);
    foreach ($instances as $instance) {

      $field_handler = 'views_handler_field_numeric';
      $filter_handler = 'views_handler_filter_numeric';
      $sort_handler = 'views_handler_sort';
      $click_sortable = TRUE;

      $field_name = $instance['field_name'];
      $data[$bundle->name][$field_name] = array(
        'title' => $instance['label'],
        'help' => $instance['description'],
        'field' => array(
          'handler' => $field_handler,
          'click sortable' => $click_sortable,
        ),
        'filter' => array(
          'handler' => $filter_handler,
        ),
        'sort' => array(
          'handler' => $sort_handler,
        ),
      );
    }
  }
  return $data;
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
function tripal_views_data_jobs($data) {

  $data['tripal_jobs']['table']['group'] = t('Tripal Jobs');
  $data['tripal_jobs']['table']['base'] = array(
    'field' => 'job_id', // This is the identifier field for the view.
    'title' => t('Tripal Jobs'),
    'help' => t('The Job Management system for Tripal.'),
    'weight' => 10,
  );

  // Job ID
  $data['tripal_jobs']['job_id'] = array(
    'title' => t('Job ID'),
    'help' => t('The job primary key.'),
    'field' => array(
      'handler' => 'views_handler_field_numeric',
      'click sortable' => TRUE,
    ),
    'filter' => array(
      'handler' => 'views_handler_filter_numeric',
    ),
    'sort' => array(
      'handler' => 'views_handler_sort',
    ),
  );

  // User ID: Submitter
  $data['tripal_jobs']['uid'] = array(
    'title' => t('Job Submitter'),
    'help' => t('The user who submitted the job.'),
    'relationship' => array(
      'base' => 'user', // The name of the table to join with.
      'base field' => 'uid', // The name of the field on the joined table.
      'handler' => 'views_handler_relationship',
      'label' => t('Submitting User'),
      'title' => t('Submitting User'),
      'help' => t('The user who submitted the job'),
    ),
  );

  // Job Name
  $data['tripal_jobs']['job_name'] = array(
    'title' => t('Job Name'),
    'help' => t('The name of the job.'),
    'field' => array(
      'handler' => 'views_handler_field',
      'click sortable' => TRUE, // This is use by the table display plugin.
    ),
    'sort' => array(
      'handler' => 'views_handler_sort',
    ),
    'filter' => array(
      'handler' => 'views_handler_filter_string',
    ),
    'argument' => array(
      'handler' => 'views_handler_argument_string',
    ),
  );

  // Module Name
  $data['tripal_jobs']['modulename'] = array(
    'title' => t('Module Name'),
    'help' => t('The name of the module that submitted the job.'),
    'field' => array(
      'handler' => 'views_handler_field',
      'click sortable' => TRUE, // This is use by the table display plugin.
    ),
    'sort' => array(
      'handler' => 'views_handler_sort',
    ),
    'filter' => array(
      'handler' => 'views_handler_filter_string',
    ),
    'argument' => array(
      'handler' => 'views_handler_argument_string',
    ),
  );

  // Callback
  $data['tripal_jobs']['callback'] = array(
    'title' => t('Callback'),
    'help' => t('The callback executed when the job runs.'),
    'field' => array(
      'handler' => 'views_handler_field',
      'click sortable' => TRUE, // This is use by the table display plugin.
    ),
    'sort' => array(
      'handler' => 'views_handler_sort',
    ),
    'filter' => array(
      'handler' => 'views_handler_filter_string',
    ),
    'argument' => array(
      'handler' => 'views_handler_argument_string',
    ),
  );

  // Arguments
  $data['tripal_jobs']['arguments'] = array(
    'title' => t('Arguements'),
    'help' => t('Any arguements passed to the callback.'),
    'field' => array(
      'handler' => 'views_handler_field',
      'click sortable' => TRUE, // This is use by the table display plugin.
    ),
    'sort' => array(
      'handler' => 'views_handler_sort',
    ),
    'filter' => array(
      'handler' => 'views_handler_filter_string',
    ),
    'argument' => array(
      'handler' => 'views_handler_argument_string',
    ),
  );

  // Progress
  $data['tripal_jobs']['progress'] = array(
    'title' => t('Progress'),
    'help' => t('The current progress of the job.'),
    'field' => array(
      'handler' => 'views_handler_field_numeric',
      'click sortable' => TRUE,
    ),
    'filter' => array(
      'handler' => 'views_handler_filter_numeric',
    ),
    'sort' => array(
      'handler' => 'views_handler_sort',
    ),
  );

  // Status
  $data['tripal_jobs']['status'] = array(
    'title' => t('Status'),
    'help' => t('The current status of the job.'),
    'field' => array(
      'handler' => 'views_handler_field',
      'click sortable' => TRUE, // This is use by the table display plugin.
    ),
    'sort' => array(
      'handler' => 'views_handler_sort',
    ),
    'filter' => array(
      'handler' => 'views_handler_filter_string',
    ),
    'argument' => array(
      'handler' => 'views_handler_argument_string',
    ),
  );

  // Submit Data
  $data['tripal_jobs']['submit_date'] = array(
    'title' => t('Submit Date'),
    'help' => t('The date the job was submitted.'),
    'field' => array(
      'handler' => 'views_handler_field_date',
      'click sortable' => TRUE,
    ),
    'sort' => array(
      'handler' => 'views_handler_sort_date',
    ),
    'filter' => array(
      'handler' => 'views_handler_filter_date',
    ),
  );

  // Start Time
  $data['tripal_jobs']['start_time'] = array(
    'title' => t('Start Time'),
    'help' => t('The time the job started.'),
    'field' => array(
      'handler' => 'views_handler_field_date',
      'click sortable' => TRUE,
    ),
    'sort' => array(
      'handler' => 'views_handler_sort_date',
    ),
    'filter' => array(
      'handler' => 'views_handler_filter_date',
    ),
  );

  // End Time
  $data['tripal_jobs']['end_time'] = array(
    'title' => t('End Time'),
    'help' => t('The time the job ended.'),
    'field' => array(
      'handler' => 'views_handler_field_date',
      'click sortable' => TRUE,
    ),
    'sort' => array(
      'handler' => 'views_handler_sort_date',
    ),
    'filter' => array(
      'handler' => 'views_handler_filter_date',
    ),
  );

  // Error Message
  $data['tripal_jobs']['error_msg'] = array(
    'title' => t('Error Message '),
    'help' => t('A short description of any error the job might have had.'),
    'field' => array(
      'handler' => 'views_handler_field',
      'click sortable' => TRUE, // This is use by the table display plugin.
    ),
    'sort' => array(
      'handler' => 'views_handler_sort',
    ),
    'filter' => array(
      'handler' => 'views_handler_filter_string',
    ),
    'argument' => array(
      'handler' => 'views_handler_argument_string',
    ),
  );

  // Unix Pid of the job
  $data['tripal_jobs']['pid'] = array(
    'title' => t('Job PID'),
    'help' => t('The Unix PID of the job.'),
    'field' => array(
      'handler' => 'views_handler_field_numeric',
      'click sortable' => TRUE,
    ),
    'filter' => array(
      'handler' => 'views_handler_filter_numeric',
    ),
    'sort' => array(
      'handler' => 'views_handler_sort',
    ),
  );

  // Priority
  $data['tripal_jobs']['priority'] = array(
    'title' => t('Priority'),
    'help' => t('The priority of this job.'),
    'field' => array(
      'handler' => 'views_handler_field_numeric',
      'click sortable' => TRUE,
    ),
    'filter' => array(
      'handler' => 'views_handler_filter_numeric',
    ),
    'sort' => array(
      'handler' => 'views_handler_sort',
    ),
  );

  return $data;
}