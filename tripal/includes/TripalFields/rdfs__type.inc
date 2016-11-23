<?php

class rdfs__type extends TripalField {

  // The term that this field maps to.  The format for the term should be:
  // [vocab]:[accession] where [vocab] is the short name of the vocabulary
  // and [acession] is the unique accession number for the term.  This
  // value should never be changed once fields exist for this type.
  public static $term = 'rdfs:type';

  // The default lable for this field.
  public static $label = 'Content Type';

  // The default description for this field.
  public static $description = 'The resource content type.';

  // Provide a list of global settings. These can be accessed witihn the
  // globalSettingsForm.  When the globalSettingsForm is submitted then
  // Drupal will automatically change these settings for all fields.
  public static $settings = array();

  // Provide a list of instance specific settings. These can be access within
  // the instanceSettingsForm.  When the instanceSettingsForm is submitted
  // then Drupal with automatically change these settings for the instnace.
  // It is recommended to put settings at the instance level whenever possible.
  public static $instance_settings  = array();

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
  public function load($entity, $details = array()) {
    $bundle = tripal_load_bundle_entity(array('name' => $entity->bundle));
    $entity->{$this->field['field_name']}['und'][0]['value'] = $bundle->label;
  }


  /**
   *  @see TripalField::settingsForm()
   */
  public function settingsForm($has_data) {
    $settings = $this->field['settings'];
    $element = array();

//     $element['semantic_web'] = array(
//       '#type' => 'textfield',
//       '#title' => 'Semantic Web',
//       '#description' => t('Each field must be associated with a term
//           from a controlled vocabulary.  This allows computer programs to understand
//           the data provided on this site.  Please be cautions changing these
//           values.  Defaults are set by Tripal and sites that use the same
//           terms can exchange information.'),
//       '#collapsed' => TRUE,
//       '#collapsible' => TRUE,
//       '#tree' => TRUE,
//     );
    $element['#field'] = $this->field;
    $element['#instance'] = $this->instance;
    $element['#element_validate'][] = 'tripal_field_settings_form_validate';

    return $element;
  }
  /**
   *  @see TripalField::settingsFormValidate()
   */
  public function settingsFormValidate($form, &$form_state) {

  }

}