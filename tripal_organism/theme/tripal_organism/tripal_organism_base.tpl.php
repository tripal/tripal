<?php

$organism  = $variables['node']->organism;
$organism = tripal_core_expand_chado_vars($organism,'field','organism.comment'); ?>

<div class="tripal_organism-data-block-desc tripal-data-block-desc"></div><?php

// generate the image tag
$image = '';
$image_url = tripal_organism_get_image_url($organism, $node->nid); 
if ($image_url) {
  $image = "<img class=\"tripal-organism-img\" src=\"$image_url\">";
}

// the $headers array is an array of fields to use as the colum headers. 
// additional documentation can be found here 
// https://api.drupal.org/api/drupal/includes%21theme.inc/function/theme_table/7
// This table for the organism has a vertical header (down the first column)
// so we do not provide headers here, but specify them in the $rows array below.
$headers = array();

// the $rows array contains an array of rows where each row is an array
// of values for each column of the table in that row.  Additional documentation
// can be found here:
// https://api.drupal.org/api/drupal/includes%21theme.inc/function/theme_table/7 
$rows = array();

// genus row
$rows[] = array(
  array(
    'data' => 'Genus', 
    'header' => TRUE,
    'width' => '20%',
  ),
  '<i>' . $organism->genus . '</i>'
);

// species row
$rows[] = array(
  array(
    'data' => 'Species', 
    'header' => TRUE
  ), 
  '<i>' . $organism->species . '</i>'
);

// common name row
$rows[] = array(
  array(
    'data' => 'Common Name',
    'header' => TRUE
  ),
  $organism->common_name,
);

// abbreviation row
$rows[] = array(
  array(
    'data' => 'Abbreviation', 
    'header' => TRUE
  ),
  $organism->abbreviation
);

// allow site admins to see the organism ID
if (user_access('administer tripal')) {
  // Organism ID
  $rows[] = array(
    array(
      'data'   => 'Organism ID',
      'header' => TRUE,
      'class'  => 'tripal-site-admin-only-table-row',
    ),
    array(
     'data'  => $organism->organism_id,
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
    'id' => 'tripal_organism-table-base',
    'class' => 'tripal-organism-data-table tripal-data-table',
  ), 
  'sticky' => FALSE,
  'caption' => '',
  'colgroups' => array(), 
  'empty' => '', 
); 

// once we have our table array structure defined, we call Drupal's theme_table()
// function to generate the table.
print theme_table($table); ?>
<div style="text-align: justify"><?php print $image . $organism->comment?></div>  