<?php
/**
 * @file
 * Provides API functions specifically for managing stock
 * records in Chado.
 */

/**
 * @defgroup tripal_stock_api Chado Stock
 * @ingroup tripal_chado_api
 * @{
 * Provides API functions specifically for managing stock
 * records in Chado.  The stock table of Chado is used for storing a variety
 * of data types besides just stocks from a stock collection.  Examples of
 * other records commonly stored in the stock table are germplasm and
 * individuals from a breeding population.
 * @}
 */

/**
 * Used for autocomplete in forms for identifying stocks
 *
 * @param $string
 *   The string to search for.
 *
 * @return
 *   A json array of terms that begin with the provided string.
 *
 * @ingroup tripal_stock_api
 */
function chado_autocomplete_stock($string = '') {
  $items = [];
  $sql = "
    SELECT
      B.stock_id as id, B.uniquename, B.name,
      O.genus, O,species,
      CVT.name as type
    FROM {stock} B
      INNER JOIN {organism} O ON O.organism_id = B.organism_id
      INNER JOIN {cvterm} CVT ON CVT.cvterm_id = B.type_id
    WHERE lower(B.uniquename) like lower(:str) OR lower(B.name) like lower(:str)
    ORDER by B.name
    LIMIT 25 OFFSET 0
  ";
  $records = chado_query($sql, [':str' => $string . '%']);
  while ($r = $records->fetchObject()) {
    $key = "$r->name [id: $r->id]";
    $items[$key] = "$r->name ($r->uniquename, $r->type, $r->genus $r->species)";
  }

  drupal_json_output($items);
}

/**
 * Retrieves a chado stock variable
 *
 * @param $identifier
 *   An array with the key stating what the identifier is. Supported keys (only
 *   one of the following unique keys is required):
 *    - stock_id: the chado stock.stock_id primary key
 *    - nid: the drupal nid of the stock
 *   There are also some specially handled keys. They are:
 *    - property: An array/object describing the property to select records for.
 *      It should at least have either a type_name (if unique across cvs) or
 *      type_id. Other supported keys include: cv_id/cv_name (of the type),
 *      value and rank
 * @param $options
 *   An array of options. Supported keys include:
 *     - Any keys supported by chado_generate_var(). See that function
 *      definition for additional details.
 *
 * NOTE: the $identifier parameter can really be any array similar to $values
 * passed into chado_select_record(). It should fully specify the stock record
 * to be returned.
 *
 * @return
 *   If unique values were passed in as an identifier then an object describing
 *   the stock will be returned (will be a chado variable from
 *   chado_generate_var()). Otherwise, FALSE will be returned.
 *
 * @ingroup tripal_stock_api
 */
function chado_get_stock($identifiers, $options = []) {

  // Set Defaults.
  if (!isset($options['include_fk'])) {
    // Tells chado_generate_var to only expand 1 level.
    $options['include_fk'] = ['type_id' => TRUE, 'dbxref_id' => TRUE];
  }

  // Error Checking of parameters.
  if (!is_array($identifiers)) {
    tripal_report_error(
      'tripal_stock_api',
      TRIPAL_ERROR,
      "chado_get_stock: The identifier passed in is expected to be an array with the key
        matching a column name in the stock table (ie: stock_id or name). You passed in %identifier.",
      [
        '%identifier' => print_r($identifiers, TRUE),
      ]
    );
  }
  elseif (empty($identifiers)) {
    tripal_report_error(
      'tripal_stock_api',
      TRIPAL_ERROR,
      "chado_get_stock: You did not pass in anything to identify the stock you want. The identifier
        is expected to be an array with the key matching a column name in the stock table
        (ie: stock_id or name). You passed in %identifier.",
      [
        '%identifier' => print_r($identifiers, TRUE),
      ]
    );
  }

  // If one of the identifiers is property then use 
  // chado_get_record_with_property().
  if (isset($identifiers['property'])) {
    $property = $identifiers['property'];
    unset($identifiers['property']);
    $stock = chado_get_record_with_property(
      ['table' => 'stock', 'base_records' => $identifiers],
      ['type_name' => $property],
      $options
    );
  }

  // Else we have a simple case and we can just use chado_generate_var to get 
  // the stock.
  else {

    // Try to get the stock.
    $stock = chado_generate_var(
      'stock',
      $identifiers,
      $options
    );
  }

  // Ensure the stock is singular. If it's an array then it is not singular.
  if (is_array($stock)) {
    tripal_report_error(
      'tripal_stock_api',
      TRIPAL_ERROR,
      "chado_get_stock: The identifiers you passed in were not unique. You passed in %identifier.",
      [
        '%identifier' => print_r($identifiers, TRUE),
      ]
    );
  }

  // Report an error if $stock is FALSE since then chado_generate_var has failed.
  elseif ($stock === FALSE) {
    tripal_report_error(
      'tripal_stock_api',
      TRIPAL_ERROR,
      "chado_get_stock: chado_generate_var() failed to return a stock based on the identifiers
        you passed in. You should check that your identifiers are correct, as well as, look
        for a chado_generate_var error for additional clues. You passed in %identifier.",
      [
        '%identifier' => print_r($identifiers, TRUE),
      ]
    );
  }

  // Else, as far we know, everything is fine so give them their stock :)
  else {
    return $stock;
  }
}

/**
 * Retrieves a chado stock variable.
 *
 * @param $identifier
 *   An array with the key stating what the identifier is. Supported keys
 *   include any field in the stock table. See the chado_select_record() $values
 *   parameter for additional details including an example.
 * @param $options
 *   An array of options. Supported keys include:
 *     - Any keys supported by chado_generate_var(). See that function
 *       definition for additional details.
 *
 * @return
 *   An array of stock objects matching the criteria.
 *
 * @ingroup tripal_stock_api
 */
function chado_get_multiple_stocks($identifiers, $options = []) {

  // Set Defaults.
  if (!isset($options['include_fk'])) {
    // Tells chado_generate_var to only expand 1 level.
    $options['include_fk'] = ['type_id' => TRUE, 'dbxref_id' => TRUE];
  }

  // Error Checking of parameters.
  if (!is_array($identifiers)) {
    tripal_report_error(
      'tripal_stock_api',
      TRIPAL_ERROR,
      "chado_get_stock: The identifier passed in is expected to be an array with the key
        matching a column name in the stock table (ie: stock_id or name). You passed in %identifier.",
      [
        '%identifier' => print_r($identifiers, TRUE),
      ]
    );
  }
  elseif (empty($identifiers)) {
    tripal_report_error(
      'tripal_stock_api',
      TRIPAL_ERROR,
      "chado_get_stock: You did not pass in anything to identify the stock you want. The identifier
        is expected to be an array with the key matching a column name in the stock table
        (ie: stock_id or name). You passed in %identifier.",
      [
        '%identifier' => print_r($identifiers, TRUE),
      ]
    );
  }

  // If one of the identifiers is property then use 
  // chado_get_record_with_property().
  if (isset($identifiers['property'])) {
    $property = $identifiers['property'];
    unset($identifiers['property']);
    $stock = chado_get_record_with_property(
      ['table' => 'stock', 'base_records' => $identifiers],
      ['type_name' => $property],
      $options
    );
  }

  // Else we have a simple case and we can just use chado_generate_var to get 
  //the stock.
  else {

    // Try to get the stock.
    $stock = chado_generate_var(
      'stock',
      $identifiers,
      $options
    );
  }

  // Report an error if $stock is FALSE since then chado_generate_var has failed.
  if ($stock === FALSE) {
    tripal_report_error(
      'tripal_stock_api',
      TRIPAL_ERROR,
      "chado_get_stock: chado_generate_var() failed to return a stock based on the identifiers
        you passed in. You should check that your identifiers are correct, as well as, look
        for a chado_generate_var error for additional clues. You passed in %identifier.",
      [
        '%identifier' => print_r($identifiers, TRUE),
      ]
    );
  }

  // Else, as far we know, everything is fine so give them their stock :)
  else {
    return $stock;
  }
}
