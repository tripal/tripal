<?php

class chado_linker__prop_adder extends TripalField {
  /**
   * @see TripalField::field_info()
   */
  function field_info() {
    return array(
      'label' => t('Add a Property Type'),
      'description' => t('This record may have any number of properties. Use
            this field to first add the type.'),
      'default_widget' => 'chado_linker__prop_adder_widget',
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
    $table_name = $details['chado_table'];
    $type_table = $details['chado_type_table'];
    $type_field = $details['chado_type_column'];
    $cv_id      = $details['chado_cv_id'];
    $cvterm_id  = $details['chado_cvterm_id'];

    // If the linker table does not exists then we don't want to add attach.
    $prop_table = $table_name . 'prop';
    if (chado_table_exists($prop_table)) {
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

    $prop_table = $table_name . 'prop';
    return array(
      'field_name' => $prop_table,
      'type' => 'chado_linker__prop_adder',
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

    $prop_table = $table_name . 'prop';
    return array(
      'field_name' => $prop_table,
      'entity_type' => $entity_type,
      'bundle' => $bundle->name,
      'label' => 'Add Properties',
      'description' => 'Add additional property types to this record.',
      'required' => FALSE,
      'settings' => array(
        'auto_attach' => FALSE,
      ),
      'widget' => array(
        'type' => 'chado_linker__prop_adder_widget',
        'settings' => array(
          'display_label' => 1,
        ),
      ),
      'display' => array(
        'deafult' => array(
          'label' => 'above',
          'type' => 'chado_linker__prop_adder_formatter',
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
      'label' => t('Add a Property'),
      'field types' => array('chado_linker__prop_adder'),
    );
  }
  /**
   * @see TripalField::widget_form()
   */
  function widget_form(&$widget, &$form, &$form_state,
      $field, $instance, $langcode, $items, $delta, $element) {

    // This field has no value field.  Just a fieldset for adding new properties.
    $widget['#element_validate'] = array('chado_linker__prop_adder_widget_validate');

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
      '#autocomplete_path' => "admin/tripal/storage/chado/auto_name/cvterm/",
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
      '#limit_validation_errors' => array(array($field['field_name'])),
    );
  }

}
/**
 * Callback function for validating the chado_linker__prop_adder_widget.
 */
function chado_linker__prop_adder_widget_validate($element, &$form_state) {

  // Add the new field to the entity
  if (array_key_exists('triggering_element', $form_state) and
      $form_state['triggering_element']['#name'] == 'kvproperty_adder_button') {

        $form_state['rebuild'] = TRUE;
        $field_name = $element['#field_name'];
        $entity_type = $element['#entity']->type;
        $bundle = $element['#entity']->bundle;

        // Get the base table name from the field properties.
        $field = field_info_field($field_name);
        $base_table = $field['settings']['base_table'];

        // Get the term for the property
        $kvproperty = tripal_chado_get_field_form_values($field_name, $form_state);
        $term = chado_generate_var('cvterm', array('name' => $kvproperty), $options = array('return_array' => TRUE));

        if (count($term) == 1) {
          $prop_field_name = $field_name . '__' . $term[0]->cvterm_id;

          // The field name is the table name in this case. We want to get the
          // primary key as this should be the field that maps th the value.
          $schema = chado_get_schema($field_name);
          $pkey = $schema['primary key'][0];

          // Add the field if it doesn't already exists.
          $field = field_info_field('cvterm');
          if (!$field) {
            $create_info = array(
              'field_name' => 'property-' . $term[0]->cvterm_id,
              'type' => 'tripal_chado_kvproperty_widget',
              'cardinality' => FIELD_CARDINALITY_UNLIMITED,
              'locked' => FALSE,
              'storage' => array(
                'type' => 'field_chado_storage',
              ),
              'settings' => array(
                'chado_table' => $field_name,
                'chado_column' => $pkey,
                'base_table' => $base_table,
                'semantic_web' => '',
              ),
            );
            $field = field_create_field($create_info);
          }

          // Attach the field to the bundle if it isn't already.
          if (!$field and array_key_exists('bundles', $field) or
              !array_key_exists('TripalEntity', $field['bundles']) or
              !in_array($bundle_name, $field['bundles']['TripalEntity'])) {

            $create_instance_info = array(
              'field_name' => 'property-' . $term[0]->cvterm_id,
              'entity_type' => 'TripalEntity',
              'bundle' => $bundle->name,
              'label' => ucfirst(preg_replace('/_/', ' ', $term[0]->name)),
              'description' => $term[0]->definition ? $term[0]->definition : '',
              'required' => FALSE,
              'settings' => array(),
              'widget' => array(
                'type' => 'tripal_chado_kvproperty_widget',
                'settings' => array(
                  'display_label' => 1,
                ),
              ),
              'display' => array(
                'deafult' => array(
                  'label' => 'above',
                  'type' => 'tripal_chado_kvproperty_formatter',
                  'settings' => array(),
                ),
              ),
            );
            $instance = field_create_instance($create_instance_info);
          }
        }
        else if (count($term) > 1) {
          form_set_error(implode('][', $element ['#parents']) . '][value', t("This term is present in multiple vocabularies. Please select the appropriate one."));
        }
        else {
          form_set_error(implode('][', $element ['#parents']) . '][value', t("Please provide a property type to add."));
        }
      }
}