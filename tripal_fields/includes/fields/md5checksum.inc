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
function tripal_fields_md5checksum_checkbox_formatter(&$element, $entity_type, $entity, $field,
    $instance, $langcode, $items, $display) {
  foreach ($items as $delta => $item) {
    $content = key_exists('value', $item) ? $item['value'] : '';
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
function tripal_fields_md5checksum_checkbox_widget($form, $form_state, $field, $instance, $langcode, $items, $delta, $element) {

  $field_name = $field['field_name'];
  $widget = $element;
  $widget +=  array(
    '#type' => 'checkbox',
    '#title' => $element['#title'],
    '#description' => $element['#description'],
    '#options' => array(0, 1),
    '#default_value' => 1,
    '#weight' => isset($element['#weight']) ? $element['#weight'] : 0,
    '#delta' => $delta,
    '#element_validate' => array('tripal_fields_md5checksum_checkbox_widget_validate'),
  );
  return $widget;
}
/**
 * Callback function for validating the tripal_fields_md5checksum_checkbox_widget.
 */
function tripal_fields_md5checksum_checkbox_widget_validate($element, &$form_state) {
  $field_name = $element['#field_name'];

  // Calculate the md5 checksum for the sequence only if md5 box is checked and
  // the residues exist.
  $residues = tripal_fields_get_field_form_values('feature__residues', $form_state);
  if (count($residues) > 0 && trim($residues[0]) != '') {
    tripal_fields_set_field_form_values($field_name, $form_state, md5($residues[0]));
  }
  else {
    // Otherwise, remove the md5 value
    tripal_fields_set_field_form_values($field_name, $form_state, '__NULL__');
  }
}