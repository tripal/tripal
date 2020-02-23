<?php

/**
 * @file
 * Provides legacy application programming interface (API) to manage controlled
 * vocabularies in the Chado database.
 */

/**
 * @defgroup tripal_legacy_chado_cv_api Legacy Chado CV
 * @ingroup tripal_legacy_api
 * @{
 * Provides an application programming interface (API) to manage entities
 * that use Chado as their base data.
 * @}
 */

/**
 * Retrieves the default vocabulary for a given table and field.
 *
 * Each table in Chado that has a 'type_id' (or foreign key constraint to
 * the cvterm table) will have a default vocabulary assigned. This indicates to
 * Tripal that terms in that vocabulary are used to set the type_id for that
 * table. An example where this is used is the
 * tripal_get_cvterm_select_options() function which generates a list of options
 * for a select box used in a Drupal form.  The select box will list the terms
 * from the default vocabulary in the drop down.
 *
 * This function uses the Chado table and field name (e.g. 'type_id') to
 * retreive the vocabulary assgined.
 *
 * @param $table
 *   The name of the table that contains a field with a foreign key
 *   relationship to the cvterm table
 * @param $field
 *   The table field name that has the foreign key relationship to the
 *   cvterm table for which the default vocabulary will be set
 *
 * @return
 *   The cv object of the default vocabulary or an empty array if not
 *   available.
 *
 * @ingroup tripal_legacy_chado_cv_api
 */
function tripal_get_default_cv($table, $field) {
  $sql = "
    SELECT cv_id
    FROM {tripal_cv_defaults}
    WHERE table_name = :table and field_name = :field
  ";
  $cv_id = db_query($sql, [
    ':table' => $table,
    ':field' => $field,
  ])->fetchField();

  return tripal_get_cv(['cv_id' => $cv_id]);
}

/**
 * Retrieves the Chado table to which a vocbulary is set as default.
 *
 * Each table in Chado that has a 'type_id' (or foreign key constraint to
 * the cvterm table) will have a default vocabulary assigned. This indicates to
 * Tripal that terms in that vocabulary are used to set the type_id for that
 * table. An example where this is used is the
 * tripal_get_cvterm_select_options() function which generates a list of options
 * for a select box used in a Drupal form.  The select box will list the terms
 * from the default vocabulary in the drop down.
 *
 * This function uses the vocabulary ID to get the Chado table to which it
 * is assigned.
 *
 * @param $cv_id
 *  The ID of the vocabulary.
 *
 * @return
 *   If an assignment is present, an object containing the 'table_name' and
 *   'field_name' is returned.
 *
 * @ingroup tripal_legacy_chado_cv_api
 */
function tripal_get_default_cv_table($cv_id) {
  $default = db_select('tripal_cv_defaults', 't')
    ->fields('t', ['table_name', 'field_name'])
    ->condition('cv_id', $cv_id)
    ->execute()
    ->fetchObject();
  return $default;
}

/**
 * Create an options array to be used in a form element
 * which provides a list of all chado cvterms. Unlike the
 * tripal_get_cvterm_select_option, this function retreives the cvterms using
 * the default vocabulary set for a given table and field.  It will also
 * notify the administrative user if a default vocabulary is missing for the
 * field and if the vocabulary is empty.
 *
 * @param $table
 *   The name of the table that contains the field with a foreign key
 *   relationship to the cvterm table
 * @param $field
 *   The table field name that has the foreign key relationship to the
 *   cvterm table for which the default vocabulary will be set
 * @param $field_desc
 *   A human readable descriptive name for the field
 *
 * @return
 *   An array(cvterm_id => name)
 *   for each cvterm in the chado cvterm table where cv_id=that supplied
 *
 * @ingroup tripal_legacy_chado_cv_api
 */
function tripal_get_cvterm_default_select_options($table, $field, $field_desc) {

  $default_cv = tripal_get_default_cv($table, $field);
  $options = [];

  if ($default_cv) {
    $options = tripal_get_cvterm_select_options($default_cv->cv_id);

    if (count($options) == 0) {
      tripal_set_message('There are no ' . $field_desc . '. Please ' .
        l('add terms',
          'admin/tripal/loaders/chado_vocabs/chado_cv/' . $default_cv->cv_id . '/cvterm/add',
          ['attributes' => ['target' => '_blank']]) . ' to the ' .
        $default_cv->name . ' vocabulary.',
        TRIPAL_WARNING);
    }

  }
  else {
    tripal_set_message('There is not a default vocabulary set for ' . $field_desc . '. ' .
      'Please set one using the ' .
      l('vocabulary defaults configuration page',
        'admin/tripal/vocab/defaults',
        ['attributes' => ['target' => '_blank']]) . '.',
      TRIPAL_WARNING);
  }

  return $options;
}


/**
 * This function sets the default vocabulary for a given table and field.
 *
 * @param $table
 *   The name of the table that contains a field with a foreign key
 *   relationship to the cvterm table
 * @param $field
 *   The table field name that has the foreign key relationship to the
 *   cvterm table for which the default vocabulary will be set
 * @param $cv_name
 *   The name of the vocabulary
 *
 * @return
 *   TRUE if set, FALSE if an error occured
 *
 * @ingroup tripal_legacy_chado_cv_api
 */
function tripal_set_default_cv($table, $field, $cv_name, $cv_id = FALSE) {

  // Get the CV object
  if ($cv_id) {
    $cv = tripal_get_cv(['cv_id' => $cv_id]);
  }
  else {
    $cv = tripal_get_cv(['name' => $cv_name]);
  }

  if ($cv) {
    // first delete any entries for this table and field
    $num_deleted = db_delete('tripal_cv_defaults')
      ->condition('table_name', $table)
      ->condition('field_name', $field)
      ->execute();

    // now add the default value
    $cv_default_id = db_insert('tripal_cv_defaults')
      ->fields([
        'table_name' => $table,
        'field_name' => $field,
        'cv_id' => $cv->cv_id,
      ])
      ->execute();

    if (!$cv_default_id) {
      tripal_report_error('tripal_chado', TRIPAL_WARNING,
        "Cannot set default vocabulary for %table.%field. Check the error logs.",
        ['%table' => $table, '%field' => $field]);
      return FALSE;
    }
  }
  else {
    tripal_report_error('tripal_chado', TRIPAL_WARNING,
      "Cannot set default vocabulary for %table.%field. The vocabulary name, '%cvname', doesn't exist.",
      ['%table' => $table, '%field' => $field, '%cvname' => $cv_name]);
    return FALSE;
  }
}