<?php

/**
 * @defgroup tripal_cv_api CV Module API
 * @ingroup tripal_api
 * @ingroup tripal_cv
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
function tripal_cv_get_cv ($select_values) {

  $columns = array(
    'cv_id', 
    'name', 
    'definition', 
  );
  $results = tripal_core_chado_select('cv', $columns, $select_values);
  if (sizeof($results) == 1) {
    return $results[0];
  } elseif (empty($results)) {
    watchdog('tripal_cv', 
      'tripal_cv_get_cv: No cv matches criteria values:%values',
      array('%values' => print_r($select_values, TRUE)),
      WATCHDOG_WARNING
    );
    return FALSE;
  } else {
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
//													<column_name> => array(
//														'type' => <type of column: INT/STRING>,
//														'value' => <the vlaue you want to filter on>,
//														'exact' => <if TRUE use =; if FALSE use ~>,
//													)
//				)
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
function tripal_cv_get_cv_by_name ($name) {
	$previous_db = tripal_db_set_active('chado');	
   $r = db_fetch_object(db_query("SELECT * FROM cv WHERE name = '%s'",$name));
   tripal_db_set_active($previous_db);

  return $r;
}

/**
 * Retrieve the cv object for the specified CV id
 *
 * NOTE: This function is deprecated.
 * @see tripal_core_chado_generate_vars
 *
 * @param $cv_id
 *   The unique identifier for the cv retrieve
 *
 * @return
 *   An object describing the cv
 *
 * @ingroup tripal_cv_api
 */
function tripal_cv_get_cv_by_id ($cv_id) {
	$previous_db = tripal_db_set_active('chado');	
   $r = db_fetch_object(db_query("SELECT * FROM cv WHERE cv_id = %d",$cv_id));
   tripal_db_set_active($previous_db);

  return $r;
}

/**
 * Create an options array to be used in a form element which provides a list of all chado cvs
 *
 * @return 
 *   An array(cv_id => name) for each cv in the chado cv table
 *
 * @ingroup tripal_cv_api
 */
function tripal_cv_get_cv_options() {

  $previous_db = tripal_db_set_active('chado');
  $result = db_query(
    "SELECT cv_id, name FROM cv"
  );
  tripal_db_set_active($previous_db);

  $options = array();
  while ( $r = db_fetch_object($result) ) {
    $options[$r->cv_id] = $r->name;
  }

  return $options;

}

/**
 * Retrieve a chado cvterm object with a given name
 *
 * @param $name
 *   the cvterm.name
 * @param $cv_id
 *   the cv_id of the term you are looking for
 *
 * @return 
 *   cvterm object
 *
 * @ingroup tripal_cv_api
 */
function tripal_cv_get_cvterm_by_name ($name, $cv_id=0) {

  if (!empty($cv_id)) {
    $sql = "SELECT * FROM cvterm WHERE name='%s' AND cv_id=%d";
    $previous_db = tripal_db_set_active('chado');
    $r = db_fetch_object(db_query($sql, $name, $cv_id));
    tripal_db_set_active($previous_db);
  } else {
    $sql = "SELECT * FROM cvterm WHERE name='%s'";
    $previous_db = tripal_db_set_active('chado');
    $r = db_fetch_object(db_query($sql, $name));
    tripal_db_set_active($previous_db);
  }
  
  return $r;
  
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

  $previous_db = tripal_db_set_active('chado');
  if ($cv_id > 0) {
  	$result = db_query(
    	"SELECT cvterm_id, name FROM cvterm WHERE cv_id=%d", $cv_id
  	);
  } else {
  	$result = db_query(
    	"SELECT cvterm_id, name FROM cvterm"
  	);
  }
  tripal_db_set_active($previous_db);

  $options = array();
  while ( $r = db_fetch_object($result) ) {
    $options[$r->cvterm_id] = $r->name;
  }

  return $options;

}

/**
 * Implements hook_chado_cvterm_schema()
 * Purpose: To add descriptions and foreign keys to default table description
 * Note: This array will be merged with the array from all other implementations
 *
 * @return
 *    Array describing the cvterm table
 *
 * @ingroup tripal_schema_api 
 */
function tripal_cv_chado_cvterm_schema() {
  $description = array();

  $description['foreign keys']['cv'] = array(
        'table' => 'cv',
        'columns' => array(
          'cv_id' => 'cv_id',
        ),
  );
      
  $description['foreign keys']['dbxref'] = array(
        'table' => 'dbxref',
        'columns' => array(
          'dbxref_id' => 'dbxref_id',
        ),
  ); 

  return $description;
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
function tripal_cv_add_cv($name,$comment){

 // see if the CV (default-namespace) exists already in the database
   $vocab = $name;
   $remark = $comment;
   $cv_sql = "SELECT * FROM {cv} WHERE name = '%s'";
   $cv = db_fetch_object(db_query($cv_sql,$vocab));

   // if the CV exists then update it, otherwise insert
   if(!$cv){
      $sql = "INSERT INTO {cv} (name,definition) VALUES ('%s','%s')";
      if(!db_query($sql,$vocab,$remark)){
         watchdog('tripal_cv', "Failed to create the CV record",NULL,WATCHDOG_WARNING);
         return 0;
      }
      $cv = db_fetch_object(db_query($cv_sql,$vocab));
   } else {
      $sql = "UPDATE {cv} SET definition = '%s' WHERE name ='%s'";
      if(!db_query($sql,$remark,$vocab)){
         watchdog('tripal_cv', "Failed to update the CV record",NULL,WATCHDOG_WARNING);
         return 0;
      }
      $cv = db_fetch_object(db_query($cv_sql,$vocab));
   }
   return $cv;
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
*   Optional. A CV object populated with the fields of the CV to which the term 
*   belongs.  If this arugment is null or not provided then the function tries
*   to find a record in the CV table with the same name provided in the 
*   $term[namespace]. 
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
function tripal_cv_add_cvterm($term,$defaultcv=NULL,$is_relationship = 0,$update = 1,$dbname=NULL){

   // get the term properties
   $id = $term['id'];
   $name = $term['name'];
   $cvname = $term['namespace'];
   $definition = preg_replace('/^\"(.*)\"/','\1',$term['def']);
   $is_obsolete = 0;
   if(isset($term['is_obsolete']) and  strcmp($term['is_obsolete'],'true')==0){
     $is_obsolete = 1;
   }
   if(!$cvname){
      $cvname = $defaultcv->name;
   }
   // make sure the CV name exists
   $cv = tripal_cv_add_cv($cvname,'');
   if(!$cv){
      watchdog('tripal_cv', "Cannot find namespace '$cvname' when adding/updating $id",NULL,WATCHDOG_WARNING);
      return 0;
   }

   // this SQL statement will be used a lot to find a cvterm so just set it
   // here for easy reference below.  Because CV terms can change their names
   // but accessions don't change, the following SQL finds cvterms based on
   // their accession rather than the name
   $cvtermsql = "SELECT CVT.name, CVT.cvterm_id, DB.name as dbname, DB.db_id 
                  FROM {cvterm} CVT
                    INNER JOIN {dbxref} DBX on CVT.dbxref_id = DBX.dbxref_id
                    INNER JOIN {db} DB on DBX.db_id = DB.db_id
                    INNER JOIN {cv} CV on CV.cv_id = CVT.cv_id
                  WHERE DBX.accession = '%s' and DB.name = '%s'";  

   // get the accession and the database from the cvterm
   if($dbname){
      $accession = $id;
   }
   elseif(preg_match('/^.+?:.*$/',$id)){
      $accession = preg_replace('/^.+?:(.*)$/','\1',$id);
      $dbname = preg_replace('/^(.+?):.*$/','\1',$id);
   } 
   if($is_relationship and !$dbname){
      $accession = $id;
      // because this is a relationship cvterm first check to see if it 
      // exists in the relationship ontology. If it does then return the cvterm.
      //  If not then set the dbname to _global and we'll add it or find it there
      $cvterm = db_fetch_object(db_query($cvtermsql,$accession,'OBO_REL'));
      if($cvterm){
         return $cvterm;
      } else {
         // next check if this term is in the _global ontology.  If it is then
         // return it no matter what the original CV
         $dbname = '_global';

         $cvterm = db_fetch_object(db_query($cvtermsql,$accesion,$dbname));
         if($cvterm){
            return $cvterm;
         }
      }
   }
   if(!$is_relationship and !$dbname){
      watchdog('tripal_cv', "A database identifier is missing from the term: $id",NULL,WATCHDOG_WARNING);
      return 0;
   }

   // add the database. The function will just return the DB object if the
   // database already exists. 
   $db = tripal_db_add_db($dbname);
   if(!$db){
      watchdog('tripal_cv', "Cannot find database '$dbname' in Chado.",NULL,WATCHDOG_WARNING);
      return 0;
   }


   // if the cvterm doesn't exist then add it otherwise just update it
   $cvterm = db_fetch_object(db_query($cvtermsql,$accession,$dbname));
   if(!$cvterm){
      // check to see if the dbxref exists if not, add it
      $dbxref =  tripal_db_add_dbxref($db->db_id,$accession);
      if(!$dbxref){
         watchdog('tripal_cv', "Failed to find or insert the dbxref record for cvterm, $name (id: $accession), for database $dbname",NULL,WATCHDOG_WARNING);
         return 0;
      }

      // check to see if the dbxref already has an entry in the cvterm table
      $sql = "SELECT * FROM {cvterm} WHERE dbxref_id = %d";
      $check = db_fetch_object(db_query($sql,$dbxref->dbxref_id));
      if(!$check){
         // now add the cvterm
         $sql = "
            INSERT INTO {cvterm} (cv_id, name, definition, dbxref_id, 
               is_obsolete, is_relationshiptype) 
            VALUES (%d,'%s','%s',%d,%d,%d)
         ";
         if(!db_query($sql,$cv->cv_id,$name,$definition,
             $dbxref->dbxref_id,$is_obsolete,$is_relationship)){
            if(!$is_relationship){
               watchdog('tripal_cv', "Failed to insert the term: $name ($dbname)",NULL,WATCHDOG_WARNING);
               return 0;
            } else {
               watchdog('tripal_cv', "Failed to insert the relationship term: $name (cv: " . $cvname . " db: $dbname)",NULL,WATCHDOG_WARNING);
               return 0;
            }
         }  
      }
      // if the dbxref already exists check to make sure it exists for the correct databaes name
      // if it does then we're good and we don't need to do anything
      elseif($check and strcmp($check->name,$name)==0){
         // this entry already exists. We're good, so do nothing
      }
      // if the dbxref exists but does not map to the same database name
      elseif($check and strcmp($check->name,$name)!=0){
         watchdog('tripal_cv', "The dbxref already exists in the cvterm table. DBXREF ID: $dbxref->dbxref_id, ACCESSION: $accession. DB:  '".$dbxref->db_name ."'. term '$name'. The requested db was '$dbname'",NULL,WATCHDOG_WARNING);
         return 0;
      }
      $cvterm = db_fetch_object(db_query($cvtermsql,$name,$dbname));
      if(!$is_relationship){
         print "Added CV term: $name ($dbname)\n";
      } else {
         print "Added relationship CV term: $name ($dbname)\n";
      }
   }
   elseif($update) { // update the cvterm
      $sql = "
         UPDATE {cvterm} SET name='%s', definition='%s',
            is_obsolete = %d, is_relationshiptype = %d
         WHERE cvterm_id = %d
      ";
      if(!db_query($sql,$term['name'],$definition,
          $is_obsolete,$is_relationship,$cvterm->cvterm_id)){
         watchdog('tripal_cv', "Failed to update the term: $name",NULL,WATCHDOG_WARNING);
         return 0;

      }  
      $cvterm = db_fetch_object(db_query($cvtermsql,$name,$dbname));         
      if(!$is_relationship){
         print "Updated CV term: $name ($dbname)\n";
      } else {
         print "Updated relationship CV term: $name ($dbname)\n";
      }
   }
   // return the cvterm
   return $cvterm;
}


