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
  // Pub Search Queries
  tripal_views_pub_search_queries($data);
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
  $data['tripal_jobs']['table']['join'] = [
    'users' => [
      'left_field' => 'uid',
      'field' => 'uid',
    ],
    'user_field_data' => [
      'left_field' => 'uid',
      'field' => 'uid',
    ],
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
    'relationship' =>
      [
        // The name of the table to join with.
        'base' => 'users_field_data',
        // The name of the field on the joined table.
        'base field' => 'uid',
        'id' => 'standard',
        'label' => t('Submitter'),
        'title' => t('Submitter'),
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
    'title' => t('Arguments'),
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

/**
 * Provides the data array for the tripal pub search queries
 *
 * @param $data
 *   Previously generated tripal views data array
 * return
 *   $data array with pub library query management system described
 *
 * @ingroup tripal
 */
function tripal_views_pub_search_queries(&$data) {
  $data['tripal_pub_library_query'] = [];
  $data['tripal_pub_library_query']['table'] = [];
  $data['tripal_pub_library_query']['table']['group'] = t('Publication Search Queries');
  // $data['pub_search_queries']['table']['provider'] = 'tripal';
  $data['tripal_pub_library_query']['table']['base'] = [
    'field' => 'pub_library_query_id',
    'title' => t('Publication Library Query'),
    'help' => t('Publication Library Query Management system for Tripal.'),
    'weight' => 10,
  ];

  $data['tripal_pub_library_query']['pub_library_query_id'] = [
    'title' => t('Publication Library Query ID'),
    'help' => t('The query primary key.'),
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

  $data['tripal_pub_library_query']['name'] = [
    'title' => t('Name'),
    'help' => t('The publication query name'),
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

  $data['tripal_pub_library_query']['criteria'] = [
    'title' => t('Criteria'),
    'help' => t("The publication query criteria - label must be either 'database' or 'search string'."),
    'field' => [
      'id' => 'criteria',
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

  // $data['tripal_pub_library_query']['criteriasearchstring'] = [
  //   'title' => t('Criteria - Search String'),
  //   'help' => t('The publication query criteria search string'),
  //   'field' => [
  //     'id' => 'criteria',
  //   ],
  //   'sort' => [
  //     'id' => 'standard'
  //   ],
  //   'filter' => [
  //     'id' => 'string',
  //   ],
  //   'argument' => [
  //     'id' => 'string',
  //   ],
  // ];

  // $data['tripal_pub_library_query']['criteriadatabase'] = [
  //   'title' => t('Criteria - Database'),
  //   'help' => t('The publication query criteria database'),
  //   'field' => [
  //     'id' => 'criteria',
  //   ],
  //   'sort' => [
  //     'id' => 'standard'
  //   ],
  //   'filter' => [
  //     'id' => 'string',
  //   ],
  //   'argument' => [
  //     'id' => 'string',
  //   ],
  // ];


  $data['tripal_pub_library_query']['disabled'] = [
    'title' => t('Disabled'),
    'help' => t('Disabled option'),
    'field' => [
      'id' => 'disabled',
    ],
    'filter' => [
      'id' => 'boolean',
    ],
    'sort' => [
      'id' => 'standard'
    ],
    'argument' => [
      'id' => 'string',
    ],
  ];

  $data['tripal_pub_library_query']['do_contact'] = [
    'title' => t('Do Contact'),
    'help' => t('Do Contact option'),
    'field' => [
      'id' => 'do_contact',
    ],
    'filter' => [
      'id' => 'boolean',
    ],
    'sort' => [
      'id' => 'standard'
    ],
    'argument' => [
      'id' => 'string',
    ],
  ];

  return $data;
}
