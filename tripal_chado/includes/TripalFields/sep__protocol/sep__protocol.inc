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

  // Provide a list of instance specific settings. These can be accessed within
  // the instanceSettingsForm.  When the instanceSettingsForm is submitted
  // then Drupal will automatically change these settings for the instance.
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
    $base_table = $this->instance['settings']['base_table'];
    $linker_field = 'chado-' . $field_table . '__' . $field_column;

    if (!$record) {
      return;
    }

    $protocol = $record->{$field_column};
    // If this record does not have a protcol then just return.
    if (!$protocol) {
      return;
    }

    // Set some defaults for the empty record.
    $entity->{$field_name}['und'][0] = [
      'value' => [],
    ];

    $entity_id = $record->entity_id;

    // Get the Protocol controlled vocabulary terms.
    $protocol_name_term = chado_get_semweb_term('protocol', 'name');
    $protocol_type_term = chado_get_semweb_term('protocol', 'type_id');

    $entity->{$field_name}['und'][0]['value'] = [
      $protocol_name_term => $protocol->name,
      $protocol_type_term => $protocol->type_id->name,
    ];

    $entity->{$field_name}['und'][0][$linker_field] = $protocol->protocol_id;

    // Is there a published entity for this protocol?
    if (property_exists($record->{$field_column}, 'entity_id')) {
      $entity->{$field_name}['und'][0]['value']['entity_id'] = 'TripalEntity:' . $record->{$field_column}->entity_id;
    }
  }

  /**
   * @see TripalField::elementInfo()
   */
  function elementInfo() {
    $field_term = $this->getFieldTermID();
    $type_id_term = tripal_get_chado_semweb_term('protocol', 'type_id');

    return [
      $field_term => [
        'operations' => ['eq', 'contains', 'starts'],
        'sortable' => TRUE,
        'searchable' => TRUE,
        'readonly' => FALSE,
        'type' => 'xs:complexType',
        'elements' => [
          'rdfs:label' => [
            'searchable' => TRUE,
            'name' => 'protocol_name',
            'operations' => ['eq', 'ne', 'contains', 'starts'],
            'sortable' => FALSE,
            'type' => 'xs:string',
            'readonly' => TRUE,
            'required' => FALSE,
          ],
          'entity' => [
            'searchable' => FALSE,
          ],
          $type_id_term => [
            'searchable' => TRUE,
            'name' => 'protocol_type',
            'operations' => ['eq', 'ne', 'contains', 'starts'],
            'sortable' => TRUE,
            'readonly' => FALSE,
            'required' => TRUE,
            'type' => 'xs:integer',
          ],
        ],
      ],
    ];
  }

  /**
   * Provide query support.  We only make use of the name and type.
   *
   * @param $query
   * @param $condition
   */

  function query($query, $condition) {
    $alias = $this->field['field_name'];
    $operator = $condition['operator'];
    $field_term_id = $this->getFieldTermID();
    $type_id_term = $field_term_id . ',' . tripal_get_chado_semweb_term('protocol', 'type_id');

    // Join to the protocol table for this field.
    $this->queryJoinOnce($query, 'protocol', $alias, "base.protocol_id = $alias.protocol_id");


    if ($condition['column'] == $field_term_id or
      $condition['column'] == $field_term_id . ',rdfs:label') {
      $query->condition("$alias.name", $condition['value'], $operator);
    }
    if ($condition['column'] == $type_id_term) {
      $this->queryJoinOnce($query, 'cvterm', 'CVT', "base.type_id = CVT.cvterm_id");
      $query->condition("CVT.name", $condition['value'], $operator);
    }
  }

  function queryOrder($query, $order) {
    $alias = $this->field['field_name'];
    $field_term_id = $this->getFieldTermID();
    $type_id_term = tripal_get_chado_semweb_term('protocol', 'type_id');
    // Join to the protocol table for this field.
    $this->queryJoinOnce($query, 'protocol', $alias, "base.protocol_id = $alias.organism_id");

    if ($order['column'] == $type_id_term) {
      if (!in_array('CVT', $joins)) {
        $this->queryJoinOnce($query, 'cvterm', 'CVT', "base.type_id = CVT.cvterm_id");
      }
      $query->orderBy("CVT.name", $order['direction']);
    }
  }


}
