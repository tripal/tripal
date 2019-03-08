<?php
/**
 * @file
 * Defines how the records table in the Bulk Loader Template Edit form should
 *   be rendered.
 *
 * @param $element
 *   The FAPI definition of the records table.
 */

// Define the header & tell drupal this table should implement table drag for row re-ordering.
drupal_add_tabledrag('records-table', 'order', 'sibling', 'records-reorder');
$header = [' ', ' ', 'Record Name', 'Chado Table', 'Mode', 'Order',];
$rows = [];

// Create a row for each sub-element that is not a form-api key (ie: #title).
foreach (element_children($element) as $key) {

  $row_element = &$element[$key];
  $row_element['new_priority']['#attributes']['class'] = ['records-reorder'];
  $row = [];

  // Add an empty cell for the tabledrag icon.
  $row[] = ['class' => ['tbl-drag', 'active'], 'data' => ''];

  // Add our action links.
  $row[] = [
    'class' => ['tbl-action-record-links', 'active'],
    'data' => drupal_render($row_element['submit-edit_record']) . ' | '
      . drupal_render($row_element['submit-delete_record']) . ' | '
      . drupal_render($row_element['submit-duplicate_record']) . '<br>'
      . drupal_render($row_element['view-fields-link']) . ' | '
      . drupal_render($row_element['submit-add_field']),
  ];

  // Add the record information.
  $row[] = drupal_render($row_element['title']);
  $row[] = drupal_render($row_element['chado_table']);
  $row[] = drupal_render($row_element['mode']);
  $row[] = drupal_render($row_element['new_priority'])
    . drupal_render($row_element['id']);

  // Finally add the current row to the table.
  $rows[] = ['data' => $row, 'class' => ['draggable']];
}

// Finally print the generated table.
print theme(
  'table',
  [
    'header' => $header,
    'rows' => $rows,
    'attributes' => ['id' => 'records-table'],
  ]
);
?>