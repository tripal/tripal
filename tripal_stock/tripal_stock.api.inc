<?php
 
/*************************************************************************
 * @section: Return a Single Stock
 *************************************************************************/

/*************************************************************************
 * Purpose: Return a given stock object using the nid
 *
 * @return stock object created by node load
 */
function triapl_stock_get_stock_by_nid ($nid) {
	
	return node_load($nid);
	
}

/*************************************************************************
 * Purpose: Return a given stock object using the stock id
 *
 * @return stock object created by node load
 */
function tripal_stock_get_stock_by_stock_id ($stock_id) {

  $sql = "SELECT nid FROM {chado_stock} WHERE stock_id=%d";
  $r = db_fetch_object(db_query($sql, $stock_id));
  if (!empty($r->nid)) {
    return node_load($r->nid);
  } else {
    watchdog('tripal_stock', 'tripal_stock_get_stock_by_stock_id(!stock_id): no stock with that stock_id is sync\'d with drupal', array('!stock_id' => $stock_id), WATCHDOG_WARNING);
	}

	return 0;
	
}

/*************************************************************************
 * @section: Return Multiple Stocks
 *************************************************************************/
 
/*************************************************************************
 * Purpose: Returns all stocks currently sync'd with drupal
 *
 * @return 
     An array of node objects keyed by stock_id
 */
function tripal_stock_get_all_stocks() {
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
 * @params $values
 *   An associative array containing the values for filtering the results.
 * @return 
 *   An array of matching stock objects (produced using node_load)
 *   matching the given criteria
 *
 * Example usage:
 * @code
 *   $values =  array(
 *     'organism_id' => array(
 *         'genus' => 'Lens',
 *         'species' => 'culinaris',
 *      ),
 *     'name' => 'CDC Redberry',
 *     'type_id' => array (
 *         'cv_id' => array (
 *            'name' => 'germplasm',
 *         ),
 *         'name' => 'registered_cultivar',
 *         'is_obsolete' => 0
 *      ),
 *   );
 *   $result = tripal_stock_get_stocks($values);
 * @endcode
 * The above code selects a record from the chado stock table using three fields with values which
 * identify a stock or multiple stocks. Then the node for each stock identified is returned, if it
 * exists. The $values array is nested such that the organism is identified by way of the 
 * organism_id foreign key constraint by specifying the genus and species.  The cvterm is also 
 * specified using its foreign key and the cv_id for the cvterm is nested as well.
 */
function tripal_stock_get_stocks($values) {

  $stock_ids = tripal_core_chado_select('stock',array('stock_id'),$values);

  // Change from stock_ids to nodes-----------------------------------
  $stock_ids = array_filter($stock_ids);
  $stock_ids = array_unique($stock_ids);
  
  $stocks = array();
  foreach ($stock_ids as $stock_id) {
    $stocks[] = tripal_stock_get_stock_by_stock_id($stock_id->stock_id);
  }
  
  return $stocks;
}

/*************************************************************************
 * Purpose: Return all stocks with a given name identifier
 *  which might match stock.name, stock.uniquename, dbxref.accession, 
 *  stockprop.value where stockprop.type='synonym'
 *
 * @param $name
 *   The name identfier to be used
 * @params $organism_id
 *   The stock.organism_id of the stock to be selected
 * @return
 *   An array of stock node objects
 */
function tripal_stock_get_stock_by_name_identifier($name, $organism_id) {
  $stock_ids = array();
  
  // where name_identifier = stock.name-------------------------------
  $current_stocks = tripal_core_chado_select('stock',array('stock_id'),
    array(
      'name' => $name,
      'organism_id' => $organism_id,
    )
  );
  if (!empty($current_stocks)) {
    $stock_ids = array_merge($stock_ids, $current_stocks);
  }

  // where name_identifier = stock.uniquename-------------------------------
  $current_stocks = tripal_core_chado_select('stock',array('stock_id'),
    array(
      'uniquename' => $name,
      'organism_id' => $organism_id,
    )
  );
  if (!empty($current_stocks)) {
    $stock_ids = array_merge($stock_ids, $current_stocks);
  }
  
  // where name_identifier = dbxref.accession-------------------------------
  // linked to stock through stock.dbxref
  $current_stocks = tripal_core_chado_select('stock',array('stock_id'),
    array(
      'dbxref_id' => array(
        'accession' => $name,
      ),
      'organism_id' => $organism_id,
    )
  );
  if (!empty($current_stocks)) {
    $stock_ids = array_merge($stock_ids, $current_stocks);  
  }
  
  // linked to stock through stock_dbxref?
  
  // where name_identifier = stockprop.value-------------------------------
  // where type='synonym' 
  
  
  // Change from stock_ids to nodes-----------------------------------
  $stock_ids = array_filter($stock_ids);
  $stock_ids = array_unique($stock_ids);
  
  $stocks = array();
  foreach ($stock_ids as $stock_id) {
    $stocks[] = tripal_stock_get_stock_by_stock_id($stock_id->stock_id);
  }
  
  return $stocks;
}
