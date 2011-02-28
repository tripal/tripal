<?php

/*************************************************************************
 * Purpose: To retrieve a chado cv object
 *
 * @params where_options: array(
 *													<column_name> => array(
 *														'type' => <type of column: INT/STRING>,
 *														'value' => <the vlaue you want to filter on>,
 *														'exact' => <if TRUE use =; if FALSE use ~>,
 *													)
 *				)
 * @return chado cv object with all fields from the chado cv table
 */
function tripal_cv_get_cv ($where_options) {
	$previous_db = tripal_db_set_active('chado');

	$where= array();
	//generate the where clause from supplied options
	// the key is the column name
	foreach ($where_options as $key => $val_array) {
		if (preg_match('/INT/', $val_array['type'])) {
			$where[] = $key."=".$val_array['value'];
		} else {
			if ($val_array['exact']) { $operator='='; }
			else { $operator='~'; }
			$where[] = $key.$operator."'".$val_array['value']."'";
		}
	}
	
  $r = db_fetch_object(db_query(
    "SELECT * FROM cv WHERE ".implode(' AND ',$where)
  ));
  
  tripal_db_set_active($previous_db);

  return $r;
}
/*************************************************************************
 * return the cv object for the specified CV name
 */
function tripal_cv_get_cv_by_name ($name) {
	$previous_db = tripal_db_set_active('chado');	
   $r = db_fetch_object(db_query("SELECT * FROM cv WHERE name = '%s'",$name));
   tripal_db_set_active($previous_db);

  return $r;
}
/*************************************************************************
 * return the cv object for the specified CV id
 */
function tripal_cv_get_cv_by_id ($cv_id) {
	$previous_db = tripal_db_set_active('chado');	
   $r = db_fetch_object(db_query("SELECT * FROM cv WHERE cv_id = %d",$cv_id));
   tripal_db_set_active($previous_db);

  return $r;
}
/*************************************************************************
 * Purpose: Create an options array to be used in a form element
 *   which provides a list of all chado cvs
 *
 * @return an array(cv_id => name) for each cv in the chado cv table
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

/*************************************************************************
 * Purpose: To retrieve a chado cvterm object
 *
 * @params where_options: array(
 *													<column_name> => array(
 *														'type' => <type of column: INT/STRING>,
 *														'value' => <the vlaue you want to filter on>,
 *														'exact' => <if TRUE use =; if FALSE use ~>,
 *													)
 *				)
 * @return chado cvterm object with all fields from the chado cvterm table
 */
function tripal_cv_get_cvterm ($where_options) {
	$previous_db = tripal_db_set_active('chado');

	$where= array();
	//generate the where clause from supplied options
	// the key is the column name
	foreach ($where_options as $key => $val_array) {
		if (preg_match('/INT/', $val_array['type'])) {
			$where[] = $key."=".$val_array['value'];
		} else {
			if ($val_array['exact']) { $operator='='; }
			else { $operator='~'; }
			$where[] = $key.$operator."'".$val_array['value']."'";
		}
	}
	
  $r = db_fetch_object(db_query(
    "SELECT * FROM cvterm WHERE ".implode(' AND ',$where)
  ));
  
  tripal_db_set_active($previous_db);

  return $r;
}

/*************************************************************************
 * Purpose: Retrieve a chado cvterm object with a given name
 *
 * @params name: the cvterm.name
 * @params cv_id: the cv_id of the term you are looking for
 * @return cvterm object
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
/*************************************************************************
 * Purpose: Create an options array to be used in a form element
 *   which provides a list of all chado cvterms
 * 
 * @params cv_id: the chado cv_id
 *   only cvterms with the supplied cv_id will be returned
 * @return an array(cvterm_id => name) 
 *   for each cvterm in the chado cvterm table where cv_id=that supplied
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

/****************************************************************************
 * @section Chado Table Descriptions
 ****************************************************************************/

/****************************************************************************
 * Implements hook_chado_cvterm_schema()
 * Purpose: To add descriptions and foreign keys to default table description
 * Note: This array will be merged with the array from all other implementations
 *
 * @return
 *    Array describing the cvterm table
 */
function tripal_stock_chado_cvterm_schema() {
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