<?php

$feature = $variables['node']->feature;
$options = array('return_array' => 1);
$feature = chado_expand_var($feature, 'table', 'featureprop', $options);
$properties = $feature->featureprop;

if(count($properties) > 0){ 
  
  // the $headers array is an array of fields to use as the colum headers.
  // additional documentation can be found here
  // https://api.drupal.org/api/drupal/includes%21theme.inc/function/theme_table/7
  $headers = array('Property Name', 'Value');
  
  // the $rows array contains an array of rows where each row is an array
  // of values for each column of the table in that row.  Additional documentation
  // can be found here:
  // https://api.drupal.org/api/drupal/includes%21theme.inc/function/theme_table/7
  $rows = array();
  
  foreach ($properties as $property){
    $property = chado_expand_var($property,'field','featureprop.value');
    $rows[] = array(
      array(
        'data' => ucfirst(preg_replace('/_/', ' ', $property->type_id->name)),
        'width' => '20%'
      ),
      urldecode($property->value)
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
      'id' => 'tripal_feature-table-properties',
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
