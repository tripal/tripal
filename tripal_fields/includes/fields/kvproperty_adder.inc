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
function tripal_fields_kvproperty_adder_formatter(&$element, $entity_type, $entity, $field,
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
function tripal_fields_kvproperty_adder_widget($form, $form_state, $field, $instance, $langcode, $items, $delta, $element) {

  $field_name = $field['field_name'];
  $widget = $element;
  $widget += array(
    '#element_validate' => array('tripal_fields_kvproperty_adder_widget_validate'),
    '#type' => 'fieldset',
    '#title' => $element['#title'],
    '#group' => 'ev_tabs',
    array(
      'kvproperty_instructions' => array(
        '#type' => 'item',
        '#markup' => t('You may add additional properties to this form by
            providing a property name (from a vocabulary) in the field below
            and clicking the "Add Property" button.  This will add a
            new field to the form above for the property you entered.'),
      ),
      'kvproperty_adder' => array(
        '#title' => t('Property Type'),
        '#type' => 'textfield',
        '#description' => t("Please enter the type of property that you want to add.  As you type, suggestions will be provided."),
        '#autocomplete_path' => "admin/tripal/chado/tripal_cv/cvterm/auto_name/",
      ),
      'kvproperty_adder_link' => array(
        '#type' => 'item',
        '#markup' => '<span class="kvproperty-adder-link">' . l('Add a term', 'admin/tripal/chado/tripal_cv/cvterm/add', array('attributes' => array('target' => '_blank'))) . '</span>',
      ),
      // When this button is clicked, the form will be validated and submitted.
      // Therefore, we set custom submit and validate functions to override the
      // default form submit.  In the validate function we set the form_state
      // to rebuild the form so the submit function never actually gets called,
      // but we need it or Drupal will run the default validate anyway.
      // we also set #limit_validation_errors to empty so fields that
      // are required that don't have values won't generate warnings.
      'kvproperty_adder_button' => array(
        '#value' => t('Add Property'),
        '#type' => 'submit',
        '#name' => 'kvproperty_adder_button',
        '#validate' => 'tripal_fields_kvproperty_adder_widget_validate',
        '#submit' => 'tripal_fields_kvproperty_adder_widget_submit',
        '#limit_validation_errors' => array(array($field_name)),
      ),
    ),
  );
  return $widget;
}

/**
 * Callback function for validating the tripal_fields_kvproperty_adder_widget.
 */
function tripal_fields_kvproperty_adder_widget_validate($element, &$form_state) {

   // Add the new field to the entity
   if (array_key_exists('triggering_element', $form_state) and
       $form_state['triggering_element']['#name'] == 'kvproperty_adder_button') {

     $form_state['rebuild'] = TRUE;

     $field_name = $element['#field_name'];
     $entity_type = $element['#entity_type'];
     $bundle = $element['#bundle'];

     // Get the term for the property
     $kvproperty = tripal_fields_get_field_form_values($field_name, $form_state, 'kvproperty_adder');
     $term = chado_generate_var('cvterm', array('name' => $kvproperty), $options = array('return_array' => TRUE));
     if (count($term) == 1) {
       $prop_field_name = $field_name . '__' . strtolower($term[0]->dbxref_id->db_id->name) . '_' . $term[0]->dbxref_id->accession;
       $field_info = array(
         'field_type' => 'kvproperty',
         'widget_type' => 'tripal_fields_kvproperty_widget',
         'field_settings' => array(),
         'widget_settings' => array(),
         'description' => '',
         'label' => ucfirst(preg_replace('/_/', ' ', $term[0]->name)),
         'is_required' => FALSE,
         // All properties are unlimited.
         'cardinality' => FIELD_CARDINALITY_UNLIMITED,
       );
       tripal_add_bundle_field($prop_field_name, $field_info, $entity_type, $bundle);
     }
     else if (count($term) > 1) {
       form_set_error(implode('][', $element ['#parents']) . '][0][kvproperty_adder', t("This term is present in multiple vocabularies. Please select the appropriate one."));
     }
     else {
       form_set_error(implode('][', $element ['#parents']) . '][0][kvproperty_adder', t("Please provide a property type to add."));
     }
   }
}
/**
 * Callback function for submitting the tripal_fields_kvproperty_adder_widget.
 */
function tripal_fields_kvproperty_adder_widget_submit($element, &$form_state) {
}