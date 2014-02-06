<?php
$feature = $variables['node']->feature;
$options = array('return_array' => 1);
$feature = tripal_core_expand_chado_vars($feature, 'table', 'analysisfeature', $options);
$analyses = $feature->analysisfeature;

// don't show this page if there are no analyses
if (count($analyses) > 0) { ?>
  <div class="tripal_feature-data-block-desc tripal-data-block-desc">This <?php print $feature->type_id->name ?> is derived from or has results from the following analyses</div> <?php
  
  // the $headers array is an array of fields to use as the colum headers.
  // additional documentation can be found here
  // https://api.drupal.org/api/drupal/includes%21theme.inc/function/theme_table/7
  $headers = array('Analysis Name' ,'Date Performed');
  
  // the $rows array contains an array of rows where each row is an array
  // of values for each column of the table in that row.  Additional documentation
  // can be found here:
  // https://api.drupal.org/api/drupal/includes%21theme.inc/function/theme_table/7
  $rows = array();
  
  foreach ($analyses as $analysis) {
    $analysis_name = $analysis->analysis_id->name;
    if (property_exists($analysis->analysis_id, 'nid')) {
      $analysis_name = l($analysis_name, "node/" . $analysis->analysis_id->nid);
    } 
    $rows[] = array(
      $analysis_name,
      preg_replace('/\d\d:\d\d:\d\d/', '',  $analysis->analysis_id->timeexecuted),
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
      'id' => 'tripal_feature-table-analyses',
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

