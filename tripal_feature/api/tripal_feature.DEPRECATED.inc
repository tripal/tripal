<?php
/**
 * @file
 * Wrapper functions to provide backwards compatibility for the tripal feature api
 */

/**
 * @deprecated Restructured API to make naming more readable and consistent.
 * Function was deprecated in Tripal 2.0 and will be removed 2 releases from now.
 * This function has been replaced by chado_get_property().
 *
 * @see chado_get_property().
 */
function tripal_feature_analysis_get_property($analysis_id = NULL, $feature_id = NUll, $analysisfeature_id = NULL, $property, $cv_name = 'tripal') {

  tripal_report_error(
    'tripal_api',
    TRIPAL_NOTICE,
    "DEPRECATED: %old_function has been replaced with %new_function. Please update your code.",
    array(
      '%old_function'=>'tripal_feature_analysis_get_property',
      '%new_function' => 'chado_get_property'
    )
  );

  return FALSE;
}

/**
 * @deprecated Restructured API to make naming more readable and consistent.
 * Function was deprecated in Tripal 2.0 and will be removed 2 releases from now.
 * This function has been replaced by chado_insert_property().
 *
 * @see chado_insert_property().
 */
function tripal_feature_analysis_insert_property($analysis_id = NULL, $feature_id = NUll, $analysisfeature_id = NULL, $property, $value, $update_if_present = 0, $cv_name = 'tripal') {

  tripal_report_error(
    'tripal_api',
    TRIPAL_NOTICE,
    "DEPRECATED: %old_function has been replaced with %new_function. Please update your code.",
    array(
      '%old_function'=>'tripal_feature_analysis_insert_property',
      '%new_function' => 'chado_insert_property'
    )
  );

  return FALSE;
}

/**
 * @deprecated Restructured API to make naming more readable and consistent.
 * Function was deprecated in Tripal 2.0 and will be removed 2 releases from now.
 * This function has been replaced by chado_update_property().
 *
 * @see chado_update_property().
 */
function tripal_feature_analysis_update_property($analysis_id = NULL, $feature_id = NUll, $analysisfeature_id = NULL, $property, $value, $insert_if_missing = 0, $cv_name = 'tripal') {

  tripal_report_error(
    'tripal_api',
    TRIPAL_NOTICE,
    "DEPRECATED: %old_function has been replaced with %new_function. Please update your code.",
    array(
      '%old_function'=>'tripal_feature_analysis_update_property',
      '%new_function' => 'chado_update_property'
    )
  );

  return FALSE;
}

/**
 * @deprecated Restructured API to make naming more readable and consistent.
 * Function was deprecated in Tripal 2.0 and will be removed 2 releases from now.
 * This function has been replaced by chado_update_property().
 *
 * @see chado_update_property().
 */
function tripal_feature_analysis_update_property_by_id($analysisfeatureprop_id, $property, $value, $cv_name = 'tripal') {

  tripal_report_error(
    'tripal_api',
    TRIPAL_NOTICE,
    "DEPRECATED: %old_function has been replaced with %new_function. Please update your code.",
    array(
      '%old_function'=>'tripal_feature_analysis_update_property_by_id',
      '%new_function' => 'chado_update_property'
    )
  );

  return FALSE;
}

/**
 * @deprecated Restructured API to make naming more readable and consistent.
 * Function was deprecated in Tripal 2.0 and will be removed 2 releases from now.
 * This function has been replaced by chado_delete_property().
 *
 * @see chado_delete_property().
 */
function tripal_feature_analysis_delete_property($analysis_id = NULL, $feature_id = NUll, $analysisfeature_id = NULL, $property, $cv_name = 'tripal') {

  tripal_report_error(
    'tripal_api',
    TRIPAL_NOTICE,
    "DEPRECATED: %old_function has been replaced with %new_function. Please update your code.",
    array(
      '%old_function'=>'tripal_feature_analysis_delete_property',
      '%new_function' => 'chado_delete_property'
    )
  );

  return FALSE;
}

/**
 * @deprecated Restructured API to make naming more readable and consistent.
 * Function was deprecated in Tripal 2.0 and will be removed 2 releases from now.
 * This function has been replaced by chado_delete_property().
 *
 * @see chado_delete_property().
 */
function tripal_feature_analysis_delete_property_by_id($analysisfeatureprop_id) {

  tripal_report_error(
    'tripal_api',
    TRIPAL_NOTICE,
    "DEPRECATED: %old_function has been replaced with %new_function. Please update your code.",
    array(
      '%old_function'=>'tripal_feature_analysis_delete_property_by_id',
      '%new_function' => 'chado_delete_property'
    )
  );

  return FALSE;
}

/**
 * @deprecated Restructured API to make naming more readable and consistent.
 * Function was deprecated in Tripal 2.0 and will be removed 2 releases from now.
 * This function has been replaced by chado_get_property().
 *
 * @see chado_get_property().
 */
function tripal_feature_get_property($feature_id, $property, $cv_name='tripal') {

  tripal_report_error(
    'tripal_api',
    TRIPAL_NOTICE,
    "DEPRECATED: %old_function has been replaced with %new_function. Please update your code.",
    array(
      '%old_function'=>'tripal_feature_get_property',
      '%new_function' => 'chado_get_property'
    )
  );

  return FALSE;
}

/**
 * @deprecated Restructured API to make naming more readable and consistent.
 * Function was deprecated in Tripal 2.0 and will be removed 2 releases from now.
 * This function has been replaced by chado_insert_property().
 *
 * @see chado_insert_property().
 */
function tripal_feature_insert_property($feature_id, $property, $value, $update_if_present = 0, $cv_name = 'tripal') {

  tripal_report_error(
    'tripal_api',
    TRIPAL_NOTICE,
    "DEPRECATED: %old_function has been replaced with %new_function. Please update your code.",
    array(
      '%old_function'=>'tripal_feature_insert_property',
      '%new_function' => 'chado_insert_property'
    )
  );

  return FALSE;
}

/**
 * @deprecated Restructured API to make naming more readable and consistent.
 * Function was deprecated in Tripal 2.0 and will be removed 2 releases from now.
 * This function has been replaced by chado_update_property().
 *
 * @see chado_update_property().
 */
function tripal_feature_update_property($feature_id, $property, $value, $insert_if_missing = 0, $cv_name = 'tripal') {

  tripal_report_error(
    'tripal_api',
    TRIPAL_NOTICE,
    "DEPRECATED: %old_function has been replaced with %new_function. Please update your code.",
    array(
      '%old_function'=>'tripal_feature_update_property',
      '%new_function' => 'chado_update_property'
    )
  );

  return FALSE;
}

/**
 * @deprecated Restructured API to make naming more readable and consistent.
 * Function was deprecated in Tripal 2.0 and will be removed 2 releases from now.
 * This function has been replaced by chado_update_property().
 *
 * @see chado_update_property().
 */
function tripal_feature_update_property_by_id($featureprop_id, $property, $value, $cv_name = 'tripal') {

  tripal_report_error(
    'tripal_api',
    TRIPAL_NOTICE,
    "DEPRECATED: %old_function has been replaced with %new_function. Please update your code.",
    array(
      '%old_function'=>'tripal_feature_update_property_by_id',
      '%new_function' => 'chado_update_property'
    )
  );

  return FALSE;
}

/**
 * @deprecated Restructured API to make naming more readable and consistent.
 * Function was deprecated in Tripal 2.0 and will be removed 2 releases from now.
 * This function has been replaced by chado_delete_property().
 *
 * @see chado_delete_property().
 */
function tripal_feature_delete_property($feature_id, $property, $cv_name='tripal') {

  tripal_report_error(
    'tripal_api',
    TRIPAL_NOTICE,
    "DEPRECATED: %old_function has been replaced with %new_function. Please update your code.",
    array(
      '%old_function'=>'tripal_feature_delete_property',
      '%new_function' => 'chado_delete_property'
    )
  );

  return FALSE;
}

/**
 * @deprecated Restructured API to make naming more readable and consistent.
 * Function was deprecated in Tripal 2.0 and will be removed 2 releases from now.
 * This function has been replaced by chado_delete_property().
 *
 * @see chado_delete_property().
 */
function tripal_feature_delete_property_by_id($featureprop_id) {

  tripal_report_error(
    'tripal_api',
    TRIPAL_NOTICE,
    "DEPRECATED: %old_function has been replaced with %new_function. Please update your code.",
    array(
      '%old_function'=>'tripal_feature_delete_property_by_id',
      '%new_function' => 'chado_delete_property'
    )
  );

  return FALSE;
}

/**
 * @deprecated Restructured API to make naming more readable and consistent.
 * Function was deprecated in Tripal 2.0 and will be removed 2 releases from now.
 * This function has been replaced by feature_sequence_reverse_complement().
 *
 * @see feature_sequence_reverse_complement().
 */
function tripal_feature_reverse_complement($sequence) {

  tripal_report_error(
    'tripal_api',
    TRIPAL_NOTICE,
    "DEPRECATED: %old_function has been replaced with %new_function. Please update your code.",
    array(
      '%old_function'=>'tripal_feature_reverse_complement',
      '%new_function' => 'feature_sequence_reverse_complement'
    )
  );

  return FALSE;
}

/**
 * @deprecated Restructured API to make naming more readable and consistent.
 * Function was deprecated in Tripal 2.0 and will be removed 2 releases from now.
 * This function has been replaced by feature_sequence_get_formatted().
 *
 * @see feature_sequence_get_formatted().
 */
function tripal_feature_get_formatted_sequence($feature_id, $feature_name, $num_bases_per_line, $derive_from_parent, $aggregate, $output_format, $upstream, $downstream, $sub_features = array(), $relationship = '', $rel_part = '') {

  tripal_report_error(
    'tripal_api',
    TRIPAL_NOTICE,
    "DEPRECATED: %old_function has been replaced with %new_function. Please update your code.",
    array(
      '%old_function'=>'tripal_feature_get_formatted_sequence',
      '%new_function' => 'feature_sequence_get_formatted'
    )
  );

  return FALSE;
}

/**
 * @deprecated Restructured API to make naming more readable and consistent.
 * Function was deprecated in Tripal 2.0 and will be removed 2 releases from now.
 * This function has been replaced by chado_associate_dbxref().
 *
 * @see chado_associate_dbxref().
 */
function tripal_feature_add_dbxref($feature_id, $dbname, $accession) {

  tripal_report_error(
    'tripal_api',
    TRIPAL_NOTICE,
    "DEPRECATED: %old_function has been replaced with %new_function. Please update your code.",
    array(
      '%old_function'=>'tripal_feature_add_dbxref',
      '%new_function' => 'chado_associate_dbxref'
    )
  );

  return FALSE;
}

/**
 * @deprecated Restructured API to make naming more readable and consistent.
 * Function was deprecated in Tripal 2.0 and will be removed 2 releases from now.
 * This function has been replaced by chado_associate_cvterm().
 *
 * @see chado_associate_cvterm().
 */
function tripal_feature_add_cvterm($feature_id, $cvname, $cvterm) {

  tripal_report_error(
    'tripal_api',
    TRIPAL_NOTICE,
    "DEPRECATED: %old_function has been replaced with %new_function. Please update your code.",
    array(
      '%old_function'=>'tripal_feature_add_cvterm',
      '%new_function' => 'chado_associate_cvterm'
    )
  );

  return FALSE;
}

/**
 * @deprecated Restructured API to make naming more readable and consistent.
 * Function was deprecated in Tripal 2.0 and will be removed 2 releases from now.
 * This function has been replaced by feature_sequence_get_fasta().
 *
 * @see feature_sequence_get_fasta().
 */
function tripal_feature_return_fasta($feature, $desc) {

  tripal_report_error(
    'tripal_api',
    TRIPAL_NOTICE,
    "DEPRECATED: %old_function has been replaced with %new_function. Please update your code.",
    array(
      '%old_function'=>'tripal_feature_return_fasta',
      '%new_function' => 'feature_sequence_get_fasta'
    )
  );

  return FALSE;
}
