<?php
$pub = $variables['node']->pub;


// expand the pub object to include the records from the pub_dbxref table
$options = array('return_array' => 1);
$pub = tripal_core_expand_chado_vars($pub, 'table', 'pub_dbxref', $options);
$pub_dbxrefs = $pub->pub_dbxref;

$references = array();
if (count($pub_dbxrefs) > 0 ) {
  foreach ($pub_dbxrefs as $pub_dbxref) {    
    $references[] = $pub_dbxref->dbxref_id;
  }
}

if(count($references) > 0){ ?>
  <div class="tripal_pub-data-block-desc tripal-data-block-desc">This publication is also available in the following databases:</div><?php 
  
  // the $headers array is an array of fields to use as the colum headers.
  // additional documentation can be found here
  // https://api.drupal.org/api/drupal/includes%21theme.inc/function/theme_table/7
  $headers = array('Database', 'Accession');
  
  // the $rows array contains an array of rows where each row is an array
  // of values for each column of the table in that row.  Additional documentation
  // can be found here:
  // https://api.drupal.org/api/drupal/includes%21theme.inc/function/theme_table/7
  $rows = array();
  
  foreach ($references as $dbxref){
    $database = $dbxref->db_id->name . ': ' . $dbxref->db_id->description;
    if ($dbxref->db_id->url) {
      $database = l($dbxref->db_id->name, $dbxref->db_id->url, array('attributes' => array('target' => '_blank'))) . ': ' . $dbxref->db_id->description; 
    }
    $accession = $dbxref->db_id->name . ':' . $dbxref->accession;
    if ($dbxref->db_id->urlprefix) {
      $accession = l($accession, $dbxref->db_id->urlprefix . $dbxref->accession, array('attributes' => array('target' => '_blank')));
    }
    if (property_exists($dbxref, 'is_primary')) {
      $accession .= " <i>(primary cross-reference)</i>";
    }
    
    $rows[] = array(
      $database,
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
      'id' => 'tripal_pub-table-references',
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

