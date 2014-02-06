<?php
$organism = $variables['node']->organism;

// expand the organism object to include the libraries from the library
// table in chado.
$options = array('return_array' => 1);
$organism = tripal_core_expand_chado_vars($organism, 'table', 'library', $options);
$libraries = $organism->library;


if (count($libraries) > 0) {?>
  <div class="tripal_organism-data-block-desc tripal-data-block-desc">The following libraries are associated with this organism.</div> <?php 
  
  // the $headers array is an array of fields to use as the colum headers. 
  // additional documentation can be found here 
  // https://api.drupal.org/api/drupal/includes%21theme.inc/function/theme_table/7
  // This table for the analysis has a vertical header (down the first column)
  // so we do not provide headers here, but specify them in the $rows array below.
  $headers = array('Library Name', 'Type');
  
  // the $rows array contains an array of rows where each row is an array
  // of values for each column of the table in that row.  Additional documentation
  // can be found here:
  // https://api.drupal.org/api/drupal/includes%21theme.inc/function/theme_table/7 
  $rows = array();
  foreach ($libraries as $library){ 
    
    $libname = $library->name;
    if ($library->nid) {
      $libname = l($libname, "node/".$library->nid, array('attributes' => array('target' => '_blank')));
    }
    
    $typename = $library->type_id->name;
    if ($typename == 'cdna_library') {
      $typename = 'cDNA';
    }
    else if ($typename == 'bac_library') {
      $typename = 'BAC';
    }
    
    $rows[] = array(
      $libname,
      $typename
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
      'id' => 'tripal_organism-table-libraries',
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




