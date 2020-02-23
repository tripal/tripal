<?php

$featuremap = $variables['node']->featuremap;

// expand the description field
$featuremap = chado_expand_var($featuremap, 'field', 'featuremap.description'); ?>

    <div class="tripal_featuremap-data-block-desc tripal-data-block-desc"></div> <?php

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

// Map Name row
$rows[] = [
  [
    'data' => 'Map Name',
    'header' => TRUE,
    'width' => '20%',
  ],
  $featuremap->name,
];
// Map Units
$rows[] = [
  [
    'data' => 'Map Units',
    'header' => TRUE,
  ],
  $featuremap->unittype_id->name,
];
// allow site admins to see the feature ID
if (user_access('view ids')) {
  // Feature Map ID
  $rows[] = [
    [
      'data' => 'Feature Map ID',
      'header' => TRUE,
      'class' => 'tripal-site-admin-only-table-row',
    ],
    [
      'data' => $featuremap->featuremap_id,
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
    'id' => 'tripal_featuremap-table-base',
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
if (property_exists($featuremap, 'description')) { ?>
    <div style="text-align: justify"><?php print $featuremap->description; ?></div> <?php
} 
