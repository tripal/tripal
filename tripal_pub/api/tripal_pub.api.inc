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
   
  // construct the callback function using the remote database name
  $callback = 'tripal_pub_remote_search_' . strtolower($remote_db);

  // manually set the $_GET['page'] parameter to trick the pager
  // into giving us the requested page
  if (is_int($page) and $page > 0) {
    $_GET['page'] = $page;
  }  
  
  // now call the callback function to get the rsults
  $pubs = array();
  if (function_exists($callback)) {
    $pubs = call_user_func($callback, $search_array, $num_to_retrieve, $pager_id);
  }    
  
  return $pubs;  
}
/*
 * @ingroup tripal_pub_api
 */
function tripal_pub_update_publications($do_contact = FALSE) {
  
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
  
  // get a list of all publications that have 
  // supported databases
  $sql = "
    SELECT DB.name as db_name, DBX.accession
    FROM pub P
      INNER JOIN pub_dbxref PDBX ON P.pub_id = PDBX.pub_id
      INNER JOIN dbxref DBX      ON DBX.dbxref_id = PDBX.dbxref_id
      INNER JOIN db DB           ON DB.db_id = DBX.db_id
    ORDER BY DB.name
  ";
  $results = chado_query($sql);
  
  $num_to_retrieve = 100;
  $i = 0;                 // count the number of IDs. When we hit $num_to_retrieve we'll do the query
  $curr_db = '';          // keeps track of the current current database
  $ids = array();         // the list of IDs for the database
  $search = array();      // the search array passed to the search function
  
  // iterate through the pub IDs
  while ($pub = db_fetch_object($results)) {    
    $accession = $pub->accession;
    $remote_db = $pub->db_name;
    
    // if we're switching databases then reset the search array    
    if($remote_db != $curr_db) {
      // if we had a previous DB then do the update.
      if ($curr_db) {
        $search['num_criteria'] = $i - 1;
        $pubs = tripal_pub_get_remote_search_results($remote_db, $search, $i, 0);
        tripal_pub_add_publications($pubs, $do_contact); 
      }
      $curr_db = $remote_db;      
      $search = array(
        'remote_db' => $remote_db,
        'criteria' => array(),
      );
      $ids = array();
      $i = 0;
    }
    
    // if we've hit the maximum number to retrieve then do the search
    if($i == $num_to_retrieve) {      
      $search['num_criteria'] = $i - 1;
      $pubs = tripal_pub_get_remote_search_results($remote_db, $search, $i, 0);
      tripal_pub_add_publications($pubs, $do_contact);
      $search['criteria'] = array();
      $i = 0;
    }
    
    // add each accession to the search criteria
    $search['criteria'][] = array(
      'search_terms' => $accession,
      'scope' => 'id',
      'operation' => 'OR'
    );
    $i++;   
  }
  // now update any remaining in the search criteria array
  $search['num_criteria'] = $i - 1;
  $pubs = tripal_pub_get_remote_search_results($remote_db, $search, $i, 0);
  tripal_pub_add_publications($pubs, $do_contact);
  
  // sync the newly added publications with Drupal
  print "Syncing publications with Drupal...\n";
  tripal_pub_sync_pubs();
  
  // if the caller wants to create contacts then we should sync them
  if ($do_contact) {
    print "Syncing contacts with Drupal...\n";
    tripal_contact_sync_contacts();
  }
  
  // transaction is complete
  tripal_db_commit_transaction();
  
  print "Done.\n";
}
/*
 * @ingroup tripal_pub_api
 */
function tripal_pub_import_publications() {
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
  $sql = "SELECT * FROM {tripal_pub_import} WHERE disabled = 0";
  $results = db_query($sql);
  $do_contact = FALSE;
  while ($import = db_fetch_object($results)) {
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
  
  // sync the newly added publications with Drupal
  print "Syncing publications with Drupal...\n";
  tripal_pub_sync_pubs();
  
  // if any of the importers wanted to create contacts from the authors then sync them
  if($do_contact) {
    print "Syncing contacts with Drupal...\n";
    tripal_contact_sync_contacts();
  }
  
  // transaction is complete
  tripal_db_commit_transaction();
  
  print "Done.\n";
}
/*
 * 
 */
function tripal_pub_add_publications($pubs, $do_contact) {
 
  // iterate through the publications and add each one
  foreach ($pubs as $pub) {
         
    // add the publication to Chado and sync it with Chado
    $pub_id = tripal_pub_add_publication($pub, $do_contact);
   
    // add the publication cross reference (e.g. to PubMed)
    if ($pub_id) {         
      $pub_dbxref = tripal_pub_add_pub_dbxref($pub_id, $pub);
    }                                      
  
    $num_pubs++;
    print $num_pubs . ".  " . $pub['Publication Database'] . ' ' . $pub['Pub Accession'] . "\n";                          
  } // end for loop        
}
/*
 * 
 */
function tripal_pub_add_pub_dbxref($pub_id, $pub) {
  
  // check to see if the pub_dbxref record already exist
  $values = array(
    'dbxref_id' => array(
      'accession' => $pub['Pub Accession'],
      'db_id' => array(
        'name' => $pub['Publication Database'],
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
  $db = tripal_db_add_db($pub['Publication Database']);
   
  // get the database cross-reference
  $dbxvalues = array(
    'accession' => $pub['Pub Accession'],
    'db_id' => $db->db_id,
  );
  $dbxoptions = array('statement_name' => 'sel_dbxref_acdb');
  $results = tripal_core_chado_select('dbxref', array('dbxref_id'), $dbxvalues, $dbxoptions);
  // if the accession doesn't exist then add it
  if(count($results) == 0){
    $dbxref = tripal_db_add_dbxref($db->db_id, $pub['Pub Accession']);          
  }
  else {
    $dbxref = $results[0];
  }
  
  // now add the record
  $options = array('statement_name' => 'ins_pubdbxref_dbpu');
  $results = tripal_core_chado_insert('pub_dbxref', $values, $options);
  if (!$results) {
    watchdog('tripal_pub', "Cannot add publication dbxref: %db:%accession.",
      array('%db' => $pub['Publication Database'], '%accession' => $pub['Pub Accession']). WATCHDOG_ERROR);
    return FALSE;
  }
  return $results;
}
/*
 * 
 */
function tripal_pub_add_publication($pub_details, $do_contact) {
  
   // check to see if the publication already exists
   $pub_id = 0;
   $values = array();
   $stmnt_suffix = '';
   if ($pub_details['Title']) {
     $values['title'] = $pub_details['Title'];
     $stmnt_suffix .= 'ti';
   }
   if ($pub_details['Year']) {
     $values['pyear'] = $pub_details['Year'];
     $stmnt_suffix .= 'py';
   }
   $options = array('statement_name' => 'sel_pub_');
   $results = tripal_core_chado_select('pub', array('*'), $values, $options);

   if (count($results) == 1) {
     $pub_id = $results[0]->pub_id;
   }
   elseif (count($results) > 1) {
     watchdog('tripal_pub', "The publication with the same title is present multiple times. Cannot ".
       "determine which to use.  Title: %title", array('%title' => $pub_details['Title']), WATCHDOG_ERROR);     
     return FALSE;          
   }
   // add the publication if it doens't exist
   elseif(count($results) == 0) {  
     // get the publication type (use the first publication type, any others will get stored as properties)
     $pub_type = tripal_cv_get_cvterm_by_name($pub_details['Publication Type'][0], NULL, 'tripal_pub');
     if (!$pub_type) {
       watchdog('tripal_pub', "Cannot find publication type: %type", 
         array('%type' => $pub_type), WATCHDOG_ERROR);
       return FALSE;   
     }
     // if the publication does not exist then create it.      
     $values = array(
       'title' => $pub_details['Title'],
       'volume' => $pub_details['Volume'],
       'series_name' => $pub_details['Journal Name'],
       'issue' => $pub_details['Issue'],
       'pyear' => $pub_details['Year'],
       'pages' => $pub_details['Pages'],
       'uniquename' => $pub_details['Citation'],
       'type_id' => $pub_type->cvterm_id,
     );

     $options = array('statement_name' => 'ins_pub_tivoseispypaunty');
     $pub = tripal_core_chado_insert('pub', $values, $options);     
     if (!$pub) {
       watchdog('tripal_pub', "Cannot insert the publication with title: %title", 
         array('%title' => $pub_details['Title']), WATCHDOG_ERROR);
       return FALSE;   
     }
     $pub_id = $pub['pub_id'];
   } 
  
  // now add in any other items that remain as properties of the publication  
  foreach ($pub_details as $key => $value) {
     
    // get the cvterm by name or synonym
     $cvterm = tripal_cv_get_cvterm_by_name($key, NULL, 'tripal_pub');
     if (!$cvterm) {
       $cvterm = tripal_cv_get_cvterm_by_synonym($key, NULL, 'tripal_pub');
     }
     if (!$cvterm) {
       print_r($cvterm);
       watchdog('tripal_pub', "Cannot find term: '%prop'. Skipping.", array('%prop' => $key), WATCHDOG_ERROR);
       continue;
     }

     // skip details that won't be stored as properties
     if ($key == 'Authors') {
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
           $success = tripal_core_insert_property('pub', $pub_id, $key, 'tripal_pub', $subvalue, TRUE);
         }
         else {
           $success = tripal_core_insert_property('pub', $pub_id, $subkey, 'tripal_pub', $subvalue, TRUE);           
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
