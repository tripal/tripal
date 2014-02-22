<?php

$library  = $variables['node']->library;

// get the library description. IT uses a tern name of 'Library Description'
$libprop = tripal_library_get_property($library->library_id, 'Library Description');
$description = $libprop->value; ?>

<div class="tripal_library-data-block-desc tripal-data-block-desc"></div> <?php 

// the $headers array is an array of fields to use as the colum headers. 
// additional documentation can be found here 
// https://api.drupal.org/api/drupal/includes%21theme.inc/function/theme_table/7
// This table for the library has a vertical header (down the first column)
// so we do not provide headers here, but specify them in the $rows array below.
$headers = array();

// the $rows array contains an array of rows where each row is an array
// of values for each column of the table in that row.  Additional documentation
// can be found here:
// https://api.drupal.org/api/drupal/includes%21theme.inc/function/theme_table/7 
$rows = array();

// Name row
$rows[] = array(
  array(
    'data' => 'Library Name',
    'header' => TRUE,
    'width' => '20%',
  ),
  $library->name
);

// Unique row
$rows[] = array(
  array(
    'data' => 'Unique Name',
    'header' => TRUE
  ),
  $library->uniquename
);

// Organism row
$organism = $library->organism_id->genus ." " . $library->organism_id->species ." (" .$library->organism_id->common_name .")";
if (property_exists($library->organism_id, 'nid')) {
  $organism = l("<i>" . $library->organism_id->genus . " " . $library->organism_id->species . "</i> (" .$library->organism_id->common_name .")", "node/".$library->organism_id->nid, array('html' => TRUE));
} 
$rows[] = array(
  array(
    'data' => 'Organism',
    'header' => TRUE
  ),
  $organism
);

// Library Type row
$rows[] = array(
  array(
    'data' => 'Type',
    'header' => TRUE
  ),
  $library->type_id->name,
);

// allow site admins to see the library ID
if (user_access('administer tripal')) {
  // Library ID
  $rows[] = array(
    array(
      'data'   => 'Library ID',
      'header' => TRUE,
      'class'  => 'tripal-site-admin-only-table-row',
    ),
    $library->uniquename,
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
    'id' => 'tripal_library-table-base',
  ),
  'sticky' => FALSE,
  'caption' => '',
  'colgroups' => array(),
  'empty' => '',
);

// once we have our table array structure defined, we call Drupal's theme_table()
// function to generate the table.
print theme_table($table); 

// now add in the description below the table if one exists
if ($description) { ?>
  <div style="text-align: justify"><?php print $description; ?></div> <?php  
}

