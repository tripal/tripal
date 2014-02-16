<?php
$analysis = $variables['node']->analysis;
$analysis = tripal_core_expand_chado_vars($analysis,'field','analysis.description'); ?>

<div class="tripal__analysis-data-block-desc tripal-data-block-desc"></div><?php 
  
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

// Analysis Name row
$rows[] = array(
  array(
    'data' => 'Analysis Name',
    'header' => TRUE,
    'width' => '20%',
  ),
  $analysis->name
);

// Implementation row
$software = $analysis->program;
if($analysis->programversion != 'n/a'){
  $software .=  " (" . $analysis->programversion . ")";
}
if($analysis->algorithm){
  $software .= ". " . $analysis->algorithm;
}
$rows[] = array(
  array(
    'data' => 'Method',
    'header' => TRUE
  ),
  $software
);

// Source row
$source = '';
if($analysis->sourceuri){
  $source = "<a href=\"$analysis->sourceuri\">$analysis->sourcename</a>";
} 
else {
  $source = $analysis->sourcename;
}
if($analysis->sourceversion){
  $source = " (" . $analysis->sourceversion . ")";
}
$rows[] = array(
  array(
    'data' => 'Source',
    'header' => TRUE
  ),
  $source
);

// Date performed row
$rows[] = array(
  array(
    'data' => 'Date performed',
    'header' => TRUE
  ),
  preg_replace("/^(\d+-\d+-\d+) .*/","$1", $analysis->timeexecuted),
); 

// allow site admins to see the analysis ID
if (user_access('administer tripal')) {
  // Analysis ID
  $rows[] = array(
    array(
      'data' => 'Analysis ID',
      'header' => TRUE,
      'class' => 'tripal-site-admin-only-table-row',
    ),
    array(
      'data' => $analysis->analysis_id,
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
    'id' => 'tripal_analysis-table-base',
  ),
  'sticky' => FALSE,
  'caption' => '',
  'colgroups' => array(),
  'empty' => '',
);

// once we have our table array structure defined, we call Drupal's theme_table()
// function to generate the table.
print theme_table($table); 
if (property_exists($analysis, 'description')) { ?>
  <div style="text-align: justify"><?php print $analysis->description; ?></div> <?php  
} ?>

