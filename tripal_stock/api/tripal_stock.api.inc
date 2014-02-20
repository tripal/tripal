<?php
/**
 * @file
 * Provides an application programming interface (API) to manage stocks
 */

/**
 * @defgroup tripal_stock_api Stock Module API
 * @ingroup tripal_api
 * @{
 * Provides an application programming interface (API) to manage stocks
 * @}
 */

/**
 * Retrieves a chado stock variable
 *
 * @param $identifier
 *   An array with the key stating what the identifier is. Supported keys (only one of the
 *   following unique keys is required):
 *    - stock_id: the chado stock.stock_id primary key
 *    - nid: the drupal nid of the stock
 *   There are also some specially handled keys. They are:
 *    - property: An array/object describing the property to select records for. It
 *      should at least have either a type_name (if unique across cvs) or type_id. Other
 *      supported keys include: cv_id/cv_name (of the type), value and rank
 * @param $options
 *   An array of options. Supported keys include:
 *     - Any keys supported by chado_generate_var(). See that function definition for
 *       additional details.
 *
 * NOTE: the $identifier parameter can really be any array similar to $values passed into
 *   chado_select_record(). It should fully specify the stock record to be returned.
 *
 * @return
 *   If unique values were passed in as an identifier then an object describing the stock
 *   will be returned (will be a chado variable from chado_generate_var()). Otherwise,
 *   FALSE will be returned.
 *
 * @ingroup tripal_stock_api
 */
function tripal_get_stock($identifiers, $options = array()) {

  // Set Defaults
  if (!isset($options['include_fk'])) {
    // Tells chado_generate_var to only expand 1 level
    $options['include_fk'] = array('type_id' => TRUE, 'dbxref_id' => TRUE);
  }

  // Error Checking of parameters
  if (!is_array($identifiers)) {
    tripal_report_error(
      'tripal_stock_api',
      TRIPAL_ERROR,
      "tripal_get_stock: The identifier passed in is expected to be an array with the key
        matching a column name in the stock table (ie: stock_id or name). You passed in %identifier.",
      array(
        '%identifier'=> print_r($identifiers, TRUE)
      )
    );
  }
  elseif (empty($identifiers)) {
    tripal_report_error(
      'tripal_stock_api',
      TRIPAL_ERROR,
      "tripal_get_stock: You did not pass in anything to identify the stock you want. The identifier
        is expected to be an array with the key matching a column name in the stock table
        (ie: stock_id or name). You passed in %identifier.",
      array(
        '%identifier'=> print_r($identifiers, TRUE)
      )
    );
  }

  // If one of the identifiers is property then use chado_get_record_with_property()
  if (isset($identifiers['property'])) {
    $property = $identifiers['property'];
    unset($identifiers['property']);
    $stock = chado_get_record_with_property('stock', $property, $identifiers, $options);
  }

  // Else we have a simple case and we can just use chado_generate_var to get the stock
  else {

    // Try to get the stock
    $stock = chado_generate_var(
      'stock',
      $identifiers,
      $options
    );
  }

  // Ensure the stock is singular. If it's an array then it is not singular
  if (is_array($stock)) {
    tripal_report_error(
      'tripal_stock_api',
      TRIPAL_ERROR,
      "tripal_get_stock: The identifiers you passed in were not unique. You passed in %identifier.",
      array(
        '%identifier'=> print_r($identifiers, TRUE)
      )
    );
  }

  // Report an error if $stock is FALSE since then chado_generate_var has failed
  elseif ($stock === FALSE) {
    tripal_report_error(
      'tripal_stock_api',
      TRIPAL_ERROR,
      "tripal_get_stock: chado_generate_var() failed to return a stock based on the identifiers
        you passed in. You should check that your identifiers are correct, as well as, look
        for a chado_generate_var error for additional clues. You passed in %identifier.",
      array(
        '%identifier'=> print_r($identifiers, TRUE)
      )
    );
  }

  // Else, as far we know, everything is fine so give them their stock :)
  else {
    return $stock;
  }
}

/**
 * Retrieves a chado stock variable
 *
 * @param $identifier
 *   An array with the key stating what the identifier is. Supported keys include any
 *   field in the stock table. See the chado_select_record() $values parameter for
 *   additional details including an example.
 * @param $options
 *   An array of options. Supported keys include:
 *     - Any keys supported by chado_generate_var(). See that function definition for
 *       additional details.
 *
 * @return
 *   An array of stock objects matching the criteria.
 *
 * @ingroup tripal_stock_api
 */
function tripal_get_multiple_stocks($identifiers, $options = array()) {

  // Set Defaults
  if (!isset($options['include_fk'])) {
    // Tells chado_generate_var to only expand 1 level
    $options['include_fk'] = array('type_id' => TRUE, 'dbxref_id' => TRUE);
  }

  // Error Checking of parameters
  if (!is_array($identifiers)) {
    tripal_report_error(
      'tripal_stock_api',
      TRIPAL_ERROR,
      "tripal_get_stock: The identifier passed in is expected to be an array with the key
        matching a column name in the stock table (ie: stock_id or name). You passed in %identifier.",
      array(
        '%identifier'=> print_r($identifiers, TRUE)
      )
    );
  }
  elseif (empty($identifiers)) {
    tripal_report_error(
      'tripal_stock_api',
      TRIPAL_ERROR,
      "tripal_get_stock: You did not pass in anything to identify the stock you want. The identifier
        is expected to be an array with the key matching a column name in the stock table
        (ie: stock_id or name). You passed in %identifier.",
      array(
        '%identifier'=> print_r($identifiers, TRUE)
      )
    );
  }

  // If one of the identifiers is property then use chado_get_record_with_property()
  if (isset($identifiers['property'])) {
    $property = $identifiers['property'];
    unset($identifiers['property']);
    $stock = chado_get_record_with_property('stock', $property, $identifiers, $options);
  }

  // Else we have a simple case and we can just use chado_generate_var to get the stock
  else {

    // Try to get the stock
    $stock = chado_generate_var(
      'stock',
      $identifiers,
      $options
    );
  }

  // Report an error if $stock is FALSE since then chado_generate_var has failed
  if ($stock === FALSE) {
    tripal_report_error(
      'tripal_stock_api',
      TRIPAL_ERROR,
      "tripal_get_stock: chado_generate_var() failed to return a stock based on the identifiers
        you passed in. You should check that your identifiers are correct, as well as, look
        for a chado_generate_var error for additional clues. You passed in %identifier.",
      array(
        '%identifier'=> print_r($identifiers, TRUE)
      )
    );
  }

  // Else, as far we know, everything is fine so give them their stock :)
  else {
    return $stock;
  }
}
