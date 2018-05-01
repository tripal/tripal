<?php
$feature = $variables['node']->feature;

// expand the feature object to include the libraries from the library
// table in chado.
$options = ['return_array' => 1];
$feature = chado_expand_var($feature, 'table', 'library_feature', $options);
$library_features = $feature->library_feature;


if (count($library_features) > 0) { ?>
    <div class="tripal_feature-data-block-desc tripal-data-block-desc">The
        following libraries are associated with this feature.
    </div> <?php

  // the $headers array is an array of fields to use as the colum headers. 
  // additional documentation can be found here 
  // https://api.drupal.org/api/drupal/includes%21theme.inc/function/theme_table/7
  // This table for the analysis has a vertical header (down the first column)
  // so we do not provide headers here, but specify them in the $rows array below.
  $headers = ['Library Name', 'Type'];

  // the $rows array contains an array of rows where each row is an array
  // of values for each column of the table in that row.  Additional documentation
  // can be found here:
  // https://api.drupal.org/api/drupal/includes%21theme.inc/function/theme_table/7 
  $rows = [];
  foreach ($library_features as $library_feature) {

    $libname = $library_feature->library_id->name;
    if ($library_feature->library_id->nid) {
      $libname = l($libname, "node/" . $library_feature->library_id->nid, ['attributes' => ['target' => '_blank']]);
    }

    $typename = $library_feature->library_id->type_id->name;
    if ($typename == 'cdna_library') {
      $typename = 'cDNA';
    }
    else {
      if ($typename == 'bac_library') {
        $typename = 'BAC';
      }
    }

    $rows[] = [
      $libname,
      $typename,
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
      'id' => 'tripal_feature-table-libraries',
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
