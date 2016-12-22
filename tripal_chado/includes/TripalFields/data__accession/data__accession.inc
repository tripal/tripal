<?php

class data__accession extends ChadoField {


  // --------------------------------------------------------------------------
  //                     EDITABLE STATIC CONSTANTS
  //
  // The following constants SHOULD be set for each descendent class.  They are
  // used by the static functions to provide information to Drupal about
  // the field and it's default widget and formatter.
  // --------------------------------------------------------------------------

  // The default lable for this field.
  public static $default_label = 'Accession';

  // The default description for this field.
  public static $description = 'The unique stable accession (ID) for
        this record on this site.';

  // Provide a list of instance specific settings. These can be access within
  // the instanceSettingsForm.  When the instanceSettingsForm is submitted
  // then Drupal with automatically change these settings for the instnace.
  // It is recommended to put settings at the instance level whenever possible.
  // If you override this variable in a child class be sure to replicate the
  // term_name, term_vocab, term_accession and term_fixed keys as these are
  // required for all TripalFields.
  public static $default_instance_settings  = array(
    // The short name for the vocabulary (e.g. shcema, SO, GO, PATO, etc.).
    'term_vocabulary' => 'data',
    // The name of the term.
    'term_name' => 'accession',
    // The unique ID (i.e. accession) of the term.
    'term_accession' => '2091',
    // Set to TRUE if the site admin is allowed to change the term
    // type. This will create form elements when editing the field instance
    // to allow the site admin to change the term settings above.
    'term_fixed' => TRUE,
  );

  // The default widget for this field.
  public static $default_widget = 'data__accession_widget';

  // The default formatter for this field.
  public static $default_formatter = 'data__accession_formatter';

  /**
   * @see TripalField::load()
   */
  public function load($entity, $details = array()) {

    $record = $details['record'];

    $field_name = $this->field['field_name'];
    $field_type = $this->field['type'];
    $field_table = $this->instance['settings']['chado_table'];
    $field_column = $this->instance['settings']['chado_column'];


    // Set some defauls for the empty record
    $entity->{$field_name}['und'][0] = array(
      'value' => array(
        'vocabulary' => '',
        'accession' => '',
        'URL' => '',
      ),
      'chado-' . $field_table . '__' . $field_column => '',
      'db_id' => '',
      'accession' => '',
      'version' => '',
      'description' => '',
    );

    // Get the primary dbxref record (if it's not NULL).  Because we have a
    // dbxref_id passed in by the base record, we will only have one record.
    if ($record->$field_column) {
      $dbxref = $record->$field_column;
      $value = $dbxref->db_id->name . ':' . $dbxref->accession;
      $entity->{$field_name}['und'][0] = array(
        'value' => array(
          'vocabulary' => $dbxref->db_id->name,
          'accession' => $dbxref->accession,
          'URL' => tripal_get_dbxref_url($dbxref),
        ),
        'chado-' . $field_table . '__' . $field_column => $record->$field_column->$field_column,
        'db_id'       => $dbxref->db_id->db_id,
        'accession'   => $dbxref->accession,
        'version'     => $dbxref->version,
        'description' => $dbxref->description,
      );
    }
  }

  /**
   * @see TripalField::validate()
   */
  public function validate($entity_type, $entity, $field, $items, &$errors) {

    $field_name = $this->field['field_name'];
    $settings = $this->field['settings'];
    $field_name = $this->field['field_name'];
    $field_type = $this->field['type'];
    $field_table = $this->instance['settings']['chado_table'];
    $field_column = $this->instance['settings']['chado_column'];

    // Get the field values.
    foreach ($items as $delta => $values) {
      $fk_val = $values['chado-' . $field_table . '__' . $field_column];
      $db_id = $values['db_id'];
      $accession = $values['accession'];
      $version = $values['version'];
      $description = $values['description'];

      // Make sure that if a database ID is provided that an accession is also
      // provided.  Here we use the form_set_error function rather than the
      // form_error function because the form_error will add a red_highlight
      // around all of the fields in the fieldset which is confusing as it's not
      // clear to the user what field is required and which isn't. Therefore,
      // we borrow the code from the 'form_error' function and append the field
      // so that the proper field is highlighted on error.
      if (!$db_id and $accession) {
        $errors[$field_name][$delta]['und'][] = array(
          'message' => t("A database and the accession must both be provided for the primary cross reference."),
          'error' => 'chado_base__dbxref',
        );
      }
      if ($db_id and !$accession) {
        $errors[$field_name][$delta]['und'][] = array(
          'message' => t("A database and the accession must both be provided for the primary cross reference."),
          'error' => 'chado_base__dbxref',
        );
      }
      if (!$db_id and !$accession and ($version or $description)) {
        $errors[$field_name][$delta]['und'][] = array(
          'message' => t("A database and the accession must both be provided for the primary cross reference."),
          'error' => 'chado_base__dbxref',
        );
      }
    }
  }

}