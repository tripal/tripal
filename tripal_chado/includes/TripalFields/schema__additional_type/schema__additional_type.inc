<?php

class schema__additional_type extends ChadoField {


  // --------------------------------------------------------------------------
  //                     EDITABLE STATIC CONSTANTS
  //
  // The following constants SHOULD be set for each descendent class.  They are
  // used by the static functions to provide information to Drupal about
  // the field and it's default widget and formatter.
  // --------------------------------------------------------------------------

  // The default lable for this field.
  public static $default_label = 'Type';

  // The default description for this field.
  public static $description = 'An additional type for this field.';

  // Provide a list of instance specific settings. These can be access within
  // the instanceSettingsForm.  When the instanceSettingsForm is submitted
  // then Drupal with automatically change these settings for the instnace.
  // It is recommended to put settings at the instance level whenever possible.
  // If you override this variable in a child class be sure to replicate the
  // term_name, term_vocab, term_accession and term_fixed keys as these are
  // required for all TripalFields.
  public static $default_instance_settings  = array(
    // The short name for the vocabulary (e.g. shcema, SO, GO, PATO, etc.).
    'term_vocabulary' => 'schema',
    // The name of the term.
    'term_name' => 'additionalType',
    // The unique ID (i.e. accession) of the term.
    'term_accession' => 'additionalType',
    // Set to TRUE if the site admin is allowed to change the term
    // type. This will create form elements when editing the field instance
    // to allow the site admin to change the term settings above.
    'term_fixed' => FALSE,
  );

  // The default widget for this field.
  public static $default_widget = 'schema__additional_type_widget';

  // The default formatter for this field.
  public static $default_formatter = 'schema__additional_type_formatter';



  /**
   *
   * @see TripalField::load()
   */
  public function load($entity) {
    $record = $entity->chado_record;
    $base_table = $this->instance['settings']['base_table'];

    $field_name = $this->field['field_name'];
    $field_type = $this->field['type'];
    $field_table = $this->instance['settings']['chado_table'];
    $field_column = $this->instance['settings']['chado_column'];


    // Set some defaults for the empty record.
    $entity->{$field_name}['und'][0] = array(
      'value' => $record->type_id->name,
      'chado-' . $field_table . '__type_id' => $record->type_id->cvterm_id,
    );
  }

}