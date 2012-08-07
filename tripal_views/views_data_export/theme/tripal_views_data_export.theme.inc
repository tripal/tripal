<?php

/**
 * @file
 * Contains template preprocessing functions
 */

/**
 * Preprocess the variables for the FASTA views data export -body template
 *   - replace the field tokens in the fasta record definition lines
 *
 * @param $vars
 *   The variables available to the template
 */
function template_preprocess_views_data_export_fasta_body(&$vars) {
  _views_data_export_header_shared_preprocess($vars);
  _views_data_export_body_shared_preprocess($vars);
  
  // get export settings
  $defline_tpl = $vars['options']['display']['defline_fields'];

  // iterate through the rows and replace the field tokens with values
  // to generate the definition line  
  foreach ($vars['themed_rows'] as $i => $fields) {
    $defline = $defline_tpl;
    foreach ($fields as $key => $value) {
      $defline = preg_replace("/\[$key\]/", $value, $defline);
    }
    $vars['themed_rows'][$i]['defline'] = $defline;
  }
}
