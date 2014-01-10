<?php

// get the analysis object and expand it to include the records from the analysisprop table
$analysis = $variables['node']->analysis;
$analysis = tripal_core_expand_chado_vars($analysis,'table', 'analysisprop', array('return_array' => 1));
$analysisprops = $analysis->analysisprop;

// put the properties in an array for easier access
$properties = array();
foreach ($analysisprops as $property) {
  $property = tripal_core_expand_chado_vars($property,'field','analysisprop.value');
  $properties[] = $property;
}

if (count($properties) > 0) { ?>
  <div id="tripal_analysis-properties-box" class="tripal_analysis-info-box tripal-info-box">
    <div class="tripal_analysis-info-box-title tripal-info-box-title">More Details</div>
    <div class="tripal_analysis-info-box-desc tripal-info-box-desc">Additional information about this analysis:</div><?php
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
    print theme_table($table); ?>
  </div> <?php
}
