<?php
$node = $variables['node'];
$phylotree = $node->phylotree;

if ($phylotree->analysis_id) {

  // the $headers array is an array of fields to use as the colum headers.
  // additional documentation can be found here
  // https://api.drupal.org/api/drupal/includes%21theme.inc/function/theme_table/7
  $header = [
    'Name',
    'Description',
    [
      'data' => 'Metadata',
      'width' => '50%',
    ],
  ];

  // the $rows array contains an array of rows where each row is an array
  // of values for each column of the table in that row.  Additional documentation
  // can be found here:
  // https://api.drupal.org/api/drupal/includes%21theme.inc/function/theme_table/7
  $rows = [];

  $analysis = $phylotree->analysis_id;
  if ($analysis) {
    $analysis = chado_expand_var($analysis, 'field', 'analysis.description');
    // Source row
    $source = '';
    if ($analysis->sourceuri) {
      $source = "<a href=\"$analysis->sourceuri\">$analysis->sourcename</a>";
    }
    else {
      $source = $analysis->sourcename;
    }
    if ($analysis->sourceversion) {
      $source = " (" . $analysis->sourceversion . ")";
    }

    $software = $analysis->program;
    if ($analysis->programversion != 'n/a') {
      $software .= " (" . $analysis->programversion . ")";
    }
    if ($analysis->algorithm) {
      $software .= ". " . $analysis->algorithm;
    }
    $date = preg_replace("/^(\d+-\d+-\d+) .*/", "$1", $analysis->timeexecuted);
    $metadata = "
      <dl class=\"tripal-dl\">
        <dt>Method</dt> <dd>: $software</dd>
        <dt>Source</dt> <dd>: $source</dd>
        <dt>Date</dt>   <dd>: $date</dd>
      </dl>
    ";

    $analysis_name = $analysis->name;
    if (property_exists($analysis, 'nid')) {
      $analysis_name = l($analysis_name, "node/" . $analysis->nid);
    }
    $rows[] = [$analysis_name, $analysis->description, $metadata];
  }

  // the $table array contains the headers and rows array as well as other
  // options for controlling the display of the table.  Additional
  // documentation can be found here:
  // https://api.drupal.org/api/drupal/includes%21theme.inc/function/theme_table/7
  $table = [
    'header' => $header,
    'rows' => $rows,
    'attributes' => [
      'id' => 'tripal_phylogeny-table-analysis',
      'class' => 'tripal-data-table',
    ],
    'sticky' => FALSE,
    'caption' => '',
    'colgroups' => [],
    'empty' => t('This tree is not associated with an analysis'),
  ];
  print theme_table($table);
}
