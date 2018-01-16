<?php

/**
 * @class
 * Purpose: Provide a field for Protocol (typically the protocol_id column of a
 *   Chado table).
 *
 * Data:
 * Assumptions:
 */
class sep__protocol extends ChadoField {

  // The default label for this field.
  public static $default_label = 'Protocol';

  // The default description for this field.
  public static $default_description = 'The protocol followed to generate this resource.';

  // The default widget for this field.
  public static $default_widget = 'sep__protocol_widget';

  // The default formatter for this field.
  public static $default_formatter = 'sep__protocol_formatter';

  // The module that manages this field.
  public static $module = 'tripal_chado';

  // A list of global settings. These can be accessed within the
  // globalSettingsForm.  When the globalSettingsForm is submitted then
  // Drupal will automatically change these settings for all fields.
  // Once instances exist for a field type then these settings cannot be
  // changed.
  public static $default_settings = [
    'storage' => 'field_chado_storage',
    // It is expected that all fields set a 'value' in the load() function.
    // In many cases, the value may be an associative array of key/value pairs.
    // In order for Tripal to provide context for all data, the keys should
    // be a controlled vocabulary term (e.g. rdfs:type). Keys in the load()
    // function that are supported by the query() function should be
    // listed here.
    'searchable_keys' => [],
  ];

  // Provide a list of instance specific settings. These can be access within
  // the instanceSettingsForm.  When the instanceSettingsForm is submitted
  // then Drupal with automatically change these settings for the instance.
  // It is recommended to put settings at the instance level whenever possible.
  // If you override this variable in a child class be sure to replicate the
  // term_name, term_vocab, term_accession and term_fixed keys as these are
  // required for all TripalFields.
  public static $default_instance_settings = [
    // The DATABASE name, as it appears in chado.db.  This also builds the link-out url.  In most cases this will simply be the CV name.  In some cases (EDAM) this will be the SUBONTOLOGY.
    'term_vocabulary' => 'sep',
    // The name of the term.
    'term_name' => 'protocol',
    // The unique ID (i.e. accession) of the term.
    'term_accession' => '00101',
    // Set to TRUE if the site admin is not allowed to change the term
    // type, otherwise the admin can change the term mapped to a field.
    'term_fixed' => FALSE,
    // Indicates if this field should be automatically attached to display
    // or web services or if this field should be loaded separately. This
    // is convenient for speed.  Fields that are slow should for loading
    // should have auto_attach set to FALSE so tha their values can be
    // attached asynchronously.
    'auto_attach' => FALSE,
  ];

  // A boolean specifying that users should not be allowed to create
  // fields and instances of this field type through the UI. Such
  // fields can only be created programmatically with field_create_field()
  // and field_create_instance().
  public static $no_ui = FALSE;

  // A boolean specifying that the field will not contain any data. This
  // should exclude the field from web services or downloads.  An example
  // could be a quick search field that appears on the page that redirects
  // the user but otherwise provides no data.
  public static $no_data = FALSE;

  /**
   * Loads the field values from the underlying data store.
   *
   * @param $entity
   *
   * @return
   *   An array of the following format:
   *     $entity->{$field_name}['und'][0]['value'] = $value;
   *   where:
   *     - $entity is the entity object to which this field is attached.
   *     - $field_name is the name of this field
   *     - 'und' is the language code (in this case 'und' == undefined)
   *     - 0 is the cardinality.  Increment by 1 when more than one item is
   *       available.
   *     - 'value' is the key indicating the value of this field. It should
   *       always be set.  The value of the 'value' key will be the contents
   *       used for web services and for downloadable content.  The value
   *       should be of the follow format types: 1) A single value (text,
   *       numeric, etc.) 2) An array of key value pair. 3) If multiple entries
   *       then cardinality should incremented and format types 1 and 2 should
   *       be used for each item.
   *   The array may contain as many other keys at the same level as 'value'
   *   but those keys are for internal field use and are not considered the
   *   value of the field.
   *
   *
   */
  public function load($entity) {

    parent::load($entity);


    $record = $entity->chado_record;
    $settings = $this->instance['settings'];


    $field_name = $this->field['field_name'];
    $field_type = $this->field['type'];
    $field_table = $this->instance['settings']['chado_table'];
    $field_column = $this->instance['settings']['chado_column'];
    $linker_field = 'chado-' . $field_table . '__protocol_id';

    // Set some defaults for the empty record.
    $entity->{$field_name}['und'][0] = [
      'value' => [],
    ];

    if (!$record->protocol_id->protocol_id) {
      return;
    }

    $protocol_id = $record->protocol_id->protocol_id;
    $protocol_name = $record->protocol_id->name;

    $entity_id = $record->entity_id;

    $entity->{$field_name}['und'][0]['value'] = [
      "protocol_id" => $protocol_id,
      "protocol_name" => $protocol_name,
      "entity_id" => $entity_id,
    ];

    // Is there a published entity for this protocol?
    if (property_exists($record->{$field_column}, 'entity_id')) {
      $entity->{$field_name}['und'][0]['value']['entity_id'] = 'TripalEntity:' . $record->{$field_column}->entity_id;
    }
  }

}
