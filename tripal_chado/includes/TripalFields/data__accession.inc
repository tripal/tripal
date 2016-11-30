<?php

class data__accession extends TripalField {


  // --------------------------------------------------------------------------
  //                     EDITABLE STATIC CONSTANTS
  //
  // The following constants SHOULD be set for each descendent class.  They are
  // used by the static functions to provide information to Drupal about
  // the field and it's default widget and formatter.
  // --------------------------------------------------------------------------

  // The term that this field maps to.  The format for the term should be:
  // [vocab]:[accession] where [vocab] is the short name of the vocabulary
  // and [acession] is the unique accession number for the term.  This term
  // must already exist in the vocabulary storage backend. This
  // value should never be changed once fields exist for this type.
  public static $term = 'data:2091';

  // The default lable for this field.
  public static $label = 'Accession';

  // The default description for this field.
  public static $description = 'The unique stable accession (ID) for
        this record on this site.';

  // Provide a list of global settings. These can be accessed witihn the
  // globalSettingsForm.  When the globalSettingsForm is submitted then
  // Drupal will automatically change these settings for all fields.
  public static $settings = array(
    'chado_table' => '',
    'chado_column' => '',
    'base_table' => '',
  );

  // Provide a list of instance specific settings. These can be access within
  // the instanceSettingsForm.  When the instanceSettingsForm is submitted
  // then Drupal with automatically change these settings for the instnace.
  // It is recommended to put settings at the instance level whenever possible.
  public static $instance_settings  = array();

  // Set this to the name of the storage backend that by default will support
  // this field.
  public static $storage = 'field_chado_storage';

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
    $field_table = $this->field['settings']['chado_table'];
    $field_column = $this->field['settings']['chado_column'];


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
    $field_table = $this->field['settings']['chado_table'];
    $field_column = $this->field['settings']['chado_column'];

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