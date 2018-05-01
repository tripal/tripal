<?php
$phylotree = $variables['node']->phylotree;
$dbxref = $phylotree->dbxref_id;

// Make sure the dbxref isn't the null database. If not, then show this pane.
if ($dbxref and $dbxref->db_id->name != 'null') { ?>

    <div class="tripal_phylogeny-data-block-desc tripal-data-block-desc">This
        tree is also available in the following databases:
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

  $database = $dbxref->db_id->name . ': ' . $dbxref->db_id->description;
  if ($dbxref->db_id->url) {
    $database = l($dbxref->db_id->name, $dbxref->db_id->url, ['attributes' => ['target' => '_blank']]) . ': ' . $dbxref->db_id->description;
  }
  $accession = $dbxref->db_id->name . ':' . $dbxref->accession;
  if ($dbxref->db_id->urlprefix) {
    $accession = l($accession, $dbxref->db_id->urlprefix . $dbxref->accession, ['attributes' => ['target' => '_blank']]);
  }

  $rows[] = [
    $database,
    $accession,
  ];

  // the $table array contains the headers and rows array as well as other
  // options for controlling the display of the table.  Additional
  // documentation can be found here:
  // https://api.drupal.org/api/drupal/includes%21theme.inc/function/theme_table/7
  $table = [
    'header' => $headers,
    'rows' => $rows,
    'attributes' => [
      'id' => 'tripal_phylogeny-table-references',
      'class' => 'tripal-data-table',
    ],
    'sticky' => FALSE,
    'caption' => '',
    'colgroups' => [],
    'empty' => t('There are no database cross-references for this tree'),
  ];

  // once we have our table array structure defined, we call Drupal's theme_table()
  // function to generate the table.
  print theme_table($table);
}

