<?php

class efo__array_design extends ChadoField {

  // The default label for this field.
  public static $default_label = 'Array Design';

  // The default description for this field.
  public static $description = 'An instrument design which describes the design of the array.';

  // Provide a list of instance specific settings. These can be accessed within
  // the instanceSettingsForm.  When the instanceSettingsForm is submitted
  // then Drupal will automatically change these settings for the instance.
  // It is recommended to put settings at the instance level whenever possible.
  // If you override this variable in a child class be sure to replicate the
  // term_name, term_vocab, term_accession and term_fixed keys as these are
  // required for all TripalFields.
  public static $default_instance_settings = [
    // The short name for the vocabulary (e.g. schema, SO, GO, PATO, etc.).
    'term_vocabulary' => 'EFO',
    // The name of the term.
    'term_name' => 'array design',
    // The unique ID (i.e. accession) of the term.
    'term_accession' => '0000269',
    // Set to TRUE if the site admin is allowed to change the term
    // type. This will create form elements when editing the field instance
    // to allow the site admin to change the term settings above.
    'term_fixed' => FALSE,
  ];

  // The default widget for this field.
  public static $default_widget = 'efo__array_design_widget';

  // The default formatter for this field.
  public static $default_formatter = 'efo__array_design_formatter';


  /**
   * @see TripalField::validate()
   */
  public function validate($entity_type, $entity, $langcode, $items, &$errors) {

    // If we don't have an entity then we don't want to validate.  The case
    // where this could happen is when a user is editing the field settings
    // and trying to set a default value. In that case there's no entity and
    // we don't want to validate.  There will always be an entity for creation
    // and update operations of a content type.
    if (!$entity) {
      return;
    }
    $settings = $this->field['settings'];
    $field_name = $this->field['field_name'];
    $field_type = $this->field['type'];
    $field_table = $this->instance['settings']['chado_table'];
    $field_column = $this->instance['settings']['chado_column'];
    $linker_field = 'chado-' . $field_table . '__' . $field_column;

    // Get the field values.
    foreach ($items as $delta => $values) {

      // Get the field values.
      $arraydesign_id = $values[$linker_field];
      if (!$arraydesign_id or $arraydesign_id == 0) {
        $errors[$field_name]['und'][0][] = [
          'message' => t("Please specify an array design."),
          'error' => 'efo__array_design',
        ];
      }
    }
  }

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
    $name_term = chado_get_semweb_term('arraydesign', 'name');
    $version_term = chado_get_semweb_term('arraydesign', 'version');

    // Set some defaults for the empty record.
    $entity->{$field_name}['und'][0] = [
      'value' => [],
    ];

    if (!$record) {
      return;
    }
    $linker_field = 'chado-' . $field_table . '__' . $field_column;
    $entity->{$field_name}['und'][0]['value'] = [
      $name_term => $record->{$field_column}->name,
      $version_term => $record->{$field_column}->version,
    ];
    $entity->{$field_name}['und'][0][$linker_field] = $record->{$field_column}->arraydesign_id;

    // Is there a published entity for this arraydesign?
    if (property_exists($record->{$field_column}, 'entity_id')) {
      $entity->{$field_name}['und'][0]['value']['entity'] = 'TripalEntity:' . $record->{$field_column}->entity_id;
    }
  }


  /**
   * @see TripalField::elementInfo()
   */
  public function elementInfo() {
    $field_term = $this->getFieldTermID();

    $name_term = chado_get_semweb_term('arraydesign', 'name');
    $version_term = chado_get_semweb_term('arraydesign', 'version');

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
          $version_term => [
            'searchable' => TRUE,
            'name' => 'version',
            'operations' => ['eq', 'ne'],
            'sortable' => TRUE,
            'readonly' => FALSE,
            'type' => 'xs:string',
            'required' => TRUE,
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
    $name_term = $field_term_id . ',' . chado_get_semweb_term('arraydesign', 'name');
    $version_term = $field_term_id . ',' . chado_get_semweb_term('arraydesign', 'version');

    // Join to the organism table for this field.
    $this->queryJoinOnce($query, 'arraydesign', $alias, "base.arraydesign_id = $alias.arraydesign_id");

    // If the column is the field name then we're during a search on the full
    // scientific name.
    if ($condition['column'] == $field_term_id or
      $condition['column'] == $name_term) {
      $query->condition("$alias.name", $condition['value'], $operator);
    }
    // If the column is a subfield.
    if ($condition['column'] == $version_term) {
      $query->condition("$alias.version", $condition['value'], $operator);
    }
  }

  /**
   * @see ChadoField::queryOrder()
   */
  public function queryOrder($query, $order) {
    $alias = $this->field['field_name'];

    $field_term_id = $this->getFieldTermID();
    $name_term = $field_term_id . ',' . chado_get_semweb_term('arraydesign', 'name');
    $version_term = $field_term_id . ',' . chado_get_semweb_term('arraydesign', 'version');

    // Join to the organism table for this field.
    $this->queryJoinOnce($query, 'arraydesign', $alias, "base.arraydesign_id = $alias.arraydesign_id");

    // Now perform the sort.
    if ($order['column'] == $name_term) {
      $query->orderBy("$alias.name", $order['direction']);
    }
    if ($order['column'] == $version_term) {
      $query->orderBy("$alias.version", $order['direction']);
    }
  }
}
