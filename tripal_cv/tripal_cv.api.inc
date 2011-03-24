<?php

/**
 * @defgroup tripal_cv_api Controlled Vocabulary Module API
 * @ingroup tripal_api
 * @ingroup tripal_cv
 * Provides an application programming interface (API) to controlled vocabularies
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
/**
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
*/  
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
 *
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
 * return the cv object for the specified CV id
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
 * Purpose: Create an options array to be used in a form element
 *   which provides a list of all chado cvs
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
 * Purpose: To retrieve a chado controlled vocabulary term object
 *
 * @param $select_values
 *   An array meant to uniquely select a given controlled vocabulary term
 *
 * @return
 *   Chado controlled vocabulary term object
 *
 * The controlled vocabulary term is selected using tripal_core_chado select and as such the
 * $select_values array parameter meant to uniquely identify the controlled vocab term to be 
 * returned follows the same form as when using tripal_core_chado_select directly.
 *
 * Example Usage:
 * @code
    $select_values = array(
      'name' => 'synonym',
      'cv_id' => array(
        'name' => 'feature_property'
      )
    );
    $cvterm_object = tripal_cv_get_cvterm($select_values);
 * @endcode
 *  The above code selects the synonym cvterm from the feature_proeprty cv and returns 
 *  the following object:
 * @code
    $cvterm_object = stdClass Object ( 
      [cvterm_id] => 2099
      [name] => synonym
      [definition] => Historic community symbol, may have originally been symbol []
      [is_obsolete] => 0
      [is_relationshiptype] => 1
      [cv_cv_id] => 13
      [cv_name] => feature_property
      [cv_definition] => 
      [dbreference_dbxref_id] => 2581
      [dbreference_accession] => synonym
      [dbreference_description] => 
      [dbreference_version] => 
      [dbreference_db_db_id] => 49
      [dbreference_db_name] => SOFP
      [dbreference_db_description] => 
      [dbreference_db_urlprefix] => 
      [dbreference_db_url] => 
    ); 
 * @endcode
 *
 * @ingroup tripal_cv_api
 */
function tripal_cv_get_cvterm ($select_values) {
/**
  $columns = array(
    'cvterm_id', 
    'cv_id', 
    'name', 
    'definition', 
    'dbxref_id', 
    'is_obsolete', 
    'is_relationshiptype'
  );
  $results = tripal_core_chado_select('cvterm', $columns, $select_values);
  if (sizeof($results) == 1) {
    // Add cv
    $cvterm = tripal_cv_add_cv_to_object(array('cv_id'=>$results[0]->cv_id),$results[0],array());
    unset($cvterm->cv_id);
    
    // Add dbxref
    $cvterm = tripal_db_add_dbxref_to_object(array('dbxref_id'=>$cvterm->dbxref_id),$cvterm,array());
    unset($cvterm->dbxref_id);
    
    return $cvterm;
  } elseif (empty($results)) {
    watchdog('tripal_cv', 
      'tripal_cv_get_cvterm: No cvterm matches criteria values:%values',
      array('%values' => print_r($select_values, TRUE)),
      WATCHDOG_WARNING
    );
    return FALSE;
  } else {
    watchdog('tripal_cv', 
      'tripal_cv_get_cvterm: 2+ cvterms match criteria values:%values',
      array('%values' => print_r($select_values, TRUE)),
      WATCHDOG_WARNING
    );
  }
*/  
}

/**
 * Purpose: Retrieve a chado cvterm object with a given name
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
 * Purpose: Create an options array to be used in a form element
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
