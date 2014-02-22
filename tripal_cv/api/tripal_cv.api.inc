<?php
/**
 * @file
 * This module provides a set of functions to simplify working with
 * controlled vocabularies.
 */

/**
 * @defgroup tripal_cv_api Controlled Vocabulary API
 * @ingroup tripal_api
 * This module provides a set of functions to simplify working with
 * controlled vocabularies.  Most of the API functions deal with retrieving
 * terms or their parent vocabularies.
 *
 * However, the API also supports
 * generation of trees for browsing a vocabulary as well as generation of
 * pie graphs for display of hierarchical counts of terms.  Version 0.3b of
 * Tripal provides a feature browser and a feature summary chart uses
 * the API functions provided here.  But in general charts and trees can be
 * created for any controlled vocabulary.
 *
 */

/**
 * Retrieves a chado controlled vocabulary variable
 *
 * @param $identifier
 *   An array with the key stating what the identifier is. Supported keys (only on of the
 *   following unique keys is required):
 *    - cv_id: the chado cv.cv_id primary key
 *    - name: the chado cv.name field (assume unique)
 * @param $options
 *   An array of options. Supported keys include:
 *     - Any keys supported by chado_generate_var(). See that function definition for
 *       additional details.
 *
 * NOTE: the $identifier parameter can really be any array similar to $values passed into
 *   chado_select_record(). It should fully specify the cv record to be returned.
 *
 * @return
 *   If unique values were passed in as an identifier then an object describing the cv
 *   will be returned (will be a chado variable from chado_generate_var()). Otherwise,
 *   FALSE will be returned.
 *
 * @ingroup tripal_cv_api
 */
function tripal_get_cv($identifiers, $options = array()) {

  // Set Defaults
  if (!isset($options['include_fk'])) {
    // Tells chado_generate_var not to follow any foreign keys
    $options['include_fk'] = array();
  }

  // Error Checking of parameters
  if (!is_array($identifiers)) {
    tripal_report_error(
      'tripal_cv_api',
      TRIPAL_ERROR,
      "tripal_get_cv: The identifier passed in is expected to be an array with the key
        matching a column name in the cv table (ie: cv_id or name). You passed in %identifier.",
      array(
        '%identifier'=> print_r($identifiers, TRUE)
      )
    );
  }
  elseif (empty($identifiers)) {
    tripal_report_error(
      'tripal_cv_api',
      TRIPAL_ERROR,
      "tripal_get_cv: You did not pass in anything to identify the cv you want. The identifier
        is expected to be an array with the key matching a column name in the cv table
        (ie: cv_id or name). You passed in %identifier.",
      array(
        '%identifier'=> print_r($identifiers, TRUE)
      )
    );
  }

  // Try to get the cv
  $cv = chado_generate_var(
    'cv',
    $identifiers,
    $options
  );

  // Ensure the cv is singular. If it's an array then it is not singular
  if (is_array($cv)) {
    tripal_report_error(
      'tripal_cv_api',
      TRIPAL_ERROR,
      "tripal_get_cv: The identifiers you passed in were not unique. You passed in %identifier.",
      array(
        '%identifier'=> print_r($identifiers, TRUE)
      )
    );
  }

  // Report an error if $cv is FALSE since then chado_generate_var has failed
  elseif ($cv === FALSE) {
    tripal_report_error(
      'tripal_cv_api',
      TRIPAL_ERROR,
      "tripal_get_cv: chado_generate_var() failed to return a cv based on the identifiers
        you passed in. You should check that your identifiers are correct, as well as, look
        for a chado_generate_var error for additional clues. You passed in %identifier.",
      array(
        '%identifier'=> print_r($identifiers, TRUE)
      )
    );
  }

  // Else, as far we know, everything is fine so give them their cv :)
  else {
    return $cv;
  }
}

/**
 * Create an options array to be used in a form element which provides a
 * list of all chado cvs
 *
 * @return
 *   An array(cv_id => name) for each cv in the chado cv table
 *
 * @ingroup tripal_cv_api
 */
function tripal_get_cv_select_options() {

  $results = chado_select_record('cv', array('cv_id', 'name'), array());

  $options = array();
  foreach ($results as $r) {
    $options[$r->cv_id] = $r->name;
  }

  return $options;

}

/**
 * Retrieves a chado controlled vocabulary term variable
 *
 * @param $identifier
 *   An array with the key stating what the identifier is. Supported keys (only on of the
 *   following unique keys is required):
 *    - cvterm_id: the chado cv.cvterm_id primary key
 *    - name: the chado cv.name field (assume unique)
 *   There are also some specially handled keys. They are:
 *    - synonym: an array with 'name' => the name of the synonym of the cvterm you want
 *        returned; 'cv_id' => the cv_id of the synonym; 'cv_name' => the name of the cv
 *        of the synonym
 *    - property: An array/object describing the property to select records for. It
 *      should at least have either a type_name (if unique across cvs) or type_id. Other
 *      supported keys include: cv_id/cv_name (of the type), value and rank
 * @param $options
 *   An array of options. Supported keys include:
 *     - Any keys supported by chado_generate_var(). See that function definition for
 *       additional details.
 *
 * NOTE: the $identifier parameter can really be any array similar to $values passed into
 *   chado_select_record(). It should fully specify the cvterm record to be returned.
 *
 * @return
 *   If unique values were passed in as an identifier then an object describing the cvterm
 *   will be returned (will be a chado variable from chado_generate_var()). Otherwise,
 *   FALSE will be returned.
 *
 * @ingroup tripal_cv_api
 */
function tripal_get_cvterm($identifiers, $options = array()) {

  // Set Defaults
  if (!isset($options['include_fk'])) {
    // Tells chado_generate_var to only get the cv
    $options['include_fk'] = array('cv_id' => TRUE);
  }

  // Error Checking of parameters
  if (!is_array($identifiers)) {
    tripal_report_error(
      'tripal_cv_api',
      TRIPAL_ERROR,
      "tripal_get_cvterm: The identifier passed in is expected to be an array with the key
        matching a column name in the cvterm table (ie: cvterm_id or name). You passed in %identifier.",
      array(
        '%identifier'=> print_r($identifiers, TRUE)
      )
    );
  }
  elseif (empty($identifiers)) {
    tripal_report_error(
      'tripal_cv_api',
      TRIPAL_ERROR,
      "tripal_get_cvterm: You did not pass in anything to identify the cvterm you want. The identifier
        is expected to be an array with the key matching a column name in the cvterm table
        (ie: cvterm_id or name). You passed in %identifier.",
      array(
        '%identifier'=> print_r($identifiers, TRUE)
      )
    );
  }

  // If synonym was passed in, then process this first before calling chado_generate_var()
  if (isset($identifiers['synonym'])) {
    $synonym = $identifiers['synonym']['name'];

    $values = array('synonym' => $synonym);
    if (isset($identifiers['synonym']['cv_id'])) {
      $values['cvterm_id'] = array('cv_id' => $identifiers['synonym']['cv_id']);
    }
    if (isset($identifiers['synonym']['cv_name'])) {
      $values['cvterm_id'] = array('cv_id' => array('name' => $identifiers['synonym']['cv_name']));
    }
    $options = array(
      'case_insensitive_columns' => array('name')
    );
    $result = chado_select_record('cvtermsynonym', array('cvterm_id'), $values, $options);

    // if the synonym doens't exist or more than one record is returned then return false
    if (count($result) == 0) {
      return FALSE;
    }
    if (count($result) > 1) {
      return FALSE;
    }

    $identifiers = array('cvterm_id' => $result[0]->cvterm_id);
  }

  // If one of the identifiers is property then use chado_get_record_with_property()
  if (isset($identifiers['property'])) {
    $property = $identifiers['property'];
    unset($identifiers['property']);
    $cvterm = chado_get_record_with_property('cvterm', $property, $identifiers, $options);
  }

  // Else we have a simple case and we can just use chado_generate_var to get the cvterm
  else {
    // Try to get the cvterm
    $cvterm = chado_generate_var('cvterm', $identifiers, $options);
  }

  // Ensure the cvterm is singular. If it's an array then it is not singular
  if (is_array($cvterm)) {
    tripal_report_error(
      'tripal_cv_api',
      TRIPAL_ERROR,
      "tripal_get_cvterm: The identifiers you passed in were not unique. You passed in %identifier.",
      array(
        '%identifier'=> print_r($identifiers, TRUE)
      )
    );
  }

  // Report an error if $cvterm is FALSE since then chado_generate_var has failed
  elseif ($cvterm === FALSE) {
    tripal_report_error(
      'tripal_cv_api',
      TRIPAL_ERROR,
      "tripal_get_cvterm: chado_generate_var() failed to return a cvterm based on the identifiers
        you passed in. You should check that your identifiers are correct, as well as, look
        for a chado_generate_var error for additional clues. You passed in %identifier.",
      array(
        '%identifier'=> print_r($identifiers, TRUE)
      )
    );
  }

  // Else, as far we know, everything is fine so give them their cvterm :)
  else {
    return $cvterm;
  }

}

/**
 * Create an options array to be used in a form element
 *   which provides a list of all chado cvterms
 *
 * @param $cv_id
 *   The chado cv_id;
 *   only cvterms with the supplied cv_id will be returned
 * @return
 *   An array(cvterm_id => name)
 *   for each cvterm in the chado cvterm table where cv_id=that supplied
 *
 * @ingroup tripal_cv_api
 */
function tripal_get_cvterm_select_options($cv_id = 0) {

  if ($cv_id > 0) {
    $results = chado_select_record('cvterm', array('cvterm_id', 'name'), array('cv_id' => $cv_id));
  }
  else {
    $results = chado_select_record('cvterm', array('cvterm_id', 'name'), array());
  }

  $options = array();
  foreach ($results as $r) {
    $options[$r->cvterm_id] = $r->name;
  }

  return $options;

}

/**
 * Updates the cvtermpath table of Chado for the specified CV.
 *
 * @param $cv_id
 *   The chado cv_id;
 * @param $job_id
 *   This function is intended to be used with the Tripal Jobs API.
 *   When this function is called as a job the $job_id is automatically
 *   passed to this function.
 * @return
 *   TRUE on success FALSE on failure
 *
 * @ingroup tripal_cv_api
 */
function tripal_update_cvtermpath($cvid, $job_id = NULL) {
  // TODO: need better error checking in this function

  // first get the controlled vocabulary name:
  $sql = "SELECT * FROM {cv} WHERE cv_id = :cv_id";
  $cv = chado_query($sql, array(':cv_id' => $cvid))->fetchObject();

  print "\nUpdating cvtermpath for $cv->name...\n";

  $previous = chado_set_active('chado');
  try {
    $sql = "SELECT * FROM fill_cvtermpath(:name)";
    db_query($sql, array(':name' => $cv->name));
    chado_set_active($previous);
  }
  catch (Exception $e) {
    chado_set_active($previous);
    $error = $e->getMessage();
    tripal_report_error('tripal_cv', TRIPAL_ERROR, "Could not fill cvtermpath table: @error", array('@error' => $error));
    return FALSE;
  }

  return TRUE;
}

/**
 * Adds a controlled vocabular to the CV table of Chado.
 *
 * @param $name
 *   The name of the controlled vocabulary. These are typically all lower case
 *   with no special characters other than an undrescore (for spaces).
 * @param $comment
 *   A description or definition of the vocabulary.
 *
 * @return
 *   An object populated with fields from the newly added database.
 *
 * @ingroup tripal_cv_api
 */
function tripal_insert_cv($name, $definition) {

  // insert/update values
  $ins_values = array(
    'name'       => $name,
    'definition' => $definition
  );

  // see if the CV (default-namespace) exists already in the database
  $sel_values = array('name' => $name);
  $sel_options = array('statement_name' => 'sel_cv_na');
  $results = chado_select_record('cv', array('*'), $sel_values, $sel_options);

  // if it does not exists then add it
  if (count($results) == 0) {
    $ins_options = array('statement_name' => 'ins_cv_nade');
    $success = chado_insert_record('cv', $ins_values, $ins_options);
    if (!$success) {
      tripal_report_error('tripal_cv', TRIPAL_WARNING, "Failed to create the CV record", NULL);
      return FALSE;
    }
    $results = chado_select_record('cv', array('*'), $sel_values, $sel_options);
  }
  // if it already exists then do an update
  else {
    $upd_options = array('statement_name' => 'upd_cv_nade');
    $success = chado_update_record('cv', $sel_values, $ins_values, $upd_options);
    if (!$success) {
      tripal_report_error('tripal_cv', TRIPAL_WARNING, "Failed to update the CV record", NULL);
      return FALSE;
    }
    $results = chado_select_record('cv', array('*'), $sel_values, $sel_options);
  }

  // return the cv object
  return $results[0];
}

/**
 *  Add's a controlled vocabulary term to the cvterm table.
 *
 *  If the parent CV does not exist then
 *  that too is added to the CV table.  If the cvterm is a relationship term
 *  then the $is_relationship argument should be set.  The function will try
 *  to first find the relationship in the relationship ontology for updating and
 *  if it can't be found will add the relationship to the __global CV.  All terms
 *  must also have a corresponding database.  This is specified in the term's
 *  ID just before the colon (e.g. GO:003824).  If the database does not exist
 *  in the DB table then it will be added automatically.  The accession (the
 *  value just after the colon in the term's ID) will be added to the dbxref
 *  table.  If the CVterm already exists and $update is set (default) then the
 *  cvterm is updated.  If the CVTerm already exists and $update is not set, then
 *  no changes are made and the CVTerm object is returned.
 *
 * @param $term
 *   An associative array with the following keys:
 *    - id: the term accession. must be of the form <DB>:<ACCESSION>, where <DB> is the
 *        name of the database to which the cvterm belongs and the <ACCESSION> is the
 *        term's accession number in the database.
 *    - name: the name of the term. usually meant to be human-readable.
 *    - namespace: the CV name for the term. DEPRECATED. Please use cv_name instead.
 *    - is_obsolete: is present and set to 1 if the term is defunct
 *    - definition: the definition of the term
 *    - cv_name: The CV name to which the term belongs.  If this arugment is null or not
 *        provided then the function tries to find a record in the CV table with the same
 *        name provided in the $term[namespace].  If this field is provided then it
 *        overrides what the value in $term[namespace]
 *    - is_relationship: If this term is a relationship term then this value should be 1.
 *    _ db_name: In some cases the database name will not be part of the $term['id'] and it
 *        needs to be explicitly set.  Use this argument only if the database name
 *        cannot be specififed in the term ID (e.g. <DB>:<ACCESSION>).
 * @param $options
 *    - update_existing: By default this is TRUE.  If the term exists it is automatically updated.
 *
 * @return
 *   A CVTerm object
 *
 * @ingroup tripal_cv_api
 */
function tripal_insert_cvterm($term, $options) {

  // Set Defaults
  if (isset($term['cv_name'])) {
    $defaultcv = $term['cv_name'];
  }
  else {
    $defaultcv = '_global';
  }

  if (isset($term['is_relationship'])) {
    $is_relationship = $term['is_relationship'];
  }
  else {
    $is_relationship = 0;
  }

  if (isset($term['db_name'])) {
    $dbname = $term['db_name'];
  }
  else {
    $dbname = 'internal';
  }

  if (isset($options['update_existing'])) {
    $update = $options['update_existing'];
  }
  else {
    $update = 1;
  }

  // get the term properties
  $id = $term['id'];
  $name = '';
  $cvname = '';
  $definition = '';
  $is_obsolete = 0;
  $accession = '';

  if (array_key_exists('name', $term)) {
    $name = $term['name'];
  }
  else {
    $name = $id;
  }

  if (array_key_exists('namespace', $term)) {
    $cvname = $term['namespace'];
  }
  else {
    $cvname = $defaultcv;
  }
  if (array_key_exists('definition', $term)) {
    $definition = preg_replace('/^\"(.*)\"/', '\1', $term['definition']);
  }
  else {
    $definition = '';
  }
  if (array_key_exists('is_obsolete', $term)) {
    $is_obsolete = $term['is_obsolete'];
    if (strcmp($is_obsolete, 'true') == 0) {
      $is_obsolete = 1;
    }
  }
  if (!$name and !$id) {
    tripal_report_error('tripal_cv', TRIPAL_WARNING, "Cannot find cvterm without 'id' or 'name'", NULL);
    return 0;
  }
  if (!$id) {
    $id = $name;
  }

  // get the accession and the database from the cvterm id
  if ($dbname) {
    $accession = $id;
  }

  if (preg_match('/^.+?:.*$/', $id)) {
    $accession = preg_replace('/^.+?:(.*)$/', '\1', $id);
    $dbname = preg_replace('/^(.+?):.*$/', '\1', $id);
  }

  // check that we have a database name, give a different message if it's a relationship
  if ($is_relationship and !$dbname) {
    tripal_report_error('tripal_cv', TRIPAL_WARNING, "A database name is not provided for this relationship term: $id", NULL);
    return 0;
  }
  if (!$is_relationship and !$dbname) {
    tripal_report_error('tripal_cv', TRIPAL_WARNING, "A database identifier is missing from the term: $id", NULL);
    return 0;
  }

  // make sure the CV name exists
  $cv = tripal_cv_get_cv_by_name($cvname);
  if (!$cv) {
    $cv = tripal_cv_add_cv($cvname, '');
  }
  if (!$cv) {
    tripal_report_error('tripal_cv', TRIPAL_WARNING, "Cannot find namespace '$cvname' when adding/updating $id", NULL);
    return 0;
  }

  // this SQL statement will be used a lot to find a cvterm so just set it
  // here for easy reference below.  Because CV terms can change their names
  // but accessions don't change, the following SQL finds cvterms based on
  // their accession rather than the name
  $cvtermsql = "
    SELECT CVT.name, CVT.cvterm_id, CV.cv_id, CV.name as cvname,
      DB.name as dbname, DB.db_id, DBX.accession
    FROM {cvterm} CVT
      INNER JOIN {dbxref} DBX on CVT.dbxref_id = DBX.dbxref_id
      INNER JOIN {db} DB on DBX.db_id = DB.db_id
      INNER JOIN {cv} CV on CV.cv_id = CVT.cv_id
    WHERE DBX.accession = :accession and DB.name = :name
  ";

  // add the database. The function will just return the DB object if the
  // database already exists.
  $db = tripal_db_get_db_by_name($dbname);
  if (!$db) {
    $db = tripal_db_add_db($dbname);
  }
  if (!$db) {
    tripal_report_error('tripal_cv', TRIPAL_WARNING, "Cannot find database '$dbname' in Chado.", NULL);
    return 0;
  }

  // the cvterm table has two unique dependencies. We need to check both.
  // first check the (name, cv_id, is_obsolete) constraint
  $values = array(
    'name' => $name,
    'is_obsolete' => $is_obsolete,
    'cv_id' => array(
      'name' => $cvname,
    ),
  );
  $options = array('statement_name' => 'sel_cvterm_c1');
  $result = chado_select_record('cvterm', array('*'), $values, $options);

  // if the constraint is met then let's check it to see if
  // the database name matches the one we have been provided
  if (count($result) == 1) {
    $cvterm = $result[0];

    // get the dbxref record
    $values = array('dbxref_id' => $cvterm->dbxref_id);
    $options = array('statement_name' => 'sel_dbxref_id');
    $result = chado_select_record('dbxref', array('*'), $values, $options);
    $dbxref = $result[0];

    // get the db
    $values = array('db_id' => $dbxref->db_id);
    $options = array('statement_name' => 'sel_db_id');
    $result = chado_select_record('db', array('*'), $values, $options);
    $db_check = $result[0];

    // the database name for this existing term does not match that of the
    // one provided to this function.  The CV name matches otherwise we
    // wouldn't have made it this far. So, let's swap the database for
    // this term
    if ($db_check->name != $db->name) {

      // look to see if the correct dbxref record already exists for this database
      $values = array(
        'db_id' => $db->db_id,
        'accession' => $accession,
      );
      $options = array('statement_name' => 'sel_dbxref_idac');
      $result = chado_select_record('dbxref', array('*'), $values, $options);

      // if we already have a good dbxref then we want to update our cvterm
      // to use this dbxref
      if (count($result) > 0) {
        $dbxref = $result[0];
        $match = array('cvterm_id' => $cvterm->cvterm_id);
        $values = array('dbxref_id' => $dbxref->dbxref_id);
        $options = array('statement_name' => 'upd_cvterm_db');
        $success = chado_update_record('cvterm', $match, $values, $options);
        if (!$success) {
          tripal_report_error('tripal_cv', TRIPAL_WARNING, "Failed to correct the dbxref id for the cvterm " .
            "'$name' (id: $accession), for database $dbname", NULL);
          return 0;
        }
      }
      // if we don't have the record then we want to delete our cvterm and let the code
      // below recreate it with the correct info
      else {
        $match = array('cvterm_id' => $cvterm->cvterm_id);
        $options = array('statement_name' => 'del_cvterm_cv');
        chado_delete_record('cvterm', $match, $options);
      }
    }

    // check that the accession matches.  Sometimes an OBO can define the same term
    // multiple times but with different accessions.  If this is the case we
    // can't do an insert or it will violate the constraint in the cvterm table.
    // so we'll need to add the record to the cvterm_dbxref table instead
    if ($dbxref->accession != $accession) {

      // get/add the dbxref fort his term
      $dbxref_new =  tripal_db_add_dbxref($db->db_id, $accession);
      if (!$dbxref_new) {
        tripal_report_error('tripal_cv', TRIPAL_WARNING, "Failed to find or insert the dbxref record for cvterm, " .
          "$name (id: $accession), for database $dbname", NULL);
        return 0;
      }

      // check to see if the cvterm_dbxref record already exists
      $values = array(
        'cvterm_id' => $cvterm->cvterm_id,
        'dbxref_id' => $dbxref_new->dbxref_id,
        'is_for_definition' => 1,
      );
      $options = array('statement_name' => 'sel_cvtermdbxref_cvdbis');
      $result = chado_select_record('cvterm_dbxref', array('*'), $values, $options);

      // if the cvterm_dbxref record does not exists then add it
      if (count($result)==0) {
        $options = array(
          'statement_name' => 'ins_cvtermdbxref_cvdbis',
          'return_record' => FALSE,
        );
        $success = chado_insert_record('cvterm_dbxref', $values, $options);
        if (!$success) {
          tripal_report_error('tripal_cv', TRIPAL_WARNING, "Failed to find or insert the cvterm_dbxref record for a " .
            "duplicated cvterm:  $name (id: $accession), for database $dbname", NULL);
          return 0;
        }
      }
      // get the original cvterm with the same name and return that.
      $result = chado_query($cvtermsql, array(':accession' => $dbxref->accession, ':name' => $dbname));
      $cvterm = $result->fetchObject();
      return $cvterm;
    }

    // continue on, we've fixed the record if the db_id did not match,
    // we can now perform and updated if we need to.
  }

  // get the CVterm record
  $result = chado_query($cvtermsql, array(':accession' => $accession, ':name' => $dbname));
  $cvterm = $result->fetchObject();
  if (!$cvterm) {

    // check to see if the dbxref exists if not, add it
    $dbxref =  tripal_db_add_dbxref($db->db_id, $accession);
    if (!$dbxref) {
      tripal_report_error('tripal_cv', TRIPAL_WARNING, "Failed to find or insert the dbxref record for cvterm, " .
        "$name (id: $accession), for database $dbname", NULL);
      return 0;
    }

    // check to see if the dbxref already has an entry in the cvterm table
    // this is the second constraint in the cvterm table
    $values = array('dbxref_id' => $dbxref->dbxref_id);
    $options = array('statement_name' => 'sel_cvterm_db');
    $check = chado_select_record('cvterm', array('cvterm_id'), $values, $options);
    if (count($check) == 0) {
      // now add the cvterm
      $ins_values = array(
        'cv_id'                => $cv->cv_id,
        'name'                 => $name,
        'definition'           => $definition,
        'dbxref_id'            => $dbxref->dbxref_id,
        'is_obsolete'          => $is_obsolete,
        'is_relationshiptype'  => $is_relationship,
      );
      $ins_options = array('statement_name' => 'ins_cvterm_all');
      $success = chado_insert_record('cvterm', $ins_values, $ins_options);
      if (!$success) {
        if (!$is_relationship) {
          tripal_report_error('tripal_cv', TRIPAL_WARNING, "Failed to insert the term: $name ($dbname)", NULL);
          return 0;
        }
        else {
          tripal_report_error('tripal_cv', TRIPAL_WARNING, "Failed to insert the relationship term: $name (cv: " . $cvname . " db: $dbname)", NULL);
          return 0;
        }
      }
    }
    // this dbxref already exists in the cvterm table
    else {
      tripal_report_error('tripal_cv', TRIPAL_WARNING, "The dbxref already exists for another cvterm record: $name (cv: " . $cvname . " db: $dbname)", NULL);
      return 0;
    }
    $result = chado_query($cvtermsql, array(':accession' => $accession, ':name' => $dbname));
    $cvterm = $result->fetchObject();
  }
  // upate the cvterm
  elseif ($update) {
    $match = array('cvterm_id' => $cvterm->cvterm_id);
    $upd_values = array(
      'name'                => $name,
      'definition'          => $definition,
      'is_obsolete'         => $is_obsolete,
      'is_relationshiptype' => $is_relationship,
    );
    $upd_options = array('statement_name' => 'upd_cvterm_nadeisis');
    $success = chado_update_record('cvterm', $match, $upd_values, $upd_options);
    if (!$success) {
      tripal_report_error('tripal_cv', TRIPAL_WARNING, "Failed to update the term: $name", NULL);
      return 0;
    }
    $result = chado_query($cvtermsql, array(':accession' => $accession, ':name' => $dbname));
    $cvterm = $result->fetchObject();
  }
  else {
     // do nothing, we have the cvterm but we don't want to update
  }
  // return the cvterm
  return $cvterm;
}

/**
 * This function allows other modules to programatically
 * submit an ontology for loading into Chado.
 *
 * This function will add a job to the Jobs subsystem for parsing the ontology.
 * You can either pass a known OBO ID to the function or the URL
 * or full path the the ontology file.  If a URL or file name is
 * passed then the $obo_name argument must also be provided.  If
 * this is the first time the ontology has been provided to Tripal
 * then it will be added to the database and will be assigned a
 * unique OBO ID.
 *
 * @param $obo_id
 *   If the ontology is already loaded into the Tripal tables then
 *   use this argument to specify the unique ID for the ontology
 *   that will be loaded
 * @param $obo_name
 *   If the OBO has not been added before then use this argument
 *   to specify the human readable name of the ontology.
 * @param $obo_url
 *   If the OBO to be loaded is located on a remote server then
 *   use this argument to provide the URL.
 * @param $obo_file
 *   If the OBO is housed on the local file system of the server then
 *   use this argument to specify the full path.
 *
 * @return
 *   returns the job_id of the submitted job or FALSE if the job was not added
 *
 * @ingroup tripal_cv_api
 */
function tripal_submit_obo_job($obo) {
  global $user;

  // Set Defaults
  $obo['obo_id'] = (isset($obo['obo_id'])) ? $obo['obo_id'] : NULL;
  $obo['name'] = (isset($obo['name'])) ? $obo['name'] : NULL;
  $obo['url'] = (isset($obo['url'])) ? $obo['url'] : NULL;
  $obo['file'] = (isset($obo['file'])) ? $obo['file'] : NULL;

  if ($obo['obo_id']) {
    $sql = "SELECT * FROM {tripal_cv_obo} WHERE obo_id = :obo_id";
    $result = db_query($sql, array(':obo_id' => $obo['obo_id']))->fetchObject();

    $args = array($result->obo_id);
    return tripal_add_job("Load OBO " . $result->name, 'tripal_cv',
       "tripal_cv_load_obo_v1_2_id", $args, $user->uid);
  }
  else {
    if ($obo['url']) {
      $args = array($obo['name'], $obo['url']);
      return tripal_add_job("Load OBO $obo_name", 'tripal_cv',
        "tripal_cv_load_obo_v1_2_url", $args, $user->uid);
    }
    elseif ($obo['file']) {
      $args = array($obo['name'], $obo['file']);
      return tripal_add_job("Load OBO $obo_name", 'tripal_cv',
        "tripal_cv_load_obo_v1_2_file", $args, $user->uid);
    }
  }
  return FALSE;
}

/**
 * Add the obo to the tripal_cv_obo table in the Drupal database
 *
 * @param $name
 * The human readable name of this ontology
 * @param $path
 * The file path or URL of the ontology
 *
 * @return
 * Returns the ontology ID
 *
 * @ingroup tripal_cv_api
 */
function tripal_insert_obo($name, $path) {
  $record = new stdClass;
  $record->name = $name;
  $record->path = $path;
  drupal_write_record('tripal_cv_obo', $record);
  return $record->obo_id;
}

/**
 * This function is intended to be used in autocomplete forms
 * for searching for CV terms that begin with the provided string
 *
 * @param $cv_id
 * The CV ID in which to search for the term
 * @param $string
 * The string to search for
 *
 * @return
 * A json array of terms that begin with the provided string
 *
 * @ingroup tripal_cv_api
 */
function tripal_autocomplete_cvterm($cv_id, $string = '') {
  $sql = "
    SELECT cvterm_id, name
    FROM {cvterm}
    WHERE cv_id = :cv_id and name like :name
    ORDER by name
    LIMIT 25 OFFSET 0
  ";
  $results = chado_query($sql, array(':cv_id' => $cv_id, ':name' => $string . '%'));
  $items = array();
  foreach ($results as $term) {
     $items[$term->name] = $term->name;
  }
  drupal_json_output($items);
}

/**
 * Add a record to a cvterm linking table (ie: feature_cvterm)
 *
 * @param $basetable
 *   The base table to which the cvterm should be linked/associated. Thus to associate a
 *   cvterm to a feature the basetable=feature and cvterm_id is added to the feature_cvterm table.
 * @param $record_id
 *   The primary key of the basetable to associate the cvterm with. This should be in integer.
 * @param $cvterm
 *   An associative array describing the cvterm. Valid keys include: 'name' => the
 *   name for the cvterm, 'cv_name' => the name of the cv the cvterm belongs to;
 *   'cv_id' => the primary key of the cv the cvterm belongs to.
 * @param $options
 *   An associative array of options. Valid keys include:
 *    - insert_cvterm: Insert the cvterm if it doesn't already exist. FALSE is the default
 *
 * @ingroup tripal_db_api
 */
function tripal_associate_cvterm($basetable, $record_id, $cvterm) {
  $linking_table = $basetable . '_cvterm';
  $foreignkey_name = $basetable . '_id';

  // Default Values
  $options['insert_cvterm'] = (isset($options['insert_cvterm'])) ? $options['insert_cvterm'] : FALSE;

  // If the cvterm_id is set then we know it already exists
  // Otherwise, select to check
  if (!isset($cvterm['cvterm_id'])) {
    $values = array(
      'name' => $cvterm['name'],
    );
    if (isset($cvterm['cv_id'])) {
      $values['cv_id'] = $cvterm['cv_id'];
    } elseif (isset($cvterm['cv_name'])) {
      $values['cv_id'] = array(
        'name' => $cvterm['cv_name']
      );
    }
    else {
      tripal_report_error(
        'tripal_cv_api',
        TRIPAL_WARNING,
        "tripal_associate_cvterm: The cvterm needs to have either the cv_name or cv_id
          supplied. You were trying to associate a cvterm with the %base %record_id
          and supplied the cvterm values: %cvterm.",
        array('%base' => $basetable, '%record_id' => $record_id, '%cvterm' => print_r($cvterm,TRUE))
      );
      return FALSE;
    }
    $select = chado_select_record('cvterm',array('*'), $values);
    if ($select) {
      $cvterm['cvterm_id'] = $select[0]->cvterm_id;
    }
    elseif ($options['insert_cvterm']) {
      // Insert the cvterm
      $insert = tripal_insert_cvterm($values);
      if (isset($insert->cvterm_id)) {
        $cvterm['cvterm_id'] = $insert->cvterm_id;
      }
      else {
        tripal_report_error(
          'tripal_cv_api',
          TRIPAL_WARNING,
          "tripal_associate_cvterm: Unable to insert the cvterm using the cvterm values: %cvterm.",
          array('%cvterm' => print_r($cvterm,TRUE))
        );
        return FALSE;
      }
    }
    else {
      tripal_report_error(
        'tripal_api',
        TRIPAL_WARNING,
        "tripal_associate_cvterm: The cvterm doesn't already exist. You supplied the cvterm values: %cvterm.",
        array('%cvterm' => print_r($cvterm,TRUE))
      );
      return FALSE;
    }
  }

  // Now add the link between the record & cvterm
  if ($cvterm['cvterm_id'] > 0) {
    $values = array(
      'cvterm_id' => $cvterm['cvterm_id'],
      $foreignkey_name => $record_id,
      'pub_id' => 1,
    );

    $result = chado_select_record($linking_table, array('*'), $values);

    // if it doesn't already exist then add it
    if (!$result) {
      $success = chado_insert_record($linking_table, $values);
      if (!$success) {
        tripal_report_error(
          'tripal_api',
          TRIPAL_WARNING,
          "Failed to insert the %base record %term",
          array('%base' => $linking_table, '%term' => $cvterm['name'])
        );
        return FALSE;
      }
      $result = chado_select_record($linking_table, array('*'), $values);
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
 */
function tripal_set_default_cv($table, $field, $cv_name) {

  // make sure the cv_id is valid
  $cv = tripal_get_cv(array('name' => $cv_name));

  if ($cv) {
    // first delete any entries for this table and field
    $num_deleted = db_delete('tripal_cv_defaults')
    ->condition('table_name', $table)
    ->condition('field_name', $field)
    ->execute();

    // now add the default value
    $cv_default_id = db_insert('tripal_cv_defaults')
    ->fields(array(
      'table_name' => $table,
      'field_name' => $field,
      'cv_id'      => $cv->cv_id,
    ))
    ->execute();

    if (!$cv_default_id) {
      tripal_report_error('tripal_cv', TRIPAL_WARNING,
      "Cannot set default vocabulary for %table.%field. Check the error logs.",
      array('%table' => $table, '%field' => $field));
      return FALSE;
    }
  }
  else {
    tripal_report_error('tripal_cv', TRIPAL_WARNING,
    "Cannot set default vocabulary for %table.%field. The vocabulary name, '%cvname', doesn't exist.",
    array('%table' => $table, '%field' => $field, '%cvname' => $cv_name));
    return FALSE;
  }
}

/**
 * Retreives the default vocabulary for a given table and field
 * @param $table
 *   The name of the table that contains a field with a foreign key
 *   relationship to the cvterm table
 * @param $field
 *   The table field name that has the foreign key relationship to the
 *   cvterm table for which the default vocabulary will be set
 *
 * @return
 *   The cv array of the default vocabulary or an empty array if not
 *   available.
 */
function tripal_get_default_cv($table, $field) {
  $sql = "
    SELECT cv_id
    FROM {tripal_cv_defaults}
    WHERE table_name = :table and field_name = :field
  ";
  $cv_id = db_query($sql, array(':table' => $table, ':field' => $field))->fetchField();

  return tripal_get_cv(array('cv_id' => $cv_id));
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
 */
function tripal_get_cvterm_default_select_options($table, $field, $field_desc) {
  
  $default_cv = tripal_get_default_cv($table, $field);
  $options = array();
  
  if ($default_cv) {
    $options = tripal_get_cvterm_select_options($default_cv->cv_id);
  }
  else {
    tripal_set_message('There is not a default vocabulary set for ' . $field_desc . '. '. 
      'Please set one using the ' .
      l('vocabulary defaults configuration page',
        'admin/tripal/chado/tripal_cv/defaults',
        array('attributes' => array('target' => '_blank'))) . '.',
      TRIPAL_WARNING);
  }
  if (count($options) == 0) {
    tripal_set_message('There are no ' . $field_desc . '. Please ' .
      l('add terms',
        'admin/tripal/chado/tripal_cv/cv/' .$default_cv->cv_id. '/cvterm/add',
         array('attributes' => array('target' => '_blank'))) . ' to the ' .
      $default_cv->name .' vocabulary.',
      TRIPAL_WARNING);
  }
  return $options;
}