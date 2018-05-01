<?php

$featuremap = $variables['node']->featuremap;
$options = ['return_array' => 1];
$featuremap = chado_expand_var($featuremap, 'table', 'featuremapprop', $options);
$properties = $featuremap->featuremapprop;

if (count($properties) > 0) {
  // the $headers array is an array of fields to use as the colum headers.
  // additional documentation can be found here
  // https://api.drupal.org/api/drupal/includes%21theme.inc/function/theme_table/7
  $headers = ['Property Name', 'Value'];

  // the $rows array contains an array of rows where each row is an array
  // of values for each column of the table in that row.  Additional documentation
  // can be found here:
  // https://api.drupal.org/api/drupal/includes%21theme.inc/function/theme_table/7
  $rows = [];

  foreach ($properties as $property) {
    $property = chado_expand_var($property, 'field', 'featuremapprop.value');
    $rows[] = [
      ucfirst(preg_replace('/_/', ' ', $property->type_id->name)),
      urldecode($property->value),
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
      'id' => 'tripal_featuremap-table-properties',
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