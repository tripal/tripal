<?php
/**
 * Implements hook_info() for fields.
 *
 * This is a hook provided by the tripal_chado module for offloading the
 * hook_field_info() hook for each field to specify.
 *
 * The field provides a widget for adding new vocabularies for cvterm
 * linker tables. This will allow cvterms to be grouped by vocabulary
 * ('category').
 *
 */
function chado_linker__cvterm_adder_info() {
  return array(
    'label' => t('Add an Annotation Type'),
    'description' => t('This record may have any number of types of
      annotations. Use this field to first add the type.'),
    'default_widget' => 'chado_linker__cvterm_adder_widget',
    'default_formatter' => 'hidden',
    'settings' => array(),
    'storage' => array(
      'type' => 'field_chado_storage',
      'module' => 'tripal_chado',
      'active' => TRUE
    ),
  );
}
/**
 * Implements hook_attach_info().
 *
 * This is a hook provided by the tripal_Chado module. It allows the field
 * to specify which bundles it will attach to and to specify thee settings.
 *
 * @param $entity_type
 * @param $entity
 * @param $term
 *
 * @return
 *   A field array
 */
function chado_linker__cvterm_adder_attach_info($entity_type, $bundle, $target) {
  $field_info = array();

  $table_name = $target['data_table'];
  $type_table = $target['type_table'];
  $type_field = $target['field'];
  $cv_id      = $target['cv_id'];
  $cvterm_id  = $target['cvterm_id'];


  // If the linker table does not exists then we don't want to add attach.
  $cvterm_table = $table_name . '_cvterm';
  if (!chado_table_exists($cvterm_table)) {
    return $field_info;
  }

  // Initialize the field array.
  $field_info = array(
    'field_name' => $cvterm_table,
    'field_type' => 'chado_linker__cvterm_adder',
    'widget_type' => 'chado_linker__cvterm_adder_widget',
    'field_settings' => array(
      'base_table' => $table_name,
    ),
    'cardinality' => 1,
    'storage' => 'field_chado_storage',
    'widget_settings' => array('display_label' => 1),
    'description' => '',
    'label' => 'Additional Annotation Types',
    'is_required' => 0,
    // This feld is never visible so there are no field settings for
    // Chado nor the semantiv web.
  );

  return $field_info;
}
/**
 * Implements hook_widget_info.
 *
 * This is a hook provided by the tripal_chado module for offloading
 * the hook_field_widget_info() hook for each field to specify.
 */
function chado_linker__cvterm_adder_widget_info() {
  return array(
    'label' => t('Add an Annotation'),
    'field types' => array('chado_linker__cvterm_adder'),
  );
}
/**
 *
 */
function chado_linker__cvterm_adder_widget(&$widget, &$form, &$form_state,
    $field, $instance, $langcode, $items, $delta, $element) {

  // This field has no value field.  Just a fieldset for adding new annotation types.

  $widget['#element_validate'] = array('chado_linker__cvterm_adder_widget_validate');

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
    '#submit' => array('chado_linker__cvterm_adder_widget_submit'),
    '#limit_validation_errors' => array(array($field['field_name'])),
  );
}
/**
 * Callback function for validating the chado_linker__cvterm_adder_widget.
 */
function chado_linker__cvterm_adder_widget_validate($element, &$form_state) {

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
 * Callback function for submitting the chado_linker__cvterm_adder_widget.
 */
function chado_linker__cvterm_adder_widget_submit($element, &$form_state) {

}