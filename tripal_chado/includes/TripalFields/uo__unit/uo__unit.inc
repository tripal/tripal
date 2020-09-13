<?php

class uo__unit extends ChadoField {

  // The default label for this field.
  public static $default_label = 'Unit';

  // The default description for this field.
  public static $description = 'The unit of measurement.';

  // Provide a list of instance specific settings. These can be accessed within
  // the instanceSettingsForm.  When the instanceSettingsForm is submitted
  // then Drupal will automatically change these settings for the instance.
  // It is recommended to put settings at the instance level whenever possible.
  // If you override this variable in a child class be sure to replicate the
  // term_name, term_vocab, term_accession and term_fixed keys as these are
  // required for all TripalFields.
  public static $default_instance_settings = [
    // The short name for the vocabulary (e.g. schema, SO, GO, PATO, etc.).
    'term_vocabulary' => 'UO',
    // The name of the term.
    'term_name' => 'unit',
    // The unique ID (i.e. accession) of the term.
    'term_accession' => '0000000',
    // Set to TRUE if the site admin is allowed to change the term
    // type. This will create form elements when editing the field instance
    // to allow the site admin to change the term settings above.
    'term_fixed' => FALSE,
  ];

  // The default widget for this field.
  public static $default_widget = 'UO__unit_widget';

  // The default formatter for this field.
  public static $default_formatter = 'UO__unit_formatter';

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


    // Set some defaults for the empty record.
    $entity->{$field_name}['und'][0] = [
      'value' => '',
    ];

    if ($record) {
      $entity->{$field_name}['und'][0]['value'] = $record->unittype_id->name;
      $entity->{$field_name}['und'][0]['chado-' . $field_table . '__unittype_id'] = $record->unittype_id->cvterm_id;
      $entity->{$field_name}['und'][0]['chado-cvterm__name'] = $record->unittype_id->name;
      $entity->{$field_name}['und'][0]['chado-cvterm__definition'] = $record->unittype_id->definition;
      $entity->{$field_name}['und'][0]['chado-cvterm__cv_id'] = $record->unittype_id->cv_id->cv_id;
      $entity->{$field_name}['und'][0]['chado-cv__name'] = $record->unittype_id->cv_id->name;
    }
  }

  /**
   * @see ChadoField::query()
   */
  public function query($query, $condition) {
    $chado_table = $this->instance['settings']['chado_table']; // featuremap
    $chado_field = $this->instance['settings']['chado_column']; // unittype_id

    $alias = $this->field['field_name'];
    $operator = $condition['operator'];

    $this->queryJoinOnce($query, 'cvterm', $alias, "base.$chado_field = $alias.cvterm_id");
    $query->condition("$alias.name", $condition['value'], $operator);
  }

  /**
   * @see ChadoField::query()
   */
  public function queryOrder($query, $order) {
    $chado_table = $this->instance['settings']['chado_table']; // featuremap
    $chado_field = $this->instance['settings']['chado_column']; // unittype_id

    $alias = $this->field['field_name'];

    $this->queryJoinOnce($query, 'cvterm', $alias, "base.$chado_field = $alias.cvterm_id");
    $query->orderBy("$alias.name", $order['direction']);
  }
}