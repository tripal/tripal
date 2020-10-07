<?php

/**
 * Describe various Tripal Core systems to Views
 *
 * @ingroup tripal
 */
function tripal_views_data() {
  $data = [];
  // Job Management System.
  tripal_views_data_jobs($data);

//   // Add all TripalEntity bundles.
//   tripal_views_data_tripal_bundles($data);
//   // Add all the TripalFields for each bundle.
//   tripal_views_data_fields($data);

//   $data['views']['tripal_area_collections'] = [
//     'title' => t('Tripal Content Data Collections'),
//     'help' => t('Save Tripal content search results into a data collection for downloading or use with other tools.'),
//     'area' => [
//       'handler' => 'tripal_views_handler_area_collections',
//     ],
//   ];
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
function tripal_views_data_jobs(&$data) {

  $data['tripal_jobs'] = [];
  $data['tripal_jobs']['table'] = [];
  $data['tripal_jobs']['table']['group'] = t('Tripal Jobs');
  $data['tripal_jobs']['table']['provider'] = 'tripal';
  $data['tripal_jobs']['table']['base'] = [
    'field' => 'job_id',
    'title' => t('Tripal Jobs'),
    'help' => t('The Job Management system for Tripal.'),
    'weight' => 10,
  ];

  // Job ID
  $data['tripal_jobs']['job_id'] = [
    'title' => t('Job ID'),
    'help' => t('The job primary key.'),
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

  // Module Name
  $data['tripal_jobs']['modulename'] = [
    'title' => t('Module Name'),
    'help' => t('The name of the module that submitted the job.'),
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

  // Callback
  $data['tripal_jobs']['callback'] = [
    'title' => t('Callback'),
    'help' => t('The callback executed when the job runs.'),
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

  // Arguments
  $data['tripal_jobs']['arguments'] = [
    'title' => t('Arguements'),
    'help' => t('Any arguments passed to the callback.'),
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

  // Progress
  $data['tripal_jobs']['progress'] = [
    'title' => t('Progress'),
    'help' => t('The current progress of the job.'),
    'field' => [
      'id' => 'numeric',
    ],
    'filter' => [
      'id' => 'numeric',
    ],
    'sort' => [
      'id' => 'standard',
    ],
    'argument' => [
      'id' => 'numeric',
    ],
  ];

  // Status
  $data['tripal_jobs']['status'] = [
    'title' => t('Status'),
    'help' => t('The current status of the job.'),
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
    'argument' => [
      'id' => 'string',
    ],
  ];

  // Submit Data
  $data['tripal_jobs']['submit_date'] = [
    'title' => t('Submit Date'),
    'help' => t('The date the job was submitted.'),
    'field' => [
      'id' => 'date',
    ],
    'sort' => [
      'id' => 'date',
    ],
    'filter' => [
      'id' => 'date',
    ],
    'argument' => [
      'id' => 'date',
    ],
  ];

  // Start Time
  $data['tripal_jobs']['start_time'] = [
    'title' => t('Start Time'),
    'help' => t('The time the job started.'),
    'field' => [
      'id' => 'date',
    ],
    'sort' => [
      'id' => 'date',
    ],
    'filter' => [
      'id' => 'date',
    ],
    'argument' => [
      'id' => 'date',
    ],
  ];

  // End Time
  $data['tripal_jobs']['end_time'] = [
    'title' => t('End Time'),
    'help' => t('The time the job ended.'),
    'field' => [
      'id' => 'date',
    ],
    'sort' => [
      'id' => 'date',
    ],
    'filter' => [
      'id' => 'date',
    ],
    'argument' => [
      'id' => 'date',
    ],
  ];

  // Error Message
  $data['tripal_jobs']['error_msg'] = [
    'title' => t('Error Message '),
    'help' => t('A short description of any error the job might have had.'),
    'field' => [
      'id' => 'standard',
    ],
    'sort' => [
      'id' => 'standard',
    ],
    'filter' => [
      'id' => 'string',
    ],
    'argument' => [
      'id' => 'string',
    ],
  ];

  // Unix Pid of the job
  $data['tripal_jobs']['pid'] = [
    'title' => t('Job PID'),
    'help' => t('The Unix PID of the job.'),
    'field' => [
      'id' => 'numeric',
    ],
    'filter' => [
      'id' => 'numeric',
    ],
    'sort' => [
      'id' => 'standard',
    ],
    'argument' => [
      'id' => 'numeric',
    ],
  ];

  // Priority
  $data['tripal_jobs']['priority'] = [
    'title' => t('Priority'),
    'help' => t('The priority of this job.'),
    'field' => [
      'id' => 'numeric',
    ],
    'filter' => [
      'id' => 'numeric',
    ],
    'sort' => [
      'id' => 'standard',
    ],
    'argument' => [
      'id' => 'numeric',
    ],
  ];
}