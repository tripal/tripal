<?php

$stock = $variables['node']->stock;
$options = array('return_array' => 1);
$stock = tripal_core_expand_chado_vars($stock, 'table', 'stockprop', $options);
$stockprops = $stock->stockprop;

// the stock synonyms are stored in the stockprop table because we do not have
// a stock_synonym table. Therefore, we don't want to synonyms in the properties
// list as those get shown by the tripal_stock_synonyms.tpl.inc template.
$properties = array();
if ($stockprops) {
  foreach ($stockprops as $property) {
    // we want to keep all properties but the stock_description as that
    // property is shown on the base template page.
    if($property->type_id->name != 'synonym' and  $property->type_id->name != 'alias') {
      $property = tripal_core_expand_chado_vars($property,'field','stockprop.value');
      $properties[] = $property;
    }
  }
}

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
    $property = tripal_core_expand_chado_vars($property,'field','stockprop.value');
    $rows[] = array(
      array('data' => ucfirst(preg_replace('/_/', ' ', $property->type_id->name)), 'width' => '20%'),
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
      'id' => 'tripal_stock-table-properties',
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
