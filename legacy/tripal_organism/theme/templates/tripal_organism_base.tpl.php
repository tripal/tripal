<?php
$chado_version = chado_get_version(TRUE);

$organism = $variables['node']->organism;
$organism = chado_expand_var($organism, 'field', 'organism.comment'); ?>

<div class="tripal_organism-data-block-desc tripal-data-block-desc"></div><?php

// generate the image tag
$image = '';
$image_url = tripal_get_organism_image_url($organism);
if ($image_url) {
  $image = "<img class=\"tripal-organism-img\" src=\"$image_url\">";
}

// the $headers array is an array of fields to use as the colum headers.
// additional documentation can be found here
// https://api.drupal.org/api/drupal/includes%21theme.inc/function/theme_table/7
// This table for the organism has a vertical header (down the first column)
// so we do not provide headers here, but specify them in the $rows array below.
$headers = [];

// the $rows array contains an array of rows where each row is an array
// of values for each column of the table in that row.  Additional documentation
// can be found here:
// https://api.drupal.org/api/drupal/includes%21theme.inc/function/theme_table/7
$rows = [];

$infra = '';
if ($chado_version > 1.2 and $organism->type_id) {
  $infra = $organism->type_id->name . ' <i>' . $organism->infraspecific_name . '</i>';
}

// full name row
$rows[] = [
  [
    'data' => 'Full Name',
    'header' => TRUE,
    'width' => '30%',
  ],
  '<i>' . $organism->genus . ' ' . $organism->species . '</i> ' . $infra,
];

// genus row
$rows[] = [
  [
    'data' => 'Genus',
    'header' => TRUE,
    'width' => '30%',
  ],
  '<i>' . $organism->genus . '</i>',
];

// species row
$rows[] = [
  [
    'data' => 'Species',
    'header' => TRUE,
  ],
  '<i>' . $organism->species . '</i>',
];

if ($chado_version > 1.2) {
  $type_id = $organism->type_id ? $organism->type_id->name : '';
  // type_id row
  $rows[] = [
    [
      'data' => 'Infraspecific Rank',
      'header' => TRUE,
    ],
    $type_id,
  ];
  // infraspecific name row
  $rows[] = [
    [
      'data' => 'Infraspecific Name',
      'header' => TRUE,
    ],
    '<i>' . $organism->infraspecific_name . '</i>',
  ];
}

// common name row
$rows[] = [
  [
    'data' => 'Common Name',
    'header' => TRUE,
  ],
  $organism->common_name,
];

// abbreviation row
$rows[] = [
  [
    'data' => 'Abbreviation',
    'header' => TRUE,
  ],
  $organism->abbreviation,
];

// allow site admins to see the organism ID
if (user_access('view ids')) {
  // Organism ID
  $rows[] = [
    [
      'data' => 'Organism ID',
      'header' => TRUE,
      'class' => 'tripal-site-admin-only-table-row',
    ],
    [
      'data' => $organism->organism_id,
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
    'id' => 'tripal_organism-table-base',
    'class' => 'tripal-organism-data-table tripal-data-table',
  ],
  'sticky' => FALSE,
  'caption' => '',
  'colgroups' => [],
  'empty' => '',
];

// once we have our table array structure defined, we call Drupal's theme_table()
// function to generate the table.
print theme_table($table); ?>
<div style="text-align: justify"><?php print $image . $organism->comment ?></div>