<?php
$featuremap = $variables['node']->featuremap;
$feature_positions = [];

// expand the featuremap object to include the records from the featurepos table
// specify the number of features to show by default and the unique pager ID
$num_results_per_page = 25;
$featurepos_pager_id = 0;

// get the features aligned on this map
$options = [
  'return_array' => 1,
  'order_by' => ['map_feature_id' => 'ASC'],
  'pager' => [
    'limit' => $num_results_per_page,
    'element' => $featurepos_pager_id,
  ],
  'include_fk' => [
    'map_feature_id' => [
      'type_id' => 1,
      'organism_id' => 1,
    ],
    'feature_id' => [
      'type_id' => 1,
    ],
    'featuremap_id' => [
      'unittype_id' => 1,
    ],
  ],
];

$featuremap = chado_expand_var($featuremap, 'table', 'featurepos', $options);
$feature_positions = $featuremap->featurepos;


// get the total number of records
$total_features = chado_pager_get_count($featurepos_pager_id);


if (count($feature_positions) > 0) { ?>
    <div class="tripal_featuremap-data-block-desc tripal-data-block-desc">This
        map contains <?php print number_format($total_features) ?> features:
    </div> <?php

  // the $headers array is an array of fields to use as the colum headers.
  // additional documentation can be found here
  // https://api.drupal.org/api/drupal/includes%21theme.inc/function/theme_table/7
  $headers = [
    'Landmark',
    'Type',
    'Organism',
    'Feature Name',
    'Type',
    'Position',
  ];

  // the $rows array contains an array of rows where each row is an array
  // of values for each column of the table in that row.  Additional documentation
  // can be found here:
  // https://api.drupal.org/api/drupal/includes%21theme.inc/function/theme_table/7
  $rows = [];

  foreach ($feature_positions as $position) {
    $map_feature = $position->map_feature_id;
    $feature = $position->feature_id;
    $organism = $map_feature->organism_id;

    // check if there are any values in the featureposprop table for the start and stop
    $mappos = $position->mappos;
    $options = [
      'return_array' => 1,
      'include_fk' => [
        'type_id' => 1,
      ],
    ];
    $position = chado_expand_var($position, 'table', 'featureposprop', $options);
    $featureposprop = $position->featureposprop;
    $start = 0;
    $stop = 0;
    if (is_array($featureposprop)) {
      foreach ($featureposprop as $index => $property) {
        if ($property->type_id->name == 'start') {
          $start = $property->value;
        }
        if ($property->type_id->name == 'stop') {
          $stop = $property->value;
        }
      }
    }
    if ($start and $stop and $start != $stop) {
      $mappos = "$start-$stop";
    }
    if ($start and !$stop) {
      $mappos = $start;
    }
    if ($start and $stop and $start == $stop) {
      $mappos = $start;
    }

    $mfname = $map_feature->name;
    if (property_exists($map_feature, 'nid')) {
      $mfname = l($mfname, 'node/' . $map_feature->nid, ['attributes' => ['target' => '_blank']]);
    }
    $orgname = $organism->genus . " " . $organism->species . " (" . $organism->common_name . ")";
    if (property_exists($organism, 'nid')) {
      $orgname = l(
        "<i>" . $organism->genus . " " . $organism->species . "</i> (" . $organism->common_name . ")",
        "node/" . $organism->nid,
        ['html' => TRUE, 'attributes' => ['target' => '_blank']]
      );
    }
    $organism = $organism->genus . ' ' . $organism->species;

    $fname = $feature->name;
    if (property_exists($feature, 'nid')) {
      $fname = l($fname, 'node/' . $feature->nid, ['attributes' => ['target' => '_blank']]);
    }

    $rows[] = [
      $mfname,
      $map_feature->type_id->name,
      $orgname,
      $fname,
      $feature->type_id->name,
      $mappos . ' ' . $position->featuremap_id->unittype_id->name,
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
      'id' => 'tripal_featuremap-table-featurepos',
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
  // here we add the paramter 'block' => 'features'. This is because
  // the pager is not on the default block that appears. When the user clicks a
  // page number we want the browser to re-appear with the page is loaded.
  // We remove the 'pane' parameter from the original query parameters because
  // Drupal won't reset the parameter if it already exists.
  $get = $_GET;
  unset($_GET['pane']);
  $pager = [
    'tags' => [],
    'element' => $featurepos_pager_id,
    'parameters' => [
      'pane' => 'featurepos',
    ],
    'quantity' => $num_results_per_page,
  ];
  print theme_pager($pager);
  $_GET = $get;
}

