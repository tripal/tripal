<?php
/**
 * @file
 * Provides API functions specifically for managing external database reference
 * records in Chado.
 */

/**
 * @defgroup tripal_chado_database_api Chado DB
 * @ingroup tripal_chado_api
 * @{
 * External databases can be used to indicate the source for a variety of data.
 * The most common use is with controlled vocabularies (CV).  Chado expects that
 * every CV have an external database record, where the database name must be
 * the short name of the CV.  In other cases, records such as features, stocks,
 * libraries, etc., can also be present in remote databases and these
 * associations can be made through dbxref linker tables.  The API functions
 * provided here provide tools to easily work with external databases.
 * @}
 */

/**
 * Retrieves a chado db variable.
 *
 * Example Usage:
 *
 * @code
 *   $select_values = array(
 *     'name' => 'SOFP'
 *   );
 *   $db_object = chado_get_db($select_values);
 * @endcode
 *
 *  The above code selects the SOFP db and returns the following object:
 * @code
 *   $db_object = stdClass Object (
 *     [db_id] => 49
 *     [name] => SOFP
 *     [description] =>
 *     [urlprefix] =>
 *     [url] =>
 *   );
 * @endcode
 *
 * @param $identifier
 *   An array with the key stating what the identifier is. Supported keys (only
 *   on of the following unique keys is required):
 *    - db_id: the chado db.db_id primary key.
 *    - name: the chado db.name field (assume unique).
 * @param $options
 *   An array of options. Supported keys include:
 *     - Any keys supported by chado_generate_var(). See that function
 *       definition for additional details.
 *
 * NOTE: the $identifier parameter can really be any array similar to $values
 * passed into chado_select_record(). It should fully specify the db record to
 * be returned.
 *
 * @return
 *   If unique values were passed in as an identifier then an object describing
 *   the cv will be returned (will be a chado variable from
 *   chado_generate_var()). Otherwise, an array of objects will be returned.
 *
 * @ingroup tripal_chado_database_api
 */
function chado_get_db($identifiers, $options = []) {

  // Set Defaults.
  if (!isset($options['include_fk'])) {
    // Tells chado_generate_var not to follow any foreign keys.
    $options['include_fk'] = [];
  }

  // Error Checking of parameters.
  if (!is_array($identifiers)) {
    tripal_report_error(
      'tripal_chado_database_api',
      TRIPAL_ERROR,
      "chado_get_db: The identifier passed in is expected to be an array with the key
        matching a column name in the db table (ie: db_id or name). You passed in %identifier.",
      [
        '%identifier' => print_r($identifiers, TRUE),
      ]
    );
  }
  elseif (empty($identifiers)) {
    tripal_report_error(
      'tripal_chado_database_api',
      TRIPAL_ERROR,
      "chado_get_db: You did not pass in anything to identify the db you want. The identifier
        is expected to be an array with the key matching a column name in the db table
        (ie: db_id or name). You passed in %identifier.",
      [
        '%identifier' => print_r($identifiers, TRUE),
      ]
    );
  }

  // Try to get the db.
  $db = chado_generate_var(
    'db',
    $identifiers,
    $options
  );

  // Ensure the db is singular. If it's an array then it is not singular.
  if (is_array($db)) {
    tripal_report_error(
      'tripal_chado_database_api',
      TRIPAL_ERROR,
      "chado_get_db: The identifiers you passed in were not unique. You passed in %identifier.",
      [
        '%identifier' => print_r($identifiers, TRUE),
      ]
    );
  }

  // Report an error if $db is FALSE since then chado_generate_var has failed.
  elseif ($db === FALSE) {
    tripal_report_error(
      'tripal_chado_database_api',
      TRIPAL_ERROR,
      "chado_get_db: chado_generate_var() failed to return a db based on the identifiers
        you passed in. You should check that your identifiers are correct, as well as, look
        for a chado_generate_var error for additional clues. You passed in %identifier.",
      [
        '%identifier' => print_r($identifiers, TRUE),
      ]
    );
  }

  // Else, as far we know, everything is fine so give them their db :)
  else {
    return $db;
  }
}

/**
 * Create an options array to be used in a form element
 *   which provides a list of all chado dbs.
 *
 * @return
 *   An array(db_id => name) for each db in the chado db table.
 *
 * @ingroup tripal_chado_database_api
 */
function chado_get_db_select_options() {

  $dbs = chado_query("SELECT db_id, name FROM {db} ORDER BY name");

  $options = [];
  $options[] = 'Select a Database';

  foreach ($dbs as $db) {
    $options[$db->db_id] = $db->name;
  }

  return $options;

}

/**
 * Retrieves a chado database reference variable.
 *
 * Example Usage:
 *
 * @code
 *   $identifiers = array(
 *     'accession' => 'synonym',
 *     'db_id' => array(
 *       'name' => 'SOFP'
 *     )
 *   );
 *   $dbxref_object = chado_get_dbxref($identifiers);
 * @endcode
 *  The above code selects the synonym database reference and returns the
 *  following object:
 * @code
 *  $dbxref_object = stdClass Object (
 *     [dbxref_id] => 2581
 *     [accession] => synonym
 *     [description] =>
 *     [version] =>
 *     [db_db_id] => 49
 *     [db_name] => SOFP
 *     [db_description] =>
 *     [db_urlprefix] =>
 *     [db_url] =>
 *   );
 * @endcode
 *
 * @param $identifier
 *   An array apropriate for use with the chado_generate_var for uniquely
 *   identifying a dbxref record. Alternatively, there are also some specially
 *   handled keys. They are:
 *    - property: An array/object describing the property to select records for.
 *      It should at least have either a type_name (if unique across cvs) or
 *      type_id. Other supported keys include: cv_id/cv_name (of the type),
 *      value and rank.
 * @param $options
 *   An array of options. Supported keys include:
 *     - Any keys supported by chado_generate_var(). See that function
 *       definition for additional details.
 *
 * NOTE: the $identifier parameter can really be any array similar to $values
 * passed into chado_select_record(). It should fully specify the dbxref record
 * to be returned.
 *
 * @return
 *   If unique values were passed in as an identifier then an object describing
 *   the dbxref will be returned (will be a chado variable from
 *   chado_generate_var()). Otherwise, FALSE will be returned.
 *
 * @ingroup tripal_chado_database_api
 */
function chado_get_dbxref($identifiers, $options = []) {

  // Set Defaults.
  if (!isset($options['include_fk'])) {
    // Tells chado_generate_var not only expand the db.
    $options['include_fk'] = ['db_id' => TRUE];
  }

  // Error Checking of parameters.
  if (!is_array($identifiers)) {
    tripal_report_error('tripal_db_api', TRIPAL_ERROR,
      "chado_get_dbxref: The identifier passed in is expected to be an array with the key
        matching a column name in the dbxref table (ie: dbxref_id or name). You passed in %identifier.",
      ['%identifier' => print_r($identifiers, TRUE)]
    );
  }
  elseif (empty($identifiers)) {
    tripal_report_error('tripal_db_api', TRIPAL_ERROR,
      "chado_get_dbxref: You did not pass in anything to identify the dbxref you want. The identifier
        is expected to be an array with the key matching a column name in the dbxref table
        (ie: dbxref_id or name). You passed in %identifier.",
      ['%identifier' => print_r($identifiers, TRUE)]
    );
  }

  // If one of the identifiers is property then use chado_get_record_with_property().
  if (isset($identifiers['property'])) {
    $property = $identifiers['property'];
    unset($identifiers['property']);
    $dbxref = chado_get_record_with_property(
      ['table' => 'dbxref', 'base_records' => $identifiers],
      ['type_name' => $property],
      $options
    );
  }

  // Else we have a simple case and we can just use chado_generate_var to get 
  // the analysis.
  else {
    $dbxref = chado_generate_var('dbxref', $identifiers, $options);
  }

  // Ensure the dbxref is singular. If it's an array then it is not singular.
  if (is_array($dbxref)) {
    tripal_report_error('tripal_db_api', TRIPAL_ERROR,
      "chado_get_dbxref: The identifiers you passed in were not unique. You passed in %identifier.",
      ['%identifier' => print_r($identifiers, TRUE)]
    );
  }

  // Report an error if $dbxref is FALSE since then chado_generate_var has 
  // failed.
  elseif ($dbxref === FALSE) {
    tripal_report_error(
      'tripal_db_api',
      TRIPAL_ERROR,
      "chado_get_dbxref: chado_generate_var() failed to return a dbxref based on the identifiers
        you passed in. You should check that your identifiers are correct, as well as, look
        for a chado_generate_var error for additional clues. You passed in %identifier.",
      [
        '%identifier' => print_r($identifiers, TRUE),
      ]
    );
  }

  // Else, as far we know, everything is fine so give them their dbxref :)
  else {
    return $dbxref;
  }
}

/**
 * Generates a URL for the controlled vocabulary term.
 *
 * If the URL and URL prefix are provided for the database record of a cvterm
 * then a URL can be created for the term.  By default, the db.name and
 * dbxref.accession are concatenated and appended to the end of the
 * db.urlprefix. But Tripal supports the use of {db} and {accession} tokens
 * when if present in the db.urlprefix string will be replaced with the db.name
 * and dbxref.accession respectively.
 *
 * @param $dbxref
 *   A dbxref object as created by the chado_generate_var() function.
 *
 * @return
 *   A string containing the URL.
 *
 * @ingroup tripal_chado_database_api
 */
function chado_get_dbxref_url($dbxref) {
  $final_url = '';

  // Create the URL for the term.
  if ($dbxref->db_id->urlprefix) {
    $db_count = 0;
    $acc_count = 0;
    $url = $dbxref->db_id->urlprefix;

    // If the URL prefix has replacement tokens then use those.
    $url = preg_replace('/\{db\}/', $dbxref->db_id->name, $url, -1, $db_count);
    $url = preg_replace('/\{accession\}/', $dbxref->accession, $url, -1, $acc_count);
    $final_url = $url;

    // If no replacements were made above then tokens weren't used and we can
    // default to just appending the db name and accession to the end.
    if (!$db_count and !$acc_count) {
      $final_url = $dbxref->db_id->urlprefix . $dbxref->db_id->name . ':' . $dbxref->accession;
    }

    // If the URL prefix is relative then convert it to a full URL.
    if (!preg_match('/^(http|https)/', $final_url)) {
      $final_url = url($final_url, ['absolute' => TRUE]);
    }
  }
  return $final_url;
}

/**
 * Adds a new database to the Chado DB table and returns the DB object.
 *
 * @param $values
 *   An associative array of the values of the db (those to be inserted):
 *   - name: The name of the database. This name is usually used as the prefix
 *     for CV term accessions.
 *   - description: (Optional) A description of the database.  By default no
 *     description is required.
 *   - url: (Optional) The URL for the database.
 *   - urlprefix: (Optional) The URL that is to be used as a prefix when
 *     constructing a link to a database term.
 * @param $options
 *   Optional. An associative array of options that can include:
 *   - update_existing: Set this to '1' to force an update of the database if it
 *     already exists. The default is to not update. If the database exists
 *     then nothing is added.
 *
 * @return
 *   An object populated with fields from the newly added database.  If the
 *   database already exists it returns the values in the current entry.
 *
 * @ingroup tripal_chado_database_api
 */
function chado_insert_db($values, $options = []) {

  // Default Values.
  $dbname = $values['name'];
  $description = (isset($values['description'])) ? $values['description'] : '';
  $url = (isset($values['url'])) ? $values['url'] : '';
  $urlprefix = (isset($values['urlprefix'])) ? $values['urlprefix'] : '';
  $update = (isset($options['update_existing'])) ? $options['update_existing'] : TRUE;

  // Build the values array for inserting/updating.
  $ins_values = [
    'name' => $dbname,
    'description' => $description,
    'url' => $url,
    'urlprefix' => $urlprefix,
  ];

  // Get the database record if it already exists.
  $sel_values = ['name' => $dbname];
  $result = chado_select_record('db', ['*'], $sel_values);

  // If it does not exists then add it.
  if (count($result) == 0) {
    $ins_options = ['statement_name' => 'ins_db_nadeurur'];
    $success = chado_insert_record('db', $ins_values, $ins_options);
    if (!$success) {
      tripal_report_error('tripal_chado', TRIPAL_WARNING, "Cannot create db '$dbname'.", NULL);
      return 0;
    }
    $result = chado_select_record('db', ['*'], $sel_values);
  }
  // If it exists and update is enabled the do the update.
  elseif ($update) {
    $upd_options = ['statement_name' => 'upd_db_nadeurur'];
    $success = chado_update_record('db', $sel_values, $ins_values, $upd_options);
    if (!$success) {
      tripal_report_error('tripal_chado', TRIPAL_WARNING, "Cannot update db '$dbname'.", NULL);
      return 0;
    }
    $result = chado_select_record('db', ['*'], $sel_values);
  }

  // Return the database object.
  return $result[0];

}

/**
 * Add a database reference.
 *
 * @param $values
 *   An associative array of the values to be inserted including:
 *    - db_id: the database_id of the database the reference is from.
 *    - accession: the accession.
 *    - version: (Optional) The version of the database reference.
 *    - description: (Optional) A description of the database reference.
 *
 * @return
 *   The newly inserted dbxref as an object, similar to that returned by
 *   the chado_select_record() function.
 *
 * @ingroup tripal_chado_database_api
 */
function chado_insert_dbxref($values) {

  $db_id = $values['db_id'];
  $accession = $values['accession'];
  $version = (isset($values['version'])) ? $values['version'] : '';
  $description = (isset($values['description'])) ? $values['description'] : '';

  $ins_values = [
    'db_id' => $db_id,
    'accession' => $accession,
    'version' => $version,
    'description' => $description,
  ];

  // Check to see if the dbxref exists.
  $sel_values = [
    'db_id' => $db_id,
    'accession' => $accession,
    'version' => $version,
  ];
  $result = chado_select_record('dbxref', ['*'], $sel_values);

  // If it doesn't already exist then add it.
  if (!$result) {
    $success = chado_insert_record('dbxref', $ins_values);
    if (!$success) {
      tripal_report_error('tripal_chado', TRIPAL_WARNING, "Failed to insert the dbxref record $accession", NULL);
      return 0;
    }
    $result = chado_select_record('dbxref', ['*'], $sel_values);
  }

  if (isset($result[0])) {
    return $result[0];
  }
  else {
    return FALSE;
  }
}

/**
 * Add a record to a database reference linking table (ie: feature_dbxref).
 *
 * @param $basetable
 *   The base table for which the dbxref should be associated. Thus to associate
 *   a dbxref with a feature the basetable=feature and dbxref_id is added to the
 *   feature_dbxref table.
 * @param $record_id
 *   The primary key of the basetable to associate the dbxref with. This should
 *   be in integer.
 * @param $dbxref
 *   An associative array describing the dbxref. Valid keys include:
 *   'accession' => the accession for the dbxref, 'db_name' => the name of the
 *    database the dbxref belongs to.
 *   'db_id' => the primary key of the database the dbxref belongs to.
 * @param $options
 *   An associative array of options. Valid keys include:
 *    - insert_dbxref: Insert the dbxref if it doesn't already exist. TRUE is
 *      the default.
 *
 * @ingroup tripal_chado_database_api
 */
function chado_associate_dbxref($basetable, $record_id, $dbxref, $options = []) {
  $linking_table = $basetable . '_dbxref';
  $foreignkey_name = $basetable . '_id';

  // Default Values.
  $options['insert_dbxref'] = (isset($options['insert_dbxref'])) ? $options['insert_dbxref'] : TRUE;

  // If the dbxref_id is set then we know it already exists.
  // Otherwise, select to check.
  if (!isset($dbxref['dbxref_id'])) {
    $values = [
      'accession' => $dbxref['accession'],
    ];
    if (isset($dbxref['db_id'])) {
      $values['db_id'] = $dbxref['db_id'];
    }
    elseif (isset($dbxref['db_name'])) {
      $values['db_id'] = [
        'name' => $dbxref['db_name'],
      ];
    }
    else {
      tripal_report_error(
        'tripal_chado_database_api',
        TRIPAL_WARNING,
        "chado_associate_dbxref: The dbxref needs to have either the db_name or db_id
          supplied. You were trying to associate a dbxref with the %base %record_id
          and supplied the dbxref values: %dbxref.",
        [
          '%base' => $basetable,
          '%record_id' => $record_id,
          '%dbxref' => print_r($dbxref, TRUE),
        ]
      );
      return FALSE;
    }
    $select = chado_select_record('dbxref', ['*'], $values);
    if ($select) {
      $dbxref['dbxref_id'] = $select[0]->dbxref_id;
    }
    elseif ($options['insert_dbxref']) {
      // Insert the dbxref.
      $insert = chado_insert_dbxref($values);
      if (isset($insert->dbxref_id)) {
        $dbxref['dbxref_id'] = $insert->dbxref_id;
      }
      else {
        tripal_report_error(
          'tripal_chado_database_api',
          TRIPAL_WARNING,
          "chado_associate_dbxref: Unable to insert the dbxref using the dbxref values: %dbxref.",
          ['%dbxref' => print_r($dbxref, TRUE)]
        );
        return FALSE;
      }
    }
    else {
      tripal_report_error(
        'tripal_api',
        TRIPAL_WARNING,
        "chado_associate_dbxref: The dbxref doesn't already exist. You supplied the dbxref values: %dbxref.",
        ['%dbxref' => print_r($dbxref, TRUE)]
      );
      return FALSE;
    }
  }

  // Now add the link between the record & dbxref.
  if ($dbxref['dbxref_id'] > 0) {
    $values = [
      'dbxref_id' => $dbxref['dbxref_id'],
      $foreignkey_name => $record_id,
    ];

    $result = chado_select_record($linking_table, ['*'], $values);

    // If it doesn't already exist then add it.
    if (!$result) {
      $success = chado_insert_record($linking_table, $values);
      if (!$success) {
        tripal_report_error(
          'tripal_api',
          TRIPAL_WARNING,
          "Failed to insert the %base record %accession",
          ['%base' => $linking_table, '%accession' => $dbxref['accession']]
        );
        return FALSE;
      }
      $result = chado_select_record($linking_table, ['*'], $values);
    }

    if (isset($result[0])) {
      return $result[0];
    }
    else {
      return FALSE;
    }
  }

  return FALSE;
}

/**
 * This function is intended to be used in autocomplete forms
 * for searching for accession that begin with the provided string.
 *
 * @param $db_id
 *   The DB ID in which to search for the term.
 * @param $string
 *   The string to search for.
 *
 * @return
 *   A json array of terms that begin with the provided string.
 *
 * @ingroup tripal_chado_database_api
 */
function chado_autocomplete_dbxref($db_id, $string = '') {
  if (!$db_id) {
    return drupal_json_output([]);
  }
  $sql = "
    SELECT dbxref_id, accession
    FROM {dbxref}
    WHERE db_id = :db_id and lower(accession) like lower(:accession)
    ORDER by accession
    LIMIT 25 OFFSET 0
  ";
  $results = chado_query($sql, [
    ':db_id' => $db_id,
    ':accession' => $string . '%',
  ]);
  $items = [];
  foreach ($results as $ref) {
    $items[$ref->accession] = $ref->accession;
  }

  drupal_json_output($items);
}
