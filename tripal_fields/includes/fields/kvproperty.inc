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
function tripal_fields_kvproperty_formatter(&$element, $entity_type, $entity, $field,
  $instance, $langcode, $items, $display) {

  $content = '';
  foreach ($items as $delta => $item) {
    $content =  key_exists('value', $item) ? $item['value'] : '';
    $element[$delta] = array(
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
function tripal_fields_kvproperty_widget($form, $form_state, $field, $instance, $langcode, $items, $delta, $element) {

  $field_name = $field['field_name'];
  $widget = array();
  $element += array(
    '#element_validate' => array('tripal_fields_kvproperty_widget_validate'),
    '#type' => 'textfield',
    '#delta' => $delta,
  );

  if ($field['cardinality'] == 1) {
    $element['#default_value'] = !empty($items) ? $items[0] : '';
    $widget = array($element);
  }
  else {
    foreach ($items as $item) {
      $widget[$delta] = $element;
      $widget[$delta]['#default_value'] = $item;
      $widget[$delta]['#weight'] = $delta;
      $delta++;
    }
  }
  // Add one more row if delta is less than the cardinality.
  if ($field['cardinality'] == FIELD_CARDINALITY_UNLIMITED || $delta < $field['cardinality']){
    $widget[$delta] = $element;
    $widget[$delta]['#default_value'] = '';
    $widget[$delta]['#weight'] = $delta;
    $widget[$delta]['#required'] = ($element['#required'] && $delta == 0);
  }

  // The group of elements all-together need some extra functionality
  // after building up the full list (like draggable table rows).
  $widget['#theme'] = 'field_multiple_value_form';
  $widget['#title'] = $element['#title'];
  $widget['#description'] = $element['#description'];
  $widget['#field_name'] = $element['#field_name'];
  $widget['#language'] = $element['#language'];
  $widget['#weight'] = isset($element['#weight']) ? $element['#weight'] : 0;
  $widget['#cardinality'] = 1;
  return $widget;
}

/**
 *
 * @param unknown $form
 * @param unknown $form_state
 */
function tripal_fields_kvproperty_widget_form_ajax_callback($form, $form_state) {
  $field_name = $form_state['triggering_element']['#parents'][0];
  return $form[$field_name];
}
/**
 * Callback function for validating the tripal_fields_kvproperty_widget.
 */
function tripal_fields_kvproperty_widget_validate($element, &$form_state) {
   $field_name = $element['#field_name'];
}
/**
 * Callback function for submitting the tripal_fields_kvproperty_widget.
 */
function tripal_fields_kvproperty_widget_submit($element, &$form_state) {
}