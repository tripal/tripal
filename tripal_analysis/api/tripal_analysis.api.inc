<?php
/**
 * @file
 * Provides functions for managing analysis'.
 *
 * @ingroup tripal_analysis
 */

/**
 * @defgroup tripal_analysis_api Analysis Module API
 * @ingroup tripal_api
 * @{
 * Provides an interface for specialized analysis modules to extend the basic functionality.
 * @}
 */

/**
 * Register tripal_analysis_api sub-modules
 *
 * @param $modulename
 *  The name of the module to be registered as a tripal analysis submodule
 *
 * @ingroup tripal_analysis_api
 */
function tripal_register_analysis_child($modulename) {
  $sql = "SELECT * FROM {tripal_analysis} WHERE modulename = :modname";
  if (!db_query($sql, array(':modname' => $modulename))->fetchField()) {
    $sql = "INSERT INTO {tripal_analysis} (modulename) VALUES (:modname)";
    db_query($sql, array(':modname' => $modulename));
  }
}

/**
 * Un-register a tripal analysis sub-module
 *
 * @param $modulename
 *  The name of the module to un-register
 *
 * @ingroup tripal_analysis_api
 */
function tripal_unregister_analysis_child($modulename) {
  if (db_table_exists('tripal_analysis')) {
      $sql = "DELETE FROM {tripal_analysis} WHERE modulename = :modname";
      db_query($sql, array(':modname' => $modulename));
  }
}

/**
 * Retrieves an chado analysis variable
 *
 * @param $itentifier
 *   an array with the key stating what the identifier is. Supported keys (only on of the
 *   following unique keys is required):
 *    - analysis_id: the chado analysis.analysis_id primary key
 *    - nid: the drupal node.nid primary key
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
 *   the analysis node matching the passed in identifier
 *
 * @ingroup tripal_analysis_api
 */
function tripal_get_analysis($identifier, $options) {

  // Set Defaults
  if (!isset($options['include_fk'])) {
    // Tells chado_generate_var not to follow any foreign keys
    $options['include_fk'] = array();
  }

  // Error Checking of parameters
  if (!is_array($identifiers)) {
    tripal_report_error(
      'tripal_stock_api',
      TRIPAL_ERROR,
      "chado_get_stock: The identifier passed in is expected to be an array with the key
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
      "chado_get_stock: You did not pass in anything to identify the stock you want. The identifier
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
    $analysis = chado_get_record_with_property('analysis', $property, $identifiers, $options);
  }

  // Else we have a simple case and we can just use chado_generate_var to get the analysis
  else {

    // Try to get the analysis
    $analysis = chado_generate_var(
      'analysis',
      $identifiers,
      $options
    );
  }

  // Ensure the analysis is singular. If it's an array then it is not singular
  if (is_array($analysis)) {
    tripal_report_error(
      'tripal_analysis_api',
      TRIPAL_ERROR,
      "tripal_get_analysis: The identifiers you passed in were not unique. You passed in %identifier.",
      array(
        '%identifier'=> print_r($identifiers, TRUE)
      )
    );
  }

  // Report an error if $analysis is FALSE since then chado_generate_var has failed
  elseif ($analysis === FALSE) {
    tripal_report_error(
      'tripal_analysis_api',
      TRIPAL_ERROR,
      "tripal_get_analysis: chado_generate_var() failed to return a analysis based on the identifiers
        you passed in. You should check that your identifiers are correct, as well as, look
        for a chado_generate_var error for additional clues. You passed in %identifier.",
      array(
        '%identifier'=> print_r($identifiers, TRUE)
      )
    );
  }

  // Else, as far we know, everything is fine so give them their analysis :)
  else {
    return $analysis;
  }
}