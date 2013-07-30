<?php

/**
 * @file
 * Controlled Vocabulary API
 *
 * @defgroup tripal_cv_api CV Module API
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
 * Purpose: To retrieve a chado controlled vocabulary object
 *
 * @param $select_values
 *   An array meant to uniquely select a given controlled vocabulary
 *
 * @return
 *   Chado controlled vocabulary object
 *
 * The controlled vocabulary is selected using tripal_core_chado select and as such the
 * $select_values array parameter meant to uniquely identify the controlled vocab to be
 * returned follows the same form as when using tripal_core_chado_select directly.
 *
 * Example Usage:
 * @code
    $select_values = array(
      'name' => 'feature_property'
    );
    $cv_object = tripal_cv_get_cv($select_values);
 * @endcode
 *  The above code selects the feature_property cv and returns the following object:
 * @code
    $cv_object = stdClass Object (
      [cv_id] => 13
      [name] => feature_property
      [definition] =>
    );
 * @endcode
 *
 * @ingroup tripal_cv_api
 */
function tripal_cv_get_cv($select_values) {

  $columns = array(
    'cv_id',
    'name',
    'definition',
  );
  $results = tripal_core_chado_select('cv', $columns, $select_values);
  if (sizeof($results) == 1) {
    return $results[0];
  }
  elseif (empty($results)) {
    watchdog('tripal_cv',
      'tripal_cv_get_cv: No cv matches criteria values:%values',
      array('%values' => print_r($select_values, TRUE)),
      WATCHDOG_WARNING
    );
    return FALSE;
  }
  else {
    watchdog('tripal_cv',
      'tripal_cv_get_cv: 2+ cvs match criteria values:%values',
      array('%values' => print_r($select_values, TRUE)),
      WATCHDOG_WARNING
    );
  }

}

// Purpose: To retrieve a chado cv object
// @param $where_options
//   @code
//        array(
//                          <column_name> => array(
//                            'type' => <type of column: INT/STRING>,
//                            'value' => <the vlaue you want to filter on>,
//                            'exact' => <if TRUE use =; if FALSE use ~>,
//                          )
//        )
// @endcode
//
// @return
//   Chado cv object with all fields from the chado cv table
//
// @ingroup tripal_cv_api
//
//function tripal_cv_get_cv ($where_options)

/**
 * Retrieve a cv given the cv name
 *
 * @param $name
 *  The name of the cv to be returned
 * @return
 *   The cv object for the specified CV name
 *
 * @ingroup tripal_cv_api
 */
function tripal_cv_get_cv_by_name($name) {

  $r = tripal_core_chado_select('cv', array('*'), array('name' => $name));

  return $r[0];
}

/**
 * Retrieve the cv object for the specified CV id
 *
 * NOTE: This function is deprecated.
 * @see tripal_core_chado_generate_vars()
 *
 * @param $cv_id
 *   The unique identifier for the cv to retrieve
 *
 * @return
 *   An object describing the cv
 *
 * @ingroup tripal_cv_api
 */
function tripal_cv_get_cv_by_id($cv_id) {

  $r = tripal_core_chado_select('cv', array('*'), array('cv_id' => $cv_id));

  return $r;
}
/**
 * Retrieve the cv id for the specified CV by name
 *
 * NOTE: This function is deprecated.
 * @see tripal_core_chado_generate_vars()
 *
 * @param $cv_name
 *   The unique name for the cv to retrieve
 *
 * @return
 *   The numeric cv ID
 *
 * @ingroup tripal_cv_api
 */
function tripal_cv_get_cv_id($cv_name) {

  $sql = "
    SELECT cv_id FROM {cv} WHERE name = '%s'
  ";
  $cv = db_fetch_object(chado_query($sql, $cv_name));

  return $cv->cv_id;
}

/**
 * Create an options array to be used in a form element which provides a 
 * list of all chado cvs
 *
 * NOTE: This function is deprecated as of Tripal v1.0
 * 
 * @return
 *   An array(cv_id => name) for each cv in the chado cv table
 *
 * @ingroup tripal_cv_api
 */
function tripal_cv_get_cv_options() {

  $results = tripal_core_chado_select('cv', array('cv_id', 'name'), array());

  $options = array();
  foreach ($results as $r) {
    $options[$r->cv_id] = $r->name;
  }

  return $options;

}
/**
 * Retrieve a chado cvterm object with a given name
 *
 * @param $cvterm_id
 *   the cvterm.cvterm_id
 *
 * @return
 *   cvterm array or FALSE on error
 *
 * @ingroup tripal_cv_api
 */
function tripal_cv_get_cvterm_by_id($cvterm_id) {
  if (!is_numeric($cvterm_id)) {
    return FALSE;
  }
  $values = array('cvterm_id' => $cvterm_id);
  $options = array('statement_name' => 'sel_cvterm_id');
  $r = tripal_core_chado_select('cvterm', array('*'), $values, $options);  
  if (!$r) {
    return FALSE;
  }
  return $r[0];
}
/**
 * Retrieve a chado cvterm object with a given name
 *
 * @param $name
 *   the cvterm.name
 * @param $cv_id
 *   the cv_id of the term you are looking for
 * @param $cv_name
 *   the name of the CV
 *
 * @return
 *   cvterm array or FALSE on error
 *
 * @ingroup tripal_cv_api
 */
function tripal_cv_get_cvterm_by_name($name, $cv_id = NULL, $cv_name = 'tripal') {

  if ($cv_id) {
    $values = array(
       'name' => $name,
       'cv_id' => $cv_id,
    );
    $options = array(
      'statement_name' => 'sel_cvterm_nacv',
      'case_insensitive_columns' => array('name')
    );
    $r = tripal_core_chado_select('cvterm', array('*'), $values, $options);
  }
  elseif ($cv_name) {
    $values = array(
      'name' => $name,
      'cv_id' => array(
        'name' => $cv_name,
      ),
    );    

    $options = array(
      'statement_name' => 'sel_cvterm_nacv',
      'case_insensitive_columns' => array('name')
    );
    $r = tripal_core_chado_select('cvterm', array('*'), $values, $options);      
  }
  else {
    $values = array(
      'name' => $name,
    );
    $options = array(
      'statement_name' => 'sel_cvterm_na',
      'case_insensitive_columns' => array('name')
    );
    $r = tripal_core_chado_select('cvterm', array('*'), $values, $options);
  }

  if (!$r) {
    return FALSE;
  }
  if (count($r) > 1) {
    watchdog('tripal_cv', "Cannot find a unique term for the term '%name' in the vocabulary '%cv'. Multiple entries exist for this name",
      array('%name' => $name, '%cv' => $cv_name ? $cv_name : $cv_id), WATCHDOG_ERROR);
    return FALSE;
  }
  if (count($r) == 0) {
  	return FALSE;
  }
  return $r[0];
}

/**
 * Retrieve a chado cvterm object with a given name
 *
 * @param $synonym
 *   the synonym of the term
 * @param $cv_id
 *   the cv_id of the term you are looking for
 * @param $cv_name
 *   the name of the CV
 *
 * @return
 *   cvterm object
 *
 * @ingroup tripal_cv_api
 */
function tripal_cv_get_cvterm_by_synonym($synonym, $cv_id = NULL, $cv_name = 'tripal') {

  // first find the CVTerm synonym
  $values = array(
     'synonym' => $synonym,
  );
  $statement = "sel_cvtermsynonym_sy";
  if ($cv_id) {
    $values['cvterm_id'] = array('cv_id' => $cv_id);
    $statement = "sel_cvtermsynonym_sycv";    
  }
  if ($cv_name) {
    $values['cvterm_id'] = array('cv_id' => array('name' => $cv_name));
    $statement = "sel_cvtermsynonym_sycv";
  }
  $options = array(
    'statement_name' => $statement,
    'case_insensitive_columns' => array('name')
  );
  $synonym = tripal_core_chado_select('cvtermsynonym', array('cvterm_id'), $values, $options);
  
  // if the synonym doens't exist or more than one record is returned then return false
  if(count($synonym) == 0) {
    return FALSE;
  }
  if(count($synonym) > 1) {
    return FALSE;
  }
  
  // get the cvterm
  $values = array('cvterm_id' => $synonym[0]->cvterm_id);
  $options = array('statement_name' => 'sel_cvterm_id');
  $cvterm = tripal_core_chado_select('cvterm', array('*'), $values, $options);
  return $cvterm[0]; 
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
function tripal_cv_get_cvterm_options($cv_id = 0) {

  if ($cv_id > 0) {
    $results = tripal_core_chado_select('cvterm', array('cvterm_id', 'name'), array('cv_id' => $cv_id));
  }
  else {
    $results = tripal_core_chado_select('cvterm', array('cvterm_id', 'name'), array());
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
function tripal_cv_update_cvtermpath($cvid, $job_id = NULL) {
  // TODO: need better error checking in this function

  // first get the controlled vocabulary name:
  $cv = db_fetch_object(chado_query("SELECT * FROM {cv} WHERE cv_id = %d", $cvid));
  print "\nUpdating cvtermpath for $cv->name...\n";

  // now fill the cvtermpath table
  // @coder-ignore: using a function rather then tablename therefore table prefixing doesn't apply
  $sql = "SELECT * FROM fill_cvtermpath('%s')";
  $success = chado_query($sql, $cv->name); 

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
function tripal_cv_add_cv($name, $definition) {
  
  // insert/update values
  $ins_values = array(
    'name'       => $name,
    'definition' => $definition
  );
  
  // see if the CV (default-namespace) exists already in the database
  $sel_values = array('name' => $name);
  $sel_options = array('statement_name' => 'sel_cv_na');
  $results = tripal_core_chado_select('cv', array('*'), $sel_values, $sel_options);

  // if it does not exists then add it
  if (count($results) == 0) {    
    $ins_options = array('statement_name' => 'ins_cv_nade');
    $success = tripal_core_chado_insert('cv', $ins_values, $ins_options);
    if (!$success) {
      watchdog('tripal_cv', "Failed to create the CV record", NULL, WATCHDOG_WARNING);
      return FALSE;
    }
    $results = tripal_core_chado_select('cv', array('*'), $sel_values, $sel_options);
  }
  // if it already exists then do an update
  else {
    $upd_options = array('statement_name' => 'upd_cv_nade');
    $success = tripal_core_chado_update('cv', $sel_values, $ins_values, $upd_options);
    if (!$success) {
      watchdog('tripal_cv', "Failed to update the CV record", NULL, WATCHDOG_WARNING);
      return FALSE;
    }
    $results = tripal_core_chado_select('cv', array('*'), $sel_values, $sel_options);
  }

  // return the cv object
  return $results[0];
}

/**
 *  Add's a CV term to the cvterm table.  If the parent CV does not exist then
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
 *   An associative array with the following keys: 'id', 'name' and 'namespace',
 *   'is_obsolete', and 'def'.  Where 'id' is the term accession, 'name' is the
 *   term name, 'namespace' is the CV name for the term, 'def' is the term
 *   definition and 'is_obsolete' is present and set to 1 if the term is defunct.
 *   The 'id' must be of the form <DB>:<ACCESSION>, where <DB> is the name of
 *   the database to which the cvterm belongs and the <ACCESSION> is the
 *   term's accession number in the database.
 * @param $defaultcv
 *   Optional. The CV name to which the term
 *   belongs.  If this arugment is null or not provided then the function tries
 *   to find a record in the CV table with the same name provided in the
 *   $term[namespace].  If this field is provided then it overrides what the
 *   value in $term[namespace]
 * @param $is_relationship
 *   If this term is a relationship term then this value should be 1.
 * @param $update
 *   By default this is set to 1.  If the term exists it is automatically updated.
 * @param $dbname
 *   In some cases the database name will not be part of the $term['id'] and it
 *   needs to be explicitly set.  Use this argument only if the database name
 *   cannot be specififed in the term ID (e.g. <DB>:<ACCESSION>).
 *
 * @return
 *   A CVTerm object
 *
 * @ingroup tripal_cv_api
 */
function tripal_cv_add_cvterm($term, $defaultcv = '_global', $is_relationship = 0, 
  $update = 1, $dbname = 'internal') {
      
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
  if (array_key_exists('def', $term)) {
    $definition = preg_replace('/^\"(.*)\"/', '\1', $term['def']);
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
    watchdog('tripal_cv', "Cannot find cvterm without 'id' or 'name'", NULL, WATCHDOG_WARNING);
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
    watchdog('tripal_cv', "A database name is not provided for this relationship term: $id", NULL, WATCHDOG_WARNING);
    return 0; 
  }
  if (!$is_relationship and !$dbname) {
    watchdog('tripal_cv', "A database identifier is missing from the term: $id", NULL, WATCHDOG_WARNING);
    return 0;
  }
  
  // make sure the CV name exists
  $cv = tripal_cv_add_cv($cvname, '');
  if (!$cv) {
    watchdog('tripal_cv', "Cannot find namespace '$cvname' when adding/updating $id", NULL, WATCHDOG_WARNING);
    return 0;
  }

  // this SQL statement will be used a lot to find a cvterm so just set it
  // here for easy reference below.  Because CV terms can change their names
  // but accessions don't change, the following SQL finds cvterms based on
  // their accession rather than the name
  if (!tripal_core_is_sql_prepared('sel_cvterm_by_accession')) {
    $pcvtermsql = "
      PREPARE sel_cvterm_by_accession(text, text) AS
      SELECT CVT.name, CVT.cvterm_id, CV.cv_id, CV.name as cvname, 
        DB.name as dbname, DB.db_id, DBX.accession 
      FROM {cvterm} CVT
        INNER JOIN {dbxref} DBX on CVT.dbxref_id = DBX.dbxref_id
        INNER JOIN {db} DB on DBX.db_id = DB.db_id
        INNER JOIN {cv} CV on CV.cv_id = CVT.cv_id
      WHERE DBX.accession = $1 and DB.name = $2";
    if (!tripal_core_chado_prepare('sel_cvterm_by_accession', $pcvtermsql, array('text', 'text'))) {
      watchdog('tripal_cv', "Cannot prepare statement 'sel_cvterm_by_accession'", NULL, WATCHDOG_WARNING);
      return 0; 
    }
  } 
  $cvtermsql = "EXECUTE sel_cvterm_by_accession('%s','%s')";  
  
  // add the database. The function will just return the DB object if the
  // database already exists.
  $db = tripal_db_add_db($dbname);
  if (!$db) {
    watchdog('tripal_cv', "Cannot find database '$dbname' in Chado.", NULL, WATCHDOG_WARNING);
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
  $result = tripal_core_chado_select('cvterm', array('*'), $values, $options);
  
  // if the constraint is met then let's check it to see if
  // the database name matches the one we have been provided
  if (count($result) == 1) {
    $cvterm = $result[0];
    
    // get the dbxref record
    $values = array('dbxref_id' => $cvterm->dbxref_id);
    $options = array('statement_name' => 'sel_dbxref_id');
    $result = tripal_core_chado_select('dbxref', array('*'), $values, $options);
    $dbxref = $result[0];
    
    // get the db
    $values = array('db_id' => $dbxref->db_id);
    $options = array('statement_name' => 'sel_db_id');
    $result = tripal_core_chado_select('db', array('*'), $values, $options);
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
      $result = tripal_core_chado_select('dbxref', array('*'), $values, $options);

      // if we already have a good dbxref then we want to update our cvterm 
      // to use this dbxref
      if (count($result) > 0) {
        $dbxref = $result[0];
        $match = array('cvterm_id' => $cvterm->cvterm_id);
        $values = array('dbxref_id' => $dbxref->dbxref_id);
        $options = array('statement_name' => 'upd_cvterm_db');
        $success = tripal_core_chado_update('cvterm', $match, $values, $options); 
        if (!$success) {
          watchdog('tripal_cv', "Failed to correct the dbxref id for the cvterm " .
            "'$name' (id: $accession), for database $dbname", NULL, WATCHDOG_WARNING);
          return 0;
        }
      }
      // if we don't have the record then we want to delete our cvterm and let the code
      // below recreate it with the correct info 
      else {          
        $match = array('cvterm_id' => $cvterm->cvterm_id);
        $options = array('statement_name' => 'del_cvterm_cv');
        tripal_core_chado_delete('cvterm', $match, $options);
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
        watchdog('tripal_cv', "Failed to find or insert the dbxref record for cvterm, " .
          "$name (id: $accession), for database $dbname", NULL, WATCHDOG_WARNING);
        return 0;
      }
      
      // check to see if the cvterm_dbxref record already exists
      $values = array(
        'cvterm_id' => $cvterm->cvterm_id,
        'dbxref_id' => $dbxref_new->dbxref_id,
        'is_for_definition' => 1,
      );
      $options = array('statement_name' => 'sel_cvtermdbxref_cvdbis');      
      $result = tripal_core_chado_select('cvterm_dbxref', array('*'), $values, $options);
      
      // if the cvterm_dbxref record does not exists then add it 
      if (count($result)==0) {
        $options = array(
          'statement_name' => 'ins_cvtermdbxref_cvdbis',
          'return_record' => FALSE,
        );
        $success = tripal_core_chado_insert('cvterm_dbxref', $values, $options);
        if (!$success) {
          watchdog('tripal_cv', "Failed to find or insert the cvterm_dbxref record for a " . 
            "duplicated cvterm:  $name (id: $accession), for database $dbname", NULL, WATCHDOG_WARNING);
          return 0;
        }
      }        
      // get the original cvterm with the same name and return that.
      $cvterm = db_fetch_object(chado_query($cvtermsql, $dbxref->accession, $dbname));
      return $cvterm;
    }
    
    // continue on, we've fixed the record if the db_id did not match, 
    // we can now perform and updated if we need to.
  }
  
  // get the CVterm record
  $cvterm = db_fetch_object(chado_query($cvtermsql, $accession, $dbname)); 
  //print "$pcvtermsql\n$cvtermsql\n$accession, $dbname\n";
  //print "CVTERM:\n";
  //print_r($cvterm);
  if (!$cvterm) {

    // check to see if the dbxref exists if not, add it
    $dbxref =  tripal_db_add_dbxref($db->db_id, $accession);
    if (!$dbxref) {
      watchdog('tripal_cv', "Failed to find or insert the dbxref record for cvterm, " .
        "$name (id: $accession), for database $dbname", NULL, WATCHDOG_WARNING);
      return 0;
    }
    
    // check to see if the dbxref already has an entry in the cvterm table
    // this is the second constraint in the cvterm table
    $values = array('dbxref_id' => $dbxref->dbxref_id);
    $options = array('statement_name' => 'sel_cvterm_db');
    $check = tripal_core_chado_select('cvterm', array('cvterm_id'), $values, $options);
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
      $success = tripal_core_chado_insert('cvterm', $ins_values, $ins_options);
      if (!$success) {
        if (!$is_relationship) {
          watchdog('tripal_cv', "Failed to insert the term: $name ($dbname)", NULL, WATCHDOG_WARNING);
          return 0;
        }
        else {
          watchdog('tripal_cv', "Failed to insert the relationship term: $name (cv: " . $cvname . " db: $dbname)", NULL, WATCHDOG_WARNING);
          return 0;
        }
      }
    }
    // this dbxref already exists in the cvterm table 
    else {
      watchdog('tripal_cv', "The dbxref already exists for another cvterm record: $name (cv: " . $cvname . " db: $dbname)", NULL, WATCHDOG_WARNING);
      return 0;
    }    
    $cvterm = db_fetch_object(chado_query($cvtermsql, $accession, $dbname));
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
    $success = tripal_core_chado_update('cvterm', $match, $upd_values, $upd_options);    
    if (!$success) {
      watchdog('tripal_cv', "Failed to update the term: $name", NULL, WATCHDOG_WARNING);
      return 0;
    }
    $cvterm = db_fetch_object(chado_query($cvtermsql, $accession, $dbname));
  } 
  else {
     // do nothing, we have the cvterm but we don't want to update
  }
  // return the cvterm
  return $cvterm;
}


/**
 * This function defines the custom tables that will be created 
 * in the chado schema.
 *
 * @ingroup tripal_cv_api
 */
function tripal_cv_get_custom_tables($table = NULL) {

 if (!$table or strcmp($table, 'tripal_obo_temp')==0) {
    $schema['tripal_obo_temp'] = array(
      'table' => 'tripal_obo_temp',
      'fields' => array(
        'id' => array(
          'type' => 'varchar',
          'length' => '255',
          'not null' => TRUE,
        ),
        'stanza' => array(
          'type' => 'text',
          'not null' => TRUE,
        ),
        'type' => array(
          'type' => 'varchar',
          'length' => '50',
          'not null' => TRUE,
        ),
      ),
      'indexes' => array(
        'tripal_obo_temp_idx0' => array('id'),
        'tripal_obo_temp_idx0' => array('type'),
      ),
      'unique keys' => array(
        'tripal_obo_temp_uq0' => array('id'),
      ),
    );
  }
  return $schema;
}

/**
 * This function allows other modules to programatically
 * submit an ontology for loading into Chado.  This function
 * will add a job to the Jobs subsystem for parsing the ontology.
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

function tripal_cv_submit_obo_job($obo_id = NULL, $obo_name = NULL, $obo_url = NULL, $obo_file = NULL) {
  global $user;

  if ($obo_id) {
    $sql = "SELECT * FROM {tripal_cv_obo} WHERE obo_id = %d";
    $obo = db_fetch_object(db_query($sql, $obo_id));
    
    $args = array($obo_id);
    return tripal_add_job("Load OBO $obo->name", 'tripal_cv',
       "tripal_cv_load_obo_v1_2_id", $args, $user->uid);
  }
  else {
    if ($obo_url) {
      $args = array($obo_name, $obo_url);
      return tripal_add_job("Load OBO $obo_name", 'tripal_cv',
        "tripal_cv_load_obo_v1_2_url", $args, $user->uid);
    }
    elseif ($obo_file) {
      $args = array($obo_name, $obo_file);
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
function tripal_cv_add_obo_ref($name, $path) {
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
function tripal_cv_cvterm_name_autocomplete($cv_id, $string = '') {
  $sql = "
    SELECT cvterm_id, name 
    FROM {cvterm} 
    WHERE cv_id = %d and lower(name) like lower('%s%%') 
    ORDER by name
  ";
  $results = chado_query($sql, $cv_id, $string);
  $items = array();
  while($term = db_fetch_object($results)) {
     $items[$term->name] = $term->name;
  }  
  drupal_json($items);   
}