<?php

/**
 * @file
 * Provides an application programming interface (API) for semantic web support.
 *
 * @ingroup tripal_chado
 */
/**
 * @defgroup tripal_chado_semweb_api Semantic Web
 * @ingroup tripal_chado_api
 * @{
 * Provides an application programming interface (API) for semantic web support.
 * @}
 */

/**
 * Adds a new Chado table to the semantic web support for Chado.
 *
 * Newly added tables (i.e. custom tables) need to be integrated into the
 * semantic web infrastructure.  After a new table is created and added to
 * the Chado schema, this function should be called to indicate that the
 * table should be included in the semantic web. No associations are made for
 * the columns. The associations should be added using the
 * chado_associate_semweb_term() function.
 *
 * If the table has already been added previously then this function does
 * nothing. It will not overwrite existing assocations.
 *
 * Temporary tables (e.g. Tripal tables that begin with 'tripal_' and end with
 * '_temp', are not supported.
 *
 * @param $chado_table
 *   The name of the Chado table.
 *
 * @ingroup tripal_chado_semweb_api
 */
function chado_add_semweb_table($chado_table) {

  // Don't include the tripal temp tables.
  if (preg_match('/tripal_.+_temp/', $chado_table)) {
    return;
  }

  // Get the table's schema and add all of it's fields if they aren't
  // already there.
  $schema = chado_get_schema($chado_table);
  foreach ($schema['fields'] as $chado_column => $details) {

    // If the record already exists don't overwrite it.
    $record = db_select('chado_semweb', 'CS')
      ->fields('CS', ['chado_semweb_id'])
      ->condition('CS.chado_table', $chado_table)
      ->condition('CS.chado_column', $chado_column)
      ->execute()
      ->fetchField();
    if (!$record) {
      $record = [
        'chado_table' => $chado_table,
        'chado_column' => $chado_column,
      ];
      drupal_write_record('chado_semweb', $record);
    }
  }
}

/**
 * Associates a controlled vocabulary term with a field in a Chado table.
 *
 * For sharing of data via the semantic web we need to associate a
 * term from a controlled vocabulary with every column of every table in Chado.
 *
 * Temporary tables (e.g. Tripal tables that begin with 'tripal_' and end with
 * '_temp', are not supported.
 *
 * @param $chado_table
 *   The name of the table in Chado. This argument is optional. If left empty
 *   or set to NULL then all fields in all Chado tables with that have the
 *   $column_name will be associated with the provided $term.
 * @param $chado_column
 *   The column name in the Chado table to which the term should be associated.
 * @param $term
 *   A cvterm object as returned by chado_generate_var().
 * @param $update
 *   Set to TRUE if the association should be updated to use the new term
 *   if a term is already associated with the table and column.  Default is
 *   FALSE.  If not TRUE and a term is already associated, then no change
 *   occurs.
 *
 * @return boolean
 *   Returns TRUE if the association was made successfully and FALSE otherwise.
 *
 * @ingroup tripal_chado_semweb_api
 */
function chado_associate_semweb_term($chado_table, $chado_column, $term,
                                     $update = FALSE) {

  // Check for required arguments.
  if (!$chado_column) {
    tripal_set_message('Please provide the $chado_column argument.', TRIPAL_ERROR);
    return FALSE;
  }
  if (!$term) {
    tripal_set_message('Please provide the $term argument.', TRIPAL_ERROR);
    return FALSE;
  }

  // Make sure the field is a real field for the table.
  if ($chado_table) {
    $schema = chado_get_schema($chado_table);
    if (!$schema) {
      tripal_set_message("Cannot associate the term with the field because the $chado_table is not a known table in Chado.", TRIPAL_ERROR);
      return FALSE;
    }
    if (!array_key_exists($chado_column, $schema['fields'])) {
      tripal_set_message("Cannot associate the term with the field because the $chado_column is not a known column in the $chado_table.", TRIPAL_ERROR);
      return FALSE;
    }
  }

  // First check to see if a valid record exists that matches the table and
  // column indicated. If it doesn't then insert the record.
  $query = db_select('chado_semweb', 'CS')
    ->fields('CS', ['chado_semweb_id'])
    ->condition('chado_column', $chado_column);
  if ($chado_table) {
    $query->condition('chado_table', $chado_table);
  }
  $query->range(0, 1);
  $id = $query->execute()->fetchField();
  if (!$id) {

    // If no $chado_table record is provided then return FALSE as we can't
    // insert a record without a table.
    if (!$chado_table) {
      tripal_set_message('The provided $chado_column has no match for any
          table currently known. This could be because the table has not yet
          been added to the semantic web management. Please provide the
          $chado_table.', TRIPAL_ERROR);
      return FALSE;
    }

    // Insert the record.
    $id = db_insert('chado_semweb')
      ->fields([
        'chado_table' => $chado_table,
        'chado_column' => $chado_column,
        'cvterm_id' => $term->cvterm_id,
      ]);
    if ($id) {
      return TRUE;
    }
    else {
      tripal_set_message('Failure associating term.', TRIPAL_ERROR);
      return FALSE;
    }
  }

  // If the $chado_table argument is empty or NULL then the term applies to
  // all fields of the specified name.
  $update = db_update('chado_semweb')
    ->fields([
      'cvterm_id' => $term->cvterm_id,
    ])
    ->condition('chado_column', $chado_column);
  if ($chado_table) {
    $update->condition('chado_table', $chado_table);
  }
  if (!$update) {
    $update->condition('cvterm_id', NULL);
  }
  $num_updated = $update->execute();
  if (!$num_updated) {
    tripal_set_message('Failure associating term.', TRIPAL_ERROR);
    return FALSE;
  }

  return TRUE;
}

/**
 * Retrieves the term that maps to the given Chado table and field.
 *
 * @param $chado_table
 *   The name of the Chado table.
 * @param $chado_column
 *   The name of the Chado field.
 * @param $options
 *   An associative array of one or more of the following keys:
 *     -return_object:  Set to TRUE to return the cvterm object rather than
 *      the string version of the term.
 *
 * @return
 *   Returns a string-based representation of the term (e.g. SO:0000704). If
 *   the 'return_object' options is provided then a cvterm object is returned.
 *   returns NULL if no term is mapped to the table and column.
 *
 * @ingroup tripal_chado_semweb_api
 */
function chado_get_semweb_term($chado_table, $chado_column, $options = []) {
  $cvterm_id = db_select('chado_semweb', 'CS')
    ->fields('CS', ['cvterm_id'])
    ->condition('chado_column', $chado_column)
    ->condition('chado_table', $chado_table)
    ->execute()
    ->fetchField();

  if ($cvterm_id) {
    $cvterm = chado_generate_var('cvterm', ['cvterm_id' => $cvterm_id]);
    if (array_key_exists('return_object', $options)) {
      return $cvterm;
    }

    return chado_format_semweb_term($cvterm);
  }
}

/**
 * Retrieves the terms that maps to the given Chado table.
 *
 * @param $chado_table
 *   The name of the Chado table.
 * @param $options
 *   An associative array of one or more of the following keys:
 *     -return_object:  Set to TRUE to return the cvterm object rather than
 *      the string version of the term.
 *
 * @return
 *   An array of terms with the table column name as the key and the term
 *   details as the avlue. If the 'return_object' options is provided then
 *   a cvterm object is used as the value. A NULL value is used if no term is
 *   mapped to a column.
 *
 * @ingroup tripal_chado_semweb_api
 */
function chado_get_semweb_terms($chado_table, $options = []) {

  $terms = [];

  $schema = chado_get_schema($chado_table);
  foreach ($schema['fields'] as $chado_column => $details) {
    $terms[$chado_column] = NULL;

    $cvterm_id = db_select('chado_semweb', 'CS')
      ->fields('CS', ['cvterm_id'])
      ->condition('chado_column', $chado_column)
      ->condition('chado_table', $chado_table)
      ->execute()
      ->fetchField();

    if ($cvterm_id) {
      $cvterm = chado_generate_var('cvterm', ['cvterm_id' => $cvterm_id]);
      if (array_key_exists('return_object', $options)) {
        $terms[$chado_column] = $cvterm;
      }
      else {
        $terms[$chado_column] = chado_format_semweb_term($cvterm);
      }
    }

  }
  return $terms;
}

/**
 * Formats a controlled vocabulary term from Chado for use with Tripal.
 *
 * @param $cvterm
 *   A cvterm object.
 *
 * @return
 *   The semantic web name for the term.
 *
 * @ingroup tripal_chado_semweb_api
 */
function chado_format_semweb_term($cvterm) {
  if ($cvterm) {
    return $cvterm->dbxref_id->db_id->name . ':' . $cvterm->dbxref_id->accession;
  }
  return '';
}

/**
 * Retreive the column name in a Chado table that matches a given term.
 *
 * @param $chado_table
 *   The name of the Chado table.
 * @param $term
 *   The term. This can be a term name or a unique identifer of the form
 *   {db}:{accession} or of the form {db}__{term_name}.
 *
 * @return
 *   The name of the Chado column that matches the given term or FALSE if the
 *   term is not mapped to the Chado table.
 *
 * @ingroup tripal_chado_semweb_api
 */
function chado_get_semweb_column($chado_table, $term) {
  $columns = db_select('chado_semweb', 'CS')
    ->fields('CS')
    ->condition('chado_table', $chado_table)
    ->execute();

  while ($column = $columns->fetchObject()) {
    $cvterm_id = $column->cvterm_id;

    if ($cvterm_id) {

      $cvterm = chado_generate_var('cvterm', ['cvterm_id' => $cvterm_id]);

      $full_accession = strtolower($cvterm->dbxref_id->db_id->name . ':' . $cvterm->dbxref_id->accession);
      $full_accession = preg_replace('/ /', '_', $full_accession);
      $full_name_uscore = strtolower($cvterm->dbxref_id->db_id->name . '__' . $cvterm->name);
      $full_name_uscore = preg_replace('/ /', '_', $full_name_uscore);

      $term = preg_replace('/ /', '_', $term);
      $term = strtolower(preg_replace('/ /', '_', $term));

      // Does the term match identically?
      if ($term == $cvterm->name) {
        return $column->chado_column;
      }
      // Is the term a concatenation of the vocab and the accession?
      else {
        if ($term == $full_accession) {
          return $column->chado_column;
        }
        // Is the term a concatenation of the vocab and the accession?
        else {
          if ($term == $full_name_uscore) {
            return $column->chado_column;
          }
        }
      }
    }
  }
  return FALSE;
}
