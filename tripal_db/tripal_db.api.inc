<?php

/*************************************************************************
 * Purpose: To retrieve a chado db object
 *
 * @params where_options: array(
 *													<column_name> => array(
 *														'type' => <type of column: INT/STRING>,
 *														'value' => <the vlaue you want to filter on>,
 *														'exact' => <if TRUE use =; if FALSE use ~>,
 *													)
 *				)
 * @return chado db object with all fields from the chado db table
 */
function tripal_db_get_db ($where_options) {
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
    "SELECT * FROM db WHERE ".implode(' AND ',$where)
  ));
  
  tripal_db_set_active($previous_db);

  return $r;
}

/*************************************************************************
 * Purpose: Create an options array to be used in a form element
 *   which provides a list of all chado dbs
 *
 * @return an array(db_id => name) for each db in the chado db table
 */
function tripal_db_get_db_options() {

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
 * Purpose: To retrieve a chado dbxref object
 *
 * @params where_options: array(
 *													<column_name> => array(
 *														'type' => <type of column: INT/STRING>,
 *														'value' => <the vlaue you want to filter on>,
 *														'exact' => <if TRUE use =; if FALSE use ~>,
 *													)
 *				)
 * @return chado dbxref object with all fields from the chado dbxref table
 */
function tripal_db_get_dbxref ($where_options) {
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
