<?php

///////////////////////////////////////////////////////////////////////////
// Module: tripal_cv
///////////////////////////////////////////////////////////////////////////

/*************************************************************************
 * Purpose: Create an options array to be used in a form element
 *   which provides a list of all chado cvs
 *
 * @return an array(cv_id => name) for each cv in the chado cv table
 */
function get_chado_cv_options() {

  $previous_db = tripal_db_set_active('chado');
  $result = db_query(
    "SELECT cv_id, name FROM cv"
  );
  db_set_active($previous_db);

  $options = array();
  while ( $r = db_fetch_object($result) ) {
    $options[$r->cv_id] = $r->name;
  }

  return $options;

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
function get_chado_cvterm_options($cv_id = 0) {

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
  db_set_active($previous_db);

  $options = array();
  while ( $r = db_fetch_object($result) ) {
    $options[$r->cvterm_id] = $r->name;
  }

  return $options;

}

///////////////////////////////////////////////////////////////////////////
// Module: tripal_db
///////////////////////////////////////////////////////////////////////////

/*************************************************************************
 * Purpose: Create an options array to be used in a form element
 *   which provides a list of all chado dbs
 *
 * @return an array(db_id => name) for each db in the chado db table
 */
function get_chado_db_options() {

  $previous_db = tripal_db_set_active('chado');
  $result = db_query(
    "SELECT db_id, name FROM db"
  );
  db_set_active($previous_db);

  $options = array();
  while ( $r = db_fetch_object($result) ) {
    $options[$r->db_id] = $r->name;
  }

  return $options;

}

/*************************************************************************
 *
 * @params where_options: array(
 *													<column_name> => array(
 *														'type' => <type of column: INT/STRING>,
 *														'value' => <the vlaue you want to filter on>,
 *														'exact' => <if TRUE use =; if FALSE use ~>,
 *													)
 *				)
 */
function get_chado_dbxref($where_options) {
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
    "SELECT * FROM dbxref WHERE ".implode(' AND ',$where)
  ));
  
  tripal_db_set_active($previous_db);

  return $r;
}

///////////////////////////////////////////////////////////////////////////
// Module: tripal_organism
///////////////////////////////////////////////////////////////////////////

/*************************************************************************
 * Purpose: Create an options array to be used in a form element
 *   which provides a list of all chado organisms
 *
 * @return an array(organism_id => common_name) 
 *   for each organism in the chado organism table
 */
function get_chado_organism_options() {

  $previous_db = tripal_db_set_active('chado');
  $result = db_query(
    "SELECT organism_id, common_name FROM organism"
  );
  tripal_db_set_active($previous_db);

  $options = array();
  while ( $r = db_fetch_object($result) ) {
    $options[$r->organism_id] = $r->common_name;
  }

  return $options;

}

/*************************************************************************
 * Purpose: Return a given organism object using the nid or organism id
 *
 * @return organism object created by node load
 */
function get_chado_organism($nid=0, $organism_id=0) {

	if (!empty($nid)) {
		return node_load($nid);
	} else {
		if (!empty($organism_id)) {
			$sql = "SELECT nid FROM {chado_organism} WHERE organism_id=%d";
			$r = db_fetch_object(db_query($sql, $organism_id));
			if (!empty($r->nid)) {
				return node_load($r->nid);
			} else {
				drupal_set_message("Function: get_chado_organism() -no organism with that organism id sync'd with drupal", 'error');
			}
		} else {
			drupal_set_message("Function: get_chado_organism() -need to supply at least one of node ID or Organism ID",'error');
		}
	}

	return 0;
	
}