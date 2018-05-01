<?php

$library = $variables['node']->library;

// get the feature_id's of the features that belong to this library.  But we only
// want 25 and we want a pager to let the user cycle between pages of features.
// so we, use the chado_select_record API function to get the results and
// generate the pager.  The function is smart enough to know which page the user is
// on and retrieves the proper set of features
$element = 0;        // an index to specify the pager if more than one is on the page
$num_per_page = 25;  // the number of features to show per page
$values = [
  'library_id' => $library->library_id,
];
$columns = ['feature_id'];
$options = [
  'pager' => [
    'limit' => $num_per_page,
    'element' => $element,
  ],
];
$results = chado_select_record('library_feature', $columns, $values, $options);

// now that we have all of the feature IDs, we want to expand each one so that we
// have all of the neccessary values, including the node ID, if one exists, and the
// cvterm type name.
$features = [];
foreach ($results as $library_feature) {
  $values = ['feature_id' => $library_feature->feature_id];
  $options = [
    'include_fk' => [
      'type_id' => 1,
    ],
  ];
  $features[] = chado_generate_var('feature', $values, $options);
}

if (count($features) > 0) { ?>
    <div class="tripal_library-data-block-desc tripal-data-block-desc">The
        following browser provides a quick view for new visitors. Use the
        searching mechanism to find specific features.
    </div> <?php

  // the $headers array is an array of fields to use as the colum headers.
  // additional documentation can be found here
  // https://api.drupal.org/api/drupal/includes%21theme.inc/function/theme_table/7
  $headers = ['Feature Name', 'Unique Name', 'Type'];

  // the $rows array contains an array of rows where each row is an array
  // of values for each column of the table in that row.  Additional documentation
  // can be found here:
  // https://api.drupal.org/api/drupal/includes%21theme.inc/function/theme_table/7
  $rows = [];

  foreach ($features as $feature) {
    $fname = $feature->name;
    if (property_exists($feature, 'nid')) {
      $fname = l($fname, "node/$feature->nid", ['attributes' => ['target' => '_blank']]);
    }
    $rows[] = [
      $fname,
      $feature->uniquename,
      $feature->type_id->name,
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
      'id' => 'tripal_library-table-features',
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

  // the $pager array values that control the behavior of the pager.  For
  // documentation on the values allows in this array see:
  // https://api.drupal.org/api/drupal/includes!pager.inc/function/theme_pager/7
  // here we add the paramter 'block' => 'feature_browser'. This is because
  // the pager is not on the default block that appears. When the user clicks a
  // page number we want the browser to re-appear with the page is loaded.
  // We remove the 'pane' parameter from the original query parameters because
  // Drupal won't reset the parameter if it already exists.
  $get = $_GET;
  unset($_GET['pane']);
  $pager = [
    'tags' => [],
    'element' => $element,
    'parameters' => [
      'pane' => 'features',
    ],
    'quantity' => $num_per_page,
  ];
  print theme_pager($pager);
  $_GET = $get;
}



