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
  public function widgetFormValidate($entity_type, $entity, $field, $items, &$errors) {

  }

  public function widgetFormSubmit($entity_type, $entity, $langcode, &$items, $form, &$form_state) {

    // Add the new field to the entity
    if (array_key_exists('triggering_element', $form_state) and
      $form_state['triggering_element']['#name'] == 'kvproperty_adder_button') {

      $form_state['rebuild'] = TRUE;
      $field_name = $this->field['field_name'];
      $bundle = $entity->bundle;

      // Get the base table name from the field properties.
      $field =$this->field;
      $base_table = $field['settings']['base_table'];

      // Get the term for the property
      foreach ($items as $delta => $values) {
        $kvproperty = tripal_get_field_item_keyval($items, $delta, 'value', '');
        $term = chado_generate_var('cvterm', array('name' => $kvproperty), $options = array('return_array' => TRUE));

        if (count($term) == 1) {
          $prop_field_name = $field_name . '__' . $term[0]->cvterm_id;

          // The field name is the table name in this case. We want to get the
          // primary key as this should be the field that maps th the value.
          $schema = chado_get_schema($field_name);
          $pkey = $schema['primary key'][0];
  
          // Add the field if it doesn't already exists.
          $field = field_info_field('cvterm');      dpm('property_' . $term[0]->cvterm_id);
          if (!$field) {
            $create_info = array(
              'field_name' => 'property_' . $term[0]->cvterm_id,
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
  
                $createInstanceInfo = array(
                  'field_name' => 'property_' . $term[0]->cvterm_id,
                  'entity_type' => 'TripalEntity',
                  'bundle' => $this->bundle->name,
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
                    'default' => array(
                      'label' => 'inline',
                      'type' => 'tripal_chado_kvproperty_formatter',
                      'settings' => array(),
                    ),
                  ),
                );
                //$instance = field_create_instance($createInstanceInfo);
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
  }
}
