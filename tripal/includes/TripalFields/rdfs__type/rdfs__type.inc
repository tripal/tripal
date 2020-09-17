<?php

class rdfs__type extends TripalField {

  // The default label for this field.
  public static $default_label = 'Content Type';

  // The default description for this field.
  public static $description = 'The resource content type.';

  // Provide a list of global settings. These can be accessed within the
  // globalSettingsForm.  When the globalSettingsForm is submitted then
  // Drupal will automatically change these settings for all fields.
  public static $default_settings = [];

  // Provide a list of instance specific settings. These can be accessed within
  // the instanceSettingsForm.  When the instanceSettingsForm is submitted
  // then Drupal will automatically change these settings for the instance.
  // It is recommended to put settings at the instance level whenever possible.
  // If you override this variable in a child class be sure to replicate the
  // term_name, term_vocab, term_accession and term_fixed keys as these are
  // required for all TripalFields.
  public static $default_instance_settings = [
    // The short name for the vocabulary (e.g. schema, SO, GO, PATO, etc.).
    'term_vocabulary' => 'rdfs',
    // The name of the term.
    'term_name' => 'type',
    // The unique ID (i.e. accession) of the term.
    'term_accession' => 'type',
    // Set to TRUE if the site admin is allowed to change the term
    // type. This will create form elements when editing the field instance
    // to allow the site admin to change the term settings above.
    'term_fixed' => TRUE,
  ];

  // Set this to the name of the storage backend that by default will support
  // this field.
  public static $storage = 'tripal_no_storage';

  // The default widget for this field.
  public static $default_widget = 'rdfs__type_widget';

  // The default formatter for this field.
  public static $default_formatter = 'rdfs__type_formatter';

  /**
   * @see TripalField::load()
   */
  public function load($entity) {
    $bundle = tripal_load_bundle_entity(['name' => $entity->bundle]);
    $entity->{$this->field['field_name']}['und'][0]['value'] = $bundle->label;
  }


  /**
   * @see TripalField::settingsForm()
   */
  public function settingsForm($has_data) {
    $settings = $this->field['settings'];
    $element = [];

    $element['#field'] = $this->field;
    $element['#instance'] = $this->instance;
    $element['#element_validate'][] = 'tripal_field_settings_form_validate';

    return $element;
  }

  /**
   * @see TripalField::settingsFormValidate()
   */
  public function settingsFormValidate($form, &$form_state) {

  }

}
