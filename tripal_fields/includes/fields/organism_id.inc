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
 * @return string
 */
function tripal_fields_organism_select_formatter(&$element, $entity_type, $entity, $field,
    $instance, $langcode, $items, $display) {
  foreach ($items as $delta => $item) {
    $organism = chado_select_record('organism', array('genus', 'species'), array('organism_id' => $item['value']));
    $content = '<i>' . $organism[0]->genus .' ' . $organism[0]->species . '</i>';
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
function tripal_fields_organism_select_widget($field_name, &$widget,
  &$form, &$form_state, $field, $instance, $langcode, $items, $delta, $element) {

  $options = tripal_get_organism_select_options();
  $widget += array(
    '#type' => 'select',
    '#title' => $element['#title'],
    '#description' => $element['#description'],
    '#options' => $options,
    '#default_value' => count($items) > 0 ? $items[0]['value'] : 0,
    '#required' => $element['#required'],
    '#weight' => isset($element['#weight']) ? $element['#weight'] : 0,
    '#delta' => $delta,
    '#element_validate' => array('tripal_fields_organism_select_widget_validate'),
  );
  $element['value'] = $widget;
}

/**
 * Callback function for validating the tripal_fields_organism_select_widget.
 */
function tripal_fields_organism_select_widget_validate($element, &$form_state) {
  $field_name = $element['#field_name'];

  // If the form ID is field_ui_field_edit_form, then the user is editing the
  // field's values in the manage fields form of Drupal.  We don't want
  // to validate it as if it were being used in a data entry form.
  if ($form_state['build_info']['form_id'] =='field_ui_field_edit_form') {
    return;
  }
  $organism_id = tripal_fields_get_field_form_values($field_name, $form_state);

  if (count($organism_id) == 0) {
    form_error($element, t("Please specify an organism that already exists in the database."));
  }
}
