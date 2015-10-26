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

  foreach ($items as $delta => $item) {
    // Do nothing, this field is only meant for the form.
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
function tripal_fields_kvproperty_widget($field_name, &$widget,
  &$form, &$form_state, $field, $instance, $langcode, $items, $delta, $element) {

  $widget += array(
    '#element_validate' => array('tripal_fields_kvproperty_widget_validate'),
    '#type' => 'fieldset',
    '#title' => $element['#title'],
    '#description' =>  $element['#description'],
    '#weight' => isset($element['#weight']) ? $element['#weight'] : 0,
    '#delta' => $delta,
    '#group' => 'entity_vetical_tabs',
    array(
      'kvproperty' => array(
        '#title' => t('Property Type'),
        '#type' => 'textfield',
        '#description' => t("Please enter the type of property that you want to add.  As you type, suggestions will be provided."),
        '#autocomplete_path' => "eadmin/tripal/chado/tripal_cv/cvterm/auto_name/",
      ),
      'kvproperty_add' => array(
        '#value' => t('Add fields for property type'),
        '#type' => 'button',
        '#name' => 'kvproperty_add',
      ),
    ),
  );
}

/**
 * Callback function for validating the tripal_fields_kvproperty_widget.
 */
function tripal_fields_kvproperty_widget_validate($element, &$form_state) {
  // Add the new field to the entity
  //$form_state['rebuild'] = TRUE;
}