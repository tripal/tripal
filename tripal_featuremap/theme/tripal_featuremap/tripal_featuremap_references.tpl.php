<?php
$featuremap = $variables['node']->featuremap;
$references = array();

// expand the featuremap object to include the records from the featuremap_dbxref table
$options = array('return_array' => 1);
$featuremap = tripal_core_expand_chado_vars($featuremap, 'table', 'featuremap_dbxref', $options);
$featuremap_dbxrefs = $featuremap->featuremap_dbxref;
if (count($featuremap_dbxrefs) > 0 ) {
  foreach ($featuremap_dbxrefs as $featuremap_dbxref) {    
    $references[] = $featuremap_dbxref->dbxref_id;
  }
}


if(count($references) > 0){ ?>
  <div class="tripal_featuremap-data-block-desc tripal-data-block-desc">External references for this map</div><?php
   
  // the $headers array is an array of fields to use as the colum headers.
  // additional documentation can be found here
  // https://api.drupal.org/api/drupal/includes%21theme.inc/function/theme_table/7
  $headers = array('Dababase', 'Accession');
  
  // the $rows array contains an array of rows where each row is an array
  // of values for each column of the table in that row.  Additional documentation
  // can be found here:
  // https://api.drupal.org/api/drupal/includes%21theme.inc/function/theme_table/7
  $rows = array();

  foreach ($references as $dbxref){
    $dbname = $dbxref->db_id->name; 
    if ($dbxref->db_id->url) { 
      $dbname = l($dbname, $dbxref->db_id->url, array('attributes' => array('target' => '_blank')));
    } 
    
    $accession = $dbxref->accession; 
    if ($dbxref->db_id->urlprefix) { 
      $accession = l($accession, $dbxref->db_id->urlprefix . $dbxref->accession, array('attributes' => array('target' => '_blank')));
    } 
    $rows[] = array(
      $dbname,
      $accession
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
      'id' => 'tripal_featuremap-table-references',
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

