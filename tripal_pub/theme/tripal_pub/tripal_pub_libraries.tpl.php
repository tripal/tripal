<?php
$pub = $variables['node']->pub;
$libraries = array();

// get the libraries that are associated with this publication.  But we only
// want 25 and we want a pager to let the user cycle between pages of libraries.
// so we, use the tripal_core_chado_select API function to get the results and
// generate the pager.  The function is smart enough to know which page the user is
// on and retrieves the proper set of libraries

$element = 3;        // an index to specify the pager this must be unique amongst all pub templates
$num_per_page = 25;  // the number of libraries to show per page$num_results_per_page = 25; 

// get the libraries from the library_pub table
$options = array(  
  'return_array' => 1,
  'pager' => array(
    'limit'   => $num_per_page, 
    'element' => $element
  ),
);

$pub = tripal_core_expand_chado_vars($pub, 'table', 'library_pub', $options);
$library_pubs = $pub->library_pub;
if (count($library_pubs) > 0 ) {
  foreach ($library_pubs as $library_pub) {    
    $libraries[] = $library_pub->library_id;
  }
}

// get the total number of records
$total_records = chado_pager_get_count($element);

if(count($libraries) > 0){ ?>
  <div class="tripal_pub-data-block-desc tripal-data-block-desc">This publication contains information about <?php print number_format($total_records) ?> libraries:</div> <?php 

  // the $headers array is an array of fields to use as the colum headers.
  // additional documentation can be found here
  // https://api.drupal.org/api/drupal/includes%21theme.inc/function/theme_table/7
  $headers = array('Library Name', 'Unique Name', 'Organism');
  
  // the $rows array contains an array of rows where each row is an array
  // of values for each column of the table in that row.  Additional documentation
  // can be found here:
  // https://api.drupal.org/api/drupal/includes%21theme.inc/function/theme_table/7
  $rows = array();
  
  foreach ($libraries as $library){
     $library_name = $library->name;
     if (property_exists($library, 'nid')) {
       $library_name = l($library_name, 'node/' . $library->nid, array('attributes' => array('target' => '_blank')));
     }
     $organism = '<i>' . $library->organism_id->genus . ' ' . $library->organism_id->species . '</i>';
     if (property_exists($library->organism_id, 'nid')) {
       $organism = l($organism, 'node/' . $library->organism_id->nid, array('attributes' => array('target' => '_blank')));
     }
     $rows[] = array(
       $library_name,
       $library->uniquename,
       $organism,
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
      'id' => 'tripal_pub-table-libraries',
    ),
    'sticky' => FALSE,
    'caption' => '',
    'colgroups' => array(),
    'empty' => '',
  );
  // once we have our table array structure defined, we call Drupal's theme_table()
  // function to generate the table.
  print theme_table($table);
  
  // the $pager array values that control the behavior of the pager.  For
  // documentation on the values allows in this array see:
  // https://api.drupal.org/api/drupal/includes!pager.inc/function/theme_pager/7
  // here we add the paramter 'block' => 'libraries'. This is because
  // the pager is not on the default block that appears. When the user clicks a
  // page number we want the browser to re-appear with the page is loaded.
  $pager = array(
    'tags' => array(),
    'element' => $element,
    'parameters' => array(
      'block' => 'libraries'
    ),
    'quantity' => $num_per_page,
  );
  print theme_pager($pager); 
}

