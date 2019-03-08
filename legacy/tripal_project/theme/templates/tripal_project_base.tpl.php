<?php
$project = $variables['node']->project;

// get the project description.  The first iteration of the project
// module incorrectly stored the project description in the Drupal 
// node->body field.  Also, the project.descriptin field is only 255
// characters which is not large neough. Therefore, we store the description
// in the  chado.projectprop table.  For backwards compatibility, we 
// will check if the node->body is empty and if not we'll use that instead.
// If there is data in the project.description field then we will use that, but
// if there is data in the projectprop table for a descrtion then that takes 
// precedence 
$description = '';
if (property_exists($node, 'body')) {
  $description = $node->body;
}
if ($project->description) {
  $description = $project->description;
}
else {
  $record = [
    'table' => 'project',
    'id' => $project->project_id,
  ];
  $property = [
    'type_name' => 'Project Description',
    'cv_name' => 'project_property',
  ];
  $projectprop = chado_get_property($record, $property);
  $description = $projectprop->value;
} ?>

    <div class="tripal_project-data-block-desc tripal-data-block-desc"></div><?php

// the $headers array is an array of fields to use as the colum headers. 
// additional documentation can be found here 
// https://api.drupal.org/api/drupal/includes%21theme.inc/function/theme_table/7
// This table for the project has a vertical header (down the first column)
// so we do not provide headers here, but specify them in the $rows array below.
$headers = [];

// the $rows array contains an array of rows where each row is an array
// of values for each column of the table in that row.  Additional documentation
// can be found here:
// https://api.drupal.org/api/drupal/includes%21theme.inc/function/theme_table/7 
$rows = [];

// Project Name row
$rows[] = [
  [
    'data' => 'Project Name',
    'header' => TRUE,
    'width' => '20%',
  ],
  $project->name,
];
// allow site admins to see the feature ID
if (user_access('view ids')) {
  // Project ID
  $rows[] = [
    [
      'data' => 'Project ID',
      'header' => TRUE,
      'class' => 'tripal-site-admin-only-table-row',
    ],
    [
      'data' => $project->project_id,
      'class' => 'tripal-site-admin-only-table-row',
    ],
  ];
}
// the $table array contains the headers and rows array as well as other
// options for controlling the display of the table.  Additional
// documentation can be found here:
// https://api.drupal.org/api/drupal/includes%21theme.inc/function/theme_table/7
$table = [
  'header' => $headers,
  'rows' => $rows,
  'attributes' => [
    'id' => 'tripal_project-table-base',
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
if ($description) { ?>
    <div style="text-align: justify"><?php print $description; ?></div> <?php
} 
