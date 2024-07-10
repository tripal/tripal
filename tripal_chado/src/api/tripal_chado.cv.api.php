<?php
/**
 * @file
 * Provides API functions specifically for managing controlled vocabulary
 * records in Chado.
 *
 * @ingroup tripal_chado
 */

/**
 * @defgroup tripal_chado_cv_api Chado CV
 * @ingroup tripal_chado_api
 * @{
 * Provides API functions specifically for managing controlled vocabulary
 * records in Chado. Please note that Tripal v3 provides a generic set of
 * API functions for working with controlled vocabularies (CVs). This allows for
 * CVs to be stored using any back-end.  By default CV's continue to be housed
 * in Chado.  Therefore, if you are working directly with controlled vocabulary
 * records inside of a Chado-aware module then these functions can be used.
 * @}
 */

/**
 * Retrieves a chado controlled vocabulary variable
 *
 * @param $identifier
 *   An array with the key stating what the identifier is. Supported keys (only
 *   on of the following unique keys is required):
 *    - cv_id: the chado cv.cv_id primary key.
 *    - name: the chado cv.name field (assume unique).
 * @param $options
 *   An array of options. Supported keys include:
 *     - Any keys supported by chado_generate_var(). See that function
 *       definition fot additional details.
 * @param $schema_name
 *   The name of the chado schema the records reside in.
 *
 * NOTE: the $identifier parameter can really be any array similar to $values
 * passed into chado_select_record(). It should fully specify the cv record to
 * be returned.
 *
 * @return
 *   If unique values were passed in as an identifier then an object describing
 *   the cv will be returned (will be a chado variable from
 *   chado_generate_var()). Otherwise, FALSE will be returned.
 *
 * @ingroup tripal_chado_cv_api
 */
function chado_get_cv($identifiers, $options = [], $schema_name = NULL) {

  // Set default options.
  if (!isset($options['include_fk'])) {
    // Tells chado_generate_var not to follow any foreign keys.
    $options['include_fk'] = [];
  }

  // Set default schema.
  if (!$schema_name) {
    $schema_name = \Drupal::config('tripal_chado.settings')->get('default_schema');
  }

  // Error Checking of parameters.
  if (!is_array($identifiers)) {
    tripal_report_error(
      'tripal_chado_api',
      TRIPAL_ERROR,
      "chado_get_cv: The identifier passed in is expected to be an array with the key
        matching a column name in the cv table (ie: cv_id or name). You passed in %identifier.",
      [
        '%identifier' => print_r($identifiers, TRUE),
      ]
    );
  }
  elseif (empty($identifiers)) {
    tripal_report_error(
      'tripal_chado_api',
      TRIPAL_ERROR,
      "chado_get_cv: You did not pass in anything to identify the cv you want. The identifier
        is expected to be an array with the key matching a column name in the cv table
        (ie: cv_id or name). You passed in %identifier.",
      [
        '%identifier' => print_r($identifiers, TRUE),
      ]
    );
  }

  // Try to get the cv.
  $cv = chado_generate_var(
    'cv',
    $identifiers,
    $options,
    $schema_name
  );

  // Ensure the cv is singular. If it's an array then it is not singular.
  if (is_array($cv)) {
    tripal_report_error(
      'tripal_chado_api',
      TRIPAL_ERROR,
      "chado_get_cv: The identifiers you passed in were not unique. You passed in %identifier.",
      [
        '%identifier' => print_r($identifiers, TRUE),
      ]
    );
  }

  // Report an error if $cv is FALSE since then chado_generate_var has failed.
  elseif ($cv === FALSE) {
    tripal_report_error(
      'tripal_chado_api',
      TRIPAL_ERROR,
      "chado_get_cv: chado_generate_var() failed to return a cv based on the identifiers
        you passed in. You should check that your identifiers are correct, as well as, look
        for a chado_generate_var error for additional clues. You passed in %identifier.",
      [
        '%identifier' => print_r($identifiers, TRUE),
      ]
    );
  }

  // Else, as far we know, everything is fine so give them their cv :)
  else {
    return $cv;
  }
}

/**
 * Create an options array to be used in a form element which provides a
 * list of all chado cvs.
 *
 * @param $schema_name
 *   The name of the chado schema the records reside in.
 *
 * @return
 *   An array(cv_id => name) for each cv in the chado cv table.
 *
 * @ingroup tripal_chado_cv_api
 */
function chado_get_cv_select_options($schema_name = NULL) {

  // Set default schema.
  if (!$schema_name) {
    $schema_name = \Drupal::config('tripal_chado.settings')->get('default_schema');
  }

  $results = chado_select_record(
    'cv',
    [
      'cv_id',
      'name',
    ],
    [],
    ['order_by' => ['name' => 'ASC']],
    $schema_name);

  $options = [];
  $options[] = 'Select a Vocabulary';

  foreach ($results as $r) {
    $options[$r->cv_id] = $r->name;
  }

  return $options;

}

/**
 * Retrieves a chado controlled vocabulary term variable.
 *
 * @param $identifier
 *   An array apropriate for use with the chado_generate_var for uniquely
 *   identifying a cvterm record. Alternativley, there are also some specially
 *   handled keys. They are:
 *    - id: an ID for the term of the for [dbname]:[accession], where [dbname]
 *      is the short name of the vocabulary and accession is the unique ID.
 *    - cv_id:  an integer indicating the cv_id or an array with 'name' => the
 *      name of the cv.
 *    - synonym: an array with 'name' => the name of the synonym of the cvterm
 *      you want returned; 'cv_id' => the cv_id of the synonym; 'cv_name' =>
 *      the name of the cv of the synonym.
 *    - property: An array/object describing the property to select records
 *      for. It should at least have either a type_name (if unique across cvs)
 *      or type_id. Other supported keys include: cv_id/cv_name (of the type),
 *      value and rank.
 * @param $options
 *   An array of options. Supported keys include:
 *     - Any keys supported by chado_generate_var(). See that function
 *       definition for additional details.
 * @param $schema_name
 *   The name of the chado schema the records reside in.
 *
 * NOTE: the $identifier parameter can really be any array similar to $values
 *   passed into chado_select_record(). It should fully specify the cvterm
 *   record to be returned.
 *
 * @return
 *   If unique values were passed in as an identifier then an object describing
 *   the cvterm will be returned (will be a chado variable from
 *   chado_generate_var()). Otherwise, FALSE will be returned.
 *
 * @ingroup tripal_chado_cv_api
 */
function chado_get_cvterm($identifiers, $options = [], $schema_name = NULL) {

  // Set default options.
  if (!isset($options['include_fk'])) {
    // Tells chado_generate_var to only get the cv.
    $options['include_fk'] = ['cv_id' => TRUE];
  }

  // Set default schema.
  if (!$schema_name) {
    $schema_name = \Drupal::config('tripal_chado.settings')->get('default_schema');
  }

  // Error Checking of parameters.
  if (!is_array($identifiers)) {
    tripal_report_error('tripal_cv_api', TRIPAL_ERROR,
      "chado_get_cvterm: The identifier passed in is expected to be an array with the key
        matching a column name in the cvterm table (ie: cvterm_id or name). You passed in %identifier.",
      ['%identifier' => print_r($identifiers, TRUE)]
    );
  }
  elseif (empty($identifiers)) {
    tripal_report_error('tripal_cv_api', TRIPAL_ERROR,
      "chado_get_cvterm: You did not pass in anything to identify the cvterm you want. The identifier
        is expected to be an array with the key matching a column name in the cvterm table
        (ie: cvterm_id or name). You passed in %identifier.",
      ['%identifier' => print_r($identifiers, TRUE)]
    );
  }

  // If synonym was passed in, then process this first before calling
  // chado_generate_var().
  if (isset($identifiers['synonym'])) {
    $synonym = $identifiers['synonym']['name'];

    $values = ['synonym' => $synonym];
    if (isset($identifiers['synonym']['cv_id'])) {
      $values['cvterm_id'] = ['cv_id' => $identifiers['synonym']['cv_id']];
    }
    if (isset($identifiers['synonym']['cv_name'])) {
      $values['cvterm_id'] = ['cv_id' => ['name' => $identifiers['synonym']['cv_name']]];
    }
    $options = [
      'case_insensitive_columns' => ['name'],
    ];
    $result = chado_select_record('cvtermsynonym', ['cvterm_id'], $values, $options, $schema_name);

    // if the synonym doens't exist or more than one record is returned then
    // return false.
    if (count($result) == 0) {
      return FALSE;
    }
    if (count($result) > 1) {
      return FALSE;
    }

    $identifiers = ['cvterm_id' => $result[0]->cvterm_id];
  }

  // If one of the identifiers is property then use chado_get_record_with_property().
  if (isset($identifiers['property'])) {
    $property = $identifiers['property'];
    unset($identifiers['property']);
    $cvterm = chado_get_record_with_property(
      ['table' => 'cvterm', 'base_records' => $identifiers],
      ['type_name' => $property],
      $options
    );
  }
  if (isset($identifiers['id'])) {
    list($db_name, $accession) = preg_split('/:/', $identifiers['id']);
    $cvterm = chado_generate_var(
      'cvterm',
      [
        'dbxref_id' => [
          'db_id' => [
            'name' => $db_name,
          ],
          'accession' => $accession,
        ],
      ],
      [],
      $schema_name
    );
  }

  // Else we have a simple case and we can just use chado_generate_var to get
  // the cvterm.
  else {
    // Try to get the cvterm.
    $cvterm = chado_generate_var('cvterm', $identifiers, $options, $schema_name);
  }

  // Ensure the cvterm is singular. If it's an array then it is not singular.
  if (is_array($cvterm)) {
    tripal_report_error(
      'tripal_cv_api',
      TRIPAL_ERROR,
      "chado_get_cvterm: The identifiers you passed in were not unique. You passed in %identifier.",
      [
        '%identifier' => print_r($identifiers, TRUE),
      ]
    );
  }

  // Report an error if $cvterm is FALSE since then chado_generate_var has
  // failed.
  elseif ($cvterm === FALSE) {
    tripal_report_error(
      'tripal_cv_api',
      TRIPAL_ERROR,
      "chado_get_cvterm: chado_generate_var() failed to return a cvterm based on the identifiers
        you passed in. You should check that your identifiers are correct, as well as, look
        for a chado_generate_var error for additional clues. You passed in %identifier.",
      [
        '%identifier' => print_r($identifiers, TRUE),
      ]
    );
  }

  // Else, as far we know, everything is fine so give them their cvterm :)
  else {
    return $cvterm;
  }

}

/**
 * Create an options array to be used in a form element
 *   which provides a list of all chado cvterms.
 *
 * @param $cv_id
 *   The chado cv_id; only cvterms with the supplied cv_id will be returnedl.
 * @param $rel_type
 *   Set to TRUE if the terms returned should only be relationship types in
 *   the vocabulary.  This is useful for creating drop-downs of terms
 *   used for relationship linker tables.
 * @param $schema_name
 *   The name of the chado schema the records reside in.
 *
 * @return
 *   An associative array with the cvterm_id's as keys. The first
 *   element in the array has a key of '0' and a value of 'Select a Type'.
 *
 * @ingroup tripal_chado_cv_api
 */
function chado_get_cvterm_select_options($cv_id, $rel_type = FALSE, $schema_name = NULL) {
  // Set default schema.
  if (!$schema_name) {
    $schema_name = \Drupal::config('tripal_chado.settings')->get('default_schema');
  }

  $columns = ['cvterm_id', 'name'];
  $values = ['cv_id' => $cv_id];
  if ($rel_type) {
    $values['is_relationshiptype'] = 1;
  }
  $s_options = ['order_by' => ['name' => 'ASC']];

  $cvterms = chado_select_record('cvterm', $columns, $values, $s_options, $schema_name);

  $options = [];
  $options[0] = 'Select a Type';
  foreach ($cvterms as $cvterm) {
    $options[$cvterm->cvterm_id] = $cvterm->name;
  }

  return $options;

}

/**
 * Adds a controlled vocabulary to the CV table of Chado.
 *
 * @param $name
 *   The name of the controlled vocabulary. These are typically all lower case
 *   with no special characters other than an undrescore (for spaces).
 * @param $comment
 *   A description or definition of the vocabulary.
 * @param $options
 *   No options currently supported. For consistency.
 * @param $schema_name
 *   The name of the chado schema the records reside in.
 *
 * @return
 *   An object populated with fields from the newly added database.
 *
 * @ingroup tripal_chado_cv_api
 */
function chado_insert_cv($name, $definition, $options = [], $schema_name = NULL) {

  // Set default schema.
  if (!$schema_name) {
    $schema_name = \Drupal::config('tripal_chado.settings')->get('default_schema');
  }

  // Insert/update values.
  $ins_values = [
    'name' => $name,
    'definition' => $definition,
  ];

  // See if the CV default exists already in the database.
  $sel_values = ['name' => $name];
  $results = chado_select_record('cv', ['*'], $sel_values, [], $schema_name);

  // If it does not exist then add it.
  if (count($results) == 0) {
    $success = chado_insert_record('cv', $ins_values, [], $schema_name);
    if (!$success) {
      tripal_report_error('tripal_chado', TRIPAL_WARNING, "Failed to create the CV record", []);
      return FALSE;
    }
    $results = chado_select_record('cv', ['*'], $sel_values, [], $schema_name);
  }
  // If it already exists then do an update.
  else {
    $success = chado_update_record('cv', $sel_values, $ins_values, [], $schema_name);
    if (!$success) {
      tripal_report_error('tripal_chado', TRIPAL_WARNING, "Failed to update the CV record", []);
      return FALSE;
    }
    $results = chado_select_record('cv', ['*'], $sel_values, [], $schema_name);
  }

  // Return the cv object.
  return $results[0];
}

/**
 *  Add's a controlled vocabulary term to Chado.
 *
 *  This function will add a cvterm record (and a dbxref record if appropriate
 *  values are provided). If the parent vocabulary does not exist then
 *  that also is added to the cv table.  If the cvterm is a relationship term
 *  then the 'is_relationship' value should be set.  All
 *  terms must also have a corresponding database.  This is specified in the
 *  term's ID just before the colon (e.g. GO:003824).  If the database does not
 *  exist in the DB table then it will be added automatically.  The accession
 *  (the value just after the colon in the term's ID) will be added to the
 *  dbxref table.  If the CVterm already exists and $update is set (default)
 *  then the cvterm is updated.  If the CVTerm already exists and $update is
 *  not set, then no changes are made and the CVTerm object is returned.
 *
 * @param $term
 *   An associative array with the following keys:
 *    - id: the term accession. must be of the form <DB>:<ACCESSION>, where
 *      <DB> is the name of the database to which the cvterm belongs and the
 *      <ACCESSION> is the term's accession number in the database.
 *    - name: the name of the term. usually meant to be human-readable.
 *    - is_obsolete: is present and set to 1 if the term is defunct.
 *    - definition: the definition of the term.
 *    - cv_name: The CV name to which the term belongs.  If this arugment is
 *        null or not provided then the function tries to find a record in the
 *        CV table with the same name provided in the $term[namespace].  If
 *        this field is provided then it overrides what the value in
 *        $term[namespace].
 *    - is_relationship: If this term is a relationship term then this value
 *        should be 1.
 *    - db_name: In some cases the database name will not be part of the
 *        $term['id'] and it needs to be explicitly set.  Use this argument
 *        only if the database name cannot be specififed in the term ID
 *        (e.g. <DB>:<ACCESSION>).
 * @param $options
 *   An associative array with the following keys:
 *    - update_existing: By default this is TRUE.  If the term exists it is
 *      automatically updated.
 * @param $schema_name
 *   The name of the chado schema the records reside in.
 *
 * @return
 *   A cvterm object
 *
 * @ingroup tripal_chado_cv_api
 */
function chado_insert_cvterm($term, $options = [], $schema_name = NULL) {
  // Set default schema.
  if (!$schema_name) {
    $schema_name = \Drupal::config('tripal_chado.settings')->get('default_schema');
  }

  // Get the term properties.
  $id = (isset($term['id'])) ? $term['id'] : '';
  $name = '';
  $cvname = '';
  $definition = '';
  $is_obsolete = 0;
  $accession = '';
  // Set Defaults.
  if (isset($term['cv_name'])) {
    $cvname = $term['cv_name'];
  }
  else {
    $cvname = 'local';
  }
  // Namespace is deprecated but must be supported for backwards
  // compatability.
  if (array_key_exists('namespace', $term)) {
    $cvname = $term['namespace'];
  }

  $is_relationship = 0; // default value
  if (isset($term['is_relationship'])) {
    $is_relationship = $term['is_relationship'];
  }

  $dbname = 'internal'; // default value
  if (isset($term['db_name'])) {
    $dbname = $term['db_name'];
  }

  $update = 1; // default value
  if (isset($options['update_existing'])) {
    $update = $options['update_existing'];
  }

  $name = $id; // default value
  if (array_key_exists('name', $term)) {
    $name = $term['name'];
  }

  $definition = ''; //default value
  if (array_key_exists('definition', $term)) {
    $definition = preg_replace('/^\"(.*)\"/', '\1', $term['definition']);
  }

  if (array_key_exists('is_obsolete', $term)) {
    $is_obsolete = $term['is_obsolete'];
    if (strcmp($is_obsolete, 'true') == 0) {
      $is_obsolete = 1;
    }
  }
  if (!$name and !$id) {
    tripal_report_error('tripal_cv', TRIPAL_WARNING, "Cannot find cvterm without 'id' or 'name'", []);
    return 0;
  }
  if (!$id) {
    $id = $name;
  }
  // Get the accession and the database from the cvterm id.
  if ($dbname) {
    $accession = $id;
  }
  if (preg_match('/^.+?:.*$/', $id)) {
    $accession = preg_replace('/^.+?:(.*)$/', '\1', $id);
    $dbname = preg_replace('/^(.+?):.*$/', '\1', $id);
  }
  // Check that we have a database name, give a different message if it's a
  // relationship.
  if ($is_relationship and !$dbname) {
    tripal_report_error('tripal_cv', TRIPAL_WARNING, "A database name is not provided for this relationship term: $id", []);
    return 0;
  }
  if (!$is_relationship and !$dbname) {
    tripal_report_error('tripal_cv', TRIPAL_WARNING, "A database identifier is missing from the term: $id", []);
    return 0;
  }

  // Check if CV already exists
  //$cv = chado_get_cv(['name' => $cvname], [], $schema_name); // BEFOREOPT: 5.9ms
  // OPT: 5.3ms
  $sql_cv = "SELECT cv_id FROM {cv} WHERE name = :name LIMIT 1";
  $results_cv = chado_query($sql_cv, [':name' => $cvname], [], $schema_name);
  $cv = null;
  foreach ($results_cv as $row_cv) {
    $cv = new stdClass();
    $cv->cv_id = $row_cv->cv_id;
  }

  // If CV does not exist
  if (!$cv) {
    $cv = chado_insert_cv($cvname, '', [], $schema_name);
  }
  if (!$cv) {
    tripal_report_error('tripal_cv', TRIPAL_WARNING, "Cannot find namespace '$cvname' when adding/updating $id", []);
    return 0;
  }
  // This SQL statement will be used a lot to find a cvterm so just set it
  // here for easy reference below.  Because CV terms can change their names
  // but accessions don't change, the following SQL finds cvterms based on
  // their accession rather than the name.
  $cvtermsql = "
    SELECT CVT.name, CVT.cvterm_id, CV.cv_id, CV.name as cvname,
      DB.name as dbname, DB.db_id, DBX.accession
    FROM {cvterm} CVT
      INNER JOIN {dbxref} DBX on CVT.dbxref_id = DBX.dbxref_id
      INNER JOIN {db} DB on DBX.db_id = DB.db_id
      INNER JOIN {cv} CV on CV.cv_id = CVT.cv_id
    WHERE DBX.accession = :accession and DB.name = :name
  ";
  // Add the database. The function will just return the DB object if the
  // database already exists.
  $db = chado_get_db(['name' => $dbname], [], $schema_name);
  if (!$db) {
    $db = chado_insert_db(['name' => $dbname], [], $schema_name);
  }
  if (!$db) {
    tripal_report_error('tripal_cv', TRIPAL_WARNING, "Cannot find database '$dbname' in Chado.", []);
    return 0;
  }
  // The cvterm table has two unique dependencies. We need to check both.
  // first check the (name, cv_id, is_obsolete) constraint.
  $values = [
    'name' => $name,
    'is_obsolete' => $is_obsolete,
    'cv_id' => [
      'name' => $cvname,
    ],
  ];
  $result = chado_select_record('cvterm', ['*'], $values, [], $schema_name);
  if (count($result) == 1) {
    $cvterm = $result[0];
    // Get the dbxref record.
    $values = ['dbxref_id' => $cvterm->dbxref_id];
    $result = chado_select_record('dbxref', ['*'], $values, [], $schema_name);
    $dbxref = $result[0];
    if (!$dbxref) {
      tripal_report_error('tripal_cv', TRIPAL_ERROR,
        'Unable to access the dbxref record for the :term cvterm. Term Record: !record',
        [':term' => $name, '!record' => print_r($cvterm, TRUE)]
      );
      return FALSE;
    }
    // Get the db.
    $values = ['db_id' => $dbxref->db_id];
    $result = chado_select_record('db', ['*'], $values, [], $schema_name);
    $db_check = $result[0];
    //     // The database name for this existing term does not match that of the
    //     // one provided to this function.  The CV name matches otherwise we
    //     // wouldn't have made it this far. So, let's swap the database for
    //     // this term.
    //     if ($db_check->name != $db->name) {
    //       // Look to see if the correct dbxref record already exists for this
    //       // database.
    //       $values = array(
    //         'db_id' => $db->db_id,
    //         'accession' => $accession,
    //       );
    //       $result = chado_select_record('dbxref', array('*'), $values);
    //       // If we already have a good dbxref then we want to update our cvterm
    //       // to use this dbxref.
    //       if (count($result) > 0) {
    //         $dbxref = $result[0];
    //         $match = array('cvterm_id' => $cvterm->cvterm_id);
    //         $values = array('dbxref_id' => $dbxref->dbxref_id);
    //         $success = chado_update_record('cvterm', $match, $values);
    //         if (!$success) {
    //           tripal_report_error('tripal_cv', TRIPAL_WARNING, "Failed to correct the dbxref id for the cvterm " .
    //             "'$name' (id: $accession), for database $dbname", NULL);
    //           return 0;
    //         }
    //       }
    //       // If we don't have the dbxref then we want to delete our cvterm and let
    //       // the code below recreate it with the correct info.
    //       else {
    //         $match = array('cvterm_id' => $cvterm->cvterm_id);
    //         chado_delete_record('cvterm', $match);
    //       }
    //     }
    // Check that the accession matches.  Sometimes an OBO can define a term
    // multiple times but with different accessions.  If this is the case we
    // can't do an insert or it will violate the constraint in the cvterm table.
    // So we'll need to add the record to the cvterm_dbxref table instead.
    if ($dbxref->accession != $accession) {
      // Get/add the dbxref for his term.
      $dbxref_new = chado_insert_dbxref([
        'db_id' => $db->db_id,
        'accession' => $accession,
      ], [], $schema_name);
      if (!$dbxref_new) {
        tripal_report_error('tripal_cv', TRIPAL_WARNING, "Failed to find or insert the dbxref record for cvterm, " .
          "$name (id: $accession), for database $dbname", []);
        return 0;
      }
      // Check to see if the cvterm_dbxref record already exists.
      $values = [
        'cvterm_id' => $cvterm->cvterm_id,
        'dbxref_id' => $dbxref_new->dbxref_id,
        'is_for_definition' => 1,
      ];
      $result = chado_select_record('cvterm_dbxref', ['*'], $values, [], $schema_name);
      // if the cvterm_dbxref record does not exist then add it
      if (count($result) == 0) {
        $options = [
          'return_record' => FALSE,
        ];
        $success = chado_insert_record('cvterm_dbxref', $values, $options, $schema_name);
        if (!$success) {
          tripal_report_error('tripal_cv', TRIPAL_WARNING, "Failed to find or insert the cvterm_dbxref record for a " .
            "duplicated cvterm:  $name (id: $accession), for database $dbname", []);
          return 0;
        }
      }
      // Get the original cvterm with the same name and return that.
      $result = chado_query($cvtermsql, [
        ':accession' => $dbxref->accession,
        ':name' => $dbname,
      ], [], $schema_name);
      $cvterm = $result->fetchObject();
      return $cvterm;
    }
    // Continue on, we've fixed the record if the db_id did not match.
    // We can now perform and updated if we need to.
  }
  // Get the CVterm record.
  $result = chado_query($cvtermsql, [
    ':accession' => $accession,
    ':name' => $dbname,
  ], [], $schema_name);
  $cvterm = $result->fetchObject();
  if (!$cvterm) {
    // Check to see if the dbxref exists if not, add it.
    $dbxref = chado_insert_dbxref([
      'db_id' => $db->db_id,
      'accession' => $accession,
    ], [], $schema_name);
    if (!$dbxref) {
      tripal_report_error('tripal_cv', TRIPAL_WARNING, "Failed to find or insert the dbxref record for cvterm, " .
        "$name (id: $accession), for database $dbname", []);
      return 0;
    }
    // Check to see if the dbxref already has an entry in the cvterm table.
    $values = ['dbxref_id' => $dbxref->dbxref_id];
    $check = chado_select_record('cvterm', ['cvterm_id'], $values, [], $schema_name);
    if (count($check) == 0) {
      // now add the cvterm
      $ins_values = [
        'cv_id' => $cv->cv_id,
        'name' => $name,
        'definition' => $definition,
        'dbxref_id' => $dbxref->dbxref_id,
        'is_obsolete' => $is_obsolete,
        'is_relationshiptype' => $is_relationship,
      ];
      $success = chado_insert_record('cvterm', $ins_values, [], $schema_name);
      if (!$success) {
        if (!$is_relationship) {
          tripal_report_error('tripal_cv', TRIPAL_WARNING, "Failed to insert the term: $name ($dbname)", []);
          return 0;
        }
        else {
          tripal_report_error('tripal_cv', TRIPAL_WARNING, "Failed to insert the relationship term: $name (cv: " . $cvname . " db: $dbname)", []);
          return 0;
        }
      }
    }
    // This dbxref already exists in the cvterm table.
    else {
      tripal_report_error('tripal_cv', TRIPAL_WARNING, "The dbxref already exists for another cvterm record: $name (cv: " . $cvname . " db: $dbname)", []);
      return 0;
    }
    $result = chado_query($cvtermsql, [
      ':accession' => $accession,
      ':name' => $dbname,
    ], [], $schema_name);
    $cvterm = $result->fetchObject();
  }
  // Update the cvterm.
  elseif ($update) {
    // First, basic update of the term.
    $match = ['cvterm_id' => $cvterm->cvterm_id];
    $upd_values = [
      'name' => $name,
      'definition' => $definition,
      'is_obsolete' => $is_obsolete,
      'is_relationshiptype' => $is_relationship,
    ];
    $success = chado_update_record('cvterm', $match, $upd_values, [], $schema_name);
    if (!$success) {
      tripal_report_error('tripal_cv', TRIPAL_WARNING, "Failed to update the term: $name", []);
      return 0;
    }
    // Second, check that the dbxref has not changed and if it has then update
    // it.
    $checksql = "
      SELECT cvterm_id
      FROM {cvterm} CVT
        INNER JOIN {dbxref} DBX on CVT.dbxref_id = DBX.dbxref_id
        INNER JOIN {db} DB on DBX.db_id = DB.db_id
        INNER JOIN {cv} CV on CV.cv_id = CVT.cv_id
      WHERE DBX.accession = :accession and DB.name = :dbname and CVT.name = :term and CV.name = :cvname
    ";
    $check = chado_query($checksql, [
      ':accession' => $accession,
      ':dbname' => $dbname,
      ':term' => $name,
      ':cvname' => $cvname,
    ], [], $schema_name)->fetchObject();
    if (!$check) {
      // Check to see if the dbxref exists if not, add it.
      $dbxref = chado_insert_dbxref([
        'db_id' => $db->db_id,
        'accession' => $accession,
      ], [], $schema_name);
      if (!$dbxref) {
        tripal_report_error('tripal_chado', TRIPAL_WARNING, "Failed to find or insert the dbxref record for cvterm, " .
          "$name (id: $accession), for database $dbname", []);
        return 0;
      }
      $match = ['cvterm_id' => $cvterm->cvterm_id];
      $upd_values = [
        'dbxref_id' => $dbxref->dbxref_id,
      ];
      $success = chado_update_record('cvterm', $match, $upd_values, [], $schema_name);
      if (!$success) {
        tripal_report_error('tripal_chado', TRIPAL_WARNING, "Failed to update the term $name with new accession $db:$accession", []);
        return 0;
      }
    }
    // Finally grab the updated details.
    $result = chado_query($cvtermsql, [
      ':accession' => $accession,
      ':name' => $dbname,
    ], [], $schema_name);
    $cvterm = $result->fetchObject();
  }
  else {
    // Do nothing, we have the cvterm but we don't want to update.
  }
  // Return the cvterm.
  return $cvterm;
}

/**
 * This function is intended to be used in autocomplete forms.
 *
 * This function searches for a matching controlled vobulary name.
 *
 * @param $string
 * The string to search for.
 *
 * @return
 * A json array of terms that begin with the provided string.
 *
 * @ingroup tripal_chado_cv_api
 */
function chado_autocomplete_cv($string = '') {
  $sql = "
    SELECT CV.cv_id, CV.name
    FROM {cv} CV
    WHERE lower(CV.name) like lower(:name)
    ORDER by CV.name
    LIMIT 25 OFFSET 0
  ";
  $results = chado_query($sql, [':name' => $string . '%']);
  $items = [];
  foreach ($results as $cv) {
    $items[$cv->name] = $cv->name;
  }

  drupal_json_output($items);
}

/**
 * This function is intended to be used in autocomplete forms
 * for searching for CV terms that begin with the provided string.
 *
 * @param $cv_id
 * The CV ID in which to search for the term.
 * @param $string
 * The string to search for.
 *
 * @return
 * A json array of terms that begin with the provided string.
 *
 * @ingroup tripal_chado_cv_api
 */
function chado_autocomplete_cvterm($cv_id, $string = '') {
  if ($cv_id) {
    $sql = "
      SELECT CVT.cvterm_id, CVT.name
      FROM {cvterm} CVT
      WHERE CVT.cv_id = :cv_id and lower(CVT.name) like lower(:name)
      UNION
      SELECT CVT2.cvterm_id, CVTS.synonym as name
      FROM {cvterm} CVT2
        INNER JOIN {cvtermsynonym} CVTS ON CVTS.cvterm_id = CVT2.cvterm_id
      WHERE CVT2.cv_id = :cv_id and lower(CVTS.synonym) like lower(:name)
      ORDER by name
      LIMIT 25 OFFSET 0
    ";
    $results = chado_query($sql, [
      ':cv_id' => $cv_id,
      ':name' => $string . '%',
    ]);
    $items = [];
    foreach ($results as $term) {
      $items[$term->name] = $term->name;
    }
  }
  // If a CV wasn't provided then search all of them, and include the cv
  // in the results.
  else {
    $sql = "
      SELECT CVT.cvterm_id, CVT.name, CV.name as cvname, CVT.cv_id
      FROM {cvterm} CVT
        INNER JOIN {cv} CV on CVT.cv_id = CV.cv_id
      WHERE lower(CVT.name) like lower(:name)
      UNION
      SELECT CVT2.cvterm_id, CVTS.synonym as name, CV2.name as cvname, CVT2.cv_id
      FROM {cvterm} CVT2
        INNER JOIN {cv} CV2 on CVT2.cv_id = CV2.cv_id
        INNER JOIN {cvtermsynonym} CVTS ON CVTS.cvterm_id = CVT2.cvterm_id
      WHERE lower(CVTS.synonym) like lower(:name)
      ORDER by name
      LIMIT 25 OFFSET 0
    ";
    $results = chado_query($sql, [':name' => $string . '%']);
    $items = [];
    foreach ($results as $term) {
      $items[$term->name] = $term->name;
    }
  }

  drupal_json_output($items);
}

/**
 * Add a record to a cvterm linking table (ie: feature_cvterm).
 *
 * @param $basetable
 *   The base table to which the cvterm should be linked/associated. Thus to
 *   associate a cvterm to a feature the basetable=feature and cvterm_id is
 *   added to the feature_cvterm table.
 * @param $record_id
 *   The primary key of the basetable to associate the cvterm with. This should
 *   be in integer.
 * @param $cvterm
 *   An associative array describing the cvterm. Valid keys include:
 *     - name: the name for the cvterm,
 *     - cv_name: the name of the cv the cvterm belongs to.
 *     - cv_id: the primary key of the cv the cvterm belongs to.
 * @param $options
 *   An associative array of options. Valid keys include:
 *     - insert_cvterm: Insert the cvterm if it doesn't already exist. FALSE is
 *       the default.
 * @param $schema_name
 *  The name of the chado schema the record resides in.
 *
 * @ingroup tripal_chado_cv_api
 */
function chado_associate_cvterm($basetable, $record_id, $cvterm, $options, $schema_name) {
  $linking_table = $basetable . '_cvterm';
  $foreignkey_name = $basetable . '_id';

  // Default Values
  $options['insert_cvterm'] = (isset($options['insert_cvterm'])) ? $options['insert_cvterm'] : FALSE;

  // If the cvterm_id is not set then find the cvterm record using the name and
  // cv_id.
  if (!isset($cvterm['cvterm_id'])) {
    $values = [
      'name' => $cvterm['name'],
    ];
    if (isset($cvterm['cv_id'])) {
      $values['cv_id'] = $cvterm['cv_id'];
    }
    elseif (isset($cvterm['cv_name'])) {
      $values['cv_id'] = [
        'name' => $cvterm['cv_name'],
      ];
    }
    else {
      tripal_report_error('tripal_chado_api', TRIPAL_WARNING,
        "chado_associate_cvterm: The cvterm needs to have either the cv_name or cv_id
          supplied. You were trying to associate a cvterm with the %base %record_id
          and supplied the cvterm values: %cvterm.",
        [
          '%base' => $basetable,
          '%record_id' => $record_id,
          '%cvterm' => print_r($cvterm, TRUE),
        ]
      );
      print "need cv_id/cv_name:" . print_r($values, TRUE);
      return FALSE;
    }

    // Get the cvterm. If it doesn't exist then add it if the option
    // 'insert_cvterm' is set.
    $select = chado_select_record('cvterm', ['*'], $values, [], $schema_name);
    if ($select) {
      $cvterm['cvterm_id'] = $select[0]->cvterm_id;
    }
    elseif ($options['insert_cvterm']) {
      // Insert the cvterm
      $insert = chado_insert_cvterm($values, [], $schema_name);
      if (isset($insert->cvterm_id)) {
        $cvterm['cvterm_id'] = $insert->cvterm_id;
      }
      else {
        tripal_report_error('tripal_chado_api', TRIPAL_WARNING,
          "chado_associate_cvterm: Unable to insert the cvterm using the cvterm values: %cvterm.",
          ['%cvterm' => print_r($cvterm, TRUE)]
        );
        print "Unable to insert cvterm:" . print_r($values, TRUE);
        return FALSE;
      }
    }
    else {
      tripal_report_error('tripal_api', TRIPAL_WARNING,
        "chado_associate_cvterm: The cvterm doesn't already exist. You supplied the cvterm values: %cvterm.",
        ['%cvterm' => print_r($cvterm, TRUE)]
      );
      print "Cvterm doesn't already exist:" . print_r($values, TRUE);
      return FALSE;
    }
  }

  // Now add the link between the record & cvterm.
  if ($cvterm['cvterm_id'] > 0) {
    $values = [
      'cvterm_id' => $cvterm['cvterm_id'],
      $foreignkey_name => $record_id,
      'pub_id' => 1,
    ];

    // Check if the cvterm is already associated. If so, don't re-add it.
    $result = chado_select_record($linking_table, ['*'], $values, [], $schema_name);
    if (!$result) {
      $success = chado_insert_record($linking_table, $values, [], $schema_name);
      if (!$success) {
        tripal_report_error('tripal_api', TRIPAL_WARNING,
          "Failed to insert the %base record %term",
          ['%base' => $linking_table, '%term' => $cvterm['name']]
        );
        print "Unable to insert linking record:" . print_r($values, TRUE);
        return FALSE;
      }
      $result = chado_select_record($linking_table, ['*'], $values, [], $schema_name);
    }

    if (isset($result[0])) {
      return $result[0];
    }
    else {
      print "2nd last:" . print_r($values, TRUE);
      return FALSE;
    }
  }

  print "last:" . print_r($values, TRUE);
  return FALSE;
}
