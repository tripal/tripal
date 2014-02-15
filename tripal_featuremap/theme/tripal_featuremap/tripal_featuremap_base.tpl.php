<?php

$featuremap  = $variables['node']->featuremap;

// expand the description field
$featuremap = tripal_core_expand_chado_vars($featuremap, 'field', 'featuremap.description'); ?>

<div class="tripal_featuremap-data-block-desc tripal-data-block-desc"></div> <?php 

// the $headers array is an array of fields to use as the colum headers. 
// additional documentation can be found here 
// https://api.drupal.org/api/drupal/includes%21theme.inc/function/theme_table/7
// This table for the analysis has a vertical header (down the first column)
// so we do not provide headers here, but specify them in the $rows array below.
$headers = array();

// the $rows array contains an array of rows where each row is an array
// of values for each column of the table in that row.  Additional documentation
// can be found here:
// https://api.drupal.org/api/drupal/includes%21theme.inc/function/theme_table/7 
$rows = array();

// Map Name row
$rows[] = array(
  array(
    'data' => 'Map Name',
    'header' => TRUE,
    'width' => '20%',
  ),
  $featuremap->name
);
// Map Units
$rows[] = array(
  array(
    'data' => 'Map Units',
    'header' => TRUE
  ),
  $featuremap->unittype_id->name
);
// allow site admins to see the feature ID
if (user_access('administer tripal')) {
  // Feature Map ID
  $rows[] = array(
    array(
      'data' => 'Feature Map ID',
      'header' => TRUE,
      'class' => 'tripal-site-admin-only-table-row',
    ),
    array(
      'data' => $featuremap->featuremap_id,
      'class' => 'tripal-site-admin-only-table-row',
    ),
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
    'id' => 'tripal_featuremap-table-base',
  ),
  'sticky' => FALSE,
  'caption' => '',
  'colgroups' => array(),
  'empty' => '',
);

// once we have our table array structure defined, we call Drupal's theme_table()
// function to generate the table.
print theme_table($table);
if (property_exists($featuremap, 'description')) { ?>
  <div style="text-align: justify"><?php print $featuremap->description; ?></div> <?php  
} 
