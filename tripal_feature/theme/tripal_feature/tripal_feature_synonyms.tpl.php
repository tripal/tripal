<?php
$feature = $variables['node']->feature;

// expand the feature object to include the synonyms from the feature_synonym 
// table in chado.
$options = array('return_array' => 1);
$feature = tripal_core_expand_chado_vars($feature, 'table', 'feature_synonym', $options);
$synonyms = $feature->feature_synonym;

if(count($synonyms) > 0){ ?>
  <div class="tripal_feature-data-block-desc tripal-data-block-desc">The feature '<?php print $feature->name ?>' has the following synonyms</div><?php
  
  // the $headers array is an array of fields to use as the colum headers. 
  // additional documentation can be found here 
  // https://api.drupal.org/api/drupal/includes%21theme.inc/function/theme_table/7
  // This table for the analysis has a vertical header (down the first column)
  // so we do not provide headers here, but specify them in the $rows array below.
  $headers = array('Synonym');
  
  // the $rows array contains an array of rows where each row is an array
  // of values for each column of the table in that row.  Additional documentation
  // can be found here:
  // https://api.drupal.org/api/drupal/includes%21theme.inc/function/theme_table/7 
  $rows = array();
  foreach ($synonyms as $feature_synonym){
    $rows[] = array(
      $feature_synonym->synonym_id->name
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
      'id' => 'tripal_feature-table-synonyms',
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
