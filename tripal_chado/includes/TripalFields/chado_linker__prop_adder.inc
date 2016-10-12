<?php

class chado_linker__prop_adder extends TripalField {
  // The default lable for this field.
  public static $default_label = 'Add a Property Type';

  // The default description for this field.
  public static $default_description = 'This record may have any number of properties. Use
            this field to first add the type.';

  // Add any default settings elements.  If you override the globalSettingsForm()
  // or the instanceSettingsForm() functions then you need to be sure that
  // any settings you want those functions to manage are listed in this
  // array.
  public static $default_settings = array(
    'chado_table' => '',
    'chado_column' => '',
    'base_table' => '',
    'semantic_web' => '',
  );

  // Set this to the name of the storage backend that by default will support
  // this field.
  public static $default_storage = 'field_chado_storage';

  /**
   * @see TripalField::formatterView()
   */
  public function formatterView(&$element, $entity_type, $entity, $langcode, $items, $display) {
    // This field should never be viewed. It's to help add new properties
    // when editing an entity.  So return nothing.
    return '';
  }

  /**
   * @see TripalField::widgetForm()
   */
  public function widgetForm(&$widget, &$form, &$form_state, $langcode, $items, $delta, $element) {

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
      '#limit_validation_errors' => array(array($this->field['field_name'])),
    );
  }

  /**
   * @see TripalField::widgetFormValidate
   */
  public function widgetFormValidate($entity_type, $entity, $langcode, $items, &$errors) {

    // We will never have more than one item for this field at a time, so
    // delta is always zero.
    $delta = 0;

    // Make sure the cvterm for this property is uniquely identified.
    $kvproperty = tripal_get_field_item_keyval($items, $delta, 'value', '');
    $cvterms = chado_generate_var('cvterm', array('name' => $kvproperty), $options = array('return_array' => TRUE));

    if (count($cvterms) > 1) {
      $errors[$this->field['field_name']][$langcode][$delta][] = array(
        'error' => 'chado_linker__prop_adder',
        'message' => t("This term is present in multiple vocabularies. Please select the appropriate one."),
      );
    }
    if (count($cvterms) == 0) {
      $errors[$this->field['field_name']][$langcode][$delta][] = array(
        'error' => 'chado_linker__prop_adder',
        'message' => t("Please provide a property type to add."),
      );
    }
  }

  /**
   * @see TripalField::widgetFormSubmit()
   */
  public function widgetFormSubmit($entity_type, $entity, $langcode, &$items, $form, &$form_state) {
    // We will never have more than one item for this field at a time, so
    // delta is always zero.
    $delta = 0;

    // Add the new field to the entity but only if the property adder button
    // was clicked
    if (!array_key_exists('triggering_element', $form_state) or
      $form_state['triggering_element']['#name'] != 'kvproperty_adder_button') {
      return;
    }

    // Because we're going to add a new property we want to rebuild the form
    // rather than have it fully submit.
    $form_state['rebuild'] = TRUE;

    // Get the table and base table.
    $base_table = $this->field['settings']['base_table'];

    // Get the term for the property
    $kvproperty = tripal_get_field_item_keyval($items, $delta, 'value', '');
    $cvterm = chado_generate_var('cvterm', array('name' => $kvproperty));

    // Generate the name for the property table and the field name that we'll
    // be creating.
    $prop_table = $base_table . 'prop';
    $field_name = $prop_table . '__' . $cvterm->cvterm_id;

    // The field name is the table name in this case. We want to get the
    // primary key as this should be the field that maps th the value.
    $schema = chado_get_schema($prop_table);
    $pkey = $schema['primary key'][0];

    // Add the field if it doesn't already exists.
    $field = field_info_field($field_name);
    if (!$field) {
      $field = field_create_field(array(
        'field_name' => $field_name,
        'type' => 'chado_linker__prop',
        'cardinality' => FIELD_CARDINALITY_UNLIMITED,
        'locked' => FALSE,
        'storage' => array(
          'type' => 'field_chado_storage',
        ),
        'settings' => array(
          'chado_table' => $prop_table,
          'chado_column' => $pkey,
          'base_table' => $base_table,
          'semantic_web' => $cvterm->dbxref_id->db_id->name . ':' . $cvterm->dbxref_id->accession,
        ),
      ));
    }

    // Create an instance of the field.
    if (!$field and array_key_exists('bundles', $field) or
        !array_key_exists('TripalEntity', $field['bundles']) or
        !in_array($bundle_name, $field['bundles']['TripalEntity'])) {

      $instance = field_create_instance(array(
        'field_name' => $field_name,
        'entity_type' => 'TripalEntity',
        'bundle' => $entity->bundle,
        'label' => ucfirst(preg_replace('/_/', ' ', $cvterm->name)),
        'description' => $cvterm->definition ? $cvterm->definition : '',
        'required' => FALSE,
        'settings' => array(),
        'widget' => array(
          'type' => $field_name . '_widget',
          'settings' => array(
            'display_label' => 1,
          ),
        ),
        'display' => array(
          'default' => array(
            'label' => 'inline',
            'type' => $field_name . '_formatter',
            'settings' => array(),
          ),
        ),
      ));
    }
  }
}
