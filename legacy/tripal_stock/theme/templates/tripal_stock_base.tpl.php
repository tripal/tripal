<?php
$stock = $node->stock;
$organism = $node->stock->organism_id;
$main_db_reference = $stock->dbxref_id;

// expand the text fields
$stock = chado_expand_var($stock, 'field', 'stock.description');
$stock = chado_expand_var($stock, 'field', 'stock.uniquename'); ?>

    <div class="tripal_stock-data-block-desc tripal-data-block-desc"></div> <?php

// the $headers array is an array of fields to use as the colum headers. 
// additional documentation can be found here 
// https://api.drupal.org/api/drupal/includes%21theme.inc/function/theme_table/7
// This table for the stock has a vertical header (down the first column)
// so we do not provide headers here, but specify them in the $rows array below.
$headers = [];

// the $rows array contains an array of rows where each row is an array
// of values for each column of the table in that row.  Additional documentation
// can be found here:
// https://api.drupal.org/api/drupal/includes%21theme.inc/function/theme_table/7 
$rows = [];

// Stock Name
$rows[] = [
  [
    'data' => 'Name',
    'header' => TRUE,
    'width' => '20%',
  ],
  $stock->name,
];
// Stock Unique Name
$rows[] = [
  [
    'data' => 'Stock Name',
    'header' => TRUE,
  ],
  $stock->uniquename,
];
// Stock Type
$rows[] = [
  [
    'data' => 'Type',
    'header' => TRUE,
  ],
  ucwords(preg_replace('/_/', ' ', $stock->type_id->name)),
];

// Organism
$organism = $stock->organism_id->genus . " " . $stock->organism_id->species . " (" . $stock->organism_id->common_name . ")";
if (property_exists($stock->organism_id, 'nid')) {
  $organism = l("<i>" . $stock->organism_id->genus . " " . $stock->organism_id->species . "</i> (" . $stock->organism_id->common_name . ")", "node/" . $stock->organism_id->nid, ['html' => TRUE]);
}
$rows[] = [
  [
    'data' => 'Organism',
    'header' => TRUE,
  ],
  $organism,
];
// allow site admins to see the stock ID
if (user_access('view ids')) {
  // stock ID
  $rows[] = [
    [
      'data' => 'Stock ID',
      'header' => TRUE,
      'class' => 'tripal-site-admin-only-table-row',
    ],
    [
      'data' => $stock->stock_id,
      'class' => 'tripal-site-admin-only-table-row',
    ],
  ];
}
// Is Obsolete Row
if ($stock->is_obsolete == TRUE) {
  $rows[] = [
    [
      'data' => '<div class="tripal_stock-obsolete">This stock is obsolete</div>',
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
    'id' => 'tripal_stock-table-base',
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

// add in the description if there is one
if (property_exists($stock, 'description')) { ?>
    <div style="text-align: justify"><?php print $stock->description; ?></div> <?php
} 