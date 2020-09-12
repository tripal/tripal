<?php

class sio__vocabulary extends ChadoField {

  // The default label for this field.
  public static $default_label = 'Vocabulary';

  // The default description for this field.
  public static $description = 'The vocabulary to which this resource is associated.';

  // Provide a list of instance specific settings. These can be accessed within
  // the instanceSettingsForm.  When the instanceSettingsForm is submitted
  // then Drupal will automatically change these settings for the instance.
  // It is recommended to put settings at the instance level whenever possible.
  // If you override this variable in a child class be sure to replicate the
  // term_name, term_vocab, term_accession and term_fixed keys as these are
  // required for all TripalFields.
  public static $default_instance_settings = [
    // The short name for the vocabulary (e.g. schema, SO, GO, PATO, etc.).
    'term_vocabulary' => 'SIO',
    // The name of the term.
    'term_name' => 'vocabulary',
    // The unique ID (i.e. accession) of the term.
    'term_accession' => '001080',
    // Set to TRUE if the site admin is allowed to change the term
    // type. This will create form elements when editing the field instance
    // to allow the site admin to change the term settings above.
    'term_fixed' => FALSE,
  ];

  // The default widget for this field.
  public static $default_widget = 'sio__vocabulary_widget';

  // The default formatter for this field.
  public static $default_formatter = 'sio__vocabulary_formatter';


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

    // Get the field values.
    foreach ($items as $delta => $values) {

      // Get the field values.
      $cv_id = $values['chado-' . $field_table . '__cv_id'];
      if (!$cv_id or $cv_id == 0) {
        $errors[$field_name]['und'][0][] = [
          'message' => t("Please specify a vocabulary."),
          'error' => 'chado-' . $field_table . '__cv_id',
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

    // Set some defaults for the empty record.
    $entity->{$field_name}['und'][0] = [
      'value' => [],
    ];

    if ($record) {
      $cv = $record->cv_id;
      $entity->{$field_name}['und'][0]['value'] = $cv->name;
      $entity->{$field_name}['und'][0]['chado-' . $field_table . '__cv_id'] = $cv->cv_id;
    }
  }

  /**
   * @see ChadoField::query()
   */
  public function query($query, $condition) {
    $alias = $this->field['field_name'];
    $operator = $condition['operator'];

    $this->queryJoinOnce($query, 'cv', $alias, "base.cv_id = $alias.cv_id");
    $query->condition("$alias.name", $condition['value'], $operator);
  }

  /**
   * @see ChadoField::queryOrder()
   */
  public function queryOrder($query, $order) {

    // If the table hasn't yet been joined then add it.
    $joins = $query->getTables();
    if (!in_array($this->field['field_name'], $joins)) {
      $alias = $this->field['field_name'];
      $this->queryJoinOnce($query, 'cv', $alias, "base.cv_id = $alias.cv_id");
      $query->orderBy("$alias.name", $order['direction']);
    }
  }
}
