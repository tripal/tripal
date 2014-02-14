<?php

$library = $variables['node']->library;

$options = array('return_array' => 1);
$library = tripal_core_expand_chado_vars($library, 'table', 'library_cvterm', $options);
$terms = $library->library_cvterm;

// order the terms by CV
$s_terms = array();
if ($terms) {
  foreach ($terms as $term) {
    $s_terms[$term->cvterm_id->cv_id->name][] = $term;  
  }
}

if (count($s_terms) > 0) { ?>
  <div class="tripal_library-data-block-desc tripal-data-block-desc">The following terms have been associated with this <?php print $node->library->type_id->name ?>:</div>  <?php
  
  // iterate through each term
  $i = 0;
  foreach ($s_terms as $cv => $terms) {  
    // the $headers array is an array of fields to use as the colum headers.
    // additional documentation can be found here
    // https://api.drupal.org/api/drupal/includes%21theme.inc/function/theme_table/7
    $headers = array('Term', 'Definition');
    
    // the $rows array contains an array of rows where each row is an array
    // of values for each column of the table in that row.  Additional documentation
    // can be found here:
    // https://api.drupal.org/api/drupal/includes%21theme.inc/function/theme_table/7
    $rows = array();
    
    foreach ($terms as $term) { 

      $accession = $term->cvterm_id->dbxref_id->accession;
      if (is_numeric($term->cvterm_id->dbxref_id->accession)) {
        $accession = $term->cvterm_id->dbxref_id->db_id->name . ":" . $term->cvterm_id->dbxref_id->accession;
      }
      if ($term->cvterm_id->dbxref_id->db_id->urlprefix) {
        $accession = l($accession, $term->cvterm_id->dbxref_id->db_id->urlprefix . $accession, array('attributes' => array("target" => '_blank')));
      } 
      
      $rows[] = array(
        $accession,
        $term->cvterm_id->name
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
        'id' => "tripal_library-table-terms-$i",
      ),
      'sticky' => FALSE,
      'caption' => '<b>Vocabulary: ' . ucwords(preg_replace('/_/', ' ', $cv)) . '</b>',
      'colgroups' => array(),
      'empty' => '',
    );
    
    // once we have our table array structure defined, we call Drupal's theme_table()
    // function to generate the table.
    print theme_table($table);
    $i++;
  }
} ?>
