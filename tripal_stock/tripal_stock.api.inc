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
 * @section: Get Additional Data Associated with a stock
 *************************************************************************/

/*************************************************************************
 * Purpose:
 *
 * @params $stock_id
 *   the unique identifier for the stock to load properties of
 * @return
 *   an array of properties where each element is a property object with
 *   the following fields:
 *     - stockprop_id (stockprop):
 *     - value (stockprop):
 *     - rank (stockprop):
 *     - type_id (stockprop):
 *     - type_name (cvterm):
 *     - type_definition (cvterm):
 *     - type_is_obsolete (cvterm):
 *     - type_is_relationshiptype (cvterm):
 *     - type_cv_id (cvterm):
 *     - type_db_reference_id (cvterm):
 *     - type_cv_name (cv):
 *     - type_cv_definition (cv):
 *     - type_db_reference_acession (dbxref):
 *     - type_db_reference_version (dbxref):
 *     - type_db_reference_description (dbxref):
 *     - type_db_reference_db_id (dbxref):
 *     - type_db_reference_db_name (db):
 *     - type_db_reference_db_description (db):
 *     - type_db_reference_db_url (db):
 *     - type_db_reference_db_urlprefix (db):
 */
function tripal_stock_load_properties($stock_id) {

  $properties = tripal_core_chado_select(
    'stockprop',
    array('stockprop_id', 'value', 'rank','type_id'),
    array('stock_id' => $stock_id)
  );
  
  dpm($properties);
  return $properties;  
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

/****************************************************************************
 * @section Chado Table Descriptions
 * Please Add Descriptions to the fields ($description['fields'][<field name>]['description]) 
 * and table ($description['description']). Also add any foreign key relelationships that
 * arn't already added.
 ****************************************************************************/
 
/****************************************************************************
 * Implements hook_chado_stock_schema()
 * Purpose: To add descriptions and foreign keys to default table description
 * Note: This array will be merged with the array from all other implementations
 *
 * @return
 *    Array describing the stock table
 */
function tripal_stock_chado_stock_schema() {
  $description = array();

  $description['description'] = 'Any stock can be globally identified by the combination of organism, uniquename and stock type. A stock is the physical entities, either living or preserved, held by collections. Stocks belong to a collection; they have IDs, type, organism, description and may have a genotype.';

  $description['foreign keys']['organism'] = array(
        'table' => 'organism',
        'columns' => array(
          'organism_id' => 'organism_id',
        ),
  );
      
  $description['foreign keys']['dbxref'] = array(
        'table' => 'dbxref',
        'columns' => array(
          'dbxref_id' => 'dbxref_id',
        ),
  );
      
  $description['foreign keys']['cvterm'] = array(
        'table' => 'cvterm',
        'columns' => array(
          'type_id' => 'cvterm_id',
        ),
  );
      
  return $description;
}

/****************************************************************************
 * Implements hook_chado_stockprop_schema()
 * Purpose: To add descriptions and foreign keys to default table description
 * Note: This array will be merged with the array from all other implementations
 *
 * @return
 *    Array describing the stockprop table
 */
function tripal_stock_chado_stockprop_schema() {
  $description = array();
  
  $description['foreign keys']['cvterm'] = array(
        'table' => 'cvterm',
        'columns' => array(
          'type_id' => 'cvterm_id',
        ),
  );
  
  $description['foreign keys']['stock'] = array(
        'table' => 'stock',
        'columns' => array(
          'stock_id' => 'stock_id',
        ),
  );
  
  return $description;
}

/****************************************************************************
 * Implements hook_chado_stockprop_pub_schema()
 * Purpose: To add descriptions and foreign keys to default table description
 * Note: This array will be merged with the array from all other implementations
 *
 * @return
 *    Array describing the stockprop_pub table
 */
function tripal_stock_chado_stockprop_pub_schema() {
  $description = array();
  
  $description['foreign keys']['pub'] = array(
        'table' => 'pub',
        'columns' => array(
          'pub_id' => 'pub_id',
        ),
  );
  
  $description['foreign keys']['stockprop'] = array(
        'table' => 'stockprop',
        'columns' => array(
          'stockprop_id' => 'stockprop_id',
        ),
  );
  
  return $description;
}

/****************************************************************************
 * Implements hook_chado_stock_cvterm_schema()
 * Purpose: To add descriptions and foreign keys to default table description
 * Note: This array will be merged with the array from all other implementations
 *
 * @return
 *    Array describing the stock_cvterm table
 */
function tripal_stock_chado_stock_cvterm_schema() {
  $description = array();
  
  $description['foreign keys']['cvterm'] = array(
        'table' => 'cvterm',
        'columns' => array(
          'cvterm_id' => 'cvterm_id',
        ),
  );
  
  $description['foreign keys']['stock'] = array(
        'table' => 'stock',
        'columns' => array(
          'stock_id' => 'stock_id',
        ),
  );

  $description['foreign keys']['pub'] = array(
        'table' => 'pub',
        'columns' => array(
          'pub_id' => 'pub_id',
        ),
  );
  
  return $description;
}

/****************************************************************************
 * Implements hook_chado_stock_dbxref_schema()
 * Purpose: To add descriptions and foreign keys to default table description
 * Note: This array will be merged with the array from all other implementations
 *
 * @return
 *    Array describing the stock_dbxref table
 */
function tripal_stock_chado_stock_dbxref_schema() {
  $description = array();
  
  $description['foreign keys']['dbxref'] = array(
        'table' => 'dbxref',
        'columns' => array(
          'dbxref_id' => 'dbxref_id',
        ),
  );
  
  $description['foreign keys']['stock'] = array(
        'table' => 'stock',
        'columns' => array(
          'stock_id' => 'stock_id',
        ),
  );
  
  return $description;
}

/****************************************************************************
 * Implements hook_chado_stock_genotype_schema()
 * Purpose: To add descriptions and foreign keys to default table description
 * Note: This array will be merged with the array from all other implementations
 *
 * @return
 *    Array describing the stock_genotype table
 */
function tripal_stock_chado_stock_genotype_schema() {
  $description = array();
  
  $description['foreign keys']['genotype'] = array(
        'table' => 'genotype',
        'columns' => array(
          'genotype_id' => 'genotype_id',
        ),
  );
  
  $description['foreign keys']['stock'] = array(
        'table' => 'stock',
        'columns' => array(
          'stock_id' => 'stock_id',
        ),
  );
  
  return $description;
}

/****************************************************************************
 * Implements hook_chado_stock_pub_schema()
 * Purpose: To add descriptions and foreign keys to default table description
 * Note: This array will be merged with the array from all other implementations
 *
 * @return
 *    Array describing the stock_pub table
 */
function tripal_stock_chado_stock_pub_schema() {
  $description = array();
  
  $description['foreign keys']['pub'] = array(
        'table' => 'pub',
        'columns' => array(
          'pub_id' => 'pub_id',
        ),
  );
  
  $description['foreign keys']['stock'] = array(
        'table' => 'stock',
        'columns' => array(
          'stock_id' => 'stock_id',
        ),
  );
  
  return $description;
}

/****************************************************************************
 * Implements hook_chado_stock_relationship_schema()
 * Purpose: To add descriptions and foreign keys to default table description
 * Note: This array will be merged with the array from all other implementations
 *
 * @return
 *    Array describing the stock_relationship table
 */
function tripal_stock_chado_stock_relationship_schema() {
  $description = array();
  
  $description['foreign keys']['cvterm'] = array(
        'table' => 'cvterm',
        'columns' => array(
          'type_id' => 'cvterm_id',
        ),
  );
  
  $description['foreign keys']['stock'] = array(
        'table' => 'stock',
        'columns' => array(
          'subject_id' => 'stock_id',
          'object_id' => 'stock_id',
        ),
  );
  
  return $description;
}

/****************************************************************************
 * Implements hook_chado_stock_relationship_pub_schema()
 * Purpose: To add descriptions and foreign keys to default table description
 * Note: This array will be merged with the array from all other implementations
 *
 * @return
 *    Array describing the stock_relationship_pub table
 */
function tripal_stock_chado_stock_relationship_pub_schema() {
  $description = array();
  
  $description['foreign keys']['pub'] = array(
        'table' => 'pub',
        'columns' => array(
          'pub_id' => 'pub_id',
        ),
  );
  
  $description['foreign keys']['stock_relationship'] = array(
        'table' => 'stock_relationship',
        'columns' => array(
          'stock_relationship_id' => 'stock_relationship_id',
        ),
  );
  
  return $description;
}

/****************************************************************************
 * Implements hook_chado_stockcollection_schema()
 * Purpose: To add descriptions and foreign keys to default table description
 * Note: This array will be merged with the array from all other implementations
 *
 * @return
 *    Array describing the stockcollection table
 */
function tripal_stock_chado_stockcollection_schema() {
  $description = array();
  
  $description['foreign keys']['cvterm'] = array(
        'table' => 'cvterm',
        'columns' => array(
          'type_id' => 'cvterm_id',
        ),
  );
  
  $description['foreign keys']['contact'] = array(
        'table' => 'contact',
        'columns' => array(
          'contact_id' => 'contact_id',
        ),
  );
  
  return $description;
}

/****************************************************************************
 * Implements hook_chado_stockcollection_stock_schema()
 * Purpose: To add descriptions and foreign keys to default table description
 * Note: This array will be merged with the array from all other implementations
 *
 * @return
 *    Array describing the stockcollection_stock table
 */
function tripal_stock_chado_stockcollection_stock_schema() {
  $description = array();
  
  $description['foreign keys']['stock'] = array(
        'table' => 'stock',
        'columns' => array(
          'stock_id' => 'stock_id',
        ),
  );
  
  $description['foreign keys']['stockcollection'] = array(
        'table' => 'stockcollection',
        'columns' => array(
          'stockcollection_id' => 'stockcollection_id',
        ),
  );
  
  return $description;
}

/****************************************************************************
 * Implements hook_chado_stockcollectionprop_schema()
 * Purpose: To add descriptions and foreign keys to default table description
 * Note: This array will be merged with the array from all other implementations
 *
 * @return
 *    Array describing the stockcollectionprop table
 */
function tripal_stock_chado_stockcollectionprop_schema() {
  $description = array();
  
  $description['foreign keys']['cvterm'] = array(
        'table' => 'cvterm',
        'columns' => array(
          'type_id' => 'cvterm_id',
        ),
  );
  
  $description['foreign keys']['stockcollection'] = array(
        'table' => 'stockcollection',
        'columns' => array(
          'stockcollection_id' => 'stockcollection_id',
        ),
  );
  
  return $description;
}