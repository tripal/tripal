<?php

/*************************************************************************
 * Purpose: To retrieve a chado db object
 *
 * @params db.db_id
 * @return chado db object with all fields from the chado db table
 */
function tripal_db_get_db_by_db_id ($db_id) {

	$previous_db = tripal_db_set_active('chado');
  $r = db_fetch_object(db_query(
    "SELECT * FROM db WHERE db_id=%d", $db_id
  ));
  tripal_db_set_active($previous_db);

  return $r;
}

/*************************************************************************
 * Purpose: To retrieve a chado db object
 *
 * @params db.name
 * @return chado db object with all fields from the chado db table
 */
function tripal_db_get_db_by_name ($name) {

	$previous_db = tripal_db_set_active('chado');
  $r = db_fetch_object(db_query(
    "SELECT * FROM db WHERE name='%s'", $name
  ));
  tripal_db_set_active($previous_db);

  return $r;
}

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
  tripal_db_set_active($previous_db);

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

/*************************************************************************
 * Purpose: To retrieve a chado dbxref object with a given accession
 *
 * @params dbxref.accession
 * @params dbxref.db_id
 * @return chado dbxref object with all fields from the chado dbxref table
 */
function tripal_db_get_dbxref_by_accession ($accession, $db_id=0) {

  if (!empty($db_id)) {
	  $previous_db = tripal_db_set_active('chado');
    $r = db_fetch_object(db_query(
      "SELECT * FROM dbxref WHERE accession='%s' AND db_id=%d",
      $accession, $db_id
    ));
    tripal_db_set_active($previous_db);
  } else {
	  $previous_db = tripal_db_set_active('chado');
    $r = db_fetch_object(db_query(
      "SELECT * FROM dbxref WHERE accession='%s'",
      $accession
    ));
    tripal_db_set_active($previous_db);  
  }
  
  return $r;
}

/****************************************************************************
 * @section Chado Table Descriptions
 ****************************************************************************/

/****************************************************************************
 * Implements hook_chado_dbxref_schema()
 * Purpose: To add descriptions and foreign keys to default table description
 * Note: This array will be merged with the array from all other implementations
 *
 * @return
 *    Array describing the dbxref table
 */
function tripal_stock_chado_dbxref_schema() {
  $description = array();

  $description['foreign keys']['db'] = array(
        'table' => 'db',
        'columns' => array(
          'cv_id' => 'db_id',
        ),
  ); 

  return $description;
}