<?php

/**
 *
 * @param unknown $entity_type
 * @param unknown $entity
 * @param unknown $field
 * @param unknown $instance
 * @param unknown $langcode
 * @param unknown $items
 * @param unknown $display
 */
function chado_feature__residues_formatter(&$element, $entity_type, $entity, $field,
  $instance, $langcode, $items, $display) {

  foreach ($items as $delta => $item) {
    $residues = key_exists('value', $item) ? $item['value'] : '';
    $content = '<pre class="residues-formatter">' . $residues . '</pre>';
    $element[$delta] = array(
      // We create a render array to produce the desired markup,
      '#type' => 'markup',
      '#markup' => $content,
    );
  }
}

/**
 *
 * @param unknown $field_name
 * @param unknown $widget
 * @param unknown $form
 * @param unknown $form_state
 * @param unknown $field
 * @param unknown $instance
 * @param unknown $langcode
 * @param unknown $items
 * @param unknown $delta
 * @param unknown $element
 */
function chado_feature__residues_widget(&$widget, $form, $form_state, $field, $instance, $langcode, $items, $delta, $element) {

  $widget['value'] = array(
    '#type' => 'textarea',
    '#title' => $element['#title'],
    '#description' => $element['#description'],
    '#weight' => isset($element['#weight']) ? $element['#weight'] : 0,
    '#default_value' => count($items) > 0 ? $items[0]['value'] : '',
    '#delta' => $delta,
    '#element_validate' => array('chado_feature__residues_widget_validate'),
    '#cols' => 30,
  );
}
/**
 * Callback function for validating the chado_feature__residues_widget.
 */
function chado_feature__residues_widget_validate($element, &$form_state) {
  $field_name = $element['#parents'][0];

  // Remove any white spaces.
  $residues = tripal_chado_get_field_form_values($field_name, $form_state);
  if ($residues) {
    $residues = preg_replace('/\s/', '', $residues);
    tripal_chado_set_field_form_values($field_name, $form_state, $residues);
  }
}