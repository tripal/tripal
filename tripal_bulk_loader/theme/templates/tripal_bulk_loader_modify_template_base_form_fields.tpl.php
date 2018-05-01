<?php
/**
 * @file
 * Defines how the fields table in the Bulk Loader Template Edit form should be
 *   rendered.
 *
 * @param $element
 *   The FAPI definition of the records table.
 */

// generate table\
$record_header = ['class' => ['record'], 'data' => 'Record'];
$header = [
  $record_header,
  'Field Operations',
  'Field Name',
  'Chado Table',
  'Chado Field',
  'Field Type',
  'Field Settings',
]; //'Data Column', 'Constant Value', 'Referred Record');
$rows = [];

// This is an array to keep track of the record => field index as well as the number of fields.
// It is used to make the record column span all the field rows providing more room for 
// information and a visual separation between records.
// Expect: record_id => array(index1, index2, index3)
$field_record = [];

// Create a row for each sub-element that is not a form-api key (ie: #title).
foreach (element_children($element) as $key) {

  $row_element = &$element[$key];
  $row = [];

  // Describe the record.
  $row[] = [
    'class' => ['record'],
    'data' => drupal_render($row_element['record_id']),
  ];
  if (isset($field_record[$row_element['record_id']['#markup']])) {
    $field_record[$row_element['record_id']['#markup']][] = $key;
  }
  else {
    $field_record[$row_element['record_id']['#markup']] = [$key];
  }

  // Provide action links to interact with fields.
  $row[] = [
    'class' => ['tbl-action-field-links', 'active'],
    'data' => drupal_render($row_element['edit_submit']) . ' | '
      . drupal_render($row_element['delete_submit']) . '<br />'
      . drupal_render($row_element['view-record-link']),
  ];

  // Describe the field.
  $row[] = drupal_render($row_element['field_name']);
  $row[] = drupal_render($row_element['chado_table_name']);
  $row[] = drupal_render($row_element['chado_field_name']);

  // Determine the Type
  // Default to data field.
  $type = 'Data Field (Spreadsheet Value)';
  $icon = 'data:image/png;base64,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';
  $type_class = 'data';
  $value = drupal_render($row_element['column_num']);
  if (!empty($row_element['constant_value']['#markup'])) {
    $type = 'Constant';
    $icon = 'data:image/png;base64,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';
    $type_class = 'constant';
    $value = drupal_render($row_element['constant_value']);
  }
  if (!empty($row_element['foreign_record_id']['#markup'])) {
    $type = 'Record Referral';
    $icon = 'data:image/png;base64,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';
    $type_class = 'record-referral';
    $value = drupal_render($row_element['foreign_record_id']);
  }
  $row[] = [
    'data' => '<img src="' . $icon . '" width=16 height=16 title="' . $type . '"/>',
    'class' => ['field-type'],
  ];

  // Finally specify the field value.

  $row[] = $value;

  // Add this field to the table.
  $rows[] = [
    'class' => [$type_class],
    'data' => $row,
  ];

}

// Finally edit the table to make the record span the correct rows.
foreach ($field_record as $record_id => $keys) {
  foreach ($keys as $row_order => $row_key) {

    // We want to adjust the first row of this record to have the rowspan.
    if ($row_order == 0) {
      $rows[$row_key]['data'][0]['rowspan'] = sizeof($keys);
      $rows[$row_key]['class'][] = 'record-first-row';
    }
    // Then we want to delete the record cell in the other rows.
    else {
      unset($rows[$row_key]['data'][0]);
    }
  }
}

// Finally print the table.
print theme(
  'table',
  [
    'header' => $header,
    'rows' => $rows,
    //'attributes' => array('style'=>'table-layout: fixed; width: 100%')
  ]
);
?>
