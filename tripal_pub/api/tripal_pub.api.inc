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
 *    Optional.  If this function is called on the command-line where the 
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
  if (is_numeric($page) and $page > 0) {
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
  $page = 1;
  $num_pubs = 0;
  
  // get all of the loaders
  $sql = "SELECT * FROM {tripal_pub_import} WHERE disabled = 0";
  $results = db_query($sql);
  while ($import = db_fetch_object($results)) {
     $criteria = unserialize($import->criteria); 
     $remote_db = $criteria['remote_db'];
     print_r($search_array);
     do {       
       // retrieve the pubs for this page
       $pubs = tripal_pub_get_remote_search_results($remote_db, $criteria, $num_to_retrieve, $pager_id, $page);
       // now add the publications
       foreach ($pubs as $pub) {
               
         $p = tripal_pub_add_publication($pub);
         $pub_id = $p->pub_id;                    
         
         // check to see if the pub_dbxref record already exist
         $values = array(
           'dbxref_id' => array(
             'accession' => $pub['pub_accession'],
             'db_id' => array(
                'name' => $pub['pub_database'],
             ),
           ), 
           'pub_id' => $pub_id,
         );
         $options = array('statement_name' => 'sel_pubdbxref_db');
         $results = tripal_core_chado_select('pub_dbxref', array('*'), $values, $options);
         
         // if the pub_dbxref record doesn't exist then we need to add the associate
         if(count($results) == 0) {
           
           // make sure our database already exists
           $db = tripal_db_add_db($pub['pub_database']);
           
           // get the database cross-reference
           $values = array(
             'accession' => $pub['pub_accession'],
             'db_id' => $db->db_id,
           );
           $options = array('statement_name' => 'sel_dbxref_acdb');
           $results = tripal_core_chado_select('dbxref', array('dbxref_id'), $values, $options);
           // if the accession doesn't exist then add it
           if(count($results) == 0){
             $dbxref = tripal_db_add_dbxref($db->db_id, $pub['pub_accession']);          
           }
           else {
             $dbxref = $results[0];
           }
           
         }
         else {
           $pub_dbxref = $results[0];
         }
         

         print $num_pubs . ".  " . $pub['pub_database'] . ' ' . $pub['pub_accession'] . "\n";                 
         $num_pubs++;
       }       
       $page++;
     } 
     while (count($pubs) > 0);    
  }
}
/*
 * 
 */
function tripal_pub_add_publication($pub_details) {
  
   // check to see if the publication already exists
   $pub_id = 0;
   $values = array(
     'title' => $pub['title'],
     'pyear' => $pub['pyear'],
   );
   $options = array('statement_name' => 'pub_tipy');
   $results = tripal_core_chado_select('pub', array('*'), $values, $options);
   
   // if the publication exists then return the record
   if(count($results) == 1) {
      return $results[0];   
   }
   if(count($results) > 1) {
     watchdog('tripal_pub', "The publication with the same title is present multiple times. Cannot ".
       "determine which to use.  Title: %title", array('%title' => $pub_details['title']), WATCHDOG_ERROR);     
     return FALSE;          
   }
  
   // if the publication does not exist then create it.      
   $values = array(
    'title' => $pub_details['title'],
    'volume' => $pub_details['volume'],
    'series_name' => $pub_details['journal_name'],
    'issue' => $pub_details['issue'],
    'pyear' => $pub_details['year'],
    'pages' => $pub_details['pages'],
    'uniquename' => $pub_details['citation'],
    'type_id' => array(
      'name' => 'XXXX',
      'cv_id' => array(
        'name' => 'tripal_pub',
      ),
    ),     
  );  
  $options = array('statment_name' => 'ins_pub_tivoseispypaunty');
  $pub = tripal_core_chado_insert('pub', $values, $options);
  if ($pub) {
    watchdog('tripal_pub', "Cannot insert the publication with title: %title", array('%title' => $pub_details['title']), WATCHDOG_ERROR);
    return FALSE;   
  }
 
  
  // now add in any other items that remain as properties of the publication
  foreach ($pub_details as $key => $value) {
    
  }
  
  return $pub;
}
