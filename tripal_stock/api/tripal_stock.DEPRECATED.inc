<?php
/**
 * @file
 * Wrapper functions to provide backwards compatibility for the tripal stock api
 */

/**
 * @deprecated Restructured API to make naming more readable and consistent.
 * Function was deprecated in Tripal 2.0 and will be removed 2 releases from now.
 * This function has been replaced by chado_get_stock().
 *
 * @see chado_get_stock().
 */
function tripal_stock_get_stock_by_nid($nid) {

  tripal_report_error(
    'tripal_api',
    TRIPAL_NOTICE,
    "DEPRECATED: %old_function has been replaced with %new_function. Please update your code.",
    array(
      '%old_function'=>'tripal_stock_get_stock_by_nid',
      '%new_function' => 'chado_get_stock'
    )
  );

  return FALSE;
}

/**
 * @deprecated Restructured API to make naming more readable and consistent.
 * Function was deprecated in Tripal 2.0 and will be removed 2 releases from now.
 * This function has been replaced by chado_get_stock().
 *
 * @see chado_get_stock().
 */
function tripal_stock_get_stock_by_stock_id($stock_id) {

  tripal_report_error(
    'tripal_api',
    TRIPAL_NOTICE,
    "DEPRECATED: %old_function has been replaced with %new_function. Please update your code.",
    array(
      '%old_function'=>'tripal_stock_get_stock_by_stock_id',
      '%new_function' => 'chado_get_stock'
    )
  );

  return FALSE;
}

/**
 * @deprecated Restructured API to make naming more readable and consistent.
 * Function was deprecated in Tripal 2.0 and will be removed 2 releases from now.
 * This function has been replaced by chado_get_multiple_stocks().
 *
 * @see chado_get_multiple_stocks().
 */
function tripal_stock_get_all_stocks() {

  tripal_report_error(
    'tripal_api',
    TRIPAL_NOTICE,
    "DEPRECATED: %old_function has been replaced with %new_function. Please update your code.",
    array(
      '%old_function'=>'tripal_stock_get_all_stocks',
      '%new_function' => 'chado_get_multiple_stocks'
    )
  );

  return FALSE;
}

/**
 * @deprecated Restructured API to make naming more readable and consistent.
 * Function was deprecated in Tripal 2.0 and will be removed 2 releases from now.
 * This function has been replaced by chado_get_multiple_stocks().
 *
 * @see chado_get_multiple_stocks().
 */
function tripal_stock_get_stocks($values) {

  tripal_report_error(
    'tripal_api',
    TRIPAL_NOTICE,
    "DEPRECATED: %old_function has been replaced with %new_function. Please update your code.",
    array(
      '%old_function'=>'tripal_stock_get_stocks',
      '%new_function' => 'chado_get_multiple_stocks'
    )
  );

  return FALSE;
}

/**
 * @deprecated Restructured API to make naming more readable and consistent.
 * Function was deprecated in Tripal 2.0 and will be removed 2 releases from now.
 * This function has been replaced by chado_get_stock().
 *
 * @see chado_get_stock().
 */
function tripal_stock_get_stocks_by_stockprop($stockprop_values, $stock_values) {

  tripal_report_error(
    'tripal_api',
    TRIPAL_NOTICE,
    "DEPRECATED: %old_function has been replaced with %new_function. Please update your code.",
    array(
      '%old_function'=>'tripal_stock_get_stocks_by_stockprop',
      '%new_function' => 'chado_get_stock'
    )
  );

  return FALSE;
}

/**
 * @deprecated Restructured API to make naming more readable and consistent.
 * Function was deprecated in Tripal 2.0 and will be removed 2 releases from now.
 * This function has been replaced by chado_get_stock().
 *
 * @see chado_get_stock().
 */
function tripal_stock_get_stock_by_name_identifier($name, $organism_id) {

  tripal_report_error(
    'tripal_api',
    TRIPAL_NOTICE,
    "DEPRECATED: %old_function has been replaced with %new_function. Please update your code.",
    array(
      '%old_function'=>'tripal_stock_get_stock_by_name_identifier',
      '%new_function' => 'chado_get_stock'
    )
  );

  return FALSE;
}