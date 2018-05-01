<?php
$feature = $variables['node']->feature; ?>

    <div class="tripal_feature-data-block-desc tripal-data-block-desc"></div> <?php

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
    'data' => 'Name',
    'header' => TRUE,
    'width' => '20%',
  ],
  $feature->name,
];
// Unique Name row
$rows[] = [
  [
    'data' => 'Unique Name',
    'header' => TRUE,
  ],
  $feature->uniquename,
];
// Type row
$rows[] = [
  [
    'data' => 'Type',
    'header' => TRUE,
  ],
  $feature->type_id->name,
];
// Organism row
$organism = $feature->organism_id->genus . " " . $feature->organism_id->species . " (" . $feature->organism_id->common_name . ")";
if (property_exists($feature->organism_id, 'nid')) {
  $organism = l("<i>" . $feature->organism_id->genus . " " . $feature->organism_id->species . "</i> (" . $feature->organism_id->common_name . ")", "node/" . $feature->organism_id->nid, ['html' => TRUE]);
}
$rows[] = [
  [
    'data' => 'Organism',
    'header' => TRUE,
  ],
  $organism,
];
// Seqlen row
if ($feature->seqlen > 0) {
  $rows[] = [
    [
      'data' => 'Sequence length',
      'header' => TRUE,
    ],
    $feature->seqlen,
  ];
}
// allow site admins to see the feature ID
if (user_access('view ids')) {
  // Feature ID
  $rows[] = [
    [
      'data' => 'Feature ID',
      'header' => TRUE,
      'class' => 'tripal-site-admin-only-table-row',
    ],
    [
      'data' => $feature->feature_id,
      'class' => 'tripal-site-admin-only-table-row',
    ],
  ];
}
// Is Obsolete Row
if ($feature->is_obsolete == TRUE) {
  $rows[] = [
    [
      'data' => '<div class="tripal_feature-obsolete">This feature is obsolete</div>',
      'colspan' => 2,
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
    'id' => 'tripal_feature-table-base',
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
