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

  $chado_entity = $form['#entity']->chado_entity;
  $field_name = $field['field_name'];
  $widget = $element;

  $matches = array();
  preg_match('/(.*?)__(\d+)/', $field_name, $matches);
  // If the field name is not properly formatted then we can't tell what
  // table and type this is.  So just return.
  if (count($matches) != 3) {
    return $widget;
  }
  $table_name = $matches[1];
  $cvterm_id = $matches[2];

  // Get the name of the pkey field for this property table.
  $schema = chado_get_schema($table_name);
  $pkey = $schema['primary key'][0];

  // Get the name of the FK field that links to the base data table.
  $schema = chado_get_schema($chado_entity->data_table);
  $fk = $schema['primary key'][0];

  $pkey_value = '';
  $fk_value = $chado_entity->record_id;

  // The group of elements all-together need some extra functionality
  // after building up the full list (like draggable table rows).
  $widget['#theme'] = 'field_multiple_value_form';
  $widget['#title'] = $element['#title'];
  $widget['#description'] = $element['#description'];
  $widget['#field_name'] = $element['#field_name'];
  $widget['#language'] = $element['#language'];
  $widget['#weight'] = isset($element['#weight']) ? $element['#weight'] : 0;
  $widget['#element_validate'] = array('tripal_fields_kvproperty_widget_validate');
  $widget['#cardinality'] = 1;

  $widget[$delta] = $element;
  $widget[$delta] += array(
    $table_name . '__' . $pkey => array(
      '#type' => 'hidden',
      '#value' => $pkey_value,
    ),
    $table_name . '__' . $fk => array(
      '#type' => 'hidden',
      '#value' => $fk_value,
    ),
    $table_name . '__value' => array(
      '#type' => 'textfield',
      '#default_value' => !empty($items) ? $items[0] : '',
    ),
    $table_name . '__type_id' => array(
      '#type' => 'hidden',
      '#value' => $cvterm_id,
    ),
    $table_name . '__rank' => array(
      '#type' => 'hidden',
      '#value' => $delta,
    ),
  );

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