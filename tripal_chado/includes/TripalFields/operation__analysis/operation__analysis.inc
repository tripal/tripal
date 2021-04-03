<?php

class operation__analysis extends ChadoField {

  // The default label for this field.
  public static $default_label = 'Analysis';

  // The default description for this field.
  public static $description = 'Application of analytical methods to existing data of a specific type.';

  // Provide a list of instance specific settings. These can be accessed within
  // the instanceSettingsForm.  When the instanceSettingsForm is submitted
  // then Drupal will automatically change these settings for the instance.
  // It is recommended to put settings at the instance level whenever possible.
  // If you override this variable in a child class be sure to replicate the
  // term_name, term_vocab, term_accession and term_fixed keys as these are
  // required for all TripalFields.
  public static $default_instance_settings = [
    // The short name for the vocabulary (e.g. schema, SO, GO, PATO, etc.).
    'term_vocabulary' => 'operation',
    // The name of the term.
    'term_name' => 'Analysis',
    // The unique ID (i.e. accession) of the term.
    'term_accession' => '2945',
    // Set to TRUE if the site admin is allowed to change the term
    // type. This will create form elements when editing the field instance
    // to allow the site admin to change the term settings above.
    'term_fixed' => FALSE,
  ];

  // The default widget for this field.
  public static $default_widget = 'operation__analysis_widget';

  // The default formatter for this field.
  public static $default_formatter = 'operation__analysis_formatter';


  /**
   * @see TripalField::load()
   */
  public function load($entity) {
    $record = $entity->chado_record;
    $settings = $this->instance['settings'];

    $field_name = $this->field['field_name'];
    $field_type = $this->field['type'];
    $field_table = $this->instance['settings']['chado_table'];
    $field_column = $this->instance['settings']['chado_column'];

    // Get the terms for each of the keys for the 'values' property.
    $name_term = chado_get_semweb_term('analysis', 'name');

    // Set some defaults for the empty record.
    $entity->{$field_name}['und'][0] = [
      'value' => [],
    ];

    if (!$record or !$record->analysis_id) {
      return;
    }
    $linker_field = 'chado-' . $field_table . '__' . $field_column;
    $entity->{$field_name}['und'][0]['value'] = [
      $name_term => $record->{$field_column}->name,
    ];
    $entity->{$field_name}['und'][0][$linker_field] = $record->{$field_column}->analysis_id;

    // Is there a published entity for this analysis?
    if (property_exists($record->{$field_column}, 'entity_id')) {
      $entity->{$field_name}['und'][0]['value']['entity'] = 'TripalEntity:' . $record->{$field_column}->entity_id;
    }
  }


  /**
   * @see TripalField::elementInfo()
   */
  public function elementInfo() {
    $field_term = $this->getFieldTermID();

    $name_term = chado_get_semweb_term('analysis', 'name');

    return [
      $field_term => [
        'operations' => ['eq', 'contains', 'starts'],
        'sortable' => TRUE,
        'searchable' => TRUE,
        'readonly' => FALSE,
        'type' => 'xs:complexType',
        'elements' => [
          $name_term => [
            'searchable' => TRUE,
            'name' => 'name',
            'operations' => ['eq', 'ne', 'contains', 'starts'],
            'sortable' => FALSE,
            'type' => 'xs:string',
            'readonly' => TRUE,
            'required' => FALSE,
          ],
          'entity' => [
            'searchable' => FALSE,
          ],
        ],
      ],
    ];
  }

  /**
   * @see ChadoField::query()
   */
  public function query($query, $condition) {
    $alias = $this->field['field_name'];
    $operator = $condition['operator'];

    $field_term_id = $this->getFieldTermID();
    $name_term = $field_term_id . ',' . chado_get_semweb_term('analysis', 'name');

    // Join to the organism table for this field.
    $this->queryJoinOnce($query, 'analysis', $alias, "base.analysis_id = $alias.analysis_id");

    // If the column is the field name then we're during a search on the full
    // scientific name.
    if ($condition['column'] == $field_term_id or
      $condition['column'] == $name_term) {
      $query->condition("$alias.name", $condition['value'], $operator);
    }
  }

  /**
   * @see ChadoField::queryOrder()
   */
  public function queryOrder($query, $order) {
    $alias = $this->field['field_name'];

    $field_term_id = $this->getFieldTermID();
    $name_term = $field_term_id . ',' . chado_get_semweb_term('analysis', 'name');

    // Join to the organism table for this field.
    $this->queryJoinOnce($query, 'analysis', $alias, "base.analysis_id = $alias.analysis_id");

    // Now perform the sort.
    if ($order['column'] == $name_term) {
      $query->orderBy("$alias.name", $order['direction']);
    }
  }
}
