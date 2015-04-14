<?php
/**
 * @file
 * The Tripal Pub API
 *
 * @defgroup tripal_pub_api Publication Module API
 * @ingroup tripal_api
 */

/*
 * Retrieves a list of publications as an associated array where
 *  keys correspond directly with Tripal Pub CV terms.
 *
 * @param remote_db
 *    The name of the remote publication database to query. These names should
 *    match the name of the databases in the Chado 'db' table. Currently
 *    supported databass include
 *      'PMID':  PubMed
 *
 * @param search_array
 *    An associate array containing the search criteria. The following key
 *    are expected
 *      'remote_db':     Specifies the name of the remote publication database
 *      'num_criteria':  Specifies the number of criteria present in the search array
 *      'days':          The number of days to include in the search starting from today
 *      'criteria':      An associate array containing the search critiera. There should
 *                       be no less than 'num_criteria' elements in this array.
 *
 *    The following keys are expected in the 'criteria' array
 *      'search_terms':  A list of terms to search on, separated by spaces.
 *      'scope':         The fields to search in the remote database. Valid values
 *                       include: 'title', 'abstract', 'author' and 'any'
 *      'operation':     The logical operation to use for this criteria. Valid
 *                       values include: 'AND', 'OR' and 'NOT'.
 * @param $num_to_retrieve
 *    The number of records to retrieve.  In cases with large numbers of
 *    records to retrieve, the remote database may limit the size of each
 *    retrieval.
 * @param $pager_id
 *    Optional.  This function uses the 'tripal_pager_callback' function
 *    to page a set of results.  This is helpful when generating results to
 *    be view online.  The pager works identical to the pager_query function
 *    of drupal. Simply provide a unique integer value for this argument.  Each
 *    form on a single page should have a unique $pager_id.
 * @param $page
 *    Optional.  If this function is called where the
 *    page for the pager cannot be set using the $_GET variable, use this
 *    argument to specify the page to retrieve.
 *
 * @return
 *   Returns an array of pubs where each element is
 *   an associative array where the keys are Tripal Pub CV terms.
 *
 * @ingroup tripal_pub_api
 */
function tripal_pub_get_remote_search_results($remote_db, $search_array,
$num_to_retrieve, $pager_id = 0, $page = 0) {

  // manually set the $_GET['page'] parameter to trick the pager
  // into giving us the requested page
  if (is_int($page) and $page > 0) {
    $_GET['page'] = $page;
  }
  // now call the callback function to get the results
  $callback = "tripal_pub_remote_search_$remote_db";
  $pubs = array();
  if (function_exists($callback)) {
    $pubs = call_user_func($callback, $search_array, $num_to_retrieve, $pager_id);
  }

  return $pubs;
}
/*
 * @ingroup tripal_pub_api
 */
function tripal_pub_get_raw_data($dbxref) {

  if(preg_match('/^(.*?):(.*?)$/', $dbxref, $matches)) {
    $remote_db = $matches[1];
    $accession = $matches[2];

    // check that the database is supported
    $supported_dbs = variable_get('tripal_pub_supported_dbs', array());
    if(!in_array($remote_db, $supported_dbs)) {
      return "Unsupported database: $dbxref";
    }

    $search = array(
      'num_criteria' => 1,
      'remote_db' => $remote_db,
      'criteria' => array(
        '1' => array(
          'search_terms' => "$remote_db:$accession",
          'scope' => 'id',
          'operation' => '',
          'is_phrase' => 0,
    ),
    ),
    );
    $pubs = tripal_pub_get_remote_search_results($remote_db, $search, 1, 0);

    return '<textarea cols=80 rows=20>' . $pubs[0]['raw'] . '</textarea>';
  }
  return 'Invalid DB xref';
}

/**
 * @ingroup tripal_pub_api
 */
function tripal_pub_update_publications($do_contact = FALSE, $dbxref = NULL, $db = NULL) {

  // get a persistent connection
  $connection = tripal_db_persistent_chado();
  if (!$connection) {
    print "A persistant connection was not obtained. Loading will be slow\n";
  }

  // if we cannot get a connection then let the user know the loading will be slow
  tripal_db_start_transaction();
  if ($connection) {
    print "\nNOTE: Loading of publications is performed using a database transaction. \n" .
           "If the load fails or is terminated prematurely then the entire set of \n" .
           "insertions/updates is rolled back and will not be found in the database\n\n";
  }

  // get a list of all publications by their Dbxrefs that have supported databases
  $sql = "
    SELECT DB.name as db_name, DBX.accession
    FROM pub P
      INNER JOIN pub_dbxref PDBX ON P.pub_id = PDBX.pub_id
      INNER JOIN dbxref DBX      ON DBX.dbxref_id = PDBX.dbxref_id
      INNER JOIN db DB           ON DB.db_id = DBX.db_id
  "; 
  $args = array();
  if ($dbxref and preg_match('/^(.*?):(.*?)$/', $dbxref, $matches)) {
    $dbname = $matches[1];
    $accession = $matches[2];
    $sql .= "WHERE DBX.accession = '%s' and DB.name = '%s' ";
    $args[] = $accession;
    $args[] = $dbname;
  }
  elseif ($db) {
    $sql .= " WHERE DB.name = '%s' ";
    $args[] = $db;
  }
  $sql .= "ORDER BY DB.name, P.pub_id";
  $results = chado_query($sql, $args);

  $num_to_retrieve = 100;
  $i = 0;                 // count the number of IDs. When we hit $num_to_retrieve we'll do the query
  $curr_db = '';          // keeps track of the current current database
  $ids = array();         // the list of IDs for the database
  $search = array();      // the search array passed to the search function

  // iterate through the pub IDs
  while ($pub = db_fetch_object($results)) {
    $accession = $pub->accession;
    $remote_db = $pub->db_name;

    // here we need to only update publications for databases we support
    $supported_dbs = variable_get('tripal_pub_supported_dbs', array());
    if(!in_array($remote_db, $supported_dbs)) {
      continue;
    }
    $search = array(
      'num_criteria' => 1,
      'remote_db' => $remote_db,
      'criteria' => array(
        '1' => array(
          'search_terms' => "$remote_db:$accession",
          'scope' => 'id',
          'operation' => '',
          'is_phrase' => 0,
        ),
      ),
    );
    $pubs = tripal_pub_get_remote_search_results($remote_db, $search, 1, 0);
    tripal_pub_add_publications($pubs, $do_contact, TRUE);

    $i++;
  }

  // transaction is complete
  tripal_db_commit_transaction();

  print "Transaction Complete\n";

  // sync the newly added publications with Drupal
  print "Syncing publications with Drupal...\n";
  tripal_pub_sync_pubs();

  // if the caller wants to create contacts then we should sync them
  if ($do_contact) {
    print "Syncing contacts with Drupal...\n";
    tripal_contact_sync_contacts();
  }

  print "Done.\n";
}
/*
 * @ingroup tripal_pub_api
 */
function tripal_pub_import_publications($report_email = FALSE, $do_update = FALSE) {
  $num_to_retrieve = 100;
  $pager_id = 0;
  $page = 0;
  $num_pubs = 0;

  // get a persistent connection
  $connection = tripal_db_persistent_chado();
  if (!$connection) {
    print "A persistant connection was not obtained. Loading will be slow\n";
  }

  // if we cannot get a connection then let the user know the loading will be slow
  tripal_db_start_transaction();
  if ($connection) {
    print "\nNOTE: Loading of publications is performed using a database transaction. \n" .
           "If the load fails or is terminated prematurely then the entire set of \n" .
           "insertions/updates is rolled back and will not be found in the database\n\n";
  }

  // get all of the loaders
  $args = array();
  $sql = "SELECT * FROM {tripal_pub_import} WHERE disabled = 0 ";
  $results = db_query($sql, $args);
  $do_contact = FALSE;
  $reports = array();
  while ($import = db_fetch_object($results)) {
    $page = 0;
    
    print "Importing: " . $import->name . "\n";
    // keep track if any of the importers want to create contacts from authors
    if ($import->do_contact == 1) {
      $do_contact = TRUE;
    }
    $criteria = unserialize($import->criteria);
    $remote_db = $criteria['remote_db'];
    do {
      // retrieve the pubs for this page. We'll retreive 10 at a time
      $pubs = tripal_pub_get_remote_search_results($remote_db, $criteria, $num_to_retrieve, $pager_id, $page);
      $reports[$import->name] = tripal_pub_add_publications($pubs, $import->do_contact, $do_update);
      $page++;
    }
    // continue looping until we have a $pubs array that does not have
    // our requested numer of records.  This means we've hit the end
    while (count($pubs) == $num_to_retrieve);
  }

  // transaction is complete
  tripal_db_commit_transaction();

  print "Transaction Complete\n";

  // sync the newly added publications with Drupal. If the user
  // requested a report then we don't want to print any syncing information
  // so pass 'FALSE' to the sync call
  print "Syncing publications with Drupal...\n";
  tripal_pub_sync_pubs();

  // iterate through each of the reports and generate a final report with HTML links
  $HTML_report = '';
  if ($report_email) {
    $HTML_report .= "<html>";
    global $base_url;
    foreach ($reports as $importer => $report) {
      $total = count($report['inserted']);
      $HTML_report .= "<b>$total new publications from importer: $importer</b><br><ol>\n";
      foreach ($report['inserted'] as $pub) {
        $item = $pub['Title'];
        if ($pub['pub_id']) {
          $item = l($pub['Title'], "$base_url/pub/" . $pub['pub_id']);
        }
        $HTML_report .= "<li>$item</li>\n";
      }
      $HTML_report .= "</ol>\n";
    }
    $HTML_report .= "</html>";
    $site_email = variable_get('site_mail', '');
    $params = array(
      'message' => $HTML_report
    );
    drupal_mail('tripal_pub', 'import_report', $report_email, language_default(), $params, $site_email, TRUE);
  }

  // if any of the importers wanted to create contacts from the authors then sync them
  if($do_contact) {
    print "Syncing contacts with Drupal...\n";
    tripal_contact_sync_contacts();
  }
   
  print "Done.\n";
}
/*
 * @ingroup tripal_pub_api
 */
function tripal_pub_import_by_dbxref($pub_dbxref, $do_contact = FALSE, $do_update) {
  $num_to_retrieve = 1;
  $pager_id = 0;
  $page = 0;
  $num_pubs = 0;

  // get a persistent connection
  $connection = tripal_db_persistent_chado();
  if (!$connection) {
    print "A persistant connection was not obtained. Loading will be slow\n";
  }

  // if we cannot get a connection then let the user know the loading will be slow
  tripal_db_start_transaction();
  if ($connection) {
    print "\nNOTE: Loading of the publication is performed using a database transaction. \n" .
           "If the load fails or is terminated prematurely then the entire set of \n" .
           "insertions/updates is rolled back and will not be found in the database\n\n";
  }

  if(preg_match('/^(.*?):(.*?)$/', $pub_dbxref, $matches)) {
    $dbname = $matches[1];
    $accession = $matches[2];

    $criteria = array(
      'num_criteria' => 1,
      'remote_db' => $dbname,
      'criteria' => array(
        '1' => array(
          'search_terms' => "$dbname:$accession",
          'scope' => 'id',
          'operation' => '',
          'is_phrase' => 0,
        ),
      ),
    );
    $remote_db = $criteria['remote_db'];
    $pubs = tripal_pub_get_remote_search_results($remote_db, $criteria, $num_to_retrieve, $pager_id, $page);
    $pub_id = tripal_pub_add_publications($pubs, $do_contact, $do_update);

  }

  // transaction is complete
  tripal_db_commit_transaction();

  print "Transaction Complete\n";

  // sync the newly added publications with Drupal
  print "Syncing publications with Drupal...\n";
  tripal_pub_sync_pubs();

  // if any of the importers wanted to create contacts from the authors then sync them
  if($do_contact) {
    print "Syncing contacts with Drupal...\n";
    tripal_contact_sync_contacts();
  }
   
  print "Done.\n";
}
/*
 *
 */
function tripal_pub_add_publications($pubs, $do_contact, $update = FALSE) {
  $report = array();
  $report['error'] = 0;
  $report['inserted'] = array();
  $report['skipped'] = array();
  $total_pubs = count($pubs);

  // iterate through the publications and add each one
  $i = 1;
  foreach ($pubs as $pub) {
    $memory = number_format(memory_get_usage()) . " bytes";
    print "Processing $i of $total_pubs. Memory usage: $memory.\r";

    // add the publication to Chado
    $action = '';
    
    // make sure the publication has a citation before trying to add. The citation
    // becomes the uniquename for the pub.  If it doesn't exist then the importer
    // couldn't create one because the pub type is not handled, so skip it
    if(!$pub['Citation']) {
      $action = 'skipped';
    }
    else {
      $pub_id = tripal_pub_add_publication($pub, $action, $do_contact, $update);
    }
    
    if ($pub_id){  
      // add the publication cross reference (e.g. to PubMed)
      if ($pub['Publication Dbxref']) {
        $pub_dbxref = tripal_pub_add_pub_dbxref($pub_id, $pub['Publication Dbxref']);
      }
      $pub['pub_id'] = $pub_id;
    }

    switch ($action) {
      case 'error':
        $report['error']++;
        break;
      case 'inserted':
        $report['inserted'][] = $pub;
        break;
      case 'updated':
        $report['updated'][] = $pub;
        break;
      case 'skipped':
        $report['skipped'][] = $pub;
        break;
    }
    $i++;
  }
  print "\n";
  return $report;
}
/*
 *
 */
function tripal_pub_add_pub_dbxref($pub_id, $pub_dbxref) {

  // break apart the dbxref
  $dbname = '';
  $accession = '';
  if(preg_match('/^(.*?):(.*?)$/', $pub_dbxref, $matches)) {
    $dbname = $matches[1];
    $accession = $matches[2];
  }
  else {
    return FALSE;
  }

  // check to see if the pub_dbxref record already exist
  $values = array(
    'dbxref_id' => array(
      'accession' => $accession,
      'db_id' => array(
        'name' => $dbname,
  ),
  ),
    'pub_id' => $pub_id,
  );
  $options = array('statement_name' => 'sel_pubdbxref_dbpu');
  $results = tripal_core_chado_select('pub_dbxref', array('*'), $values, $options);

  // if the pub_dbxref record  exist then we don't need to re-add it.
  if(count($results) > 0) {
    return $results[0];
  }

  // make sure our database already exists
  $db = tripal_db_add_db($dbname);
   
  // get the database cross-reference
  $dbxvalues = array(
    'accession' => $accession,
    'db_id' => $db->db_id,
  );
  $dbxoptions = array('statement_name' => 'sel_dbxref_acdb');
  $results = tripal_core_chado_select('dbxref', array('dbxref_id'), $dbxvalues, $dbxoptions);
  // if the accession doesn't exist then add it
  if(count($results) == 0){
    $dbxref = tripal_db_add_dbxref($db->db_id, $accession);
  }
  else {
    $dbxref = $results[0];
  }

  // now add the record
  $options = array('statement_name' => 'ins_pubdbxref_dbpu');
  $results = tripal_core_chado_insert('pub_dbxref', $values, $options);
  if (!$results) {
    watchdog('tripal_pub', "Cannot add publication dbxref: %db:%accession.",
    array('%db' => $dbname, '%accession' => $accession). WATCHDOG_ERROR);
    return FALSE;
  }
  return $results;
}
/**
 * Returns the list of publications that are assigned the database
 * cross-reference provided
 *
 * @param $pub_dbxref
 *   The database cross reference accession.  It should be in the form
 *   DB:ACCESSION, where DB is the database name and ACCESSION is the
 *   unique publication identifier (e.g. PMID:4382934)
 *
 * @return
 *   Returns an array of all the publications that have the provided
 *   cross reference. If no publications match, then an empty array
 *   is returned.
 *
 * @ingroup tripal_pub_api
 *
 */
function tripal_pub_get_pubs_by_dbxref($pub_dbxref) {

  $return = array();

  if(preg_match('/^(.*?):(.*?)$/', $pub_dbxref, $matches)) {
    $dbname = $matches[1];
    $accession = $matches[2];

    $values = array(
      'dbxref_id' => array (
        'accession' => $accession,
        'db_id' => array(
          'name' => $dbname
    ),
    ),
    );
    $options = array('statement_name' => 'sel_pubdbxref_db');
    $results = tripal_core_chado_select('pub_dbxref', array('pub_id'), $values, $options);
    foreach ($results as $index => $pub) {
      $return[] = $pub->pub_id;
    }
  }
  return $return;
}
/**
 * Returns the list of publications that match a given title, type and year
 *
 * @param title
 *   The title of the publication to look for
 * @param type
 *   Optional. The publication type. The value of this field should come from
 *   the Tripal Pub vocabulary. This should be the type name (e.g. cvterm.name)
 * @param year
 *   Optional. The year the publication was published.
 * @param series_name
 *   Optional.  The name of the series (e.g. Journal name)
 *
 * @return
 *   Returns an array of all the publications that have the provided
 *   cross reference. If no publications match, then an empty array
 *   is returned.
 *
 * @ingroup tripal_pub_api
 *
 */
function tripal_pub_get_pubs_by_title_type_pyear_series($title, $type = NULL, $pyear = NULL, $series_name = NULL) {

  $return = array();

  // build the values array for the query.
  $values = array(
    'title' => $title,
  );
  $stmnt_suffix = 'ti';
  if ($type) {
    $values['type_id'] = array(
      'name' => $type,
      'cv_id' => array(
        'name' => 'tripal_pub'
        )
        );
        $stmnt_suffix .= 'ty';
  }
  if ($pyear) {
    $values['pyear'] = $pyear;
    $stmnt_suffix .= 'py';
  }
  if ($series_name) {
    $values['series_name'] = strtolower($series_name);
    $stmnt_suffix .= 'se';
  }
  $options = array(
    'statement_name' => 'sel_pub_' . $stmnt_suffix,
    'case_insensitive_columns' => array('title', 'series_name'),
  );
  $results = tripal_core_chado_select('pub', array('pub_id'), $values, $options);

  // iterate through any matches and pull out the pub_id
  foreach ($results as $index => $pub) {
    $return[] = $pub->pub_id;
  }
  return $return;
}
/**
 * Returns the list of publications that match a given title, type and year
 *
 * @param title
 *   The title of the publication to look for
 * @param type
 *   Optional. The publication type. The value of this field should come from
 *   the Tripal Pub vocabulary. This should be the type name (e.g. cvterm.name)
 * @param year
 *   Optional. The year the publication was published.
 * @param series_name
 *   Optional.  The name of the series (e.g. Journal name)
 *
 * @return
 *   Returns an array of all the publications that have the provided
 *   cross reference. If no publications match, then an empty array
 *   is returned.
 *
 * @ingroup tripal_pub_api
 *
 */
function tripal_pub_get_pub_by_uniquename($name) {

  $return = array();

  // build the values array for the query.
  $values = array(
    'uniquename' => $name,
  );
  $options = array(
    'statement_name' => 'sel_pub_un',
    'case_insensitive_columns' => array('uniquename'),
  );
  $results = tripal_core_chado_select('pub', array('pub_id'), $values, $options);

  // iterate through any matches and pull out the pub_id
  foreach ($results as $index => $pub) {
    $return[] = $pub->pub_id;
  }
  return $return;
}
/**
 * Adds a new publication to the Chado, along with all properties and
 * database cross-references. If the publication does not already exist
 * in Chado then it is added.  If it does exist nothing is done.  If
 * the $update parameter is TRUE then the publication is updated if it exists.
 *
 * @param $pub_details
 *   An associative array containing all of the details about the publication.
 * @param $action
 *   This variable will get set to a text value indicating the action that was
 *   performed. The values include 'skipped', 'inserted', 'updated' or 'error'.
 * @param $do_contact
 *   Optional. Set to TRUE if a contact entry should be added to the Chado contact table
 *   for authors of the publication.
 * @param $update_if_exists
 *   Optional.  If the publication already exists then this function will return
 *   without adding a new publication.  However, set this value to TRUE to force
 *   the function to pudate the publication using the $pub_details that are provided.
 *
 * @return
 *   If the publication already exists, is inserted or updated then the publication
 *   ID is returned, otherwise FALSE is returned. If the publication already exists
 *   and $update_if_exists is not TRUE then the $action variable is set to 'skipped'.
 *   If the publication already exists and $update_if_exists is TRUE and if the update
 *   was successful then $action is set to 'updated'.  Otherwise on successful insert
 *   the $action variable is set to 'inserted'.  If the function failes then the
 *   $action variable is set to 'error'
 *
 */
function tripal_pub_add_publication($pub_details, &$action, $do_contact = FALSE, $update_if_exists = FALSE) {

  $pub_id = 0;

  // first try to find the publication using the accession number. It will have
  // one if the pub has already been loaded for the publication database
  if ($pub_details['Publication Dbxref']) {
    $results = tripal_pub_get_pubs_by_dbxref($pub_details['Publication Dbxref']);
    if(count($results) == 1) {
      $pub_id = $results[0];
      if ($pub_id and !$update_if_exists) {
        watchdog('tripal_pub', "A publication with this Dbxref already exists... Skipping: %dbxref", 
          array('%dbxref' => $pub_details['Publication Dbxref']), WATCHDOG_WARNING);
        $action = 'skipped';
        return $pub_id;
      }
    }
    elseif (count($results) > 1) {
      watchdog('tripal_pub', "There are two publications with this accession: %db:%accession. Cannot determine which to update.",
      array('%db' => $dbname, '%accession' => $accession), WATCHDOG_ERROR);
      $action = 'error';
      return FALSE;
    }
  }

  // if we couldn't find a publication by the accession (which means it doesn't
  // yet exist or it has been added using a different publication database) then
  // try to find it using the title and publication year.
  if (!$pub_id and $pub_details['Title']) {

    $results = tripal_pub_get_pubs_by_title_type_pyear_series($pub_details['Title'], NULL, $pub_details['Year']);
    if (count($results) == 1) {
      $pub_id = $results[0];
      if ($pub_id and !$update_if_exists) {
        watchdog('tripal_pub', "The publication with the same title, type and year already exists. Skipping. ".
          " Title: '%title'. Type: '%type'. Year: '%year'", 
          array('%title' => $pub_details['Title'], '%type' => $pub_details['Publication Type'], '%year' => $pub_details['Year']), WATCHDOG_WARNING);
        $action = 'skipped';
        return $pub_id;
      }
    }
    elseif (count($results) > 1) {
      watchdog('tripal_pub', "The publication with the same title, type and year is present multiple times. Cannot ".
        "determine which to use.  Title: '%title'. Type: '%type'. Year: '%year'", 
      array('%title' => $pub_details['Title'], '%type' => $pub_details['Publication Type'], '%year' => $pub_details['Year']), WATCHDOG_ERROR);
      $action = 'error';
      return FALSE;
    }
  }

  // get the publication type (use the first publication type, any others will get stored as properties)
  if (is_array($pub_details['Publication Type'])) {
    $pub_type = tripal_cv_get_cvterm_by_name($pub_details['Publication Type'][0], NULL, 'tripal_pub');
  }
  elseif ($pub_details['Publication Type']) {
    $pub_type = tripal_cv_get_cvterm_by_name($pub_details['Publication Type'], NULL, 'tripal_pub');
  }
  else {
    watchdog('tripal_pub', "The Publication Type is a required property but is missing", array(), WATCHDOG_ERROR);
    $action = 'error';
    return FALSE;
  }
  if (!$pub_type) {
    watchdog('tripal_pub', "Cannot find publication type: '%type'",
    array('%type' => $pub_details['Publication Type'][0]), WATCHDOG_ERROR);
    $action = 'error';
    return FALSE;
  }

  // build the values array for inserting or updating
  $values = array(
    'title'       => $pub_details['Title'],
    'volume'      => $pub_details['Volume'],
    'series_name' => substr($pub_details['Journal Name'], 0, 255),
    'issue'       => $pub_details['Issue'],
    'pyear'       => $pub_details['Year'],
    'pages'       => $pub_details['Pages'],
    'uniquename'  => $pub_details['Citation'],
    'type_id'     => $pub_type->cvterm_id,
  );

  // if there is no pub_id then we need to do an insert.
  if (!$pub_id) {
    $options = array('statement_name' => 'ins_pub_tivoseispypaunty');
    $pub = tripal_core_chado_insert('pub', $values, $options);
    if (!$pub) {
      watchdog('tripal_pub', "Cannot insert the publication with title: %title",
      array('%title' => $pub_details['Title']), WATCHDOG_ERROR);
      $action = 'error';
      return FALSE;
    }
    $pub_id = $pub['pub_id'];
    $action = 'inserted';
  }

  // if there is a pub_id and we've been told to update, then do the update
  if ($pub_id and $update_if_exists) {
    $match = array('pub_id' => $pub_id);
    $options = array('statement_name' => 'up_pub_tivoseispypaunty');
    $success = tripal_core_chado_update('pub', $match, $values, $options);
    if (!$success) {
      watchdog('tripal_pub', "Cannot update the publication with title: %title",
      array('%title' => $pub_details['Title']), WATCHDOG_ERROR);
      $action = 'error';
      return FALSE;
    }
    $action = 'updated';
  }

  // before we add any new properties we need to remove those that are there if this
  // is an update.  The only thing we don't want to remove are the 'Publication Dbxref'
  if ($update_if_exists) {
    $sql = "
      DELETE FROM {pubprop} 
      WHERE 
        pub_id = %d AND 
        NOT type_id in (
          SELECT cvterm_id
          FROM {cvterm} 
          WHERE name = 'Publication Dbxref' 
        )
    ";
    chado_query($sql, $pub_id);
  }

  // iterate through the properties and add them
  foreach ($pub_details as $key => $value) {
    // the pub_details may have the raw search data (e.g. in XML from PubMed.  We'll irgnore this for now
    if($key == 'raw') {
      continue;
    }
    // get the cvterm by name or synonym
    $cvterm = tripal_cv_get_cvterm_by_name($key, NULL, 'tripal_pub');
    if (!$cvterm) {
      $cvterm = tripal_cv_get_cvterm_by_synonym($key, NULL, 'tripal_pub');
    }
    if (!$cvterm) {
      watchdog('tripal_pub', "Cannot find term: '%prop'. Skipping.", array('%prop' => $key), WATCHDOG_ERROR);
      continue;
    }

    // skip details that won't be stored as properties
    if ($key == 'Author List') {
      tripal_pub_add_authors($pub_id, $value, $do_contact);
      continue;
    }
    if ($key == 'Title' or $key == 'Volume' or $key == 'Journal Name' or $key == 'Issue' or
    $key == 'Year' or $key == 'Pages') {
      continue;
    }
     
    $success = 0;
    if (is_array($value)) {
      foreach ($value as $subkey => $subvalue) {
        // if the key is an integer then this array is a simple list and
        // we will insert using the primary key. Otheriwse, use the new key
        if(is_int($subkey)) {
          $success = tripal_core_insert_property('pub', $pub_id, $key, 'tripal_pub', $subvalue, FALSE);
        }
        else {
          $success = tripal_core_insert_property('pub', $pub_id, $subkey, 'tripal_pub', $subvalue, FALSE);
        }
      }
    }
    else {
      $success = tripal_core_insert_property('pub', $pub_id, $key, 'tripal_pub', $value, TRUE);
    }
    if (!$success) {
      watchdog('tripal_pub', "Cannot add property '%prop' to publication. Skipping.",
      array('%prop' => $key), WATCHDOG_ERROR);
      continue;
    }
  }

  return $pub_id;
}

/*
 *
 */
function tripal_pub_add_authors($pub_id, $authors, $do_contact) {
  $rank = 0;

  // first remove any of the existing pubauthor entires
  $sql = "DELETE FROM {pubauthor} WHERE pub_id = %d";
  chado_query($sql, $pub_id);

  // iterate through the authors and add them to the pubauthors and contact
  // tables of chado, then link them through the custom pubauthors_contact table
  foreach ($authors as $author) {
    // skip invalid author entires
    if ($author['valid'] == 'N') {
      continue;
    }
    // remove the 'valid' property as we don't have a CV term for it
    unset($author['valid']);

    // construct the contact.name field using the author information
    $name = '';
    $type = 'Person';
    if ($author['Given Name']) {
      $name .= $author['Given Name'];
    }
    if ($author['Surname']) {
      $name .= ' ' . $author['Surname'];
    }
    if ($author['Suffix']) {
      $name .= ' ' . $author['Suffix'];
    }
    if ($author['Collective']) {
      $name = $author['Collective'];
      $type = 'Collective';
    }
    $name = trim($name);
     
    // add an entry to the pubauthors table
    $values = array(
      'pub_id' => $pub_id,
      'rank' => $rank,
      'surname' => $author['Surname'] ? substr($author['Surname'], 0, 100) : substr($author['Collective'], 0, 100),
      'givennames' => $author['Given Name'],
      'suffix' => $author['Suffix'],
    );
    $options = array('statement_name' => 'ins_pubauthor_idrasugisu');
    $pubauthor = tripal_core_chado_insert('pubauthor', $values, $options);

    // if the user wants us to create a contact for each author then do it.
    if ($do_contact) {
      // Add the contact
      $contact = tripal_contact_add_contact($name, '', $type, $author);

      // if we have succesfully added the contact and the pubauthor entries then we want to
      // link them together
      if ($contact and $pubauthor) {

        // link the pubauthor entry to the contact
        $values = array(
          'pubauthor_id' => $pubauthor['pubauthor_id'],
          'contact_id' => $contact['contact_id'],
        );
        $options = array('statement_name' => 'ins_pubauthorcontact_puco');
        $pubauthor_contact = tripal_core_chado_insert('pubauthor_contact', $values, $options);
        if (!$pubauthor_contact) {
          watchdog('tripal_pub', "Cannot link pub authro and contact.", array(), WATCHDOG_ERROR);
        }
      }
    }
    $rank++;
  }
}

/**
 * Retrieve properties of a given type for a given pub
 *
 * @param $pub_id
 *    The pub_id of the properties you would like to retrieve
 * @param $property
 *    The cvterm name of the properties to retrieve
 *
 * @return
 *    An pub chado variable with the specified properties expanded
 *
 * @ingroup tripal_pub_api
 */
function tripal_pub_get_property($pub_id, $property) {
  return tripal_core_get_property('pub', $pub_id, $property, 'tripal_pub');
}

/**
 * Insert a given property
 *
 * @param $pub_id
 *   The pub_id of the property to insert
 * @param $property
 *   The cvterm name of the property to insert
 * @param $value
 *   The value of the property to insert
 * @param $update_if_present
 *   A boolean indicated whether to update the record if it's already present
 *
 * @return
 *   True of success, False otherwise
 *
 * @ingroup tripal_pub_api
 */
function tripal_pub_insert_property($pub_id, $property, $value, $update_if_present = 0) {
  return tripal_core_insert_property('pub', $pub_id, $property, 'tripal_pub', $value, $update_if_present);
}

/**
 * Update a given property
 *
 * @param $pub_id
 *   The pub_id of the property to update
 * @param $property
 *   The cvterm name of the property to update
 * @param $value
 *   The value of the property to update
 * @param $insert_if_missing
 *   A boolean indicated whether to insert the record if it's absent
 *
 * Note: The property will be identified using the unique combination of the $pub_id and $property
 * and then it will be updated with the supplied value
 *
 * @return
 *   True of success, False otherwise
 *
 * @ingroup tripal_pub_api
 */
function tripal_pub_update_property($pub_id, $property, $value, $insert_if_missing = 0) {
  return tripal_core_update_property('pub', $pub_id, $property, 'tripal_pub', $value, $insert_if_missing);
}
/**
 * Delete a given property
 *
 * @param $pub_id
 *   The pub_id of the property to delete
 * @param $property
 *   The cvterm name of the property to delete
 *
 * Note: The property will be identified using the unique combination of the $pub_id and $property
 * and then it will be deleted
 *
 * @return
 *   True of success, False otherwise
 *
 * @ingroup tripal_pub_api
 */
function tripal_pub_delete_property($pub_id, $property) {
  return tripal_core_delete_property('pub', $pub_id, $property, 'tripal_pub');
}



/**
 * This function generates an array suitable for use with the 
 * tripal_pub_create_citation function for any publication
 * already stored in the Chado tables.
 *  
 * @param $pub_id
 *   The publication ID
 * @param $skip_existing
 *   Set to TRUE to skip publications that already have a citation
 *   in the pubprop table.  Set to FALSE to generate a citation 
 *   regardless if the citation already exists.
 *   
 * @return
 *   An array suitable for the trpial_pub_create_citation function. On
 *   failure returns FALSE.
 */

function tripal_pub_get_publication_array($pub_id, $skip_existing = TRUE) {

  $options = array('return_array' => 1);

  // ---------------------------------
  // get the publication
  // ---------------------------------
  $values = array('pub_id' => $pub_id);
  $pub = tripal_core_generate_chado_var('pub', $values);

  // expand the title
  $pub = tripal_core_expand_chado_vars($pub, 'field', 'pub.title');
  $pub = tripal_core_expand_chado_vars($pub, 'field', 'pub.volumetitle');
  $pub = tripal_core_expand_chado_vars($pub, 'field', 'pub.uniquename');
  $pub_array = array();
  if (trim($pub->title)) {
    $pub_array['Title'] = $pub->title;
  }
  if (trim($pub->volumetitle)) {
    $pub_array['Volume Title'] = $pub->volumetitle;
  }
  if (trim($pub->volume)) {
    $pub_array['Volume'] = $pub->volume;
  }
  if (trim($pub->series_name)) {
    $pub_array['Series Name'] = $pub->series_name;
  }
  if (trim($pub->issue)) {
    $pub_array['Issue'] = $pub->issue;
  }
  if (trim($pub->pyear)) {
    $pub_array['Year'] = $pub->pyear;
  }
  if (trim($pub->pages)) {
    $pub_array['Pages'] = $pub->pages;
  }
  if (trim($pub->miniref)) {
    $pub_array['Mini Ref'] = $pub->miniref;
  }
  if (trim($pub->uniquename)) {
    $pub_array['Uniquename'] = $pub->uniquename;
  }
  $pub_array['Publication Type'][] = $pub->type_id->name;

  // ---------------------------------
  // get the citation
  // ---------------------------------
  $values = array(
    'pub_id' => $pub->pub_id, 
    'type_id' => array(
      'name' => 'Citation',
    ),
  );
  $citation = tripal_core_generate_chado_var('pubprop', $values);
  if ($citation) {
    $citation = tripal_core_expand_chado_vars($citation, 'field', 'pubprop.value', $options);
    if (count($citation) > 1) {
      watchdog('tripal_pub', "Publication has multiple citations already: %pub_id",
      array('%pub_id' => $pubid), WATCHDOG_ERROR);
      return FALSE;
    }
    elseif (count($citation) == 1 and $skip_existing == TRUE) {
      // skip this publication, it already has a citation
      return FALSE;
    }
  }  

  // ---------------------------------
  // get the publication types
  // ---------------------------------
  $values = array(
    'pub_id' => $pub->pub_id, 
    'type_id' => array(
      'name' => 'Publication Type',
    ),
  );
  $ptypes = tripal_core_generate_chado_var('pubprop', $values, $options);
  if ($ptypes) {
    $ptypes = tripal_core_expand_chado_vars($ptypes, 'field', 'pubprop.value', $options);
    foreach ($ptypes as $ptype) {
     $pub_array['Publication Type'][] = $ptype->value;
    }
  }

  // ---------------------------------
  // get the authors list
  // ---------------------------------
  $values = array(
    'pub_id' => $pub->pub_id, 
    'type_id' => array(
      'name' => 'Authors',
    ),
  );
  $authors = tripal_core_generate_chado_var('pubprop', $values);
  $authors = tripal_core_expand_chado_vars($authors, 'field', 'pubprop.value', $options);
  if (count($authors) > 1) {
   watchdog('tripal_pub', "Publication has multiple author lists. It should have only one list: %pub_id",
   array('%pub_id' => $pubid), WATCHDOG_ERROR);
   return FALSE;
  }
  else if (trim($authors->value)) {
    $pub_array['Authors'] = $authors->value;
  }
  // if there is no 'Author's property then try to retreive authors from the pubauthor table
  else {
    $sql = "SELECT string_agg(surname || ' ' || givennames, ', ') FROM {pubauthor} WHERE pub_id = %d GROUP BY pub_id";
    $au = db_result(chado_query($sql));
    if ($au) {
      $pub_array['Authors'] = $au;
    }
  }

  //Get other props
  $props = array(
    'Journal Abbreviation',
    'Elocation',
    'Media Code',
    'Conference Name',
    'Keywords',
    'Series Name',
    'pISSN',
    'Publication Date',
    'Journal Code',
    'Journal Alias',
    'Journal Country',
    'Published Location',
    'Publication Model',
    'Language Abbr',
    'Alias',
    'Publication Dbxref',
    'Copyright',
    'Abstract',
    'Notes',
    'Citation',
    'Language',
    'URL',
    'eISSN',
    'DOI',
    'ISSN',
    'Publication Code',
    'Comments',
    'Publisher',
    'Media Alias',
    'Original Title');
  foreach ($props AS $prop) {
    $sql = 
      "SELECT value FROM {pubprop} 
       WHERE type_id = (SELECT cvterm_id FROM {cvterm} WHERE name = '%s' AND cv_id = (SELECT cv_id FROM cv WHERE name = 'tripal_pub')) 
       AND pub_id = %d";
    $val = trim(db_result(chado_query($sql, $prop, $pub->pub_id)));
    if ($val) {
      $pub_array[$prop] =$val;
    } 
  }
  return $pub_array;  
}


/**
 * This function generates citations for publications.  It requires
 * an array structure with keys being the terms in the Tripal
 * publication ontology.  This function is intended to be used
 * for any function that needs to generate a citation. 
 * 
 * @param $pub
 *   An array structure containing publication details where the keys
 *   are the publication ontology term names and values are the 
 *   corresponding details.  The pub array can contain the following
 *   keys with corresponding values:   
 *     - Publication Type:  an array of publication types. a publication can have more than one type
 *     - Authors: a  string containing all of the authors of a publication
 *     - Journal Name:  a string containing the journal name
 *     - Journal Abbreviation: a string containing the journal name abbreviation
 *     - Series Name: a string containing the series (e.g. conference proceedings) name
 *     - Series Abbreviation: a string containing the series name abbreviation     
 *     - Volume: the serives volume number
 *     - Issue: the series issue number
 *     - Pages: the page numbers for the publication
 *     - Publication Date:  A date in the format "Year Month Day"
 *      
 * @return
 *   A text string containing the citation
 */
function tripal_pub_create_citation($pub) {
  $citation = '';
  $pub_type = '';
  
  // An article may have more than one publication type. For example,
  // a publication type can be 'Journal Article' but also a 'Clinical Trial'.
  // Therefore, we need to select the type that makes most sense for 
  // construction of the citation. Here we'll iterate through them all
  // and select the one that matches best.
  if(is_array($pub['Publication Type'])) {
    foreach ($pub['Publication Type'] as $ptype) {
      if ($ptype == 'Journal Article' ) {
        $pub_type = $ptype;
        break;
      } 
      else if ($ptype == 'Conference Proceedings'){ 
        $pub_type = $ptype;
        break;
      } 
      else if ($ptype == 'Book') {
        $pub_type = $ptype;
        break;
      } 
      else if ($ptype == 'Letter') {
        $pub_type = $ptype;
        break;
      } 
      else if ($ptype == 'Book Chapter') {
        $pub_type = $ptype;
        break;
      }
      else if ($ptype == "Research Support, Non-U.S. Gov't") {
        $pub_type = $ptype; 
        // we don't break because if the article is also a Journal Article
        // we prefer that type
      }
    }
    if (!$pub_type) {
      watchdog('tripal_pub', "Cannot generate citation for publication type: %types", 
        array('%types' => print_r($pub['Publication Type'], TRUE)), WATCHDOG_WARNING);
      return FALSE;
    }
  }
  else {
    $pub_type = $pub['Publication Type'];
  }    
  //----------------------
  // Journal Article
  //----------------------
  if ($pub_type == 'Journal Article') {
    $citation = $pub['Authors'] . '. ' . $pub['Title'] .  '. ';
  
    if ($pub['Journal Name']) {
      $citation .= $pub['Journal Name'] . '. ';
    }
    elseif ($pub['Journal Abbreviation']) {
      $citation .= $pub['Journal Abbreviation'] . '. ';
    }
    elseif ($pub['Series Name']) {
      $citation .= $pub['Series Name'] . '. ';  
    }
    elseif ($pub['Series Abbreviation']) {
      $citation .= $pub['Series Abbreviation'] . '. ';
    }
    if ($pub['Publication Date']) {
      $citation .= $pub['Publication Date'];
    }
    elseif ($pub['Year']) {
      $citation .= $pub['Year'];
    }
    if ($pub['Volume'] or $pub['Issue'] or $pub['Pages']) {
      $citation .= '; ';
    }
    if ($pub['Volume']) {
      $citation .= $pub['Volume'];
    }
    if ($pub['Issue']) {
      $citation .= '(' . $pub['Issue'] . ')';
    }
    if ($pub['Pages']) {
      if($pub['Volume']) {
        $citation .= ':';
      }
      $citation .= $pub['Pages'];
    }
    $citation .= '.';
  }
  //----------------------
  // Research Support, Non-U.S. Gov't
  //----------------------
  elseif ($pub_type == "Research Support, Non-U.S. Gov't") {
    $citation = $pub['Authors'] . '. ' . $pub['Title'] .  '. ';
  
    if ($pub['Journal Name']) {
      $citation .= $pub['Journal Name'] . '. ';
    }
    if ($pub['Publication Date']) {
      $citation .= $pub['Publication Date'];
    }
    elseif ($pub['Year']) {
      $citation .= $pub['Year'];
    }    
    $citation .= '.';
  }
  //----------------------
  // Letter
  //----------------------
  elseif ($pub_type == 'Letter') {
    $citation = $pub['Authors'] . '. ' . $pub['Title'] .  '. ';
  
    if ($pub['Journal Name']) {
      $citation .= $pub['Journal Name'] . '. ';
    }
    elseif ($pub['Journal Abbreviation']) {
      $citation .= $pub['Journal Abbreviation'] . '. ';
    }
    elseif ($pub['Series Name']) {
      $citation .= $pub['Series Name'] . '. ';  
    }
    elseif ($pub['Series Abbreviation']) {
      $citation .= $pub['Series Abbreviation'] . '. ';
    }
    if ($pub['Publication Date']) {
      $citation .= $pub['Publication Date'];
    }
    elseif ($pub['Year']) {
      $citation .= $pub['Year'];
    }
    if ($pub['Volume'] or $pub['Issue'] or $pub['Pages']) {
      $citation .= '; ';
    }
    if ($pub['Volume']) {
      $citation .= $pub['Volume'];
    }
    if ($pub['Issue']) {
      $citation .= '(' . $pub['Issue'] . ')';
    }
    if ($pub['Pages']) {
      if($pub['Volume']) {
        $citation .= ':';
      }
      $citation .= $pub['Pages'];
    }
    $citation .= '.';
  }
  //----------------------
  // Book
  //----------------------
  elseif ($pub_type == 'Book') {
  
  }
  //----------------------
  // Book Chapter
  //----------------------
  elseif ($pub_type == 'Book Chapter') {
    
  }
  //----------------------
  // Conference Proceedings
  //----------------------
  elseif ($pub_type == 'Conference Proceedings') {
    $citation = $pub['Authors'] . '. ' . $pub['Title'] .  '. ';
  
    if ($pub['Conference Name']) {
      $citation .= $pub['Conference Name'] . '. ';
    }
    elseif ($pub['Series Name']) {
      $citation .= $pub['Series Name'] . '. ';  
    }
    elseif ($pub['Series Abbreviation']) {
      $citation .= $pub['Series Abbreviation'] . '. ';
    }
    if ($pub['Publication Date']) {
      $citation .= $pub['Publication Date'];
    }
    elseif ($pub['Year']) {
      $citation .= $pub['Year'];
    }
    if ($pub['Volume'] or $pub['Issue'] or $pub['Pages']) {
      $citation .= '; ';
    }
    if ($pub['Volume']) {
      $citation .= $pub['Volume'];
    }
    if ($pub['Issue']) {
      $citation .= '(' . $pub['Issue'] . ')';
    }
    if ($pub['Pages']) {
      if($pub['Volume']) {
        $citation .= ':';
      }
      $citation .= $pub['Pages'];
    }
    $citation .= '.';
  }

  return $citation;
}
