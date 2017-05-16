<?php

class local__source_data extends ChadoField {


  // --------------------------------------------------------------------------
  //                     EDITABLE STATIC CONSTANTS
  //
  // The following constants SHOULD be set for each descendent class.  They are
  // used by the static functions to provide information to Drupal about
  // the field and it's default widget and formatter.
  // --------------------------------------------------------------------------

  // The default lable for this field.
  public static $default_label = 'Source of Data';

  // The default description for this field.
  public static $description = 'The source data used for this analysis.';

  // Provide a list of instance specific settings. These can be access within
  // the instanceSettingsForm.  When the instanceSettingsForm is submitted
  // then Drupal with automatically change these settings for the instnace.
  // It is recommended to put settings at the instance level whenever possible.
  // If you override this variable in a child class be sure to replicate the
  // term_name, term_vocab, term_accession and term_fixed keys as these are
  // required for all TripalFields.
  public static $default_instance_settings  = array(
    // The short name for the vocabulary (e.g. shcema, SO, GO, PATO, etc.).
    'term_vocabulary' => 'local',
    // The name of the term.
    'term_name' => 'source_data',
    // The unique ID (i.e. accession) of the term.
    'term_accession' => 'source_data',
    // Set to TRUE if the site admin is allowed to change the term
    // type. This will create form elements when editing the field instance
    // to allow the site admin to change the term settings above.
    'term_fixed' => FALSE,
  );

  // The default widget for this field.
  public static $default_widget = 'local__source_data_widget';

  // The default formatter for this field.
  public static $default_formatter = 'local__source_data_formatter';

  // --------------------------------------------------------------------------
  //              PROTECTED CLASS MEMBERS -- DO NOT OVERRIDE
  // --------------------------------------------------------------------------
  // An array containing details about the field. The format of this array
  // is the same as that returned by field_info_fields()
  protected $field;
  // An array containing details about an instance of the field. A field does
  // not have to have an instance.  But if dealing with an instance (such as
  // when using the widgetForm, formatterSettingsForm, etc.) it should be set.
  protected $instance;


  /**
   *
   * @see TripalField::load()
   */
  public function load($entity) {
    $analysis = $entity->chado_record;
    $field_name = $this->field['field_name'];
    $field_type = $this->field['type'];
    $field_table = $this->instance['settings']['chado_table'];
    $field_column = $this->instance['settings']['chado_column'];

    $entity->{$field_name}['und'][0] = array(
      'value' => array(
        // The name of the data source.
        'schema:name' => $analysis->sourcename,
        // The version of the data source.
        'IAO:0000129' => $analysis->sourceversion,
        // The URI of the data source.
        'data:1047' => $analysis->sourceuri,
      ),
      'chado-analysis__sourcename' => $analysis->sourcename,
      'chado-analysis__sourceversion' => $analysis->sourceversion,
      'chado-analysis__sourceuri' => $analysis->sourceuri,
    );
  }
}