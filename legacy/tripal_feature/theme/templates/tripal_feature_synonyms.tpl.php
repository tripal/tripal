<?php
$feature = $variables['node']->feature;

// expand the feature object to include the synonyms from the feature_synonym 
// table in chado.
$options = ['return_array' => 1];
$feature = chado_expand_var($feature, 'table', 'feature_synonym', $options);
$synonyms = $feature->feature_synonym;

if (count($synonyms) > 0) { ?>
    <div class="tripal_feature-data-block-desc tripal-data-block-desc">The
    feature '<?php print $feature->name ?>' has the following
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
  foreach ($synonyms as $feature_synonym) {
    $rows[] = [
      $feature_synonym->synonym_id->name,
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
      'id' => 'tripal_feature-table-synonyms',
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
