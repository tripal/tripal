<?php

// expand the feature object to include the records from the featurepos table
// specify the number of features to show by default and the unique pager ID
$num_results_per_page = 5;
$featurepos_pager_id = 20;

// get the maps associated with this feature
$feature = $variables['node']->feature;
$options = [
  'return_array' => 1,
  'order_by' => [
    'map_feature_id' => 'ASC',
  ],
  'pager' => [
    'limit' => $num_results_per_page,
    'element' => $featurepos_pager_id,
  ],
  'include_fk' => [
    'map_feature_id' => [
      'type_id' => 1,
      'organism_id' => 1,
    ],
    'featuremap_id' => [
      'unittype_id' => 1,
    ],
  ],
];

$feature = chado_expand_var($feature, 'table', 'featurepos', $options);

// because the featurepos table has  FK relationships with map_feature_id and feature_id with the feature table
// the function call above will try to expand both and will create an array of matches for each FK.
// we only want to show the map that this feature belongs to
$map_positions = $feature->featurepos->map_feature_id;

// get the total number of records
$total_records = chado_pager_get_count($featurepos_pager_id);


if (count($map_positions) > 0) { ?>
    <div class="tripal_feature-data-block-desc tripal-data-block-desc">This
    feature is contained in the
    following <?php print number_format($total_records) ?> map(s):</div><?php

  // the $headers array is an array of fields to use as the colum headers.
  // additional documentation can be found here
  // https://api.drupal.org/api/drupal/includes%21theme.inc/function/theme_table/7
  $headers = ['Map Name', 'Landmark', 'Type', 'Position'];

  // the $rows array contains an array of rows where each row is an array
  // of values for each column of the table in that row.  Additional documentation
  // can be found here:
  // https://api.drupal.org/api/drupal/includes%21theme.inc/function/theme_table/7
  $rows = [];

  // iterate through our map positions
  foreach ($map_positions as $position) {
    $map_feature = $position->map_feature_id;

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
    $start = '';
    $stop = '';
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

    // get the map name feature
    $map_name = $position->featuremap_id->name;
    if (property_exists($position->featuremap_id, 'nid')) {
      $map_name = l($map_name, 'node/' . $position->featuremap_id->nid, ['attributes' => ['target' => '_blank']]);
    }


    // get the landmark
    $landmark = $map_feature->name;
    if (property_exists($map_feature, 'nid')) {
      $landmark = l($landmark, 'node/' . $map_feature->nid, ['attributes' => ['target' => '_blank']]);
    }

    $rows[] = [
      $map_name,
      $landmark,
      $map_feature->type_id->name,
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
      'id' => 'tripal_feature-table-featurepos',
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

