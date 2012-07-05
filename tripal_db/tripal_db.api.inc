<?php

/**
 * @file
 * Provides an application programming interface (API) to manage references to external databases
 *
 * @defgroup tripal_db_api DB Module API
 * @ingroup tripal_api
 * @ingroup tripal_db
 */

/**
 * Purpose: To retrieve a chado database object
 *
 * @param $select_values
 *   An array meant to uniquely select a given database
 *
 * @return
 *   Chado database object
 *
 * The database is selected using tripal_core_chado select and as such the
 * $select_values array parameter meant to uniquely identify the database to be
 * returned follows the same form as when using tripal_core_chado_select directly.
 *
 * Example Usage:
 * @code
    $select_values = array(
      'name' => 'SOFP'
    );
    $db_object = tripal_db_get_db($select_values);
 * @endcode
 *  The above code selects the SOFP db and returns the following object:
 * @code
    $db_object = stdClass Object (
      [db_id] => 49
      [name] => SOFP
      [description] =>
      [urlprefix] =>
      [url] =>
    );
 * @endcode
 *
 * @ingroup tripal_db_api
 */
function tripal_db_get_db($select_values) {

  $columns = array(
    'db_id',
    'name',
    'description',
    'urlprefix',
    'url'
  );
  $results = tripal_core_chado_select('db', $columns, $select_values);
  if (sizeof($results) == 1) {
    return $results[0];
  }
  elseif (empty($results)) {
    watchdog('tripal_cdb',
      'tripal_db_get_db: No db matches criteria values:%values',
      array('%values' => print_r($select_values, TRUE)),
      WATCHDOG_WARNING
    );
    return FALSE;
  }
  else {
    watchdog('tripal_db',
      'tripal_db_get_db: 2+ dbs match criteria values:%values',
      array('%values' => print_r($select_values, TRUE)),
      WATCHDOG_WARNING
    );
  }

}

/**
 * Purpose: To retrieve a chado db object
 *
 * @param $db_id
 *   db.db_id
 * @return
 *   Chado db object with all fields from the chado db table
 *
 * @ingroup tripal_db_api
 */
function tripal_db_get_db_by_db_id($db_id) {

  $previous_db = tripal_db_set_active('chado');
  $r = db_fetch_object(db_query(
    "SELECT * FROM {db} WHERE db_id=%d", $db_id
  ));
  tripal_db_set_active($previous_db);

  return $r;
}

/**
 * Purpose: To retrieve a chado db object
 *
 * @param $name
 *   db.name
 * @return
 *   chado db object with all fields from the chado db table
 *
 * @ingroup tripal_db_api
 */
function tripal_db_get_db_by_name($name) {

  $previous_db = tripal_db_set_active('chado');
  $r = db_fetch_object(db_query(
    "SELECT * FROM {db} WHERE name='%s'", $name
  ));
  tripal_db_set_active($previous_db);

  return $r;
}

// Purpose: To retrieve a chado db object
//
// @params where_options: array(
//                          <column_name> => array(
//                            'type' => <type of column: INT/**STRING>,
//                            'value' => <the vlaue you want to filter on>,
//                            'exact' => <if TRUE use =; if FALSE use ~>,
//                          )
//        )
// @return chado db object with all fields from the chado db table
//
//function tripal_db_get_db ($where_options) {

/**
 * Purpose: Create an options array to be used in a form element
 *   which provides a list of all chado dbs
 *
 * @return
 *   An array(db_id => name) for each db in the chado db table
 *
 * @ingroup tripal_db_api
 */
function tripal_db_get_db_options() {

  $previous_db = tripal_db_set_active('chado');
  $result = db_query(
    "SELECT db_id, name FROM {db}"
  );
  tripal_db_set_active($previous_db);

  $options = array();
  while ( $r = db_fetch_object($result) ) {
    $options[$r->db_id] = $r->name;
  }

  return $options;

}

// Purpose: To retrieve a chado dbxref object
//
// @param where_options: array(
//                          <column_name> => array(
//                            'type' => <type of column: INT/**STRING>,
//                            'value' => <the vlaue you want to filter on>,
//                            'exact' => <if TRUE use =; if FALSE use ~>,
//                          )
//        )
// @return chado dbxref object with all fields from the chado dbxref table
//
//function tripal_db_get_dbxref ($where_options) {

/**
 * Purpose: To retrieve a chado database reference object
 *
 * @param $select_values
 *   An array meant to uniquely select a given database reference
 *
 * @return
 *   Chado database reference object
 *
 * The database reference is selected using tripal_core_chado select and as such the
 * $select_values array parameter meant to uniquely identify the database reference to be
 * returned follows the same form as when using tripal_core_chado_select directly.
 *
 * Example Usage:
 * @code
    $select_values = array(
      'accession' => 'synonym',
      'db_id' => array(
        'name' => 'SOFP'
      )
    );
    $dbxref_object = tripal_db_get_dbxref($select_values);
 * @endcode
 *  The above code selects the synonym database reference and returns the following object:
 * @code
    $dbxref_object = stdClass Object (
      [dbxref_id] => 2581
      [accession] => synonym
      [description] =>
      [version] =>
      [db_db_id] => 49
      [db_name] => SOFP
      [db_description] =>
      [db_urlprefix] =>
      [db_url] =>
    );
 * @endcode
 *
 * @ingroup tripal_db_api
 */
function tripal_db_get_dbxref($select_values) {

  $columns = array(
    'dbxref_id',
    'db_id',
    'accession',
    'description',
    'version'
  );
  $results = tripal_core_chado_select('dbxref', $columns, $select_values);
  if (sizeof($results) == 1) {
    $dbxref = tripal_db_add_db_to_object(array('db_id' => $results[0]->db_id), $results[0], array());
    unset($dbxref->db_id);

    return $dbxref;
  }
  elseif (empty($results)) {
    watchdog('tripal_db',
      'tripal_db_get_dbxref: No dbxref matches criteria values:%values',
      array('%values' => print_r($select_values, TRUE)),
      WATCHDOG_WARNING
    );
    return FALSE;
  }
  else {
    watchdog('tripal_db',
      'tripal_db_get_dbxref: 2+ dbxrefs match criteria values:%values',
      array('%values' => print_r($select_values, TRUE)),
      WATCHDOG_WARNING
    );
  }

}

/**
 * Purpose: To retrieve a chado dbxref object with a given accession
 *
 * @param $accession
 *   dbxref.accession
 * @param $db_id
 *   dbxref.db_id
 * @return
 *   chado dbxref object with all fields from the chado dbxref table
 *
 * @ingroup tripal_db_api
 */
function tripal_db_get_dbxref_by_accession($accession, $db_id=0) {

  if (!empty($db_id)) {
    $previous_db = tripal_db_set_active('chado');
    $r = db_fetch_object(db_query(
      "SELECT * FROM {dbxref} WHERE accession='%s' AND db_id=%d",
      $accession, $db_id
    ));
    tripal_db_set_active($previous_db);
  }
  else {
    $previous_db = tripal_db_set_active('chado');
    $r = db_fetch_object(db_query(
      "SELECT * FROM {dbxref} WHERE accession='%s'",
      $accession
    ));
    tripal_db_set_active($previous_db);
  }

  return $r;
}

/**
 * Implements hook_chado_dbxref_schema()
 * Purpose: To add descriptions and foreign keys to default table description
 * Note: This array will be merged with the array from all other implementations
 *
 * @return
 *    Array describing the dbxref table
 *
 * @ingroup tripal_schema_api
 */
function tripal_db_chado_dbxref_schema() {
  $description = array();

  $description['foreign keys']['db'] = array(
        'table' => 'db',
        'columns' => array(
          'db_id' => 'db_id',
        ),
  );

  return $description;
}
/**
 * Adds a new database to the Chado DB table and returns the DB object.
 *
 * @param $dbname
 *   The name of the database. This name is usually used as the prefix for
 *   CV term accessions
 * @param $description
 *   Optional. A description of the database.  By default no description is required.
 * @param $url
 *   Optional. The URL for the database
 * @param $urlprefix
 *   Optional. The URL that is to be used as a prefix when constructing a link to
 *   a database term
 * @param $update
 *   Optional. Set this to '1' to force an update of the database if it
 *   already exists. The default is to not update. If the database exists
 *   then nothing is added.
 *
 * @return
 *   An object populated with fields from the newly added database.
 *
 * @ingroup tripal_db_api
 */
function tripal_db_add_db($dbname, $description='', $url='', $urlprefix='', $update=0) {

  $values = array(
    'name' => $dbname,
    'description' => $description,
    'url' => $url,
    'urlprefix' => $urlprefix
  );

  $db_sql = "SELECT * FROM {db} WHERE name ='%s'";
  $db = db_fetch_object(db_query($db_sql, $dbname));
  if (!$db) {
    if (!tripal_core_chado_insert('db', $values)) {
      watchdog('tripal_db', "Cannot create db '$dbname'.", NULL, WATCHDOG_WARNING);
      return 0;
    }
    $db = tripal_core_chado_select('db', array('*'), $values);
  }
  elseif ($update) {
    $match = array('db_id' => $db->db_id);
    if (!tripal_core_chado_update('db', $match, $values)) {
      watchdog('tripal_db', "Cannot update db '$dbname'.", NULL, WATCHDOG_WARNING);
      return 0;
    }
    $db = tripal_core_chado_select('db', array('*'), $values);
  }
  else {
    return $db;
  }
}
/**
 *
 * @ingroup tripal_db_api
 */
function tripal_db_add_dbxref($db_id, $accession, $version='', $description='') {

  // check to see if the dbxref exists if not, add it
  $dbxsql = "
    SELECT DBX.dbxref_id, DBX.db_id, DBX.description, DBX.version, DBX.accession,
       DB.name as db_name
    FROM {dbxref} DBX
       INNER JOIN db DB on DB.db_id = DBX.db_id
    WHERE DBX.db_id = %d and DBX.accession = '%s'
  ";
  $dbxref = db_fetch_object(db_query($dbxsql, $db_id, $accession));
  if (!$dbxref) {
    $sql = "
       INSERT INTO {dbxref} (db_id, accession, version, description)
       VALUES (%d,'%s','%s','%s')
    ";
    if (!db_query($sql, $db_id, $accession, $version, $description)) {
      watchdog('tripal_cv', "Failed to insert the dbxref record $accession", NULL, WATCHDOG_WARNING);
      return 0;
    }
    print "Added Dbxref accession: $accession\n";
    $dbxref = db_fetch_object(db_query($dbxsql, $db_id, $accession));
  }
  return $dbxref;

}
