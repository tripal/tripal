<?php

class chado_linker__prop extends TripalField {


  // --------------------------------------------------------------------------
  //                     EDITABLE STATIC CONSTANTS
  //
  // The following constants SHOULD be set for each descendent class.  They are
  // used by the static functions to provide information to Drupal about
  // the field and it's default widget and formatter.
  // --------------------------------------------------------------------------

  // The default lable for this field.
  public static $default_label = 'Property';

  // The default description for this field.
  public static $description = 'Add details about this property.';

  // Provide a list of global settings. These can be accessed witihn the
  // globalSettingsForm.  When the globalSettingsForm is submitted then
  // Drupal will automatically change these settings for all fields.
  public static $default_settings = array(
    'chado_table' => '',
    'chado_column' => '',
    'base_table' => '',
  );

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
    'term_name' => 'property',
    // The unique ID (i.e. accession) of the term.
    'term_accession' => 'property',
    // Set to TRUE if the site admin is allowed to change the term
    // type. This will create form elements when editing the field instance
    // to allow the site admin to change the term settings above.
    'term_fixed' => FALSE,
  );

  // Set this to the name of the storage backend that by default will support
  // this field.
  public static $storage = 'field_chado_storage';

  // The default widget for this field.
  public static $default_widget = 'chado_linker__prop_widget';

  // The default formatter for this field.
  public static $default_formatter = 'chado_linker__prop_formatter';

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
   * @see TripalField::validate()
   */
  public function validate($entity_type, $entity, $field, $items, &$errors) {

  }


  /**
   *
   * @see TripalField::load()
   */
  public function load($entity, $details = array()) {
    $field_name = $this->field['field_name'];
    $field_type = $this->field['type'];
    $field_table = $this->field['settings']['chado_table'];
    $field_column = $this->field['settings']['chado_column'];
    $base_table = $this->field['settings']['base_table'];

    $matches = array();
    preg_match('/(.*?)__(\d+)/', $field_name, $matches);
    $table_name = $matches[1];
    $cvterm_id = $matches[2];
    // Get the FK that links to the base record.
    $schema = chado_get_schema($field_table);
    $pkey = $schema['primary key'][0];
    $fkey_lcolumn = key($schema['foreign keys'][$base_table]['columns']);
    $fkey_rcolumn = $schema['foreign keys'][$base_table]['columns'][$fkey_lcolumn];

    // Set some defaults for the empty record.
    $chado_record = $entity->chado_record;
    $entity->{$field_name}['und'][0] = array(
      'value' => '',
      'chado-' . $field_table . '__' . $pkey => '',
      'chado-' . $field_table . '__' . $fkey_lcolumn => $chado_record->{$fkey_lcolumn},
      'chado-' . $field_table . '__value' => '',
      'chado-' . $field_table . '__type_id' => '',
      'chado-' . $field_table . '__rank' => '',
    );

    // Get the properties associated with this base record for this fields
    // given type.
    $columns = array('*');
    $match = array(
      $fkey_lcolumn => $chado_record->{$fkey_lcolumn},
      'type_id' => $cvterm_id,
      );
    $options = array(
      'return_array' => TRUE,
      'order_by' => array('rank' => 'ASC')
    );
    $properties = chado_select_record($field_table, $columns, $match, $options);
    for ($i = 0; $i < count($properties); $i++) {
      $property = $properties[$i];
      foreach ($schema['fields'] as $fname => $details) {
        $entity->{$field_name}['und'][$i] = array(
          'value' => array(),
          'chado-' . $field_table . '__' . $pkey => $property->$pkey,
          'chado-' . $field_table . '__' . $fkey_lcolumn => $property->$fkey_lcolumn,
          'chado-' . $field_table . '__value' => $property->value,
          'chado-' . $field_table . '__type_id' => $property->type_id,
          'chado-' . $field_table . '__rank' => $property->rank,
        );
      }
    }
  }
}