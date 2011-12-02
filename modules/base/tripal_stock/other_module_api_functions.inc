<?php

///////////////////////////////////////////////////////////////////////////
// Module: tripal_core
///////////////////////////////////////////////////////////////////////////

/*************************************************************************
 * Purpose: Get max rank for a given set of criteria
 *   This function was developed with the many property tables in chado in mind
 *
 * @params tablename: the name of the chado table you want to select the max rank from
 *    this table must contain a rank column of type integer
 * @params where_options: array(
 *													<column_name> => array(
 *														'type' => <type of column: INT/STRING>,
 *														'value' => <the value you want to filter on>,
 *														'exact' => <if TRUE use =; if FALSE use ~>,
 *													)
 *				)
 *     where options should include the id and type for that table to correctly
 *     group a set of records together where the only difference are the value and rank
 * @return the maximum rank
 *
 */
function get_max_chado_rank ($tablename, $where_options) {

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
	
  $previous_db = tripal_db_set_active('chado');
  $result = db_fetch_object(db_query(
    "SELECT max(rank) as max_rank, count(rank) as count FROM %s WHERE %s",
    $tablename,
    implode(' AND ',$where)
  ));
  tripal_db_set_active($previous_db);
	//drupal_set_message("Max Rank Query=SELECT max(rank) as max_rank, count(rank) as count FROM ".$tablename." WHERE ".implode(' AND ',$where));
	if ($result->count > 0) {
	  return $result->max_rank;
	} else {
		return -1;
	}
}
