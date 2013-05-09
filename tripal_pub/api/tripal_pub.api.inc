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
    // build the search criteria
    $search = array(
      'remote_db' => $remote_db,
      'criteria' => array(
        array(
          'search_terms' => $accession,
          'scope' => 'id',
        ),
      ),
    );  
    $search['num_criteria'] = 0;
    $pubs = tripal_pub_get_remote_search_results($remote_db, $search, 1, 0);
    
    return '<textarea cols=80 rows=20>' . $pubs[0]['raw'] . '</textarea>';
  }  
  return 'Invalid DB xref';    
}
/*
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
function tripal_pub_import_publications($pub_import_id = NULL) {
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
  if ($pub_import_id) {
    $sql .= " AND pub_import_id = %d";
    $args[] = $pub_import_id;
  }
  $results = db_query($sql, $args);
  $do_contact = FALSE;
  while ($import = db_fetch_object($results)) {
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
      tripal_pub_add_publications($pubs, $import->do_contact);             
      $page++;
    } 
    // continue looping until we have a $pubs array that does not have
    // our requested numer of records.  This means we've hit the end
    while (count($pubs) == $num_to_retrieve);    
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
 
  // iterate through the publications and add each one
  foreach ($pubs as $pub) {
         
    // add the publication to Chado and sync it with Chado
    $pub_id = tripal_pub_add_publication($pub, $do_contact, $update);
    if (is_numeric($pub_id)){
      // add the publication cross reference (e.g. to PubMed)
      if ($pub_id and $pub['Publication Dbxref']) {              
        $pub_dbxref = tripal_pub_add_pub_dbxref($pub_id, $pub['Publication Dbxref']);
      }                                        
      $num_pubs++; 
      print "Success: " . $pub['Publication Dbxref'] . "\n";  
    } 
    elseif($pub_id) {
      print "Skipped: " . $pub['Publication Dbxref'] . "\n";  
    }
    else {
      print "Failed: " . $pub['Publication Dbxref'] . "\n";  
    } 
    
  }         
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
 * Returns the list of publications that match a given title and year
 * 
 * @param title
 *   The title of the publication to look for
 * @param type
 *   The publication type. The value of this field should come from 
 *   the Tripal Pub vocabulary
 * @param year
 *   Optional. The year the publication was published.
 * 
 * @return
 *   Returns an array of all the publications that have the provided
 *   cross reference. If no publications match, then an empty array
 *   is returned.
 *   
 * @ingroup tripal_pub_api 
 * 
 */
function tripal_pub_get_pubs_by_title_type_pyear($title, $type, $pyear = '') {
  
  $return = array();
  
  // build the values array for the query.  The $pyear is not required.
  $values = array(
    'title' => $title,
    'type_id' => array(
      'name' => $type,
      'cv_id' => array(
        'name' => 'tripal_pub'
      )
    )
  );
  $stmnt_suffix = 'tity';
  if ($pub_details['Year']) {
    $values['pyear'] = $pyear;
    $stmnt_suffix .= 'py';
  }
  $options = array('statement_name' => 'sel_pub_' . $stmnt_suffix);
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
 * @param $do_contact
 *   Optional. Set to TRUE if a contact entry should be added to the Chado contact table
 *   for authors of the publication.
 * @param $update
 *   Optional.  If the publication already exists then this function will return
 *   without adding a new publication.  However, set this value to TRUE to force
 *   the function to pudate the publication using the $pub_details that are provided.
 *   
 * @return
 *   On successful addition of the publication, the new publication ID is returned. If
 *   the publication already exists but $update is FALSE then TRUE is returned indicating
 *   that the publication is there already.  If $update is TRUE and the publication
 *   exists then the publication ID is returned.
 *   
 */
function tripal_pub_add_publication($pub_details, $do_contact = FALSE, $update = FALSE) {
  
  $pub_id = 0;
  
  // first try to find the publication using the accession number. It will have
  // one if the pub has already been loaded for the publication database
  if ($pub_details['Publication Dbxref']) {
    $results = tripal_pub_get_pubs_by_dbxref($pub_details['Publication Dbxref']);       
    if(count($results) == 1) {
      $pub_id = $results[0];     
    }
    elseif(count($results) > 1) {
      watchdog('tripal_pub', "There are two publications with this accession: %db:%accession. Cannot determine which to update.", 
        array('%db' => $dbname, '%accession' => $accession), WATCHDOG_ERROR);     
      return FALSE;    
    }
    // If we found the publication and we do not want to do the update then
    // return true to indicate the publication has been added
    if (!$update and $pub_id) {
      return TRUE;
    }    
  }   
  // if we couldn't find a publication by the accession (which means it doesn't
  // yet exist or it has been added using a different publication database) then
  // try to find it using the title and publication year.
  elseif ($pub_details['Title']) {
    $results = tripal_pub_get_pubs_by_title_type_pyear($pub_details['Title'], $pub_details['Publication Type'], $pub_details['Year']);    
    if (count($results) == 1) {
      $pub_id = $results[0];
    }
    elseif (count($results) > 1) {
      watchdog('tripal_pub', "The publication with the same title, type and year is present multiple times. Cannot ".
        "determine which to use.  Title: '%title'. Type: '%type'. Year: '%year'", 
        array('%title' => $pub_details['Title'], '%type' => $pub_details['Publication Type'], '%year' => $pub_details['Year']), WATCHDOG_ERROR);     
      return FALSE;          
    }
    // If we found the publication and we do not want to do the update then
    // return true to indicate the publication has been added
    if (!$update and $pub_id) {
      return TRUE;
    }
  }
  
  // get the publication type (use the first publication type, any others will get stored as properties)
  if(is_array($pub_details['Publication Type'])) {
    $pub_type = tripal_cv_get_cvterm_by_name($pub_details['Publication Type'][0], NULL, 'tripal_pub');
  } 
  elseif($pub_details['Publication Type']) {
    $pub_type = tripal_cv_get_cvterm_by_name($pub_details['Publication Type'], NULL, 'tripal_pub');
  }
  else {
    watchdog('tripal_pub', "The Publication Type is a required property but is missing", array(), WATCHDOG_ERROR);
    return FALSE;
  }
  if (!$pub_type) {
    watchdog('tripal_pub', "Cannot find publication type: '%type'", 
      array('%type' => $pub_details['Publication Type'][0]), WATCHDOG_ERROR);
    return FALSE;   
  }
  // if the publication does not exist then create it.      
  $values = array(
    'title'       => $pub_details['Title'],
    'volume'      => $pub_details['Volume'],
    'series_name' => $pub_details['Journal Name'],
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
      return FALSE;   
    }
    $pub_id = $pub['pub_id'];
  }
  elseif($update) {
    $match = array('pub_id' => $pub_id);
    $options = array('statement_name' => 'up_pub_tivoseispypaunty');  
    $success = tripal_core_chado_update('pub', $match, $values, $options);     
    if (!$success) {
      watchdog('tripal_pub', "Cannot update the publication with title: %title", 
        array('%title' => $pub_details['Title']), WATCHDOG_ERROR);
      return FALSE;   
    } 
  }

  
  // before we add any new properties we need to remove those that are there if this 
  // is an update.  The only thing we don't want to remove are the 'Publication Dbxref'
  if($update) {
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
      'surname' => $author['Surname'] ? $author['Surname'] : $author['Collective'],
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
  return tripal_core_get_property('pub', $pub_id, $property, 'tripal');
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
  return tripal_core_delete_property('pub', $pub_id, $property, 'tripal');
}

/*
 * 
 */
function tripal_pub_create_citation($pub) {
  $citation = $pub['Authors'] . '. ' . $pub['Title'] .  '. ';
  
  if ($pub['Journal Name']) {
    $citation .= $pub['Journal Name'] . '. ';
  }
  elseif ($pub['Journal Abbreviation']) {
    $citation .= $pub['Journal Abbreviation'] . '. ';  
  }  
  $citation .= $pub['Publication Date'];
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
  
  return $citation;
}