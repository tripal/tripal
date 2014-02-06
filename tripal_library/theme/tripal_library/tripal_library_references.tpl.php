<?php
$library = $variables['node']->library;
$references = array();

// Second, expand the library object to include the records from the library_dbxref table
$options = array('return_array' => 1);
$library = tripal_core_expand_chado_vars($library, 'table', 'library_dbxref', $options);
$library_dbxrefs = $library->library_dbxref;
if (count($library_dbxrefs) > 0 ) {
  foreach ($library_dbxrefs as $library_dbxref) {    
    $references[] = $library_dbxref->dbxref_id;
  }
}


if(count($references) > 0){ ?>
  <div class="tripal_library-data-block-desc tripal-data-block-desc">External references for this <?php print $library->type_id->name ?></div><?php
   
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
      'id' => 'tripal_library-table-references',
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

