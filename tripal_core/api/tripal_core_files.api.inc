<?php 
/**
 * @file
 * The Tripal Files API
 *
 * This file provides the API to help Tripal modules to storing files
 * in a consistent directory strcuture.
 *
 * @defgroup tripal_files_api  Files API
 * @ingroup tripal_core_api
 * @{
 * Provides an application programming interface (API) for managing files within
 * the Tripal data directory structure.
 *
 * @}
 *
 */
 
/**
 * This function is a substitute for Drupal 6's file_directory_path()
 * function which no longer exists in Drupal 7.  However, the function
 * call is useful because it's more intitutive. So this wrapper was created
 * to mimic the behavior of the old function.  
 * 
 * @return
 *   the public directory where tripal files are housed: sites/default/files/tripal
 */
function tripal_file_directory_path() {
  return variable_get('file_public_path', conf_path() . '/files/tripal');
}
/**
 * This function is typically used in the '.install' file for a Tripal module
 * Each module should call this function during installation to create
 * the module data directory which is sites/default/files/tripal/[module_name]
 * for default Drupal settings.  This directory can then be used by the module
 * for storing files.
 *
 * @param $module_name
 *   the name of the module being installed.
 *
 * @returns
 *   nothing
 *
 * @ingroup tripal_files_api
 */
function tripal_create_moddir($module_name) {

  // make the data directory for this module
  $data_dir = tripal_file_directory_path() . "$module_name";
  if (!file_prepare_directory($data_dir, FILE_CREATE_DIRECTORY | FILE_MODIFY_PERMISSIONS)) {
    $message = "Cannot create directory $data_dir. This module may not " .
               "behave correctly without this directory.  Please  create " .
               "the directory manually or fix the problem and reinstall.";
    drupal_set_message(check_plain(t($message)), 'error');
    watchdog('tripal_core', $message, array(), WATCHDOG_ERROR);
  }
}

/**
 * Each Tripal module has a unique data directory which was creatd using the
 * tripal_create_moddir function during installation.  This function
 * retrieves the directory path.
 *
 * @param $module_name
 *   The name of the module
 *
 * @returns
 *   The path within the Drupal installation where the data directory resides
 *   
 * @ingroup tripal_files_api
 */
function tripal_get_moddir($module_name) {
  $data_dir = tripal_file_directory_path() . "$module_name";
  return $data_dir;
}