<?php
$pub = $node->pub;

// expand the pub to include the properties.
$options = [
  'return_array' => 1,
  'order_by' => ['rank' => 'ASC'],
];
$pub = chado_expand_var($pub, 'table', 'pubprop', $options);
$pubprops = $pub->pubprop;
$properties = [];
if (is_array($pubprops)) {
  foreach ($pubprops as $property) {
    // skip the following properties as those are already on other templates
    if ($property->type_id->name == 'Abstract' or
      $property->type_id->name == 'Citation' or
      $property->type_id->name == 'Publication Dbxref' or
      $property->type_id->name == 'Authors' or
      $property->type_id->name == 'Structured Abstract Part') {
      continue;
    }
    $property = chado_expand_var($property, 'field', 'pubprop.value');
    $properties[] = $property;
  }
}
// we'll keep track of the keywords so we can lump them into a single row
$keywords = [];

if (count($properties)) { ?>
    <div class="tripal_pub-data-block-desc tripal-data-block-desc">Additional
        details for this publication include:
    </div> <?php

  // the $headers array is an array of fields to use as the colum headers.
  // additional documentation can be found here
  // https://api.drupal.org/api/drupal/includes%21theme.inc/function/theme_table/7
  $headers = ['Property Name', 'Value'];

  // the $rows array contains an array of rows where each row is an array
  // of values for each column of the table in that row.  Additional documentation
  // can be found here:
  // https://api.drupal.org/api/drupal/includes%21theme.inc/function/theme_table/7
  $rows = [];

  $keywords = [];
  foreach ($properties as $property) {
    // each keyword is stored as a seperate properties. We want to show them
    // only in a single field not as a bunc of individual properties, so when we see one, 
    // save it in an array for later and down't add it yet to the table yet.
    if ($property->type_id->name == 'Keywords') {
      $keywords[] = $property->value;
      continue;
    }
    $rows[] = [
      $property->type_id->name,
      $property->value,
    ];
  }
  // now add in a single row for all keywords
  if (count($keywords) > 0) {
    $rows[] = [
      'Keywords',
      implode(', ', $keywords),
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
      'id' => 'tripal_pub-table-properties',
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
