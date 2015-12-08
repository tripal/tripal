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
function tripal_chado_seqlen_hidden_formatter(&$element, $entity_type, $entity, $field,
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
function tripal_chado_seqlen_hidden_widget(&$widget, $form, $form_state, $field, $instance, $langcode, $items, $delta, $element) {

  $widget['value'] =  array(
    '#type' => 'hidden',
    '#title' => $element['#title'],
    '#description' => $element['#description'],
    '#weight' => isset($element['#weight']) ? $element['#weight'] : 0,
    '#delta' => $delta,
    '#element_validate' => array('tripal_chado_seqlen_hidden_widget_validate'),
  );
}
/**
 * Callback function for validating the tripal_chado_seqlen_hidden_widget.
 */
function tripal_chado_seqlen_hidden_widget_validate($element, &$form_state) {
  $field_name = $element['#parents'][0];

  // Get the residues so we can calculate teh length.
  $residues = tripal_chado_get_field_form_values('feature__residues', $form_state);

  // Remove any white spaces.
  if ($residues) {
    $residues = preg_replace('/\s/', '', $residues);
    tripal_chado_set_field_form_values($field_name, $form_state, strlen($residues));
  }
  else {
    // Otherwise, remove the md5 value
    tripal_chado_set_field_form_values($field_name, $form_state, '__NULL__');
  }
}