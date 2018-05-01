<?php

$library = $variables['node']->library;

// get the library description. IT uses a tern name of 'Library Description'
$record = [
  'table' => 'library',
  'id' => $library->library_id,
];
$property = [
  'type_name' => 'Library Description',
  'cv_name' => 'library_property',
];
$libprop = chado_get_property($record, $property);
$description = isset($libprop->value) ? $libprop->value : ''; ?>

    <div class="tripal_library-data-block-desc tripal-data-block-desc"></div> <?php

// the $headers array is an array of fields to use as the colum headers. 
// additional documentation can be found here 
// https://api.drupal.org/api/drupal/includes%21theme.inc/function/theme_table/7
// This table for the library has a vertical header (down the first column)
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
    'data' => 'Library Name',
    'header' => TRUE,
    'width' => '20%',
  ],
  $library->name,
];

// Unique row
$rows[] = [
  [
    'data' => 'Unique Name',
    'header' => TRUE,
  ],
  $library->uniquename,
];

// Organism row
$organism = $library->organism_id->genus . " " . $library->organism_id->species . " (" . $library->organism_id->common_name . ")";
if (property_exists($library->organism_id, 'nid')) {
  $organism = l("<i>" . $library->organism_id->genus . " " . $library->organism_id->species . "</i> (" . $library->organism_id->common_name . ")", "node/" . $library->organism_id->nid, ['html' => TRUE]);
}
$rows[] = [
  [
    'data' => 'Organism',
    'header' => TRUE,
  ],
  $organism,
];

// Library Type row
$rows[] = [
  [
    'data' => 'Type',
    'header' => TRUE,
  ],
  $library->type_id->name,
];

// allow site admins to see the library ID
if (user_access('view ids')) {
  // Library ID
  $rows[] = [
    [
      'data' => 'Library ID',
      'header' => TRUE,
      'class' => 'tripal-site-admin-only-table-row',
    ],
    $library->library_id,
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
    'id' => 'tripal_library-table-base',
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

// now add in the description below the table if one exists
if ($description) { ?>
    <div style="text-align: justify"><?php print $description; ?></div> <?php
}

