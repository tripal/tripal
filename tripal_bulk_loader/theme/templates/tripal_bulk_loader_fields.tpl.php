<?php
$node = $variables['node'];

// Retrieve Template
$template = db_select('tripal_bulk_loader_template', 't')
  ->fields('t')
  ->condition('template_id', $node->template->template_id, '=')
  ->execute()
  ->fetchObject();

$template->template_array = unserialize($template->template_array);

// Summarize Template
$fields = [];
$constants = [];
foreach ($template->template_array as $priority => $table_array) {
  if (!is_array($table_array)) {
    continue;
  }

  $table = $table_array['table'];
  $record = $table_array['record_id'];
  foreach ($table_array['fields'] as $field) {
    if (preg_match('/table field/', $field['type'])) {
      $field['table'] = $table;
      $field['record'] = $record;
      $sheet = 0;//$field['spreadsheet sheet'];
      $column = $field['spreadsheet column'];
      $fields[$sheet . '-' . $column][] = $field;
    }
    elseif ($field['type'] == 'constant') {
      $field['table'] = $table;
      $field['record'] = $record;
      $constants[] = $field;
    }
  }
} ?>

    <p><b>Constants</b></p> <?php

// add a table describing the constants specified in the file
if (sizeof($constants)) {
  $headers = [
    'Record Name',
    'Field Name',
    'Value',
    'Chado Table',
    'Chado Field',
  ];
  $rows = [];

  // iterate through the fields and add rows to the table
  foreach ($constants as $field) {
    $rows[] = [
      $field['record'],
      $field['title'],
      $field['constant value'],
      $field['table'],
      $field['field'],
    ];
  }

  // theme the table
  $table = [
    'header' => $headers,
    'rows' => $rows,
    'attributes' => [
      'id' => 'tripal_bulk_loader-table-constants',
      'class' => 'tripal-data-table',
    ],
    'sticky' => FALSE,
    'caption' => '',
    'colgroups' => [],
    'empty' => '',
  ];
  print theme_table($table);

}
?>

    <br><p><b>Data Columns</b></p> <?php

// add a table specifying the data file columns
if (sizeof($fields)) {

  $headers = [
    'Record Name',
    'Field Name',
    'Data File Column',
    'Chado Table',
    'Chado Field',
  ];
  $rows = [];

  // iterate through the fields and add rows to the table
  foreach ($fields as $column) {
    foreach ($column as $field) {
      $rows[] = [
        $field['record'],
        $field['title'],
        $field['spreadsheet column'],
        $field['table'],
        $field['field'],
      ];
    }
  }

  // theme the table
  $table = [
    'header' => $headers,
    'rows' => $rows,
    'attributes' => [
      'id' => 'tripal_bulk_loader-table-columns',
      'class' => 'tripal-data-table',
    ],
    'sticky' => FALSE,
    'caption' => '',
    'colgroups' => [],
    'empty' => '',
  ];
  print theme_table($table);
}
