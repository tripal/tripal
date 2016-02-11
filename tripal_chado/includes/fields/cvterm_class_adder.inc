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
function tripal_chado_cvterm_class_adder_formatter(&$element, $entity_type, $entity, $field,
  $instance, $langcode, $items, $display) {

  foreach ($items as $delta => $item) {
    // Do nothing, this field is only meant for the form.
  }
}
/**
 *
 */
function tripal_chado_cvterm_class_adder_widget(&$widget, &$form, &$form_state,
    $field, $instance, $langcode, $items, $delta, $element) {

  // This field has no value field.  Just a fieldset for adding new annotation types.

  $widget['#element_validate'] = array('tripal_chado_cvterm_class_adder_widget_validate');

  $widget['#type'] = 'fieldset';
  $widget['#title'] = $element['#title'];
  $widget['#description'] = $element['#description'];
  $widget['#group'] = 'entity_form_vtabs';

  $widget['cvterm_class_adder_instructions'] = array(
    '#type' => 'item',
    '#markup' => t('You may add annotation types to this form by
        providing a vocabulary name in the field below
        and clicking the "Add Annotation Type" button.  This will add a
        new field to the form above for the vocabulary you entered which
        will allow users to associate terms from that vocabulary to
        this record.'),
  );

  $options = tripal_get_cv_select_options();
  $widget['value'] = array(
    '#type' => 'select',
    '#title' => t('Vocabulary'),
    '#options' => $options,
    '#description' => t("Please enter the vocabulary that contains terms
        you want to allow users to use for annotations."),
  );

  // When this button is clicked, the form will be validated and submitted.
  // Therefore, we set custom submit and validate functions to override the
  // default form submit.  In the validate function we set the form_state
  // to rebuild the form so the submit function never actually gets called,
  // but we need it or Drupal will run the default validate anyway.
  // we also set #limit_validation_errors to empty so fields that
  // are required that don't have values won't generate warnings.
  $widget['cvterm_class_adder_button'] = array(
    '#value' => t('Add Annotation Type'),
    '#type' => 'submit',
    '#name' => 'cvterm_class_adder_button',
    '#submit' => array('tripal_chado_cvterm_class_adder_widget_submit'),
    '#limit_validation_errors' => array(array($field['field_name'])),
  );
}
/**
 * Callback function for validating the tripal_chado_cvterm_class_adder_widget.
 */
function tripal_chado_cvterm_class_adder_widget_validate($element, &$form_state) {

   // Add the new field to the entity
   if (array_key_exists('triggering_element', $form_state) and
       $form_state['triggering_element']['#name'] == 'cvterm_class_adder_button') {

     $form_state['rebuild'] = TRUE;
     $field_name = $element['#field_name'];
     $entity_type = $element['#entity']->type;
     $bundle = $element['#entity']->bundle;

     // Get the base table name from the field annotations.
     $field = field_info_field($field_name);
     $base_table = $field['settings']['base_table'];

     // Get the vocabulary.
     $cvterm_class_adder = tripal_chado_get_field_form_values($field_name, $form_state);
     $cv = chado_generate_var('cv', array('cv_id' => $cvterm_class_adder));

     if (!$cv) {
       form_set_error(implode('][', $element ['#parents']) . '][value', t("Please select a vocabulary."));
       return;
     }

     $type_field_name = $field_name . '__' . $cv->cv_id;

     // The field name is the table name in this case. We want to get the
     // primary key as this should be the field that maps th the value.
     $schema = chado_get_schema($field_name);
     $pkey = $schema['primary key'][0];

     $field_info = array(
       'field_type' => 'cvterm',
       'widget_type' => 'tripal_chado_cvterm_widget',
       'field_settings' => array(
         'chado_table' => $field_name,
         'chado_column' => $pkey,
         'base_table' => $base_table,
       ),
       'storage' => 'field_chado_storage',
       'widget_settings' => array(),
       'description' => "Annotations from the $cv->name vocabulary",
       'label' => ucfirst(preg_replace('/_/', ' ', $cv->name)),
       'is_required' => FALSE,
       // All annotations are unlimited.
       'cardinality' => FIELD_CARDINALITY_UNLIMITED,
     );
     tripal_add_bundle_field($type_field_name, $field_info, $entity_type, $bundle);

   }
}
/**
 * Callback function for submitting the tripal_chado_cvterm_class_adder_widget.
 */
function tripal_chado_cvterm_class_adder_widget_submit($element, &$form_state) {

}