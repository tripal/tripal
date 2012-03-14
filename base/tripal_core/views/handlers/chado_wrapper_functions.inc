<?php

/**
 * Splits an SQL array of results in a single field
 * into a php array
 *
 * @param $field
 *   An SQL array (ie: {"",^(.*)$,646,tripal_analysis_blast} )
 * @return
 *   A PHP version of the SQL array (ie: array('','^(.*)$','646','tripal_analysis_blast') )
 */
function chado_wrapper_split_array_agg_results($handler, $field) {
  
  if(preg_match('/^{(.*)}$/',$field, $matches)) {
    return str_getcsv($matches[1]);
  } else {
    return array();
  }
}
