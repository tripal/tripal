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
 *    The name of the remote publication database to query. Valid values
 *    include: 'pubmed'.
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
function tripal_pub_import_publications() {
  $num_to_retrieve = 10;
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
  while ($import = db_fetch_object($results)) {
     $criteria = unserialize($import->criteria); 
     $remote_db = $criteria['remote_db'];
     do {       
       // get the number of records
       
       // retrieve the pubs for this page. We'll retreive 10 at a time
       $pubs = tripal_pub_get_remote_search_results($remote_db, $criteria, $num_to_retrieve, $pager_id, $page);
       
       // now add the publications
       foreach ($pubs as $pub) {
               
         $pub_id = tripal_pub_add_publication($pub);
         if ($pub_id) {         
           $pub_dbxref = tripal_pub_add_pub_dbxref($pub_id, $pub);
         }                                      

         $num_pubs++;
         print $num_pubs . ".  " . $pub['Publication Database'] . ' ' . $pub['Pub Accession'] . "\n";                          
       } // end for loop       
       $page++;
     } 
     // continue looping until we have a $pubs array that does not have
     // our requested numer of records.  This means we've hit the end
     while (count($pubs) == $num_to_retrieve);    
  }
  
  // transaction is complete
  tripal_db_commit_transaction();
  
  print "Done.\n";
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
function tripal_pub_add_publication($pub_details) {
  
   // check to see if the publication already exists
   $pub_id = 0;
   $values = array(
     'title' => $pub_details['Title'],
     'pyear' => $pub_details['Year'],
   );
   $options = array('statement_name' => 'sel_pub_tipy');
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
       watchdog('tripal_pub', "Cannot find term: '%prop'. Skipping.", 
         array('%prop' => $key), WATCHDOG_ERROR);
       continue;
     }

     // skip details that won't be stored as properties
     if ($key == 'Authors') {
       tripal_pub_add_authors($pub_id, $value);
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
       print_r($success); 
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
function tripal_pub_add_authors($pub_id, $authors) {
  print_r($authors);
}