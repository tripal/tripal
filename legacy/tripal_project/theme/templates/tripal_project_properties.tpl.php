<?php

// expand the project to include the properties.
$project = $variables['node']->project;
$project = chado_expand_var($project, 'table', 'projectprop', ['return_array' => 1]);
$projectprops = $project->projectprop;

// put the properties in an array so we can remove the project_description property
$properties = [];
if ($projectprops) {
  foreach ($projectprops as $property) {
    // we want to keep all properties but the project_description as that
    // property is shown on the base template page.
    if ($property->type_id->name != 'Project Description') {
      $property = chado_expand_var($property, 'field', 'projectprop.value');
      $properties[] = $property;
    }
  }
}


if (count($properties) > 0) { ?>
    <div class="tripal_project-data-block-desc tripal-data-block-desc">
        Additional information about this project:
    </div><?php

  // the $headers array is an array of fields to use as the colum headers.
  // additional documentation can be found here
  // https://api.drupal.org/api/drupal/includes%21theme.inc/function/theme_table/7
  $headers = ['Property Name', 'Value'];

  // the $rows array contains an array of rows where each row is an array
  // of values for each column of the table in that row.  Additional documentation
  // can be found here:
  // https://api.drupal.org/api/drupal/includes%21theme.inc/function/theme_table/7
  $rows = [];

  // add the properties as individual rows
  foreach ($properties as $property) {
    $rows[] = [
      ucfirst(preg_replace('/_/', ' ', $property->type_id->name)),
      $property->value,
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
      'id' => 'tripal_project-table-properties',
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
}