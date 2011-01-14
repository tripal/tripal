<?php

/*************************************************************************
 * Purpose: Returns all stocks currently sync'd with drupal
 *
 * @return array(
 *			<stock_id> => <stock object created by node load>
 *		)
 */
function get_all_chado_stocks() {
	$sql = "SELECT stock_id, nid from {chado_stock}";
	$resource = db_query($sql);
	$stocks = array();
	while ($r = db_fetch_object($resource)) {
		$stocks[$r->stock_id] = node_load($r->nid);
	}
	return $stocks;
}

/*************************************************************************
 * Purpose: Return all stocks that match a given criteria
 *
 * @params Criteria = array(
 *						<column name> => array( 
 *							'value'=> <value of column>, 
 *							'regex' => <False if exact value, TRUE otherwise>,
 *							'type' => <INT or STRING>
 *						)
 *         )
 *			column name can be any of those for the stock table
 *			additional column names include: accession, synonym
 *	if you don't know which column the value is from and want to match
 *		the value against any of name, uniquename, dbxref accessions, and synonyms
 *   	use 'unknown' => array(
 *												'value' => <value to search for>,
 *												'columns' => array of columns to search 
 *											)
 *		where columns can be any combination of 'name', 'uniquename', 'stock','accession', or 'synonym'
 * @params match_type: can be either ALL or ANY
 *	 where ALL= mathcing stocks must match all criteria supplied
 *				 ANY= matching stock need only match ONE of the supplied criteria
 * @return an array of matching stock objects (produced using node_load)
 *   matching the given criteria
 */
function get_chado_stocks($criteria, $match_type, $organism_id = NULL) {
	
	//Deal with unknown column----------------------------
	if (!empty($criteria['unknown'])) {
		$unknown_provided = TRUE;
		$new_criteria = array();
		foreach ($criteria['unknown']['columns'] as $column_name) {
			if (in_array($column_name, array('stock_id','dbxref_id','organism_id','type_id') )) {
				$new_criteria[$column_name] = array('type'=>'INT','value' => $criteria['unknown']['value']);
			} else {
				$new_criteria[$column_name] = array('type'=>'STRING','value' => $criteria['unknown']['value'], 'regex'=>TRUE);
			}
		}
		$unknown_stocks = get_chado_stocks($new_criteria, 'ANY', $organism_id);
	}
	unset($criteria['unknown']); //so it's not mistaken as a column in the stock table
	
	// Determine all criteria------------------------------
	$where = array(); // parts of the where portion of the SQL query
	$joins = array(); //joins between the stock table and other tables
	foreach ($criteria as $column_name => $v) {
		if (preg_match("/accession/i",$column_name)) {
			if ($v['regex']) { $operator = '~'; }
			else { $operator = '='; }
			
			$where[] = 'dbxref.accession'.$operator."'".$v['value']."'";
			$joins[] = 'LEFT JOIN stock_dbxref stock_dbxref ON stock_dbxref.stock_id=stock.stock_id';
			$joins[] = 'LEFT JOIN dbxref dbxref ON dbxref.dbxref_id=stock_dbxref.dbxref_id';
			
		} elseif (preg_match("/synonym/i",$column_name)) {
			if ($v['regex']) { $operator = '~'; }
			else { $operator = '='; }
			
			$synonym_cvterm = get_chado_cvterm( array('name' => array('type'=>'STRING','exact'=>TRUE,'value'=>'synonym'),
																								'cv_id' => array('type'=>'INT', 'value'=> variable_get('chado_stock_prop_types_cv', 'null')) ));
			$where[] = '(stockprop.type_id='.$synonym_cvterm->cvterm_id.' AND stockprop.value'.$operator."'".$v['value']."')";
			$joins[] = 'LEFT JOIN stockprop stockprop ON stockprop.stock_id=stock.stock_id';
	
		} else {
			if ($v['regex']) { $operator = '~'; }
			else { $operator = '='; }
			
			if (preg_match('/INT/', $v['type'])) {
				$where[] = 'stock.'.$column_name.'='.$v['value'];
			} else {
				$where[] = 'stock.'.$column_name.$operator."'".$v['value']."'";
			}		
		}
	}
	
	//Build query-----------------------------------------
	if (preg_match('/ANY/', $match_type)) {
		$where_string = implode(' OR ',$where);
		if ($organism_id) {
			$where_string = '('.$where_string.') AND organism_id='.$organism_id;
		}
	} else {
		$where_string = implode(' AND ',$where);
		if ($organism_id) {
			$where_string .= ' AND organism_id='.$organism_id; }
	}
	
	if (sizeof($where) >= 1) {
		$execute_query = TRUE;
		$sql_query = 'SELECT stock.stock_id FROM stock '.implode(' ',$joins).' WHERE '.$where_string;
		//drupal_set_message('Query='.$sql_query);
	} elseif (!$unknown_provided) {	
		$execute_query = TRUE;
		$sql_query = 'SELECT stock.stock_id FROM stock';
		drupal_set_message('You did not enter any criteria during the stock selection process. All stocks will be returned.','warning');
	} else {
		$execute_query = FALSE;
	}
	
	if ($execute_query) {
		//Get stock_ids---------------------------------------
		$previous_db = tripal_db_set_active('chado');
		$resource = db_query($sql_query);
		tripal_db_set_active($previous_db);
	
		//drupal_set_message($sql_query);
	
		$stock_ids = array();	
		while ($r = db_fetch_object($resource)) {
			$stock_ids[] = $r->stock_id;
		}
		$stock_ids = array_unique($stock_ids);
	
		//Get Stocks------------------------------------------
		if (!empty($stock_ids)) {
			$resource = db_query("SELECT nid FROM {chado_stock} WHERE stock_id IN (%s)",implode(',',$stock_ids));
			$main_stocks = array();
			while ($r = db_fetch_object($resource)) {
				$main_stocks[] = node_load($r->nid);
			}
		}
	}
	
	if (!empty($main_stocks)) {
		if(!empty($unknown_stocks)){
			return array_merge($unknown_stocks,$main_stocks);
		} else {
			return $main_stocks;
		}
	} else {
		if(!empty($unknown_stocks)){
			return $unknown_stocks;
		} else {
			//drupal_set_message('No Stocks matched the given criteria','warning');
			return array();
		}
	}

}

/*************************************************************************
 * Purpose: Return a given stock object using the nid or stock id
 *
 * @return stock object created by node load
 */
function get_chado_stock($nid=0, $stock_id=0) {

	if (!empty($nid)) {
		return node_load($nid);
	} else {
		if (!empty($stock_id)) {
			$sql = "SELECT nid FROM {chado_stock} WHERE stock_id=%d";
			$r = db_fetch_object(db_query($sql, $stock_id));
			if (!empty($r->nid)) {
				return node_load($r->nid);
			} else {
				drupal_set_message("Function: get_chado_stock() -no stock with that stock id sync'd with drupal", 'error');
			}
		} else {
			drupal_set_message("Function: get_chado_stock() -need to supply at least one of node ID or Stock ID",'error');
		}
	}

	return 0;
	
}