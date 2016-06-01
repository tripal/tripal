<?php
class chado_linker__cvterm_addr extends TripalField {
  /**
   * @see TripalField::field_info()
   */
  function field_info() {
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
      'no_ui' => TRUE
    );
  }
  /**
   * @see TripalField::can_attach()
   */
  protected function can_attach($entity_type, $bundle, $details) {
    $field_info = array();

    $table_name = $details['chado_table'];
    $type_table = $details['chado_type_table'];
    $type_field = $details['chado_type_column'];
    $cv_id      = $details['chado_cv_id'];
    $cvterm_id  = $details['chado_cvterm_id'];


    // If the linker table does not exists then we don't want to add attach.
    $cvterm_table = $table_name . '_cvterm';
    if (chado_table_exists($cvterm_table)) {
      return TRUE;
    }
    return FALSE;
  }
  /**
   * @see TripalField::create_info()
   */
  function create_info($entity_type, $bundle, $details) {
    if (!$this->can_attach($entity_type, $bundle, $details)) {
      return;
    }

    $table_name = $details['chado_table'];
    $type_table = $details['chado_type_table'];
    $type_field = $details['chado_type_column'];
    $cv_id      = $details['chado_cv_id'];
    $cvterm_id  = $details['chado_cvterm_id'];

    return array(
      'field_name' => $table_name . '_cvterm',
      'type' => 'chado_linker__cvterm_adder',
      'cardinality' => 1,
      'locked' => FALSE,
      'storage' => array(
        'type' => 'field_chado_storage',
      ),
      'settings' => array(
      ),
    );
  }
  /**
   * @see TripalField::create_instance_info()
   */
  function create_instance_info($entity_type, $bundle, $details) {
    if (!$this->can_attach($entity_type, $bundle, $details)) {
      return;
    }
    $table_name = $details['chado_table'];
    $type_table = $details['chado_type_table'];
    $type_field = $details['chado_type_column'];
    $cv_id      = $details['chado_cv_id'];
    $cvterm_id  = $details['chado_cvterm_id'];

    return array(
      'field_name' => $table_name . '_cvterm',
      'entity_type' => $entity_type,
      'bundle' => $bundle->name,
      'label' => 'Add Annotation Types',
      'description' => 'Add additional annotations types to this record.',
      'required' => FALSE,
      'settings' => array(
        'auto_attach' => FALSE,
      ),
      'widget' => array(
        'type' => 'chado_linker__cvterm_adder_widget',
        'settings' => array(
          'display_label' => 1,
        ),
      ),
      'display' => array(
        'deafult' => array(
          'label' => 'above',
          'type' => 'chado_linker__cvterm_adder_formatter',
          'settings' => array(),
        ),
      ),
    );
  }

  /**
   * @see TripalField::widget_info()
   */
  function widget_info() {
    return array(
      'label' => t('Add an Annotation'),
      'field types' => array('chado_linker__cvterm_adder'),
    );
  }
  /**
   * @see TripalField::widget_form()
   */
  public function widget_form(&$widget, &$form, &$form_state, $field, $instance,
      $langcode, $items, $delta, $element) {

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

    // Add the field if it doesn't already exists.
    $field = field_info_field('cvterm');
    if (!$field) {
      $create_info = array(
        'field_name' => 'cvterm',
        'type' => 'tripal_chado_cvterm_widget',
        'cardinality' => FIELD_CARDINALITY_UNLIMITED,
        'locked' => FALSE,
        'storage' => array(
          'type' => 'field_chado_storage',
        ),
        'settings' => array(
          'chado_table' => $field_name,
          'chado_column' => $pkey,
          'base_table' => $base_table,
        ),
      );
      $field = field_create_field($create_info);
    }

    // Attach the field to the bundle if it isn't already.
    if (!$field or !array_key_exists('bundles', $field) or
        !array_key_exists('TripalEntity', $field['bundles']) or
        !in_array($bundle_name, $field['bundles']['TripalEntity'])) {
      $create_instance_info = array(
        'field_name' => 'cvtmerm',
        'entity_type' => 'TripalEntity',
        'bundle' => $bundle->name,
        'label' => ucfirst(preg_replace('/_/', ' ', $cv->name)),
        'description' => "Annotations from the $cv->name vocabulary",
        'required' => FALSE,
        'settings' => array(),
        'widget' => array(
          'type' => 'tripal_chado_cvterm_widget',
          'settings' => array(
            'display_label' => 1,
          ),
        ),
        'display' => array(
          'deafult' => array(
            'label' => 'above',
            'type' => 'tripal_chado_cvterm_formatter',
            'settings' => array(),
          ),
        ),
      );
      $instance = field_create_instance($create_instance_info);
    }
  }
}