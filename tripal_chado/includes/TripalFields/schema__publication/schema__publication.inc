<?php

class schema__publication extends ChadoField {


  // --------------------------------------------------------------------------
  //                     EDITABLE STATIC CONSTANTS
  //
  // The following constants SHOULD be set for each descendent class.  They are
  // used by the static functions to provide information to Drupal about
  // the field and it's default widget and formatter.
  // --------------------------------------------------------------------------

  // The default label for this field.
  public static $default_label = 'Publication';

  // The default description for this field.
  public static $description = 'Associates a publication (e.g. journal article,
            conference proceedings, book chapter, etc.) with this record.';

  // Provide a list of instance specific settings. These can be accessed within
  // the instanceSettingsForm.  When the instanceSettingsForm is submitted
  // then Drupal will automatically change these settings for the instance.
  // It is recommended to put settings at the instance level whenever possible.
  // If you override this variable in a child class be sure to replicate the
  // term_name, term_vocab, term_accession and term_fixed keys as these are
  // required for all TripalFields.
  public static $default_instance_settings = [
    // The short name for the vocabulary (e.g. schema, SO, GO, PATO, etc.).
    'term_vocabulary' => 'schema',
    // The name of the term.
    'term_name' => 'publication',
    // The unique ID (i.e. accession) of the term.
    'term_accession' => 'publication',
    // Set to TRUE if the site admin is allowed to change the term
    // type. This will create form elements when editing the field instance
    // to allow the site admin to change the term settings above.
    'term_fixed' => FALSE,
  ];


  // The default widget for this field.
  public static $default_widget = 'schema__publication_widget';

  // The default formatter for this field.
  public static $default_formatter = 'schema__publication_formatter';

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
   * @see TripalField::elementInfo()
   */
  public function elementInfo() {
    $field_term = $this->getFieldTermID();
    return [
      $field_term => [
        'operations' => [],
        'sortable' => FALSE,
        'searchable' => FALSE,
        'type' => 'xs:string',
        'readonly' => TRUE,
      ],
    ];
  }

  /**
   *
   * @see TripalField::load()
   */
  public function load($entity) {
    $record = $entity->chado_record;

    $field_name = $this->field['field_name'];
    $field_type = $this->field['type'];
    $field_table = $this->instance['settings']['chado_table'];
    $field_column = $this->instance['settings']['chado_column'];
    $base_table = $this->instance['settings']['base_table'];

    // These fields are used when the publications come through a linker table.
    $pkey = '';
    $fkey_lcolumn = '';
    $fkey_rcolumn = '';
    $linker_table = '';

    // If we don't have a chado record return before creating a stub for this field!
    if (!$record) {
      return;
    }

    // If the field table and the base table are not the same thing then
    // we are going through a linker table.
    if ($field_table != $base_table) {

      // Get the FK that links to the base record.
      $schema = chado_get_schema($field_table);
      $pkey = $schema['primary key'][0];
      $fkey_lcolumn = key($schema['foreign keys'][$base_table]['columns']);
      $fkey_rcolumn = $schema['foreign keys'][$base_table]['columns'][$fkey_lcolumn];
      $linker_table = $base_table . '_pub';

      // Set some defaults for the empty record.
      $entity->{$field_name}['und'][0] = [
        'value' => [],
        'chado-' . $field_table . '__' . $pkey => '',
        'chado-' . $field_table . '__' . $fkey_lcolumn => '',
        'chado-' . $field_table . '__' . 'pub_id' => '',
      ];
    }
    // Otherwise, the base table has a pub_id field.
    else {
      $entity->{$field_name}['und'][0] = [
        'value' => [],
        'chado-' . $field_table . '__' . $field_column => '',
      ];
    }

    // Get the list of publications
    $pubs = [];
    if ($linker_table) {
      $options = [
        'return_array' => 1,
      ];
      $record = chado_expand_var($record, 'table', $linker_table, $options);
      if (property_exists($record, $linker_table) and is_array($record->$linker_table) and count($record->$linker_table) > 0) {
        $i = 0;
        foreach ($record->$linker_table as $index => $linker) {
          $pub = $linker->pub_id;
          $pubs[$pub->pub_id] = $pub;
        }
      }
    }
    else {
      $pub = $record->{$field_column};
      if ($pub) {
        $pubs[$pub->pub_id] = $pub;
      }
    }

    $i = 0;
    foreach ($pubs as $pub_id => $pub) {
      $pub_details = chado_get_minimal_pub_info($pub);

      $entity->{$field_name}['und'][$i]['value'] = $pub_details;
      if ($linker_table) {
        $entity->{$field_name}['und'][$i]['chado-' . $field_table . '__' . $pkey] = $linker->$pkey;
        $entity->{$field_name}['und'][$i]['chado-' . $field_table . '__' . $fkey_lcolumn] = $linker->$fkey_lcolumn->$fkey_lcolumn;
        $entity->{$field_name}['und'][$i]['chado-' . $field_table . '__' . 'pub_id'] = $pub_id;
      }
      else {
        $entity->{$field_name}['und'][$i]['chado-' . $field_table . '__' . $field_column] = $pub_id;
      }

      if (property_exists($pub, 'entity_id')) {
        $entity->{$field_name}['und'][$i]['value']['entity'] = 'TripalEntity:' . $pub->entity_id;
      }
      $i++;
    }
  }
}
