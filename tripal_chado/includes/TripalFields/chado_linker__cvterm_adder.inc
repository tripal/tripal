<?php
class chado_linker__cvterm_adder extends TripalField {

  // The default lable for this field.
  public static $default_label = 'Add an Annotation Type';

  // The default description for this field.
  public static $default_description = 'This record may have any number of types of
        annotations. Use this field to first add the type.';

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
    // This field should never be viewed. It's to help add new cvterms
    // when editing an entity.  So return nothing.
    return '';
  }

  /**
   * @see TripalField::widgetForm()
   */
  public function widgetForm(&$widget, &$form, &$form_state, $langcode, $items, $delta, $element) {
    parent::widgetForm($widget, $form, $form_state, $langcode, $items, $delta, $element);


    // This field has no value field.  Just a fieldset for adding new annotation types.
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

    //    $widget['#element_validate'] = array('chado_linker__cvterm_adder_widget_validate');

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
//      '#submit' => array('chado_linker__cvterm_adder_widget_submit'),
      '#limit_validation_errors' => array(array($this->field['field_name'])),
    );
  }

  /**
   * @see TripalField::widgetFormSubmit()
   */
  public function widgetFormSubmit($form, &$form_state, $entity_type, $entity, $langcode, $delta) {

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
          //$cvterm_class_adder = tripal_chado_get_field_form_values($field_name, $form_state);
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
                $createInstanceInfo = array(
                  'field_name' => 'cvtmerm',
                  'entity_type' => 'TripalEntity',
                  'bundle' => $this->bundle->name,
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
                    'default' => array(
                      'label' => 'above',
                      'type' => 'tripal_chado_cvterm_formatter',
                      'settings' => array(),
                    ),
                  ),
                );
                $instance = field_create_instance($createInstanceInfo);
              }
        }
  }

}