<?php

/** 
 * @defgroup tripal_core_cv_api Core Module CV API
 * @{
 * For working with controlled vocabularies see the Controlled Vocabulary
 * API.  However, there are cases where a CV does not exist. For example in
 * some cases a module may want to support adding of properties (such as to
 * for an analysis) to a record but an organized CV does not exist.  The 
 * progammer may create their own ontology and populate it, or they may use
 * the basic CV interface provided by Tripal.  Tripal provides a CV named
 * 'Tripal' and modules may add terms to this CV to support the functionality
 * of their modules.  
 * @}
 * @ingroup tripal_api
 */

/**
 * Add a materialized view to the chado database to help speed data access.
 *
 * @param $name 
 *   The name of the term
 * @param $definition 
 *   The definition for the term
 * @param $cv_name 
 *   The name of the controlled vocabulary to which the term will be added.
 *   The default CV is 'tripal'.
 * @param $db_name 
 *   CV records also have a corresponding database reference.  This argument
 *   specifies the name of the database to which this term belongs. The default
 *   database name is 'tripal'.
 *
 * @ingroup tripal_core_cv_api
 */
function tripal_add_cvterms ($name,$definition,$cv_name = 'tripal',$db_name='tripal'){
   
   
   $previous_db = tripal_db_set_active('chado');  // use chado database
   $cv = db_fetch_object(db_query("SELECT * FROM {cv} WHERE name = '$cv_name'"));
   if (!$cv->cv_id) {
      tripal_db_set_active($previous_db);
      tripal_add_cv('tripal', 'Terms used by Tripal for modules to manage data such as that 
                               stored in property tables like featureprop, analysisprop, etc');
      tripal_db_set_active('chado');
      $cv = db_fetch_object(db_query("SELECT * FROM {cv} WHERE name = '$cv_name'"));
   }
   $db = db_fetch_object(db_query("SELECT * FROM {db} WHERE name = '$db_name'"));
	if (!$db->db_id) {
	   tripal_db_set_active($previous_db);
	   tripal_add_db('tripal', 'Used as a database placeholder for tripal defined objects such as tripal cvterms', '', '');
	   tripal_db_set_active('chado');
	   $db = db_fetch_object(db_query("SELECT * FROM {db} WHERE name = '$db_name'"));
	}

	// check to see if the dbxref already exists if not then add it
	$sql = "SELECT * FROM {dbxref} WHERE db_id = $db->db_id and accession = '$name'";
	$dbxref = db_fetch_object(db_query($sql));
	if(!$dbxref){
	   db_query("INSERT INTO {dbxref} (db_id,accession) VALUES ($db->db_id,'$name')");
		$dbxref = db_fetch_object(db_query($sql));
	}

   
   // now add the cvterm only if it doesn't already exist
	$sql = "SELECT * FROM {cvterm} ".
	       "WHERE cv_id = $cv->cv_id and dbxref_id = $dbxref->dbxref_id and name = '$name'";
	$cvterm = db_fetch_object(db_query($sql));
	if(!$cvterm){
      $result = db_query("INSERT INTO {cvterm} (cv_id,name,definition,dbxref_id) ".
                         "VALUES ($cv->cv_id,'$name','$definition',$dbxref->dbxref_id)");
	}
   tripal_db_set_active($previous_db);  // now use drupal database	
	
   if(!$result){
     // TODO -- ERROR HANDLING
   }
}
/**
 * Add a database to the Chado 'db' table.
 * NOTE: This function is deprecated and may not be used in future releases of Tripal
 *  
 * @ingroup tripal_core_cv_api
 */
function tripal_add_db($db_name,$description,$urlprefix,$url){
   $previous_db = tripal_db_set_active('chado');  // use chado database

   // use this SQL statement to get the db_id for the database name
   $id_sql = "SELECT db_id FROM {db} WHERE name = '%s'";

   $db = db_fetch_object(db_query($id_sql,$db_name));

   // if the database doesn't exist then let's add it.
   if(!$db->db_id){
      $sql = "
         INSERT INTO {db} (name,description,urlprefix,url) VALUES 
         ('%s','%s','%s','%s');
      ";
      db_query($sql,$db_name,$description,$urlprefix,$url);
    
      # now get the id for this new db entry
      $db = db_fetch_object(db_query($id_sql,$db_name));
   }
   tripal_db_set_active($previous_db);  // now use drupal database	
   return $db->db_id;
}


/**
 * Get cvterm_id for a tripal term by passing its name
 * NOTE: This function is deprecated and may not be used in future releases of Tripal
 *  
 * @ingroup tripal_core_cv_api
 */
function tripal_get_cvterm_id ($cvterm){
	$sql = "SELECT CVT.cvterm_id FROM {cvterm} CVT
         	    INNER JOIN cv ON cv.cv_id = CVT.cv_id 
            	 WHERE CVT.name = '$cvterm' 
                AND CV.name = 'tripal'";
	return db_result(chado_query($sql));
}

