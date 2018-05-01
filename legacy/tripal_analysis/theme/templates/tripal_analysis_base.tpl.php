<?php
$analysis = $variables['node']->analysis;
$analysis = chado_expand_var($analysis, 'field', 'analysis.description'); ?>

<div class="tripal__analysis-data-block-desc tripal-data-block-desc"></div><?php

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

// Analysis Name row
$rows[] = [
  [
    'data' => 'Analysis Name',
    'header' => TRUE,
    'width' => '20%',
  ],
  $analysis->name,
];

// Implementation row
$software = $analysis->program;
if ($analysis->programversion != 'n/a') {
  $software .= " (" . $analysis->programversion . ")";
}
if ($analysis->algorithm) {
  $software .= ". " . $analysis->algorithm;
}
$rows[] = [
  [
    'data' => 'Method',
    'header' => TRUE,
  ],
  $software,
];

// Source row
$source = '';
if ($analysis->sourceuri) {
  $source = "<a href=\"$analysis->sourceuri\">$analysis->sourcename</a>";
}
else {
  $source = $analysis->sourcename;
}
if ($analysis->sourceversion) {
  $source = " (" . $analysis->sourceversion . ")";
}
$rows[] = [
  [
    'data' => 'Source',
    'header' => TRUE,
  ],
  $source,
];

// Date performed row
$rows[] = [
  [
    'data' => 'Date performed',
    'header' => TRUE,
  ],
  preg_replace("/^(\d+-\d+-\d+) .*/", "$1", $analysis->timeexecuted),
];

// allow site admins to see the analysis ID
if (user_access('view ids')) {
  // Analysis ID
  $rows[] = [
    [
      'data' => 'Analysis ID',
      'header' => TRUE,
      'class' => 'tripal-site-admin-only-table-row',
    ],
    [
      'data' => $analysis->analysis_id,
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
    'id' => 'tripal_analysis-table-base',
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
if (property_exists($analysis, 'description')) { ?>
    <div style="text-align: justify"><?php print $analysis->description; ?></div> <?php
} ?>

