<?php
$node = $variables['node']; ?>

    <div class="tripal_bulk_loader-data-block-desc tripal-data-block-desc"></div> <?php

// the $headers array is an array of fields to use as the colum headers. 
// additional documentation can be found here 
// https://api.drupal.org/api/drupal/includes%21theme.inc/function/theme_table/7
// This table for the analysis has a vertical header (down the first column)
// so we do not provide headers here, but specify them in the $rows array below.
$headers = [];

// the $rows array contains an array of rows where each row is an array
// of values for each column of the table in that row.  Additional documentation
// can be found here:
// https://api.drupal.org/api/drupal/includes%21theme.inc/function/theme_table/7 
$rows = [];

// Name row
$rows[] = [
  [
    'data' => 'Job Name',
    'header' => TRUE,
    'width' => '20%',
  ],
  $node->loader_name,
];
// Submitted By
$rows[] = [
  [
    'data' => 'Submitted By',
    'header' => TRUE,
  ],
  $node->uid,
];
// Job Creation Date
$rows[] = [
  [
    'data' => 'Job Creation Date',
    'header' => TRUE,
  ],
  format_date($node->created, 'custom', "F j, Y, g:i a"),
];
// Last Updated
$rows[] = [
  [
    'data' => 'Last Updated',
    'header' => TRUE,
  ],
  format_date($node->changed, 'custom', "F j, Y, g:i a"),
];
// Template Name
$rows[] = [
  [
    'data' => 'Template Name',
    'header' => TRUE,
  ],
  $node->template->name,
];
// Data File
$rows[] = [
  [
    'data' => 'Data File',
    'header' => TRUE,
  ],
  $node->file,
];
// Job Status
$rows[] = [
  [
    'data' => 'Job Status',
    'header' => TRUE,
  ],
  $node->job_status,
];
//Job Progress
if (isset($node->job)) {
  if (isset($node->job->progress)) {
    $rows[] = [
      [
        'data' => 'Job Progress',
        'header' => TRUE,
      ],
      $node->job->progress . '% (' . l('view job', 'admin/tripal/tripal_jobs/view/' . $node->job_id) . ')',
    ];
  }
}

// the $table array contains the headers and rows array as well as other
// options for controlling the display of the table.  Additional
// documentation can be found here:
// https://api.drupal.org/api/drupal/includes%21theme.inc/function/theme_table/7
$table = [
  'header' => $headers,
  'rows' => $rows,
  'attributes' => [
    'id' => 'tripal_bulk_loader-table-base',
    'class' => 'tripal-data-table',
  ],
  'sticky' => FALSE,
  'caption' => '',
  'colgroups' => [],
  'empty' => '',
];

// once we have our table array structure defined, we call Drupal's theme_table()
// function to generate the table.
print theme_table($table);


// add the "submit" button for adding a loading job to the Tripal jobs management system
$form = drupal_get_form('tripal_bulk_loader_add_loader_job_form', $node);
print drupal_render($form);


// if we have inserted records then load the summary:
if (!empty($node->inserted_records)) {

  print '<h3>Loading Summary</h3>';
  // the $headers array is an array of fields to use as the colum headers.
  // additional documentation can be found here
  // https://api.drupal.org/api/drupal/includes%21theme.inc/function/theme_table/7
  $headers = ['Chado Table', 'Number of Records Inserted'];

  // the $rows array contains an array of rows where each row is an array
  // of values for each column of the table in that row.  Additional documentation
  // can be found here:
  // https://api.drupal.org/api/drupal/includes%21theme.inc/function/theme_table/7
  $rows = [];
  $total = 0;

  foreach ($node->inserted_records as $r) {
    $row = [];
    $row[] = $r->table_inserted_into;
    $row[] = $r->num_inserted;
    $rows[] = $row;
    $total = $total + $r->num_inserted;
  }
  $rows[] = ['<b>TOTAL</b>', '<b>' . $total . '</b>'];

  // the $table array contains the headers and rows array as well as other
  // options for controlling the display of the table.  Additional
  // documentation can be found here:
  // https://api.drupal.org/api/drupal/includes%21theme.inc/function/theme_table/7
  $table = [
    'header' => $headers,
    'rows' => $rows,
    'attributes' => [],
    'sticky' => FALSE,
    'caption' => '',
    'colgroups' => [],
    'empty' => '',
  ];

  // once we have our table array structure defined, we call Drupal's theme_table()
  // function to generate the table.
  print theme_table($table);
} ?>
    <br> <?php

// add the form for setting any constants values
$form = drupal_get_form('tripal_bulk_loader_set_constants_form', $node);
print drupal_render($form);

// add in the constant details
print theme('tripal_bulk_loader_template', ['template_id' => $node->template->template_id]);