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
  $projectprop = tripal_project_get_property($project->project_id, 'Project Description');
  $description = $projectprop->value;
} ?>

<div class="tripal_project-data-block-desc tripal-data-block-desc"></div><?php 

// the $headers array is an array of fields to use as the colum headers. 
// additional documentation can be found here 
// https://api.drupal.org/api/drupal/includes%21theme.inc/function/theme_table/7
// This table for the project has a vertical header (down the first column)
// so we do not provide headers here, but specify them in the $rows array below.
$headers = array();

// the $rows array contains an array of rows where each row is an array
// of values for each column of the table in that row.  Additional documentation
// can be found here:
// https://api.drupal.org/api/drupal/includes%21theme.inc/function/theme_table/7 
$rows = array();

// Project Name row
$rows[] = array(
  array(
    'data' => 'Project Name',
    'header' => TRUE,
    'width' => '20%',
  ),
  $project->name
);
// allow site admins to see the feature ID
if (user_access('administer tripal')) {
  // Project ID
  $rows[] = array(
    array(
      'data' => 'Project ID',
      'header' => TRUE,
      'class' => 'tripal-site-admin-only-table-row',
    ),
    array(
      'data' => $project->project_id,
      'class' => 'tripal-site-admin-only-table-row',
    ),
  );
}
// the $table array contains the headers and rows array as well as other
// options for controlling the display of the table.  Additional
// documentation can be found here:
// https://api.drupal.org/api/drupal/includes%21theme.inc/function/theme_table/7
$table = array(
  'header' => $headers,
  'rows' => $rows,
  'attributes' => array(
    'id' => 'tripal_project-table-base',
  ),
  'sticky' => FALSE,
  'caption' => '',
  'colgroups' => array(),
  'empty' => '',
);

// once we have our table array structure defined, we call Drupal's theme_table()
// function to generate the table.
print theme_table($table);
if ($description) { ?>
  <div style="text-align: justify"><?php print $description; ?></div> <?php  
} 
