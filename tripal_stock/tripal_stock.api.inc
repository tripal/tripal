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
		$node = node_load($r->nid);
		if ($node) {
      $stocks[$r->stock_id] = $node;
    }
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
    $node = tripal_stock_get_stock_by_stock_id($stock_id->stock_id);
    if ($node) {
      $stocks[] = $node;
    }
  }
  
  return $stocks;
}

/*************************************************************************
 * Purpose: Retrieve stocks based on associated stock properties
 *
 * @params $stockprop_values
 *   An array of column_name => value where column_name is any column in the stockprop table
 *   and value is the value you want that column to be. This is used as a tripal_core_chado_select
 *   values array so nesting is allowed.
 * @params $stock_values
 *   An array of column_name => value where column_name is any column in the stock table
 *   and value is the value you want that column to be. This is used as a tripal_core_chado_select
 *   values array so nesting is allowed. 
 * @return
 *   An array of stock node objects
 *
 * Example usage:
 * @code
 *   $stockprop_values =  array(
 *     'value' => 'CDC Redberry',
 *     'type_id' => array (
 *         'cv_id' => array (
 *            'name' => 'stock_properties',
 *         ),
 *         'name' => 'synonym',
 *         'is_obsolete' => 0
 *      ),
 *   );
 *   $stock_values =  array(
 *     'organism_id' => array(
 *         'genus' => 'Lens',
 *         'species' => 'culinaris',
 *      ),
 *   );
 *   $result = tripal_stock_get_stocks_by_stockprop($stockprop_values, $stock_values);
 * @endcode
 * The above code selects all Lens culinaris stocks with the synonym (stock property) CDC Redberry.
 * The nodes for each stock selected are loaded and returned in an array.
 */
function tripal_stock_get_stocks_by_stockprop($stockprop_values, $stock_values) {

  //add stock values to stockprop values
  if (!empty($stock_values)) {
    $stockprop_values['stock_id'] = $stock_values;
  }
  
  //get stock_ids from stockprop table
  $stock_ids = tripal_core_chado_select('stockprop',array('stock_id'),$stockprop_values);

  // Change from stock_ids to nodes-----------------------------------
  $stock_ids = array_filter($stock_ids);
  $stock_ids = array_unique($stock_ids);
  
  $stocks = array();
  foreach ($stock_ids as $stock_id) {
    $node = tripal_stock_get_stock_by_stock_id($stock_id->stock_id);
    if ($node) {
      $stocks[] = $node;
    }
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
    foreach ($current_stocks as $c) { $stock_ids[] = $c->stock_id; }
  }

  // where name_identifier = stock.uniquename-------------------------------
  $current_stocks = tripal_core_chado_select('stock',array('stock_id'),
    array(
      'uniquename' => $name,
      'organism_id' => $organism_id,
    )
  );
  if (!empty($current_stocks)) {
    foreach ($current_stocks as $c) { $stock_ids[] = $c->stock_id; }
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
    foreach ($current_stocks as $c) { $stock_ids[] = $c->stock_id; }  
  }
  
  // linked to stock through stock_dbxref?
  $current_stocks = tripal_core_chado_select('stock_dbxref',array('stock_id'),
    array(
      'dbxref_id' => array(
        'accession' => $name,
      ),
      'stock_id' => array(
        'organism_id' => $organism_id,
      ),
    )
  );
  if (!empty($current_stocks)) {
    foreach ($current_stocks as $c) { $stock_ids[] = $c->stock_id; }  
  }
  
  // where name_identifier = stockprop.value-------------------------------
  // where type='synonym' 
  $current_stocks = tripal_core_chado_select('stockprop',array('stock_id'),
    array(
      'stock_id' => array(
        'organism_id' => $organism_id,
      ),
      'type_id' => array(
        'cv_id' => variable_get('chado_stock_prop_types_cv', 'null'),
        'name' => 'synonym',
      ),
      'value' => $name,
    )
  );
  if (!empty($current_stocks)) {
    foreach ($current_stocks as $c) { $stock_ids[] = $c->stock_id; }
  }
  
  // Change from stock_ids to nodes-----------------------------------
  $stock_ids = array_filter($stock_ids);
  $stock_ids = array_unique($stock_ids);
  
  $stocks = array();
  foreach ($stock_ids as $stock_id) {
    $node = tripal_stock_get_stock_by_stock_id($stock_id);
    if ($node) {
      $stocks[] = $node;
    }
  }
  
  return $stocks;
}
