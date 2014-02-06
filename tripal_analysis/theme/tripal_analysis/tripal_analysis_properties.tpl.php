<?php

// get the analysis object and expand it to include the records from the analysisprop table
$analysis = $variables['node']->analysis;
$analysis = tripal_core_expand_chado_vars($analysis,'table', 'analysisprop', array('return_array' => 1));
$properties = $analysis->analysisprop;

if (count($properties) > 0) { ?>
  <div class="tripal_analysis-data-block-desc tripal-data-block-desc">Additional information about this analysis:</div><?php
  
  // the $headers array is an array of fields to use as the colum headers.
  // additional documentation can be found here
  // https://api.drupal.org/api/drupal/includes%21theme.inc/function/theme_table/7
  $headers = array('Property Name', 'Value');
  
  // the $rows array contains an array of rows where each row is an array
  // of values for each column of the table in that row.  Additional documentation
  // can be found here:
  // https://api.drupal.org/api/drupal/includes%21theme.inc/function/theme_table/7
  $rows = array();      
  foreach ($properties as $property) {
    $rows[] = array(
      ucfirst(preg_replace('/_/', ' ', $property->type_id->name)),
      $property->value
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
      'id' => 'tripal_analysis-table-properties',
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
