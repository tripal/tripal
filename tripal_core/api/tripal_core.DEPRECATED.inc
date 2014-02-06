<?php
/**
 * @file
 * Contains all the deprecated functions to keep the tripal api backwards compatible.
 */

/**
 * @deprecated Restructured API to make naming more readable and consistent.
 * Function was deprecated in Tripal 2.0 and will be removed 2 releases from now.
 * This function has been replaced by @see chado_get_id_from_nid().
 */
function chado_get_id_for_node($table, $nid) {
  //New API Function
  return chado_get_id_from_nid($table, $nid);
}

/**
 * @deprecated Restructured API to make naming more readable and consistent.
 * Function was deprecated in Tripal 2.0 and will be removed 2 releases from now.
 * This function has been replaced by @see chado_get_nid_from_id().
 */
function chado_get_node_id($table, $id) {
  //New API Function
  return chado_get_nid_from_id($table, $id);
}

/**
 * @deprecated Restructured API to make naming more readable and consistent.
 * Function was deprecated in Tripal 2.0 and will be removed 2 releases from now.
 * This function has been replaced by @see chado_cleanup_orphaned_nodes().
 */
function tripal_core_chado_node_cleanup_orphaned($table, $job_id = NULL) {
  //New API Function
  return chado_cleanup_orphaned_nodes($table, $job_id);
}

/**
 * @deprecated Restructured API to make naming more readable and consistent.
 * Function was deprecated in Tripal 2.0 and will be removed 2 releases from now.
 * This function has been replaced by @see chado_add_node_form_dbxrefs().
 */
function chado_node_additional_dbxrefs_form(&$form, &$form_state, $details) {
  //New API Function
  return chado_add_node_form_dbxrefs($form, $form_state, $details);
}

/**
 * @deprecated Restructured API to make naming more readable and consistent.
 * Function was deprecated in Tripal 2.0 and will be removed 2 releases from now.
 * This function has been replaced by @see chado_add_node_form_dbxrefs().
 */
function chado_node_additional_dbxrefs_form_add_button_validate($form, &$form_state) {
  //New API Function
  return chado_add_node_form_dbxrefs_add_button_validate($form, $form_state);
}

/**
 * @deprecated Restructured API to make naming more readable and consistent.
 * Function was deprecated in Tripal 2.0 and will be removed 2 releases from now.
 * This function has been replaced by @see chado_add_node_form_dbxrefs().
 */
function chado_node_additional_dbxrefs_form_add_button_submit(&$form, &$form_state) {
  //New API Function
  return chado_add_node_form_dbxrefs_add_button_submit($form, $form_state);
}

/**
 * @deprecated Restructured API to make naming more readable and consistent.
 * Function was deprecated in Tripal 2.0 and will be removed 2 releases from now.
 * This function has been replaced by @see chado_add_node_form_dbxrefs().
 */
function chado_node_additional_dbxrefs_form_remove_button_validate($form, $form_state) {
  //New API Function
  return chado_add_node_form_dbxrefs_remove_button_validate($form, $form_state);
}

/**
 * @deprecated Restructured API to make naming more readable and consistent.
 * Function was deprecated in Tripal 2.0 and will be removed 2 releases from now.
 * This function has been replaced by @see chado_add_node_form_dbxrefs().
 */
function chado_node_additional_dbxrefs_form_remove_button_submit(&$form, &$form_state) {
  //New API Function
  return chado_add_node_form_dbxrefs_remove_button_submit($form, $form_state);
}

/**
 * @deprecated Restructured API to make naming more readable and consistent.
 * Function was deprecated in Tripal 2.0 and will be removed 2 releases from now.
 * This function has been replaced by @see chado_add_node_form_dbxrefs().
 */
function chado_node_additional_dbxrefs_form_ajax_update($form, $form_state) {
  //New API Function
  return chado_add_node_form_dbxrefs_ajax_update($form, $form_state);
}

/**
 * @deprecated Restructured API to make naming more readable and consistent.
 * Function was deprecated in Tripal 2.0 and will be removed 2 releases from now.
 * This function has been replaced by @see chado_add_node_form_dbxrefs().
 */
function chado_node_additional_dbxrefs_form_create_dbxref_formstate_array($form, &$form_state) {
  //New API Function
  return chado_add_node_form_dbxrefs_create_dbxref_formstate_array($form, $form_state);
}

/**
 * @deprecated Restructured API to make naming more readable and consistent.
 * Function was deprecated in Tripal 2.0 and will be removed 2 releases from now.
 * This function has been replaced by @see chado_add_node_form_dbxrefs().
 */
function theme_chado_node_additional_dbxrefs_form_table($variables) {
  //New API Function
  return theme_chado_add_node_form_dbxrefs_table($variables);
}

/**
 * @deprecated Restructured API to make naming more readable and consistent.
 * Function was deprecated in Tripal 2.0 and will be removed 2 releases from now.
 * This function has been replaced by @see chado_retrieve_node_form_dbxrefs().
 */
function chado_node_additional_dbxrefs_form_retreive($node) {
  //New API Function
  return chado_retrieve_node_form_dbxrefs($node);
}

/**
 * @deprecated Restructured API to make naming more readable and consistent.
 * Function was deprecated in Tripal 2.0 and will be removed 2 releases from now.
 * This function has been replaced by @see chado_update_node_form_dbxrefs().
 */
function chado_node_additional_dbxrefs_form_update_dbxrefs($node, $details, $retrieved_dbxrefs = FALSE) {
  //New API Function
  return chado_update_node_form_dbxrefs($node, $details, $retrieved_dbxrefs);
}

/**
 * @deprecated Restructured API to make naming more readable and consistent.
 * Function was deprecated in Tripal 2.0 and will be removed 2 releases from now.
 * This function has been replaced by @see chado_add_node_form_properties().
 */
function chado_node_properties_form($form, $form_state, $details) {
  //New API Function
  return chado_add_node_form_properties($form, $form_state, $details);
}

/**
 * @deprecated Restructured API to make naming more readable and consistent.
 * Function was deprecated in Tripal 2.0 and will be removed 2 releases from now.
 * This function has been replaced by @see chado_add_node_form_properties().
 */
function chado_node_properties_form_add_button_validate($form, &$form_state) {
  //New API Function
  return chado_update_node_form_properties_add_button_validate($form, $form_state);
}

/**
 * @deprecated Restructured API to make naming more readable and consistent.
 * Function was deprecated in Tripal 2.0 and will be removed 2 releases from now.
 * This function has been replaced by @see chado_add_node_form_properties().
 */
function chado_node_properties_form_add_button_submit(&$form, &$form_state) {
  //New API Function
  return chado_add_node_form_properties_add_button_submit($form, $form_state);
}

/**
 * @deprecated Restructured API to make naming more readable and consistent.
 * Function was deprecated in Tripal 2.0 and will be removed 2 releases from now.
 * This function has been replaced by @see chado_add_node_form_properties().
 */
function chado_node_properties_form_remove_button_validate($form, $form_state) {
  //New API Function
  return chado_add_node_form_properties_remove_button_validate($form, $form_state);
}

/**
 * @deprecated Restructured API to make naming more readable and consistent.
 * Function was deprecated in Tripal 2.0 and will be removed 2 releases from now.
 * This function has been replaced by @see chado_add_node_form_properties().
 */
function chado_node_properties_form_remove_button_submit(&$form, &$form_state) {
  //New API Function
  return chado_add_node_form_properties_remove_button_submit($form, $form_state);
}

/**
 * @deprecated Restructured API to make naming more readable and consistent.
 * Function was deprecated in Tripal 2.0 and will be removed 2 releases from now.
 * This function has been replaced by @see chado_add_node_form_properties().
 */
function chado_node_properties_form_ajax_update($form, $form_state) {
  //New API Function
  return chado_add_node_form_properties_ajax_update($form, $form_state);
}

/**
 * @deprecated Restructured API to make naming more readable and consistent.
 * Function was deprecated in Tripal 2.0 and will be removed 2 releases from now.
 * This function has been replaced by @see chado_add_node_form_properties().
 */
function chado_node_properties_form_create_property_formstate_array($form, &$form_state) {
  //New API Function
  return chado_add_node_form_properties_create_property_formstate_array($form, $form_state);
}

/**
 * @deprecated Restructured API to make naming more readable and consistent.
 * Function was deprecated in Tripal 2.0 and will be removed 2 releases from now.
 * This function has been replaced by @see chado_add_node_form_properties().
 */
function theme_chado_node_properties_form_table($variables) {
  //New API Function
  return theme_chado_add_node_form_properties($variables);
}

/**
 * @deprecated Restructured API to make naming more readable and consistent.
 * Function was deprecated in Tripal 2.0 and will be removed 2 releases from now.
 * This function has been replaced by @see chado_retrieve_node_form_properties().
 */
function chado_node_properties_form_retreive($node) {
  //New API Function
  return chado_retrieve_node_form_properties($node);
}

/**
 * @deprecated Restructured API to make naming more readable and consistent.
 * Function was deprecated in Tripal 2.0 and will be removed 2 releases from now.
 * This function has been replaced by @see chado_update_node_form_properties().
 */
function chado_node_properties_form_update_properties($node, $details, $retrieved_properties = FALSE) {
  //New API Function
  return chado_update_node_form_properties($node, $details, $retrieved_properties);
}

/**
 * @deprecated Restructured API to make naming more readable and consistent.
 * Function was deprecated in Tripal 2.0 and will be removed 2 releases from now.
 * This function has been replaced by @see chado_add_node_form_relationships().
 */
function chado_node_relationships_form(&$form, &$form_state, $details) {
  //New API Function
  return chado_add_node_form_relationships($form, $form_state, $details);
}

/**
 * @deprecated Restructured API to make naming more readable and consistent.
 * Function was deprecated in Tripal 2.0 and will be removed 2 releases from now.
 * This function has been replaced by @see chado_add_node_form_relationships().
 */
function chado_node_relationships_form_add_button_validate($form, &$form_state) {
  //New API Function
  return chado_add_node_form_relationships_add_button_validate($form, $form_state);
}

/**
 * @deprecated Restructured API to make naming more readable and consistent.
 * Function was deprecated in Tripal 2.0 and will be removed 2 releases from now.
 * This function has been replaced by @see chado_add_node_form_relationships().
 */
function chado_node_relationships_form_add_button_submit(&$form, &$form_state) {
  //New API Function
  return chado_add_node_form_relationships_add_button_submit($form, $form_state);
}

/**
 * @deprecated Restructured API to make naming more readable and consistent.
 * Function was deprecated in Tripal 2.0 and will be removed 2 releases from now.
 * This function has been replaced by @see chado_add_node_form_relationships().
 */
function chado_node_relationships_form_remove_button_validate($form, $form_state) {
  //New API Function
  return chado_add_node_form_relationships_form_remove_button_validate($form, $form_state);
}

/**
 * @deprecated Restructured API to make naming more readable and consistent.
 * Function was deprecated in Tripal 2.0 and will be removed 2 releases from now.
 * This function has been replaced by @see chado_add_node_form_relationships().
 */
function chado_node_relationships_form_remove_button_submit(&$form, &$form_state) {
  //New API Function
  return chado_add_node_form_relationships_remove_button_submit($form, $form_state);
}

/**
 * @deprecated Restructured API to make naming more readable and consistent.
 * Function was deprecated in Tripal 2.0 and will be removed 2 releases from now.
 * This function has been replaced by @see chado_add_node_form_relationships().
 */
function chado_node_relationships_form_ajax_update($form, $form_state) {
  //New API Function
  return chado_add_node_form_relationships_ajax_update($form, $form_state);
}

/**
 * @deprecated Restructured API to make naming more readable and consistent.
 * Function was deprecated in Tripal 2.0 and will be removed 2 releases from now.
 * This function has been replaced by @see chado_add_node_form_relationships().
 */
function chado_node_relationships_form_create_relationship_formstate_array($form, &$form_state) {
  //New API Function
  return chado_add_node_form_relationships_create_relationship_formstate_array($form, $form_state);
}

/**
 * @deprecated Restructured API to make naming more readable and consistent.
 * Function was deprecated in Tripal 2.0 and will be removed 2 releases from now.
 * This function has been replaced by @see chado_add_node_form_relationships().
 */
function theme_chado_node_relationships_form_table($variables) {
  //New API Function
  return theme_chado_add_node_form_relationships_table($variables);
}

/**
 * @deprecated Restructured API to make naming more readable and consistent.
 * Function was deprecated in Tripal 2.0 and will be removed 2 releases from now.
 * This function has been replaced by @see chado_add_node_form_relationships().
 */
function chado_node_relationships_name_to_id_callback($base_table, $name_field, $string) {
  //New API Function
  return chado_add_node_form_relationships_name_to_id_callback($base_table, $name_field, $string);
}

/**
 * @deprecated Restructured API to make naming more readable and consistent.
 * Function was deprecated in Tripal 2.0 and will be removed 2 releases from now.
 * This function has been replaced by @see chado_retrieve_node_form_relationships().
 */
function chado_node_relationships_form_retreive($node) {
  //New API Function
  return chado_retrieve_node_form_relationships($node);
}

/**
 * @deprecated Restructured API to make naming more readable and consistent.
 * Function was deprecated in Tripal 2.0 and will be removed 2 releases from now.
 * This function has been replaced by @see chado_update_node_form_relationships().
 */
function chado_node_relationships_form_update_relationships($node, $details, $retrieved_relationships = FALSE) {
  //New API Function
  return chado_update_node_form_relationships($node, $details, $retrieved_relationships);
}

/**
 * @deprecated Restructured API to make naming more readable and consistent.
 * Function was deprecated in Tripal 2.0 and will be removed 2 releases from now.
 * This function has been replaced by @see chado_node_sync_form().
 */
function tripal_core_chado_node_sync_form($form, &$form_state) {
  //New API Function
  return chado_node_sync_form($form, $form_state);
}

/**
 * @deprecated Restructured API to make naming more readable and consistent.
 * Function was deprecated in Tripal 2.0 and will be removed 2 releases from now.
 * This function has been replaced by @see chado_node_sync_form_submit().
 */
function tripal_core_chado_node_sync_form_submit($form, $form_state) {
  //New API Function
  return chado_node_sync_form_submit($form, $form_state);
}

/**
 * @deprecated Restructured API to make naming more readable and consistent.
 * Function was deprecated in Tripal 2.0 and will be removed 2 releases from now.
 * This function has been replaced by @see chado_node_sync_records().
 */
function tripal_core_chado_node_sync_records($base_table, $max_sync = FALSE, $organism_id = FALSE,
    $types = array(), $ids = array(), $job_id = NULL) {
  //New API Function
  return chado_node_sync_records($base_table, $max_sync, $organism_id, $types, $ids, $job_id);
}

/**
 * @deprecated Restructured API to make naming more readable and consistent.
 * Function was deprecated in Tripal 2.0 and will be removed 2 releases from now.
 * There is no new functionality as this function was deamed unnecessary.
 */
function tripal_core_is_tripal_node_type() {

}

/**
 * @deprecated Restructured API to make naming more readable and consistent.
 * Function was deprecated in Tripal 2.0 and will be removed 2 releases from now.
 * This function has been replaced by @see chado_schema_get_foreign_key().
 */
function tripal_core_chado_get_foreign_key($table_desc, $field, $values, $options = NULL) {
  //New API Function
  return chado_schema_get_foreign_key($table_desc, $field, $values, $options);
}

/**
 * @deprecated Restructured API to make naming more readable and consistent.
 * Function was deprecated in Tripal 2.0 and will be removed 2 releases from now.
 * This function has been replaced by @see chado_delete_record().
 */
function tripal_core_chado_delete($table, $match, $options = NULL) {
  //New API Function
  return chado_delete_record();
}

/**
 * @deprecated Restructured API to make naming more readable and consistent.
 * Function was deprecated in Tripal 2.0 and will be removed 2 releases from now.
 * This function has been replaced by @see chado_expand_var().
 */
function tripal_core_expand_chado_vars($object, $type, $to_expand, $table_options = array()) {
  //New API Function
  return chado_expand_var($object, $type, $to_expand, $table_options);
}

/**
 * @deprecated Restructured API to make naming more readable and consistent.
 * Function was deprecated in Tripal 2.0 and will be removed 2 releases from now.
 * This function has been replaced by @see chado_generate_var().
 */
function tripal_core_generate_chado_var($table, $values, $base_options = array()) {
  //New API Function
  return chado_generate_var($table, $values, $base_options);
}

/**
 * @deprecated Restructured API to make naming more readable and consistent.
 * Function was deprecated in Tripal 2.0 and will be removed 2 releases from now.
 * This function has been replaced by @see chado_insert_record().
 */
function tripal_core_chado_insert($table, $values, $options = array()) {
  //New API Function
  return chado_insert_record($table, $values, $options);
}

/**
 * @deprecated Restructured API to make naming more readable and consistent.
 * Function was deprecated in Tripal 2.0 and will be removed 2 releases from now.
 * This function has been replaced by @see chado_select_record();
 */
function tripal_core_chado_select($table, $columns, $values, $options = NULL) {
  //New API Function
  return chado_select_record($table, $columns, $values, $options);
}

/**
 * @deprecated Restructured API to make naming more readable and consistent.
 * Function was deprecated in Tripal 2.0 and will be removed 2 releases from now.
 * This function has been replaced by @see chado_update_record();
 */
function tripal_core_chado_update($table, $match, $values, $options = NULL) {
  //New API Function
  return chado_update_record($table, $match, $values, $options);
}

/**
 * @deprecated Restructured API to make naming more readable and consistent.
 * Function was deprecated in Tripal 2.0 and will be removed 2 releases from now.
 * This function has been replaced by @see tripal_create_files_dir();
 */
function tripal_create_moddir($module_name) {
  //New API Function
  return tripal_create_files_dir($module_name);
}

/**
 * @deprecated Restructured API to make naming more readable and consistent.
 * Function was deprecated in Tripal 2.0 and will be removed 2 releases from now.
 * This function has been replaced by @see tripal_create_files_dir();
 */
function tripal_create_mod_subdir($module_name, $path) {
  //New API Function
  return tripal_create_files_dir($module_name, $path);
}

/**
 * @deprecated Restructured API to make naming more readable and consistent.
 * Function was deprecated in Tripal 2.0 and will be removed 2 releases from now.
 * This function has been replaced by @see tripal_create_files_dir().
 */
function tripal_file_directory_path() {
  //New API Function
  return tripal_get_files_dir();
}

/**
 * @deprecated Restructured API to make naming more readable and consistent.
 * Function was deprecated in Tripal 2.0 and will be removed 2 releases from now.
 * This function has been replaced by @see tripal_get_files_dir().
 */
function tripal_get_moddir($module_name) {
  //New API Function
  return tripal_get_files_dir($module_name);
}

/**
 * @deprecated Restructured API to make naming more readable and consistent.
 * Function was deprecated in Tripal 2.0 and will be removed 2 releases from now.
 * This function has been replaced by @see tripal_cancel_job().
 */
function tripal_jobs_cancel($job_id, $redirect = TRUE) {
  //New API Function
  return tripal_cancel_job($job_id);
}

/**
 * @deprecated Restructured API to make naming more readable and consistent.
 * Function was deprecated in Tripal 2.0 and will be removed 2 releases from now.
 * This function has been replaced by @see tripal_get_active_jobs().
 */
function tripal_get_module_active_jobs($modulename) {
  //New API Function
  return tripal_get_active_jobs($modulename);
}

/**
 * @deprecated Restructured API to make naming more readable and consistent.
 * Function was deprecated in Tripal 2.0 and will be removed 2 releases from now.
 * This function has been replaced by @see tripal_get_job_end().
 */
function tripal_jobs_get_end_time($job) {
  //New API Function
  return tripal_get_job_end($job);
}

/**
 * @deprecated Restructured API to make naming more readable and consistent.
 * Function was deprecated in Tripal 2.0 and will be removed 2 releases from now.
 * This function has been replaced by @see tripal_get_job_start().
 */
function tripal_jobs_get_start_time($job) {
  //New API Function
  return tripal_get_job_start($job);
}

/**
 * @deprecated Restructured API to make naming more readable and consistent.
 * Function was deprecated in Tripal 2.0 and will be removed 2 releases from now.
 * This function has been replaced by @see tripal_get_job_submit_date().
 */
function tripal_jobs_get_submit_date($job) {
  //New API Function
  return tripal_get_job_submit_date($job);
}

/**
 * @deprecated Restructured API to make naming more readable and consistent.
 * Function was deprecated in Tripal 2.0 and will be removed 2 releases from now.
 * This function has been replaced by @see tripal_is_job_running().
 */
function tripal_jobs_check_running() {
  //New API Function
  return tripal_is_job_running();
}

/**
 * @deprecated Restructured API to make naming more readable and consistent.
 * Function was deprecated in Tripal 2.0 and will be removed 2 releases from now.
 * This function has been replaced by @see tripal_launch_job().
 */
function tripal_jobs_launch($do_parallel = 0, $job_id = NULL) {
  //New API Function
  return tripal_launch_job($do_parallel, $job_id);
}

/**
 * @deprecated Restructured API to make naming more readable and consistent.
 * Function was deprecated in Tripal 2.0 and will be removed 2 releases from now.
 * This function has been replaced by @see tripal_rerun_job().
 */
function tripal_jobs_rerun($job_id, $goto_jobs_page = TRUE) {
  //New API Function
  return tripal_rerun_job($job_id, $goto_jobs_page);
}

/**
 * @deprecated Restructured API to make naming more readable and consistent.
 * Function was deprecated in Tripal 2.0 and will be removed 2 releases from now.
 * This function has been replaced by @see tripal_set_job_progress().
 */
function tripal_job_set_progress($job_id, $percentage) {
  //New API Function
  return tripal_set_job_progress($job_id, $percentage);
}

/**
 * @deprecated Restructured API to make naming more readable and consistent.
 * Function was deprecated in Tripal 2.0 and will be removed 2 releases from now.
 * This function has been replaced by @see tripal_update_mview().
 */
function tripal_mviews_action($op, $mview_id, $redirect = FALSE) {

  if ($op == 'update') {
    //New API Function
    return tripal_add_populate_mview($mview_id, $redirect);
  }
  if ($op == 'delete') {
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
}

/**
 * @deprecated Restructured API to make naming more readable and consistent.
 * Function was deprecated in Tripal 2.0 and will be removed 2 releases from now.
 * This function has been replaced by @see tripal_populate_mview();
 */
function tripal_update_mview($mview_id) {
  //New API Function
  return tripal_populate_mview($mview_id);
}

/**
 * @deprecated Restructured API to make naming more readable and consistent.
 * Function was deprecated in Tripal 2.0 and will be removed 2 releases from now.
 * This function has been replaced by @see chado_delete_property().
 */
function tripal_core_delete_property_by_id($basetable, $record_id) {
  //New API Function
  // D7 @todo: change parameters
  return chado_delete_property($basetable, $record_id);
}

/**
 * @deprecated Restructured API to make naming more readable and consistent.
 * Function was deprecated in Tripal 2.0 and will be removed 2 releases from now.
 * This function has been replaced by @see chado_delete_property().
 */
function tripal_core_delete_property($basetable, $record_id, $property, $cv_name) {
  //New API Function
  // D7 @todo: change parameters
  return chado_delete_property($basetable, $record_id, $property, $cv_name);
}

/**
 * @deprecated Restructured API to make naming more readable and consistent.
 * Function was deprecated in Tripal 2.0 and will be removed 2 releases from now.
 * This function has been replaced by @see chado_get_property().
 */
function tripal_core_get_property($basetable, $record_id, $property, $cv_name) {
  //New API Function
  return chado_get_property($basetable, $record_id, $property, $cv_name);
}

/**
 * @deprecated Restructured API to make naming more readable and consistent.
 * Function was deprecated in Tripal 2.0 and will be removed 2 releases from now.
 * This function has been replaced by @see chado_get_table_max_rank().
 */
function tripal_core_get_max_chado_rank($tablename, $where_options) {
  //New API Function
  return chado_get_table_max_rank($tablename, $where_options);
}

/**
 * @deprecated Restructured API to make naming more readable and consistent.
 * Function was deprecated in Tripal 2.0 and will be removed 2 releases from now.
 * This function has been replaced by @see chado_insert_property().
 */
function tripal_core_insert_property($basetable, $record_id, $property,
$cv_name, $value, $update_if_present = 0) {
  //New API Function
  return chado_insert_property($basetable, $record_id, $property, $cv_name, $value, $update_if_present);
}

/**
 * @deprecated Restructured API to make naming more readable and consistent.
 * Function was deprecated in Tripal 2.0 and will be removed 2 releases from now.
 * This function has been replaced by @see chado_update_property().
 */
function tripal_core_update_property_by_id($basetable, $record_id, $property,
$cv_name, $value) {
  //New API Function
  // D7 @todo: change parameters
  return chado_update_property($basetable, $record_id, $property, $cv_name, $value);
}

/**
 * @deprecated Restructured API to make naming more readable and consistent.
 * Function was deprecated in Tripal 2.0 and will be removed 2 releases from now.
 * This function has been replaced by @see chado_update_property().
 */
function tripal_core_update_property($basetable, $record_id, $property,
$cv_name, $value, $insert_if_missing = 0) {
  //New API Function
  // D7 @todo: change parameters
  return chado_update_property($basetable, $record_id, $property, $cv_name, $value, $insert_if_missing);
}

/**
 * @deprecated Restructured API to make naming more readable and consistent.
 * Function was deprecated in Tripal 2.0 and will be removed 2 releases from now.
 * This function has been replaced by @see chado_create_custom_table().
 */
function tripal_core_create_custom_table($table, $schema, $skip_creation = 1) {
  //New API Function
  return chado_create_custom_table($table, $schema, $skip_creation);
}

/**
 * @deprecated Restructured API to make naming more readable and consistent.
 * Function was deprecated in Tripal 2.0 and will be removed 2 releases from now.
 * This function has been replaced by @see chado_edit_custom_table().
 */
function tripal_core_edit_custom_table($table_id, $table_name, $schema, $skip_creation = 1) {
  //New API Function
  return chado_edit_custom_table($table_id, $table_name, $schema, $skip_creation);
}

/**
 * @deprecated Restructured API to make naming more readable and consistent.
 * Function was deprecated in Tripal 2.0 and will be removed 2 releases from now.
 * This function has been replaced by @see chado_get_custom_table_id().
 */
function tripal_custom_tables_get_table_id($table_name) {
  //New API Function
  return chado_get_custom_table_id($table_name);
}

/**
 * @deprecated Restructured API to make naming more readable and consistent.
 * Function was deprecated in Tripal 2.0 and will be removed 2 releases from now.
 * This function has been replaced by @see chado_get_custom_table_schema().
 */
function tripal_get_chado_custom_schema($table) {
  //New API Function
  return chado_get_custom_table_schema($table);
}

/**
 * @deprecated Restructured API to make naming more readable and consistent.
 * Function was deprecated in Tripal 2.0 and will be removed 2 releases from now.
 * This function has been replaced by @see chado_is_installed().
 */
function tripal_core_is_chado_installed() {
  //New API Function
  return chado_is_installed();
}

/**
 * @deprecated Restructured API to make naming more readable and consistent.
 * Function was deprecated in Tripal 2.0 and will be removed 2 releases from now.
 * This function has been replaced by @see chado_get_schema().
 */
function tripal_core_get_chado_table_schema($table) {
  //New API Function
  return chado_get_schema($table);
}

/**
 * @deprecated Restructured API to make naming more readable and consistent.
 * Function was deprecated in Tripal 2.0 and will be removed 2 releases from now.
 * This function has been replaced by @see chado_get_version().
 */
function tripal_core_get_chado_version($exact = FALSE, $warn_if_unsupported = FALSE) {
  //New API Function
  return chado_get_version($exact, $warn_if_unsupported);
}

/**
 * @deprecated Restructured API to make naming more readable and consistent.
 * Function was deprecated in Tripal 2.0 and will be removed 2 releases from now.
 * This function has been replaced by @see chado_set_active().
 */
function tripal_db_set_active($dbname  = 'default') {
  //New API Function
  return chado_set_active($dbname);
}

/**
 * @deprecated Restructured API to make naming more readable and consistent.
 * Function was deprecated in Tripal 2.0 and will be removed 2 releases from now.
 * This function has been replaced by @see chado_is_local().
 */
function tripal_core_is_chado_local() {
  //New API Function
  return chado_is_local();
}

/**
 * @deprecated Restructured API to make naming more readable and consistent.
 * Function was deprecated in Tripal 2.0 and will be removed 2 releases from now.
 * This function has been replaced by @see chado_get_table_names().
 */
function tripal_core_get_chado_tables($include_custom = NULL) {
  //New API Function
  return chado_get_table_names($include_custom);
}

/**
 * @deprecated Restructured API to make naming more readable and consistent.
 * Function was deprecated in Tripal 2.0 and will be removed 2 releases from now.
 * This function has been replaced by @see chado_is_local().
 */
function tripal_core_chado_schema_exists() {
  //New API Function
  return chado_is_local();
}

/**
 * @deprecated Restructured API to make naming more readable and consistent.
 * Function was deprecated in Tripal 2.0 and will be removed 2 releases from now.
 * This function has been replaced by @see chado_dbschema_exists().
 */
function tripal_core_schema_exists($schema) {
  //New API Function
  return chado_dbschema_exists($schema);
}

/**
 * @deprecated Restructured API to make naming more readable and consistent.
 * Function was deprecated in Tripal 2.0 and will be removed 2 releases from now.
 * This function has been replaced by @see tripal_set_admin_message().
 *
function theme_tripal_admin_message($variables) {
  //New API Function
  return tripal_set_admin_message();
}
*/

/**
 * @deprecated Restructured API to make naming more readable and consistent.
 * Function was deprecated in Tripal 2.0 and will be removed 2 releases from now.
 * This function has been replaced by @see tripal_report_error().
 */
function tripal_core_report_error($type, $severity, $message, $variables = array(), $options = array()) {
  //New API Function
  return tripal_report_error($type, $severity, $message, $variables, $options);
}