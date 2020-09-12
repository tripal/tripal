<?php

class data__accession extends ChadoField {


  // --------------------------------------------------------------------------
  //                     EDITABLE STATIC CONSTANTS
  //
  // The following constants SHOULD be set for each descendent class.  They are
  // used by the static functions to provide information to Drupal about
  // the field and it's default widget and formatter.
  // --------------------------------------------------------------------------

  // The default label for this field.
  public static $default_label = 'Site Accession';

  // The default description for this field.
  public static $description = 'The unique stable accession (ID) for this record on this site.';

  // Provide a list of instance specific settings. These can be accessed within
  // the instanceSettingsForm.  When the instanceSettingsForm is submitted
  // then Drupal will automatically change these settings for the instance.
  // It is recommended to put settings at the instance level whenever possible.
  // If you override this variable in a child class be sure to replicate the
  // term_name, term_vocab, term_accession and term_fixed keys as these are
  // required for all TripalFields.
  public static $default_instance_settings = [
    // The short name for the vocabulary (e.g. schema, SO, GO, PATO, etc.).
    'term_vocabulary' => 'data',
    // The name of the term.
    'term_name' => 'accession',
    // The unique ID (i.e. accession) of the term.
    'term_accession' => '2091',
    // Set to TRUE if the site admin is allowed to change the term
    // type. This will create form elements when editing the field instance
    // to allow the site admin to change the term settings above.
    'term_fixed' => FALSE,
  ];

  // The default widget for this field.
  public static $default_widget = 'data__accession_widget';

  // The default formatter for this field.
  public static $default_formatter = 'data__accession_formatter';

  /**
   * @see TripalField::load()
   */
  public function load($entity) {

    $record = $entity->chado_record;

    $field_name = $this->field['field_name'];
    $field_type = $this->field['type'];
    $field_table = $this->instance['settings']['chado_table'];
    $field_column = $this->instance['settings']['chado_column'];


    // Set some defauls for the empty record
    $entity->{$field_name}['und'][0] = [
      'value' => '',
      'chado-' . $field_table . '__' . $field_column => '',
      'db_id' => '',
      'accession' => '',
    ];

    // Get the primary dbxref record (if it's not NULL).  Because we have a
    // dbxref_id passed in by the base record, we will only have one record.
    if ($record->$field_column) {
      $dbxref = $record->$field_column;
      $value = $dbxref->db_id->name . ':' . $dbxref->accession;
      // Skip the local:null accession as it is just a placeholder.
      if ($value != 'local:null') {
        $entity->{$field_name}['und'][0] = [
          'value' => $dbxref->accession,
          'chado-' . $field_table . '__' . $field_column => $record->$field_column->$field_column,
          'db_id' => $dbxref->db_id->db_id,
          'accession' => $dbxref->accession,
        ];
      }
    }
  }

  /**
   * @see ChadoField::query()
   */
  public function query($query, $condition) {
    $alias = $this->field['field_name'];
    $operator = $condition['operator'];

    $field_table = $this->instance['settings']['chado_table'];
    $field_column = $this->instance['settings']['chado_column'];

    $field_term_id = $this->getFieldTermID();
    $accession_term = chado_get_semweb_term($field_table, $field_column);

    // We don't offer any sub elements so the value coming in should
    // always be the field_name.
    if ($condition['column'] == $accession_term) {
      $this->queryJoinOnce($query, 'dbxref', 'DBX', "DBX.dbxref_id = base.dbxref_id");
      $query->condition("DBX.accession", $condition['value'], $operator);
    }
  }

  public function queryOrder($query, $order) {
    $alias = $this->field['field_name'];
    $operator = $condition['operator'];
    $field_table = $this->instance['settings']['chado_table'];
    $field_column = $this->instance['settings']['chado_column'];

    $field_term_id = $this->getFieldTermID();
    $accession_term = chado_get_semweb_term($field_table, $field_column);

    // We don't offer any sub elements so the value coming in should
    // always be the field_name.
    if ($order['column'] == $accession_term) {
      $this->queryJoinOnce($query, 'dbxref', 'DBX', "DBX.dbxref_id = base.dbxref_id", "LEFT OUTER");
      $query->orderBy("DBX.accession", $order['direction']);
    }
  }

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

    $field_name = $this->field['field_name'];
    $settings = $this->field['settings'];
    $field_type = $this->field['type'];
    $field_table = $this->instance['settings']['chado_table'];
    $field_column = $this->instance['settings']['chado_column'];

    // Get the field values.
    foreach ($items as $delta => $values) {
      $db_id = $values['db_id'];
      $accession = $values['accession'];

      // Make sure that if a database ID is provided that an accession is also
      // provided.  Here we use the form_set_error function rather than the
      // form_error function because the form_error will add a red_highlight
      // around all of the fields in the fieldset which is confusing as it's not
      // clear to the user what field is required and which isn't. Therefore,
      // we borrow the code from the 'form_error' function and append the field
      // so that the proper field is highlighted on error.
      if (!$db_id and $accession) {
        $errors[$field_name][$delta]['und'][] = [
          'message' => t("A database and the accession must both be provided for the primary cross reference."),
          'error' => 'chado_base__dbxref',
        ];
      }
      if ($db_id and !$accession) {
        $errors[$field_name][$delta]['und'][] = [
          'message' => t("A database and the accession must both be provided for the primary cross reference."),
          'error' => 'chado_base__dbxref',
        ];
      }
      if (!$db_id and !$accession) {
        $errors[$field_name][$delta]['und'][] = [
          'message' => t("A database and the accession must both be provided for the primary cross reference."),
          'error' => 'chado_base__dbxref',
        ];
      }
    }
  }

}