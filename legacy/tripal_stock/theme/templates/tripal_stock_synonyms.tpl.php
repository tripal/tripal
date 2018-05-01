<?php
// there is no stock_synonym table, analogous to the stock_synonym table.
// Therefore, synonyms have been stored in the stockprop table with a type 
// of 'synonym' or 'alias'.
$stock = $node->stock;
$synonyms = [];

// expand the stock object to include the stockprop records
$options = ['return_array' => 1];
$stock = chado_expand_var($stock, 'table', 'stockprop', $options);
$stockprops = $stock->stockprop;

// iterate through all of the properties and pull out only the synonyms
if ($stockprops) {
  foreach ($stockprops as $stockprop) {
    if ($stockprop->type_id->name == 'synonym' or $stockprop->type_id->name == 'alias') {
      $synonyms[] = $stockprop;
    }
  }
}

if (count($synonyms) > 0) { ?>
    <div class="tripal_stock-data-block-desc tripal-data-block-desc">The stock
        '<?php print $stock->name ?>' has the following synonyms
    </div> <?php

  // the $headers array is an array of fields to use as the colum headers. 
  // additional documentation can be found here 
  // https://api.drupal.org/api/drupal/includes%21theme.inc/function/theme_table/7
  // This table for the analysis has a vertical header (down the first column)
  // so we do not provide headers here, but specify them in the $rows array below.
  $headers = ['Synonym'];

  // the $rows array contains an array of rows where each row is an array
  // of values for each column of the table in that row.  Additional documentation
  // can be found here:
  // https://api.drupal.org/api/drupal/includes%21theme.inc/function/theme_table/7 
  $rows = [];
  foreach ($synonyms as $property) {
    $rows[] = [
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
      'id' => 'tripal_stock-table-synonyms',
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
