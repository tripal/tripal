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
 */
function tripal_fields_kvproperty_adder_widget(&$widget, $form, $form_state,
    $field, $instance, $langcode, $items, $delta, $element) {
  // This field has no value field.  Just a fieldset for adding new properties.
  $widget['#element_validate'] = array('tripal_fields_kvproperty_adder_widget_validate');

  $widget['#type'] = 'fieldset';
  $widget['#title'] = $element['#title'];
  $widget['#description'] = $element['#description'];
  $widget['#group'] = 'entity_form_vtabs';

  $widget['kvproperty_instructions'] = array(
    '#type' => 'item',
    '#markup' => t('You may add additional properties to this form by
        providing a property name (from a vocabulary) in the field below
        and clicking the "Add Property" button.  This will add a
        new field to the form above for the property you entered.
        In the future, this field will be present for all records
        of this type.'),
  );
  $widget['value'] = array(
    '#title' => t('Property Type'),
    '#type' => 'textfield',
    '#description' => t("Please enter the type of property that you want to
        add.  As you type, suggestions will be provided."),
    '#autocomplete_path' => "admin/tripal/vocab/cvterm/auto_name/",
  );
  $widget['kvproperty_adder_link'] = array(
    '#type' => 'item',
    '#markup' => '<span class="kvproperty-adder-link">' . l('Add a term', 'admin/tripal/vocab/cvterm/add', array('attributes' => array('target' => '_blank'))) . '</span>',
  );
  // When this button is clicked, the form will be validated and submitted.
  // Therefore, we set custom submit and validate functions to override the
  // default form submit.  In the validate function we set the form_state
  // to rebuild the form so the submit function never actually gets called,
  // but we need it or Drupal will run the default validate anyway.
  // we also set #limit_validation_errors to empty so fields that
  // are required that don't have values won't generate warnings.
  $widget['kvproperty_adder_button'] = array(
    '#value' => t('Add Property'),
    '#type' => 'submit',
    '#name' => 'kvproperty_adder_button',
    '#submit' => array('tripal_fields_kvproperty_adder_widget_submit'),
    '#limit_validation_errors' => array(array($field['field_name'])),
  );
}
/**
 * Callback function for validating the tripal_fields_kvproperty_adder_widget.
 */
function tripal_fields_kvproperty_adder_widget_validate($element, &$form_state) {

   // Add the new field to the entity
   if (array_key_exists('triggering_element', $form_state) and
       $form_state['triggering_element']['#name'] == 'kvproperty_adder_button') {

     $form_state['rebuild'] = TRUE;
     $field_name = $element['#parents'][0];
     $entity_type = $element['#entity']->type;
     $bundle = $element['#entity']->bundle;

     // Get the term for the property
     $kvproperty = tripal_fields_get_field_form_values($field_name, $form_state);
     $term = chado_generate_var('cvterm', array('name' => $kvproperty), $options = array('return_array' => TRUE));
     if (count($term) == 1) {
       $prop_field_name = $field_name . '__' . $term[0]->cvterm_id;

       // The field name is the table name in this case. We want to get the
       // primary key as this should be the field that maps th the value.
       $schema = chado_get_schema($field_name);
       $pkey = $schema['primary key'][0];

       $field_info = array(
         'field_type' => 'kvproperty',
         'widget_type' => 'tripal_fields_kvproperty_widget',
         'field_settings' => array(
           'chado_table' => $field_name,
           'chado_column' => $pkey,
         ),
         'widget_settings' => array(),
         'description' => $term[0]->definition ? $term[0]->definition : '',
         'label' => ucfirst(preg_replace('/_/', ' ', $term[0]->name)),
         'is_required' => FALSE,
         // All properties are unlimited.
         'cardinality' => FIELD_CARDINALITY_UNLIMITED,
       );
       tripal_add_bundle_field($prop_field_name, $field_info, $entity_type, $bundle);
     }
     else if (count($term) > 1) {
       form_set_error(implode('][', $element ['#parents']) . '][value', t("This term is present in multiple vocabularies. Please select the appropriate one."));
     }
     else {
       form_set_error(implode('][', $element ['#parents']) . '][value', t("Please provide a property type to add."));
     }
   }
}
/**
 * Callback function for submitting the tripal_fields_kvproperty_adder_widget.
 */
function tripal_fields_kvproperty_adder_widget_submit($element, &$form_state) {
}