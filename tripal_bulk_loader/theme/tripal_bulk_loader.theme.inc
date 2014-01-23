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

/**
 * Preprocessor function for the tripal_bulk_loader template
 *
 * @ingroup tripal_bulk_loader
 */
function tripal_bulk_loader_preprocess_tripal_bulk_loader_template(&$variables) {

  $resource = db_select('tripal_bulk_loader_template','t')
    ->fields('t')
    ->condition('template_id', $variables['template_id'])
    ->execute();
  $template = $resource->fetchObject();
  $template->template_array = unserialize($template->template_array);
  $variables['template'] = $template;

}