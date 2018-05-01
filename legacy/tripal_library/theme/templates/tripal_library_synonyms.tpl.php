<?php
$library = $variables['node']->library;

// expand the library object to include the synonyms from the library_synonym 
// table in chado.
$options = ['return_array' => 1];
$library = chado_expand_var($library, 'table', 'library_synonym', $options);
$synonyms = $library->library_synonym;

if (count($synonyms) > 0) { ?>
    <div class="tripal_library-data-block-desc tripal-data-block-desc">The
    library '<?php print $library->name ?>' has the following
    synonyms</div><?php

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
  foreach ($synonyms as $library_synonym) {
    $rows[] = [
      $library_synonym->synonym_id->name,
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
      'id' => 'tripal_library-table-synonyms',
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
