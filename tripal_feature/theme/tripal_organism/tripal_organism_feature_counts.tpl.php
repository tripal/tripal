<?php

$organism = $variables['node']->organism;
$enabled = 1;
$types = array();

if(property_exists($organism, 'feature_counts')) {
  $types    = $organism->feature_counts['types'];
  $names    = $organism->feature_counts['names'];
}


if (count($types) > 0){ ?>
  <div class="tripal_organism-data-block-desc tripal-data-block-desc">The following features are currently present for this organism</div> <?php
  
  // let admins know they can customize the terms that appear in the list
  print tripal_set_message("
    Administrators, you can customize the types of terms that appear in this report by navigating to the " . 
    l('Tripal feature configuration page', 'admin/tripal/chado/tripal_feature/configuration', array('attributes' => array('target' => '_blank'))) . "
    opening the section \"Feature Summary Report\" and adding the list of
    terms you want to appear in the list. You can rename terms as well. To refresh the data,re-populate the " .
    l('organism_feature_count', 'admin/tripal/schema/mviews', array('attributes' => array('target' => '_blank'))) . " 
    materialized view.",
    TRIPAL_INFO,
    array('return_html' => 1)
  ); 
  
  // the $headers array is an array of fields to use as the colum headers.
  // additional documentation can be found here
  // https://api.drupal.org/api/drupal/includes%21theme.inc/function/theme_table/7
  $headers = array('Feature Type' ,'Count');
  
  // the $rows array contains an array of rows where each row is an array
  // of values for each column of the table in that row.  Additional documentation
  // can be found here:
  // https://api.drupal.org/api/drupal/includes%21theme.inc/function/theme_table/7
  $rows = array();
  
  for ($j = 0; $j < count($types); $j++) {
    $type = $types[$j];
    $name = $names[$j];
    
    $rows[] = array(
      "<span title=\"" . $type->definition . "\">$name</span>",
      number_format($type->num_features),
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
      'id' => 'tripal_organism-table-features',
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




