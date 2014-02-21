<?php
/**
 * @file
 * Contains all the deprecated functions to keep the tripal api backwards compatible.
 */

/**
 * @deprecated Restructured API to make naming more readable and consistent.
 * Function was deprecated in Tripal 2.0 and will be removed 2 releases from now.
 * This function has been replaced by chado_get_id_from_nid().
 *
 * @see chado_get_id_from_nid()
 */
function chado_get_id_for_node($table, $node) {

  tripal_report_error(
    'tripal_deprecated',
    TRIPAL_NOTICE,
    "DEPRECATED: %old_function has been replaced with %new_function. The arguments have been changed slightly (ie: $nid instead of $node). Please update your code.",
    array(
      '%old_function'=>'chado_get_id_for_node',
      '%new_function' => 'chado_get_id_from_nid'
    )
  );

  //New API Function
  return chado_get_id_from_nid($table, $node->nid);
}

/**
 * @deprecated Restructured API to make naming more readable and consistent.
 * Function was deprecated in Tripal 2.0 and will be removed 2 releases from now.
 * This function has been replaced by chado_get_nid_from_id().
 *
 * @see chado_get_nid_from_id().
 */
function chado_get_node_id($table, $id) {

  tripal_report_error(
    'tripal_deprecated',
    TRIPAL_NOTICE,
    "DEPRECATED: %old_function has been replaced with %new_function. Please update your code.",
    array(
      '%old_function'=>'chado_get_node_id',
      '%new_function' => 'chado_get_nid_from_id'
    )
  );

  //New API Function
  return chado_get_nid_from_id($table, $id);
}

/**
 * @deprecated Restructured API to make naming more readable and consistent.
 * Function was deprecated in Tripal 2.0 and will be removed 2 releases from now.
 * This function has been replaced by chado_cleanup_orphaned_nodes().
 *
 * @see chado_cleanup_orphaned_nodes().
 */
function tripal_core_chado_node_cleanup_orphaned($table, $job_id = NULL) {

  tripal_report_error(
    'tripal_deprecated',
    TRIPAL_NOTICE,
    "DEPRECATED: %old_function has been replaced with %new_function. Please update your code.",
    array(
      '%old_function'=>'tripal_core_chado_node_cleanup_orphaned',
      '%new_function' => 'chado_cleanup_orphaned_nodes'
    )
  );

  //New API Function
  return chado_cleanup_orphaned_nodes($table, $job_id);
}

/**
 * @deprecated Restructured API to make naming more readable and consistent.
 * Function was deprecated in Tripal 2.0 and will be removed 2 releases from now.
 * This function has been replaced by chado_add_node_form_dbxrefs().
 *
 * @see chado_add_node_form_dbxrefs().
 */
function chado_node_additional_dbxrefs_form(&$form, &$form_state, $details) {

  tripal_report_error(
    'tripal_deprecated',
    TRIPAL_NOTICE,
    "DEPRECATED: %old_function has been replaced with %new_function. Please update your code.",
    array(
      '%old_function'=>'chado_node_additional_dbxrefs_form',
      '%new_function' => 'chado_add_node_form_dbxrefs'
    )
  );

  //New API Function
  return chado_add_node_form_dbxrefs($form, $form_state, $details);
}

/**
 * @deprecated Restructured API to make naming more readable and consistent.
 * Function was deprecated in Tripal 2.0 and will be removed 2 releases from now.
 * This function has been replaced by chado_retrieve_node_form_dbxrefs().
 *
 * @see chado_retrieve_node_form_dbxrefs().
 */
function chado_node_additional_dbxrefs_form_retreive($node) {

  tripal_report_error(
    'tripal_deprecated',
    TRIPAL_NOTICE,
    "DEPRECATED: %old_function has been replaced with %new_function. Please update your code.",
    array(
      '%old_function'=>'chado_node_additional_dbxrefs_form_retreive',
      '%new_function' => 'chado_retrieve_node_form_dbxrefs'
    )
  );

  //New API Function
  return chado_retrieve_node_form_dbxrefs($node);
}

/**
 * @deprecated Restructured API to make naming more readable and consistent.
 * Function was deprecated in Tripal 2.0 and will be removed 2 releases from now.
 * This function has been replaced by chado_update_node_form_dbxrefs().
 *
 * @see chado_update_node_form_dbxrefs().
 */
function chado_node_additional_dbxrefs_form_update_dbxrefs($node, $details, $retrieved_dbxrefs = FALSE) {

  tripal_report_error(
    'tripal_deprecated',
    TRIPAL_NOTICE,
    "DEPRECATED: %old_function has been replaced with %new_function. Please update your code.",
    array(
      '%old_function'=>'chado_node_additional_dbxrefs_form_update_dbxrefs',
      '%new_function' => 'chado_update_node_form_dbxrefs'
    )
  );

  //New API Function
  return chado_update_node_form_dbxrefs($node, $details, $retrieved_dbxrefs);
}

/**
 * @deprecated Restructured API to make naming more readable and consistent.
 * Function was deprecated in Tripal 2.0 and will be removed 2 releases from now.
 * This function has been replaced by chado_add_node_form_properties().
 *
 * @see chado_add_node_form_properties().
 */
function chado_node_properties_form($form, $form_state, $details) {

  tripal_report_error(
    'tripal_deprecated',
    TRIPAL_NOTICE,
    "DEPRECATED: %old_function has been replaced with %new_function. Please update your code.",
    array(
      '%old_function'=>'chado_node_properties_form',
      '%new_function' => 'chado_add_node_form_properties'
    )
  );

  //New API Function
  return chado_add_node_form_properties($form, $form_state, $details);
}

/**
 * @deprecated Restructured API to make naming more readable and consistent.
 * Function was deprecated in Tripal 2.0 and will be removed 2 releases from now.
 * This function has been replaced by chado_retrieve_node_form_properties().
 *
 * @see chado_retrieve_node_form_properties().
 */
function chado_node_properties_form_retreive($node) {

  tripal_report_error(
    'tripal_deprecated',
    TRIPAL_NOTICE,
    "DEPRECATED: %old_function has been replaced with %new_function. Please update your code.",
    array(
      '%old_function'=>'chado_node_properties_form_retreive',
      '%new_function' => 'chado_retrieve_node_form_properties'
    )
  );

  //New API Function
  return chado_retrieve_node_form_properties($node);
}

/**
 * @deprecated Restructured API to make naming more readable and consistent.
 * Function was deprecated in Tripal 2.0 and will be removed 2 releases from now.
 * This function has been replaced by chado_update_node_form_properties().
 *
 * @see chado_update_node_form_properties().
 */
function chado_node_properties_form_update_properties($node, $details, $retrieved_properties = FALSE) {

  tripal_report_error(
    'tripal_deprecated',
    TRIPAL_NOTICE,
    "DEPRECATED: %old_function has been replaced with %new_function. Please update your code.",
    array(
      '%old_function'=>'chado_node_properties_form_update_properties',
      '%new_function' => 'chado_update_node_form_properties'
    )
  );

  //New API Function
  return chado_update_node_form_properties($node, $details, $retrieved_properties);
}

/**
 * @deprecated Restructured API to make naming more readable and consistent.
 * Function was deprecated in Tripal 2.0 and will be removed 2 releases from now.
 * This function has been replaced by chado_add_node_form_relationships().
 *
 * @see chado_add_node_form_relationships().
 */
function chado_node_relationships_form(&$form, &$form_state, $details) {

  tripal_report_error(
    'tripal_deprecated',
    TRIPAL_NOTICE,
    "DEPRECATED: %old_function has been replaced with %new_function. Please update your code.",
    array(
      '%old_function'=>'chado_node_relationships_form',
      '%new_function' => 'chado_add_node_form_relationships'
    )
  );

  //New API Function
  return chado_add_node_form_relationships($form, $form_state, $details);
}

/**
 * @deprecated Restructured API to make naming more readable and consistent.
 * Function was deprecated in Tripal 2.0 and will be removed 2 releases from now.
 * This function has been replaced by chado_retrieve_node_form_relationships().
 *
 * @see chado_retrieve_node_form_relationships().
 */
function chado_node_relationships_form_retreive($node) {

  tripal_report_error(
    'tripal_deprecated',
    TRIPAL_NOTICE,
    "DEPRECATED: %old_function has been replaced with %new_function. Please update your code.",
    array(
      '%old_function'=>'chado_node_relationships_form_retreive',
      '%new_function' => 'chado_retrieve_node_form_relationships'
    )
  );

  //New API Function
  return chado_retrieve_node_form_relationships($node);
}

/**
 * @deprecated Restructured API to make naming more readable and consistent.
 * Function was deprecated in Tripal 2.0 and will be removed 2 releases from now.
 * This function has been replaced by chado_update_node_form_relationships().
 *
 * @see chado_update_node_form_relationships().
 */
function chado_node_relationships_form_update_relationships($node, $details, $retrieved_relationships = FALSE) {

  tripal_report_error(
    'tripal_deprecated',
    TRIPAL_NOTICE,
    "DEPRECATED: %old_function has been replaced with %new_function. Please update your code.",
    array(
      '%old_function'=>'chado_node_relationships_form_update_relationships',
      '%new_function' => 'chado_update_node_form_relationships'
    )
  );

  //New API Function
  return chado_update_node_form_relationships($node, $details, $retrieved_relationships);
}

/**
 * @deprecated Restructured API to make naming more readable and consistent.
 * Function was deprecated in Tripal 2.0 and will be removed 2 releases from now.
 * This function has been replaced by chado_node_sync_form().
 *
 * @see chado_node_sync_form().
 */
function tripal_core_chado_node_sync_form($form, &$form_state) {

  tripal_report_error(
    'tripal_deprecated',
    TRIPAL_NOTICE,
    "DEPRECATED: %old_function has been replaced with %new_function. Please update your code.",
    array(
      '%old_function'=>'tripal_core_chado_node_sync_form',
      '%new_function' => 'chado_node_sync_form'
    )
  );

  //New API Function
  return chado_node_sync_form($form, $form_state);
}

/**
 * @deprecated Restructured API to make naming more readable and consistent.
 * Function was deprecated in Tripal 2.0 and will be removed 2 releases from now.
 * This function has been replaced by chado_node_sync_records().
 *
 * @see chado_node_sync_records().
 */
function tripal_core_chado_node_sync_records($base_table, $max_sync = FALSE, $organism_id = FALSE,
    $types = array(), $ids = array(), $job_id = NULL) {

  tripal_report_error(
    'tripal_deprecated',
    TRIPAL_NOTICE,
    "DEPRECATED: %old_function has been replaced with %new_function. Please update your code.",
    array(
      '%old_function'=>'tripal_core_chado_node_sync_records',
      '%new_function' => 'chado_node_sync_records'
    )
  );

  //New API Function
  return chado_node_sync_records($base_table, $max_sync, $organism_id, $types, $ids, $job_id);
}

/**
 * @deprecated Restructured API to make naming more readable and consistent.
 * Function was deprecated in Tripal 2.0 and will be removed 2 releases from now.
 * There is no new functionality as this function was deamed unnecessary.
 */
function tripal_core_is_tripal_node_type() {

  tripal_report_error(
    'tripal_deprecated',
    TRIPAL_ERROR,
    "DEPRECATED: %old_function has been completely removed since we felt there wasn't a real need for it.",
    array(
      '%old_function'=>'tripal_core_is_tripal_node_type'
    )
  );

}

/**
 * @deprecated Restructured API to make naming more readable and consistent.
 * Function was deprecated in Tripal 2.0 and will be removed 2 releases from now.
 * This function has been replaced by chado_schema_get_foreign_key().
 *
 * @see chado_schema_get_foreign_key().
 */
function tripal_core_chado_get_foreign_key($table_desc, $field, $values, $options = NULL) {

  tripal_report_error(
    'tripal_deprecated',
    TRIPAL_NOTICE,
    "DEPRECATED: %old_function has been replaced with %new_function. Please update your code.",
    array(
      '%old_function'=>'tripal_core_chado_get_foreign_key',
      '%new_function' => 'chado_schema_get_foreign_key'
    )
  );

  //New API Function
  return chado_schema_get_foreign_key($table_desc, $field, $values, $options);
}

/**
 * @deprecated Restructured API to make naming more readable and consistent.
 * Function was deprecated in Tripal 2.0 and will be removed 2 releases from now.
 * This function has been replaced by chado_delete_record().
 *
 * @see chado_delete_record().
 */
function tripal_core_chado_delete($table, $match, $options = NULL) {

  tripal_report_error(
    'tripal_deprecated',
    TRIPAL_NOTICE,
    "DEPRECATED: %old_function has been replaced with %new_function. Please update your code.",
    array(
      '%old_function'=>'tripal_core_chado_delete',
      '%new_function' => 'chado_delete_record'
    )
  );

  //New API Function
  return chado_delete_record($table, $match, $options);
}

/**
 * @deprecated Restructured API to make naming more readable and consistent.
 * Function was deprecated in Tripal 2.0 and will be removed 2 releases from now.
 * This function has been replaced by chado_expand_var().
 *
 * @see chado_expand_var().
 */
function tripal_core_expand_chado_vars($object, $type, $to_expand, $table_options = array()) {

  tripal_report_error(
    'tripal_deprecated',
    TRIPAL_NOTICE,
    "DEPRECATED: %old_function has been replaced with %new_function. Please update your code.",
    array(
      '%old_function'=>'tripal_core_expand_chado_vars',
      '%new_function' => 'chado_expand_var'
    )
  );

  //New API Function
  return chado_expand_var($object, $type, $to_expand, $table_options);
}

/**
 * @deprecated Restructured API to make naming more readable and consistent.
 * Function was deprecated in Tripal 2.0 and will be removed 2 releases from now.
 * This function has been replaced by chado_generate_var().
 *
 * @see chado_generate_var().
 */
function tripal_core_generate_chado_var($table, $values, $base_options = array()) {

  tripal_report_error(
    'tripal_deprecated',
    TRIPAL_NOTICE,
    "DEPRECATED: %old_function has been replaced with %new_function. Please update your code.",
    array(
      '%old_function'=>'tripal_core_generate_chado_var',
      '%new_function' => 'chado_generate_var'
    )
  );

  //New API Function
  return chado_generate_var($table, $values, $base_options);
}

/**
 * @deprecated Restructured API to make naming more readable and consistent.
 * Function was deprecated in Tripal 2.0 and will be removed 2 releases from now.
 * This function has been replaced by chado_insert_record().
 *
 * @see chado_insert_record().
 */
function tripal_core_chado_insert($table, $values, $options = array()) {

  tripal_report_error(
    'tripal_deprecated',
    TRIPAL_NOTICE,
    "DEPRECATED: %old_function has been replaced with %new_function. Please update your code.",
    array(
      '%old_function'=>'tripal_core_chado_insert',
      '%new_function' => 'chado_insert_record'
    )
  );

  //New API Function
  return chado_insert_record($table, $values, $options);
}

/**
 * @deprecated Restructured API to make naming more readable and consistent.
 * Function was deprecated in Tripal 2.0 and will be removed 2 releases from now.
 * This function has been replaced by chado_select_record().
 *
 * @see chado_select_record().
 */
function tripal_core_chado_select($table, $columns, $values, $options = NULL) {
  tripal_report_error(
    'tripal_deprecated',
    TRIPAL_NOTICE,
    "DEPRECATED: %old_function has been replaced with %new_function. Please update your code.",
    array(
      '%old_function'=>'tripal_core_chado_select',
      '%new_function' => 'chado_select_record'
    )
  );

  //New API Function
  return chado_select_record($table, $columns, $values, $options);
}

/**
 * @deprecated Restructured API to make naming more readable and consistent.
 * Function was deprecated in Tripal 2.0 and will be removed 2 releases from now.
 * This function has been replaced by chado_update_record().
 *
 * @see chado_update_record().
 */
function tripal_core_chado_update($table, $match, $values, $options = NULL) {
  tripal_report_error(
    'tripal_deprecated',
    TRIPAL_NOTICE,
    "DEPRECATED: %old_function has been replaced with %new_function. Please update your code.",
    array(
      '%old_function'=>'tripal_core_chado_update',
      '%new_function' => 'chado_update_record'
    )
  );

  //New API Function
  return chado_update_record($table, $match, $values, $options);
}

/**
 * @deprecated Restructured API to make naming more readable and consistent.
 * Function was deprecated in Tripal 2.0 and will be removed 2 releases from now.
 * This function has been replaced by tripal_create_files_dir().
 *
 * @see tripal_create_files_dir().
 */
function tripal_create_moddir($module_name) {
  tripal_report_error(
    'tripal_deprecated',
    TRIPAL_NOTICE,
    "DEPRECATED: %old_function has been replaced with %new_function. Please update your code.",
    array(
      '%old_function'=>'tripal_create_moddir',
      '%new_function' => 'tripal_create_files_dir'
    )
  );

  //New API Function
  return tripal_create_files_dir($module_name);
}

/**
 * @deprecated Restructured API to make naming more readable and consistent.
 * Function was deprecated in Tripal 2.0 and will be removed 2 releases from now.
 * This function has been replaced by tripal_create_files_dir().
 *
 * @see tripal_create_files_dir().
 */
function tripal_create_mod_subdir($module_name, $path) {
  tripal_report_error(
    'tripal_deprecated',
    TRIPAL_NOTICE,
    "DEPRECATED: %old_function has been replaced with %new_function. Please update your code.",
    array(
      '%old_function'=>'tripal_create_mod_subdir',
      '%new_function' => 'tripal_create_files_dir'
    )
  );

  //New API Function
  return tripal_create_files_dir($module_name, $path);
}

/**
 * @deprecated Restructured API to make naming more readable and consistent.
 * Function was deprecated in Tripal 2.0 and will be removed 2 releases from now.
 * This function has been replaced by tripal_create_files_dir().
 *
 * @see tripal_create_files_dir().
 * @todo Remove. This was not in Tripal 1.x.
 */
function tripal_file_directory_path() {
  tripal_report_error(
    'tripal_deprecated',
    TRIPAL_NOTICE,
    "DEPRECATED: %old_function has been replaced with %new_function. Please update your code.",
    array(
      '%old_function'=>'tripal_file_directory_path',
      '%new_function' => 'tripal_get_files_dir'
    )
  );

  //New API Function
  return tripal_get_files_dir();
}

/**
 * @deprecated Restructured API to make naming more readable and consistent.
 * Function was deprecated in Tripal 2.0 and will be removed 2 releases from now.
 * This function has been replaced by tripal_get_files_dir().
 *
 * @see tripal_get_files_dir().
 */
function tripal_get_moddir($module_name) {
  tripal_report_error(
    'tripal_deprecated',
    TRIPAL_NOTICE,
    "DEPRECATED: %old_function has been replaced with %new_function. Please update your code.",
    array(
      '%old_function'=>'tripal_get_moddir',
      '%new_function' => 'tripal_get_files_dir'
    )
  );

  //New API Function
  return tripal_get_files_dir($module_name);
}

/**
 * @deprecated Restructured API to make naming more readable and consistent.
 * Function was deprecated in Tripal 2.0 and will be removed 2 releases from now.
 * This function has been replaced by tripal_cancel_job().
 *
 * @see tripal_cancel_job().
 */
function tripal_jobs_cancel($job_id, $redirect = TRUE) {
  tripal_report_error(
    'tripal_deprecated',
    TRIPAL_NOTICE,
    "DEPRECATED: %old_function has been replaced with %new_function. Please update your code.",
    array(
      '%old_function'=>'tripal_jobs_cancel',
      '%new_function' => 'tripal_cancel_job'
    )
  );

  //New API Function
  return tripal_cancel_job($job_id);
}

/**
 * @deprecated Restructured API to make naming more readable and consistent.
 * Function was deprecated in Tripal 2.0 and will be removed 2 releases from now.
 * This function has been replaced by tripal_get_active_jobs().
 *
 * @see tripal_get_active_jobs().
 */
function tripal_get_module_active_jobs($modulename) {
  tripal_report_error(
    'tripal_deprecated',
    TRIPAL_NOTICE,
    "DEPRECATED: %old_function has been replaced with %new_function. Please update your code.",
    array(
      '%old_function'=>'tripal_get_module_active_jobs',
      '%new_function' => 'tripal_get_active_jobs'
    )
  );

  //New API Function
  return tripal_get_active_jobs($modulename);
}

/**
 * @deprecated Restructured API to make naming more readable and consistent.
 * Function was deprecated in Tripal 2.0 and will be removed 2 releases from now.
 * This function has been replaced by tripal_get_job_end().
 *
 * @see tripal_get_job_end().
 */
function tripal_jobs_get_end_time($job) {
  tripal_report_error(
    'tripal_deprecated',
    TRIPAL_NOTICE,
    "DEPRECATED: %old_function has been replaced with %new_function. Please update your code.",
    array(
      '%old_function'=>'tripal_jobs_get_end_time',
      '%new_function' => 'tripal_get_job_end'
    )
  );

  //New API Function
  return tripal_get_job_end($job);
}

/**
 * @deprecated Restructured API to make naming more readable and consistent.
 * Function was deprecated in Tripal 2.0 and will be removed 2 releases from now.
 * This function has been replaced by tripal_get_job_start().
 *
 * @see tripal_get_job_start().
 */
function tripal_jobs_get_start_time($job) {
  tripal_report_error(
    'tripal_deprecated',
    TRIPAL_NOTICE,
    "DEPRECATED: %old_function has been replaced with %new_function. Please update your code.",
    array(
      '%old_function'=>'tripal_jobs_get_start_time',
      '%new_function' => 'tripal_get_job_start'
    )
  );

  //New API Function
  return tripal_get_job_start($job);
}

/**
 * @deprecated Restructured API to make naming more readable and consistent.
 * Function was deprecated in Tripal 2.0 and will be removed 2 releases from now.
 * This function has been replaced by tripal_get_job_submit_date().
 *
 * @see tripal_get_job_submit_date().
 */
function tripal_jobs_get_submit_date($job) {
  tripal_report_error(
    'tripal_deprecated',
    TRIPAL_NOTICE,
    "DEPRECATED: %old_function has been replaced with %new_function. Please update your code.",
    array(
      '%old_function'=>'tripal_jobs_get_submit_date',
      '%new_function' => 'tripal_get_job_submit_date'
    )
  );

  //New API Function
  return tripal_get_job_submit_date($job);
}

/**
 * @deprecated Restructured API to make naming more readable and consistent.
 * Function was deprecated in Tripal 2.0 and will be removed 2 releases from now.
 * This function has been replaced by tripal_is_job_running().
 *
 * @see tripal_is_job_running().
 */
function tripal_jobs_check_running() {
  tripal_report_error(
    'tripal_deprecated',
    TRIPAL_NOTICE,
    "DEPRECATED: %old_function has been replaced with %new_function. Please update your code.",
    array(
      '%old_function'=>'tripal_jobs_check_running',
      '%new_function' => 'tripal_is_job_running'
    )
  );

  //New API Function
  return tripal_is_job_running();
}

/**
 * @deprecated Restructured API to make naming more readable and consistent.
 * Function was deprecated in Tripal 2.0 and will be removed 2 releases from now.
 * This function has been replaced by tripal_launch_job().
 *
 * @see tripal_launch_job().
 */
function tripal_jobs_launch($do_parallel = 0, $job_id = NULL) {
  tripal_report_error(
    'tripal_deprecated',
    TRIPAL_NOTICE,
    "DEPRECATED: %old_function has been replaced with %new_function. Please update your code.",
    array(
      '%old_function'=>'tripal_jobs_launch',
      '%new_function' => 'tripal_launch_job'
    )
  );

  //New API Function
  return tripal_launch_job($do_parallel, $job_id);
}

/**
 * @deprecated Restructured API to make naming more readable and consistent.
 * Function was deprecated in Tripal 2.0 and will be removed 2 releases from now.
 * This function has been replaced by tripal_rerun_job().
 *
 * @see tripal_rerun_job().
 */
function tripal_jobs_rerun($job_id, $goto_jobs_page = TRUE) {
  tripal_report_error(
    'tripal_deprecated',
    TRIPAL_NOTICE,
    "DEPRECATED: %old_function has been replaced with %new_function. Please update your code.",
    array(
      '%old_function'=>'tripal_jobs_rerun',
      '%new_function' => 'tripal_rerun_job'
    )
  );

  //New API Function
  return tripal_rerun_job($job_id, $goto_jobs_page);
}

/**
 * @deprecated Restructured API to make naming more readable and consistent.
 * Function was deprecated in Tripal 2.0 and will be removed 2 releases from now.
 * This function has been replaced by tripal_set_job_progress().
 *
 * @see tripal_set_job_progress().
 */
function tripal_job_set_progress($job_id, $percentage) {
  tripal_report_error(
    'tripal_deprecated',
    TRIPAL_NOTICE,
    "DEPRECATED: %old_function has been replaced with %new_function. Please update your code.",
    array(
      '%old_function'=>'tripal_job_set_progress',
      '%new_function' => 'tripal_set_job_progress'
    )
  );

  //New API Function
  return tripal_set_job_progress($job_id, $percentage);
}

/**
 * @deprecated Restructured API to make naming more readable and consistent.
 * Function was deprecated in Tripal 2.0 and will be removed 2 releases from now.
 * This function has been replaced by tripal_update_mview().
 *
 * @see tripal_update_mview().
 */
function tripal_mviews_action($op, $mview_id, $redirect = FALSE) {

  if ($op == 'update') {
    tripal_report_error(
      'tripal_deprecated',
      TRIPAL_NOTICE,
      "DEPRECATED: %old_function has been replaced with %new_function. Please update your code.",
      array(
        '%old_function'=>'tripal_mviews_action',
        '%new_function' => 'tripal_add_populate_mview'
      )
    );

    //New API Function
    return tripal_add_populate_mview($mview_id, $redirect);
  }
  if ($op == 'delete') {
    tripal_report_error(
      'tripal_deprecated',
      TRIPAL_NOTICE,
      "DEPRECATED: %old_function has been replaced with %new_function. Please update your code.",
      array(
        '%old_function'=>'tripal_mviews_action',
        '%new_function' => 'tripal_delete_mview'
      )
    );

    //New API Function
    return tripal_delete_mview($mview_id, $redirect);
  }
}

/**
 * @deprecated Restructured API to make naming more readable and consistent.
 * Function was deprecated in Tripal 1.0 and will is in the process of being removed.
 */
function tripal_add_legacy_mview($name, $modulename, $mv_table, $mv_specs, $indexed,
  $query, $special_index, $comment) {

  tripal_report_error(
    'tripal_deprecated',
    TRIPAL_ERROR,
    "DEPRECATED: %old_function has been completely removed after a period of two releases
      since deprecation. Please use the new Materialized View Interface.",
    array(
      '%old_function'=>'tripal_add_legacy_mview'
    )
  );

}

/**
 * @deprecated Restructured API to make naming more readable and consistent.
 * Function was deprecated in Tripal 2.0 and will be removed 2 releases from now.
 * This function has been replaced by tripal_populate_mview().
 *
 * @see tripal_populate_mview().
 */
function tripal_update_mview($mview_id) {
  tripal_report_error(
    'tripal_deprecated',
    TRIPAL_NOTICE,
    "DEPRECATED: %old_function has been replaced with %new_function. Please update your code.",
    array(
      '%old_function'=>'tripal_update_mview',
      '%new_function' => 'tripal_populate_mview'
    )
  );

  //New API Function
  return tripal_populate_mview($mview_id);
}

/**
 * @deprecated Restructured API to make naming more readable and consistent.
 * Function was deprecated in Tripal 2.0 and will be removed 2 releases from now.
 * This function is considered to be no easier to use than chado_delete_record directly
 * chado_delete_record().
 *
 * @see chado_delete_record().
 */
function tripal_core_delete_property_by_id($basetable, $property_id) {

  tripal_report_error(
    'tripal_deprecated',
    TRIPAL_NOTICE,
    "DEPRECATED: %old_function has been replaced with %new_function. This requires manual
      intervention since the arguements for the two functions are different.
      Please update your code.",
    array(
      '%old_function'=>'tripal_core_delete_property_by_id',
      '%new_function' => 'chado_delete_record'
    )
  );

  // construct the array that will match the exact record to update
  $match = array(
    $basetable.'prop_id' => $property_id,
  );

  return chado_delete_record($basetable.'prop', $match);

}

/**
 * @deprecated Restructured API to make naming more readable and consistent.
 * Function was deprecated in Tripal 2.0 and will be removed 2 releases from now.
 * This function has been replaced by chado_delete_property().
 *
 * @see chado_delete_property().
 */
function tripal_core_delete_property($basetable, $record_id, $property, $cv_name) {
  tripal_report_error(
    'tripal_deprecated',
    TRIPAL_NOTICE,
    "DEPRECATED: %old_function has been replaced with %new_function. Please update your code.",
    array(
      '%old_function'=>'tripal_core_delete_property',
      '%new_function' => 'chado_delete_property'
    )
  );

  //New API Function
  return chado_delete_property($basetable, $record_id, $property, $cv_name);
}

/**
 * @deprecated Restructured API to make naming more readable and consistent.
 * Function was deprecated in Tripal 2.0 and will be removed 2 releases from now.
 * This function has been replaced by chado_get_property().
 *
 * @see chado_get_property().
 */
function tripal_core_get_property($basetable, $record_id, $property, $cv_name) {
  tripal_report_error(
    'tripal_deprecated',
    TRIPAL_NOTICE,
    "DEPRECATED: %old_function has been replaced with %new_function. Please update your code.",
    array(
      '%old_function'=>'tripal_core_get_property',
      '%new_function' => 'chado_get_property'
    )
  );

  //New API Function
  return chado_get_property($basetable, $record_id, $property, $cv_name);
}

/**
 * @deprecated Restructured API to make naming more readable and consistent.
 * Function was deprecated in Tripal 2.0 and will be removed 2 releases from now.
 * This function has been replaced by chado_get_table_max_rank().
 *
 * @see chado_get_table_max_rank().
 */
function tripal_get_max_chado_rank($tablename, $where_options) {
  tripal_report_error(
    'tripal_deprecated',
    TRIPAL_NOTICE,
    "DEPRECATED: %old_function has been replaced with %new_function. Please update your code.",
    array(
      '%old_function'=>'tripal_get_max_chado_rank',
      '%new_function' => 'chado_get_table_max_rank'
    )
  );

  //New API Function
  return chado_get_table_max_rank($tablename, $where_options);
}

/**
 * @deprecated Restructured API to make naming more readable and consistent.
 * Function was deprecated in Tripal 2.0 and will be removed 2 releases from now.
 * This function has been replaced by chado_get_table_max_rank().
 *
 * @see chado_get_table_max_rank().
 */
function get_max_chado_rank($tablename, $where_options) {
  tripal_report_error(
    'tripal_deprecated',
    TRIPAL_NOTICE,
    "DEPRECATED: %old_function has been replaced with %new_function. Please update your code.",
    array(
      '%old_function'=>'get_max_chado_rank',
      '%new_function' => 'chado_get_table_max_rank'
    )
  );

  //New API Function
  return chado_get_table_max_rank($tablename, $where_options);
}


/**
 * @deprecated Restructured API to make naming more readable and consistent.
 * Function was deprecated in Tripal 2.0 and will be removed 2 releases from now.
 * This function has been replaced by chado_insert_property().
 *
 * @see chado_insert_property().
 */
function tripal_core_insert_property($basetable, $record_id, $property,
$cv_name, $value, $update_if_present = 0) {
  tripal_report_error(
    'tripal_deprecated',
    TRIPAL_NOTICE,
    "DEPRECATED: %old_function has been replaced with %new_function. Please update your code.",
    array(
      '%old_function'=>'tripal_core_insert_property',
      '%new_function' => 'chado_insert_property'
    )
  );

  //New API Function
  return chado_insert_property($basetable, $record_id, $property, $cv_name, $value, $update_if_present);
}

/**
 * @deprecated Restructured API to make naming more readable and consistent.
 * Function was deprecated in Tripal 2.0 and will be removed 2 releases from now.
 * This function has been replaced by chado_update_property().
 *
 * @see chado_update_property().
 */
function tripal_core_update_property_by_id($basetable, $record_id, $property,
$cv_name, $value) {
  tripal_report_error(
    'tripal_deprecated',
    TRIPAL_NOTICE,
    "DEPRECATED: %old_function has been replaced with %new_function. This requires manual
      intervention since the arguments have changed. Please update your code.",
    array(
      '%old_function'=>'tripal_core_update_property_by_id',
      '%new_function' => 'chado_update_property'
    )
  );

  //New API Function
  return chado_update_property($basetable, FALSE, $property, $cv_name, $value, FALSE, $record_id);
}

/**
 * @deprecated Restructured API to make naming more readable and consistent.
 * Function was deprecated in Tripal 2.0 and will be removed 2 releases from now.
 * This function has been replaced by chado_update_property().
 *
 * @see chado_update_property().
 */
function tripal_core_update_property($basetable, $record_id, $property,
$cv_name, $value, $insert_if_missing = 0) {
  tripal_report_error(
    'tripal_deprecated',
    TRIPAL_NOTICE,
    "DEPRECATED: %old_function has been replaced with %new_function. Please update your code.",
    array(
      '%old_function'=>'tripal_core_update_property',
      '%new_function' => 'chado_update_property'
    )
  );

  //New API Function
  return chado_update_property($basetable, $record_id, $property, $cv_name, $value, $insert_if_missing);
}

/**
 * @deprecated Restructured API to make naming more readable and consistent.
 * Function was deprecated in Tripal 2.0 and will be removed 2 releases from now.
 * This function has been replaced by chado_create_custom_table().
 *
 * @see chado_create_custom_table().
 */
function tripal_core_create_custom_table(&$ret, $table, $schema, $skip_creation = 1) {
  tripal_report_error(
    'tripal_deprecated',
    TRIPAL_NOTICE,
    "DEPRECATED: %old_function has been replaced with %new_function. Note that $ret has
      been removed from the paramters since this is not longer used in Drupal 7.
      Please update your code.",
    array(
      '%old_function'=>'tripal_core_create_custom_table',
      '%new_function' => 'chado_create_custom_table'
    )
  );

  //New API Function
  return chado_create_custom_table($table, $schema, $skip_creation);
}

/**
 * @deprecated Restructured API to make naming more readable and consistent.
 * Function was deprecated in Tripal 2.0 and will be removed 2 releases from now.
 * This function has been replaced by chado_edit_custom_table().
 *
 * @see chado_edit_custom_table().
 */
function tripal_core_edit_custom_table($table_id, $table_name, $schema, $skip_creation = 1) {
  tripal_report_error(
    'tripal_deprecated',
    TRIPAL_NOTICE,
    "DEPRECATED: %old_function has been replaced with %new_function. Please update your code.",
    array(
      '%old_function'=>'tripal_core_edit_custom_table',
      '%new_function' => 'chado_edit_custom_table'
    )
  );

  //New API Function
  return chado_edit_custom_table($table_id, $table_name, $schema, $skip_creation);
}

/**
 * @deprecated Restructured API to make naming more readable and consistent.
 * Function was deprecated in Tripal 2.0 and will be removed 2 releases from now.
 * This function has been replaced by chado_get_custom_table_id().
 *
 * @see chado_get_custom_table_id().
 */
function tripal_custom_tables_get_table_id($table_name) {
  tripal_report_error(
    'tripal_deprecated',
    TRIPAL_NOTICE,
    "DEPRECATED: %old_function has been replaced with %new_function. Please update your code.",
    array(
      '%old_function'=>'tripal_custom_tables_get_table_id',
      '%new_function' => 'chado_get_custom_table_id'
    )
  );

  //New API Function
  return chado_get_custom_table_id($table_name);
}

/**
 * @deprecated Restructured API to make naming more readable and consistent.
 * Function was deprecated in Tripal 2.0 and will be removed 2 releases from now.
 * This function has been replaced by chado_get_custom_table_schema().
 *
 * @see chado_get_custom_table_schema().
 */
function tripal_get_chado_custom_schema($table) {
  tripal_report_error(
    'tripal_deprecated',
    TRIPAL_NOTICE,
    "DEPRECATED: %old_function has been replaced with %new_function. Please update your code.",
    array(
      '%old_function'=>'tripal_get_chado_custom_schema',
      '%new_function' => 'chado_get_custom_table_schema'
    )
  );

  //New API Function
  return chado_get_custom_table_schema($table);
}

/**
 * @deprecated Restructured API to make naming more readable and consistent.
 * Function was deprecated in Tripal 2.0 and will be removed 2 releases from now.
 * This function has been replaced by chado_is_installed().
 *
 * @see chado_is_installed().
 */
function tripal_core_is_chado_installed() {
  tripal_report_error(
    'tripal_deprecated',
    TRIPAL_NOTICE,
    "DEPRECATED: %old_function has been replaced with %new_function. Please update your code.",
    array(
      '%old_function'=>'tripal_core_is_chado_installed',
      '%new_function' => 'chado_is_installed'
    )
  );

  //New API Function
  return chado_is_installed();
}

/**
 * @deprecated Restructured API to make naming more readable and consistent.
 * Function was deprecated in Tripal 2.0 and will be removed 2 releases from now.
 * This function has been replaced by chado_get_schema().
 *
 * @see chado_get_schema().
 */
function tripal_core_get_chado_table_schema($table) {
  tripal_report_error(
    'tripal_deprecated',
    TRIPAL_NOTICE,
    "DEPRECATED: %old_function has been replaced with %new_function. Please update your code.",
    array(
      '%old_function'=>'tripal_core_get_chado_table_schema',
      '%new_function' => 'chado_get_schema'
    )
  );

  //New API Function
  return chado_get_schema($table);
}

/**
 * @deprecated Restructured API to make naming more readable and consistent.
 * Function was deprecated in Tripal 2.0 and will be removed 2 releases from now.
 * This function has been replaced by chado_get_version().
 *
 * @see chado_get_version().
 */
function tripal_core_get_chado_version($exact = FALSE, $warn_if_unsupported = FALSE) {
  tripal_report_error(
    'tripal_deprecated',
    TRIPAL_NOTICE,
    "DEPRECATED: %old_function has been replaced with %new_function. Please update your code.",
    array(
      '%old_function'=>'tripal_core_get_chado_version',
      '%new_function' => 'chado_get_version'
    )
  );

  //New API Function
  return chado_get_version($exact, $warn_if_unsupported);
}

/**
 * @deprecated Restructured API to make naming more readable and consistent.
 * Function was deprecated in Tripal 2.0 and will be removed 2 releases from now.
 * This function has been replaced by chado_set_active().
 *
 * @see chado_set_active().
 */
function tripal_db_set_active($dbname  = 'default') {
  tripal_report_error(
    'tripal_deprecated',
    TRIPAL_NOTICE,
    "DEPRECATED: %old_function has been replaced with %new_function. Please update your code.",
    array(
      '%old_function'=>'tripal_db_set_active',
      '%new_function' => 'chado_set_active'
    )
  );

  //New API Function
  return chado_set_active($dbname);
}

/**
 * @deprecated Restructured API to make naming more readable and consistent.
 * Function was deprecated in Tripal 2.0 and will be removed 2 releases from now.
 * This function has been replaced by chado_is_local().
 *
 * @see chado_is_local().
 */
function tripal_core_is_chado_local() {
  tripal_report_error(
    'tripal_deprecated',
    TRIPAL_NOTICE,
    "DEPRECATED: %old_function has been replaced with %new_function. Please update your code.",
    array(
      '%old_function'=>'tripal_core_is_chado_local',
      '%new_function' => 'chado_is_local'
    )
  );

  //New API Function
  return chado_is_local();
}

/**
 * @deprecated Restructured API to make naming more readable and consistent.
 * Function was deprecated in Tripal 2.0 and will be removed 2 releases from now.
 * This function has been replaced by chado_get_table_names().
 *
 * @see chado_get_table_names().
 */
function tripal_core_get_chado_tables($include_custom = NULL) {
  tripal_report_error(
    'tripal_deprecated',
    TRIPAL_NOTICE,
    "DEPRECATED: %old_function has been replaced with %new_function. Please update your code.",
    array(
      '%old_function'=>'tripal_core_get_chado_tables',
      '%new_function' => 'chado_get_table_names'
    )
  );

  //New API Function
  return chado_get_table_names($include_custom);
}

/**
 * @deprecated Restructured API to make naming more readable and consistent.
 * Function was deprecated in Tripal 2.0 and will be removed 2 releases from now.
 * This function has been replaced by chado_is_local().
 *
 * @see chado_is_local().
 */
function tripal_core_chado_schema_exists() {
  tripal_report_error(
    'tripal_deprecated',
    TRIPAL_NOTICE,
    "DEPRECATED: %old_function has been replaced with %new_function. Please update your code.",
    array(
      '%old_function'=>'tripal_core_chado_schema_exists',
      '%new_function' => 'chado_is_local'
    )
  );

  //New API Function
  return chado_is_local();
}

/**
 * @deprecated Restructured API to make naming more readable and consistent.
 * Function was deprecated in Tripal 2.0 and will be removed 2 releases from now.
 * This function has been replaced by chado_dbschema_exists().
 *
 * @see chado_dbschema_exists().
 */
function tripal_core_schema_exists($schema) {
  tripal_report_error(
    'tripal_deprecated',
    TRIPAL_NOTICE,
    "DEPRECATED: %old_function has been replaced with %new_function. Please update your code.",
    array(
      '%old_function'=>'tripal_core_schema_exists',
      '%new_function' => 'chado_dbschema_exists'
    )
  );

  //New API Function
  return chado_dbschema_exists($schema);
}

/**
 * @deprecated Restructured API to make naming more readable and consistent.
 * Function was deprecated in Tripal 2.0 and will be removed 2 releases from now.
 * This function has been replaced by tripal_set_admin_message().
 *
 * @see tripal_set_admin_message().
 *
function theme_tripal_admin_message($variables) {
  tripal_report_error(
    'tripal_deprecated',
    TRIPAL_NOTICE,
    "DEPRECATED: %old_function has been replaced with %new_function. Please update your code.",
    array(
      '%old_function'=>'chado_get_id_for_node',
      '%new_function' => 'chado_get_id_from_nid'
    )
  );

  //New API Function
  return tripal_set_admin_message();
}
*/

/**
 * @deprecated Restructured API to make naming more readable and consistent.
 * Function was deprecated in Tripal 2.0 and will be removed 2 releases from now.
 * This function has been replaced by tripal_report_error().
 *
 * @see tripal_report_error().
 * @todo Remove. This was not in Tripal 1.x.
 */
function tripal_core_report_error($type, $severity, $message, $variables = array(), $options = array()) {

  tripal_report_error(
    'tripal_deprecated',
    TRIPAL_NOTICE,
    "DEPRECATED: %old_function has been replaced with %new_function. Please update your code.",
    array(
      '%old_function'=>'tripal_core_report_error',
      '%new_function' => 'tripal_report_error'
    )
  );

  //New API Function
  return tripal_report_error($type, $severity, $message, $variables, $options);
}