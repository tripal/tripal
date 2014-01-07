<?php

/**
 * Theme the regex data in the tripal_bulk_loader_template_field_form form
 */
function tripal_bulk_loader_field_regex_fieldset($variables) {
  $element = $variables['element'];
  drupal_add_tabledrag('regex_list_form_id', 'order', 'sibling', 'rank-weight'); // needed for table dragging

  $header = array(
    'Match Pattern',
    'Replacement Pattern',
    'Order',
    ''
  );

  $rows = array();
  foreach (element_children($element) as $key) {
    $row = array();
    $row['data'][] = $element[$key]['pattern']['#markup'];
    $row['data'][] = $element[$key]['replace']['#markup'];
    $row['data'][] = drupal_render($element[$key]['new_index']) . drupal_render($element[$key]['id']);
    $row['data'][] = drupal_render($element[$key]['submit-delete']);
    $row['class'] = array('draggable'); // needed for table dragging
    $rows[] = $row;
  }

  return theme('table', array(
    'header' => $header,
    'rows' => $rows,
    'attributes' => array('id' => 'regex_list_form_id'), // needed for table dragging
  ));
}