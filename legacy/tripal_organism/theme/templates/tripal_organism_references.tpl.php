<?php
$organism = $variables['node']->organism;
$references = [];

// expand the organism object to include the records from the organism_dbxref table
$options = ['return_array' => 1];
$organism = chado_expand_var($organism, 'table', 'organism_dbxref', $options);
$organism_dbxrefs = $organism->organism_dbxref;
if (count($organism_dbxrefs) > 0) {
  foreach ($organism_dbxrefs as $organism_dbxref) {
    if ($organism_dbxref->dbxref_id->db_id->name == 'GFF_source') {
      // check to see if the reference 'GFF_source' is there.  This reference is
      // used to if the Chado Perl GFF loader was used to load the organisms   
    }
    else {
      $references[] = $organism_dbxref->dbxref_id;
    }
  }
}


if (count($references) > 0) { ?>
    <div class="tripal_organism-data-block-desc tripal-data-block-desc">External
        references for this organism
    </div><?php

  // the $headers array is an array of fields to use as the colum headers.
  // additional documentation can be found here
  // https://api.drupal.org/api/drupal/includes%21theme.inc/function/theme_table/7
  $headers = ['Database', 'Accession'];

  // the $rows array contains an array of rows where each row is an array
  // of values for each column of the table in that row.  Additional documentation
  // can be found here:
  // https://api.drupal.org/api/drupal/includes%21theme.inc/function/theme_table/7
  $rows = [];

  foreach ($references as $dbxref) {

    // skip the GFF_source entry as this is just needed for the GBrowse chado adapter 
    if ($dbxref->db_id->name == 'GFF_source') {
      continue;
    }
    $dbname = $dbxref->db_id->name;
    if ($dbxref->db_id->url) {
      $dbname = l($dbname, $dbxref->db_id->url, ['attributes' => ['target' => '_blank']]);
    }

    $accession = $dbxref->accession;
    if ($dbxref->db_id->urlprefix) {
      $accession = l($accession, $dbxref->db_id->urlprefix . $dbxref->accession, ['attributes' => ['target' => '_blank']]);
    }
    if (property_exists($dbxref, 'is_primary')) {
      $accession .= " <i>(primary cross-reference)</i>";
    }
    $rows[] = [
      $dbname,
      $accession,
    ];
  }

  // the $table array contains the headers and rows array as well as other
  // options for controlling the display of the table.  Additional
  // documentation can be found here:
  // https://api.drupal.org/api/drupal/includes%21theme.inc/function/theme_table/7
  $table = [
    'header' => $headers,
    'rows' => $rows,
    'attributes' => [
      'id' => 'tripal_organism-table-references',
      'class' => 'tripal-data-table',
    ],
    'sticky' => FALSE,
    'caption' => '',
    'colgroups' => [],
    'empty' => '',
  ];

  // once we have our table array structure defined, we call Drupal's theme_table()
  // function to generate the table.
  print theme_table($table);
} ?>

