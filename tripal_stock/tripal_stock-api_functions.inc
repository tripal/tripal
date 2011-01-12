<?php

/*************************************************************************
 * Purpose: Returns all stocks currently sync'd with drupal
 *
 * @return array(
 *			<stock_id> => <stock object created by node load>
 *		)
 */
function get_chado_stocks() {
	$sql = "SELECT stock_id, nid from {chado_stock}";
	$resource = db_query($sql);
	$stocks = array();
	while ($r = db_fetch_object($resource)) {
		$stocks[$r->stock_id] = node_load($r->nid);
	}
	return $stocks;
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