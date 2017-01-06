<?php

class ChadoField extends TripalField {

  // The default lable for this field.
  public static $default_label = 'Chado Field';

  // The default description for this field.
  public static $default_description = 'The generic base class for all Chado fields. ' .
    'Replace this text as appropriate for the child implementation.';

  // A list of global settings. These can be accessed witihn the
  // globalSettingsForm.  When the globalSettingsForm is submitted then
  // Drupal will automatically change these settings for all fields.
  // Once instances exist for a field type then these settings cannot be
  // changed.
  public static $default_settings = array(
    'storage' => 'field_chado_storage',
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
    'term_vocabulary' => 'schema',
    // The name of the term.
    'term_name' => 'Thing',
    // The unique ID (i.e. accession) of the term.
    'term_accession' => 'Thing',
    // Set to TRUE if the site admin is allowed to change the term
    // type. This will create form elements when editing the field instance
    // to allow the site admin to change the term settings above.
    'term_fixed' => FALSE,
    // The table in Chado that the instance maps to.
    'chado_table' => '',
    // The primary key column of hte table in Dhado.
    'chado_column' => '',
    // The base table.
    'base_table' => '',
  );

  // The module that manages this field.
  public static $module = 'tripal_chado';

  /**
   * Used to filter records that match a given condition.
   *
   * Entities can be filtered using the fields.  This function should be
   * implemented if the field  supports filtering.  To provide filtering,
   * the $query object should be updated to including any joins and
   * conditions necessary. The following rules should be followed when
   * implementing this function:
   * - Try to implement a filter for every element of the 'value' array
   *   returned by the load() function.
   * - However, avoid making filteres for non-indexed database columns.
   * - When giving alias to joined tables be sure to use aliases that are
   *   unique to avoid conflicts with other fields.
   * - When joining with the base table its alias is 'base'.
   * - This function should never set the fields that should be returned
   *   nor ordering or group by.
   * - You may join to materialized views if need be to help speed queries.
   *
   * @param $query
   *   A SelectQuery object.
   * @param $condition
   *   The field specific condition as set in the TripalFieldQuery object.
   */
  public function query($query, $condition) {

  }

  /**
   * Used to sort records that have been filtered.
   *
   * @param $query
   *   A SelectQuery object.
   * @param $order
   *   The field ordering as set in the TripalFieldQuery object.  This function
   *   should handle the ordering request as specified by this object.
   */
  public function queryOrder($query, $order) {

  }

  /**
   * @see TripalField::instanceSettingsForm()
   */
  public function instanceSettingsForm() {
    // Make sure we don't lose our Chado table mappings when the settings
    // are updated.  Setting them as values in the form ensures they don't
    // get accidentally overwritten.
    $element['base_table'] = array(
      '#type' => 'value',
      '#value' => $this->instance['settings']['base_table'],
    );
    $element['chado_table'] = array(
      '#type' => 'value',
      '#value' => $this->instance['settings']['chado_table'],
    );
    $element['chado_column'] = array(
      '#type' => 'value',
      '#value' => $this->instance['settings']['chado_column'],
    );
    return $element;
  }
}
