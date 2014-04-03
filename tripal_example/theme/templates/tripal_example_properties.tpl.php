<?php
$example = $node->example;

// expand the example to include the properties.
$options = array(
  'return_array' => 1,
  'order_by' => array('rank' => 'ASC'),
);
$example = chado_expand_var($example,'table', 'exampleprop', $options);
$exampleprops = $example->exampleprop;
$properties = array();

if (count($properties)) { ?>
  <div class="tripal_example-data-block-desc tripal-data-block-desc">Additional details for this example include:</div> <?php 

  // the $headers array is an array of fields to use as the colum headers.
  // additional documentation can be found here
  // https://api.drupal.org/api/drupal/includes%21theme.inc/function/theme_table/7
  $headers = array('Property Name', 'Value');
  
  // the $rows array contains an array of rows where each row is an array
  // of values for each column of the table in that row.  Additional documentation
  // can be found here:
  // https://api.drupal.org/api/drupal/includes%21theme.inc/function/theme_table/7
  $rows = array();
  
  $keywords = array();
  foreach ($properties as $property) {
    // each keyword is stored as a seperate properties. We want to show them
    // only in a single field not as a bunc of individual properties, so when we see one, 
    // save it in an array for later and down't add it yet to the table yet.
    if ($property->type_id->name == 'Keywords') {
      $keywords[] = $property->value;
      continue;
    }
    $rows[] = array(
      $property->type_id->name,
      $property->value
    );
  }
  // now add in a single row for all keywords
  if (count($keywords) > 0) {
    $rows[] = array(
      'Keywords',
      implode(', ', $keywords),
    );
  } 

  // the $table array contains the headers and rows array as well as other
  // options for controlling the display of the table.  Additional
  // documentation can be found here:
  // https://api.drupal.org/api/drupal/includes%21theme.inc/function/theme_table/7
  $table = array(
    'header' => $headers,
    'rows' => $rows,
    'attributes' => array(
      'id' => 'tripal_example-table-properties',
      'class' => 'tripal-data-table'
    ),
    'sticky' => FALSE,
    'caption' => '',
    'colgroups' => array(),
    'empty' => '',
  );
  
  // once we have our table array structure defined, we call Drupal's theme_table()
  // function to generate the table.
  print theme_table($table);
}
