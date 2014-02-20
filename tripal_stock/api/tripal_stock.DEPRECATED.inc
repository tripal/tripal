<?php
/**
 * @file
 * Wrapper functions to provide backwards compatibility for the tripal stock api
 */

/**
 * @deprecated Restructured API to make naming more readable and consistent.
 * Function was deprecated in Tripal 2.0 and will be removed 2 releases from now.
 * This function has been replaced by tripal_get_stock().
 *
 * @see tripal_get_stock().
 */
function tripal_stock_get_stock_by_nid($nid) {

  tripal_report_error(
    'tripal_deprecated',
    TRIPAL_NOTICE,
    "DEPRECATED: %old_function has been replaced with %new_function. Please update your code.",
    array(
      '%old_function'=>'tripal_stock_get_stock_by_nid',
      '%new_function' => 'tripal_get_stock'
    )
  );

  return tripal_get_stock(array('nid' => $nid));
}

/**
 * @deprecated Restructured API to make naming more readable and consistent.
 * Function was deprecated in Tripal 2.0 and will be removed 2 releases from now.
 * This function has been replaced by tripal_get_stock().
 *
 * @see tripal_get_stock().
 */
function tripal_stock_get_stock_by_stock_id($stock_id) {

  tripal_report_error(
    'tripal_deprecated',
    TRIPAL_NOTICE,
    "DEPRECATED: %old_function has been replaced with %new_function. Please update your code.",
    array(
      '%old_function'=>'tripal_stock_get_stock_by_stock_id',
      '%new_function' => 'tripal_get_stock'
    )
  );

  return tripal_get_stock(array('stock_id' => $stock_id));
}

/**
 * @deprecated Restructured API to make naming more readable and consistent.
 * Function was deprecated in Tripal 2.0 and will be removed 2 releases from now.
 * This function has been replaced by tripal_get_multiple_stocks().
 *
 * @see tripal_get_multiple_stocks().
 */
function tripal_stock_get_all_stocks() {

  tripal_report_error(
    'tripal_deprecated',
    TRIPAL_NOTICE,
    "DEPRECATED: %old_function has been completely deprecated. There are often so many
     stocks in a chado database that it is unlikely a user really wants all of them.",
    array(
      '%old_function'=>'tripal_stock_get_all_stocks'
    )
  );

  $sql = "SELECT stock_id, nid from {chado_stock}";
  $resource = db_query($sql);
  $stocks = array();
  while ($r = $resource->fetchObject()) {
    $node = node_load($r->nid);
    if ($node) {
      $stocks[$r->stock_id] = $node;
    }
  }
  return $stocks;
}

/**
 * @deprecated Restructured API to make naming more readable and consistent.
 * Function was deprecated in Tripal 2.0 and will be removed 2 releases from now.
 * This function has been replaced by tripal_get_multiple_stocks().
 *
 * @see tripal_get_multiple_stocks().
 */
function tripal_stock_get_stocks($values) {

  tripal_report_error(
    'tripal_deprecated',
    TRIPAL_NOTICE,
    "DEPRECATED: %old_function has been replaced with %new_function. Please update your code.",
    array(
      '%old_function'=>'tripal_stock_get_stocks',
      '%new_function' => 'tripal_get_multiple_stocks'
    )
  );

  return tripal_get_multiple_stocks($values);
}

/**
 * @deprecated Restructured API to make naming more readable and consistent.
 * Function was deprecated in Tripal 2.0 and will be removed 2 releases from now.
 * This function has been replaced by tripal_get_stock().
 *
 * @see tripal_get_stock().
 */
function tripal_stock_get_stocks_by_stockprop($stockprop_values, $stock_values) {

  tripal_report_error(
    'tripal_deprecated',
    TRIPAL_NOTICE,
    "DEPRECATED: %old_function has been replaced with %new_function. Please update your code.",
    array(
      '%old_function'=>'tripal_stock_get_stocks_by_stockprop',
      '%new_function' => 'tripal_get_stock'
    )
  );

  $stock_values['property'] = $stockprop_values;
  return tripal_get_multiple_stocks($stock_values);
}

/**
 * @deprecated Restructured API to make naming more readable and consistent.
 * Function was deprecated in Tripal 2.0 and will be removed 2 releases from now.
 * This function has been replaced by tripal_get_stock().
 *
 * Return all stocks with a given name identifier
 *  which might match stock.name, stock.uniquename, dbxref.accession,
 *  stockprop.value where stockprop.type='synonym'
 *
 * @param $name
 *   The name identfier to be used
 * @param $organism_id
 *   The stock.organism_id of the stock to be selected
 *
 * @return
 *   An array of stock node objects
 *
 * @see tripal_get_stock().
 */
function tripal_stock_get_stock_by_name_identifier($name, $organism_id) {

  tripal_report_error(
    'tripal_deprecated',
    TRIPAL_NOTICE,
    "DEPRECATED: %old_function has been replaced with %new_function. Please update your code.",
    array(
      '%old_function'=>'tripal_stock_get_stock_by_name_identifier',
      '%new_function' => 'tripal_get_stock'
    )
  );

  $stock_ids = array();

  $options = array(
    'case_insensitive_columns' => array('name', 'uniquename', 'accession', 'value')
  );

  // where name_identifier = stock.name-------------------------------
  $current_stocks = chado_select_record('stock', array('stock_id'),
    array(
      'name' => $name,
      'organism_id' => $organism_id,
    ),
    array(
      'case_insensitive_columns' => array('name'),
    )
  );
  if (!empty($current_stocks)) {
    foreach ($current_stocks as $c) {
    $stock_ids[] = $c->stock_id; }
  }

  // where name_identifier = stock.uniquename-------------------------------
  $current_stocks = chado_select_record('stock', array('stock_id'),
    array(
      'uniquename' => $name,
      'organism_id' => $organism_id,
    ),
    array(
      'case_insensitive_columns' => array('uniquename'),
    )
  );
  if (!empty($current_stocks)) {
    foreach ($current_stocks as $c) {
    $stock_ids[] = $c->stock_id; }
  }

  // where name_identifier = dbxref.accession-------------------------------
  // linked to stock through stock.dbxref
  $current_stocks = chado_select_record('stock', array('stock_id'),
    array(
      'dbxref_id' => array(
        'accession' => $name,
      ),
      'organism_id' => $organism_id,
    ),
    array(
      'case_insensitive_columns' => array('accession'),
    )
  );
  if (!empty($current_stocks)) {
    foreach ($current_stocks as $c) {
    $stock_ids[] = $c->stock_id; }
  }

  // linked to stock through stock_dbxref?
  $current_stocks = chado_select_record('stock_dbxref', array('stock_id'),
    array(
      'dbxref_id' => array(
        'accession' => $name,
      ),
      'stock_id' => array(
        'organism_id' => $organism_id,
      ),
    ),
    array(
      'case_insensitive_columns' => array('accession'),
    )
  );
  if (!empty($current_stocks)) {
    foreach ($current_stocks as $c) {
      $stock_ids[] = $c->stock_id;
    }
  }

  // where name_identifier = stockprop.value-------------------------------
  // where type='synonym'
  $current_stocks = chado_select_record('stockprop', array('stock_id'),
    array(
      'stock_id' => array(
        'organism_id' => $organism_id,
      ),
      'type_id' => array(
        'cv_id' => variable_get('chado_stock_prop_types_cv', 'null'),
        'name' => 'synonym',
      ),
      'value' => $name,
    ),
    array(
      'case_insensitive_columns' => array('value'),
    )
  );
  if (!empty($current_stocks)) {
    foreach ($current_stocks as $c) {
      $stock_ids[] = $c->stock_id;
    }
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