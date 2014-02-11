<?php
/**
 * @file
 * Provides an application programming interface (API) for managing files within
 * the Tripal data directory structure.
 */

/**
 * @defgroup tripal_files_api Tripal Files API
 * @ingroup tripal_core_api
 * @{
 * Provides an application programming interface (API) for managing files within
 * the Tripal data directory structure.
 * @}
 *
 */

/**
 * This function is typically used in the '.install' file for a Tripal module
 * Each module should call this function during installation to create
 * the module data directory which is sites/default/files/tripal/[module_name].
 * This directory can then be used by the module for storing files.
 *
 * @param $module_name
 *   the name of the module being installed
 * @param $path
 *   Optional sub-path to create
 *
 * @ingroup tripal_files_api
 */
function tripal_create_files_dir($module_name, $path = FALSE) {

  // if the path is not supplied then assume they want to create the base files directory
  // for the specified module
  if (!$path) {
    // make the data directory for this module
    $data_dir = tripal_get_files_dir() . "/$module_name";
    if (!file_prepare_directory($data_dir, FILE_CREATE_DIRECTORY | FILE_MODIFY_PERMISSIONS)) {
      $message = "Cannot create directory $data_dir. This module may not " .
                 "behave correctly without this directory.  Please  create " .
                 "the directory manually or fix the problem and reinstall.";
      drupal_set_message(check_plain(t($message)), 'error');
      tripal_report_error('tripal_core', TRIPAL_ERROR, $message, array());
    }
  }
  else {
    // make sure the module data directory exists, we make a recursive call
    // but without the path
    tripal_create_files_dir($module_name);

    // now make sure the sub dir exists
    $sub_dir = tripal_get_files_dir() . $module_name . $path;
    if (!file_prepare_directory($sub_dir, FILE_CREATE_DIRECTORY | FILE_MODIFY_PERMISSIONS)) {
      $message = "Cannot create directory $sub_dir. ";
      drupal_set_message(check_plain(t($message)), 'error');
      tripal_report_error('tripal_core', TRIPAL_ERROR, $message, array());
    }
  }
}

/**
 * Each Tripal module has a unique data directory which was created using the
 * tripal_create_files_dir function during installation.  This function
 * retrieves the directory path.
 *
 * @param $module_name
 *   (Optional) The name of the module.
 *
 * @returns
 *   The path within the Drupal installation where the data directory resides
 *
 * @ingroup tripal_files_api
 */
function tripal_get_files_dir($module_name = FALSE) {

  $data_dir = variable_get('file_public_path', conf_path() . '/files/tripal');

  if ($module_name) {
    $data_dir .= "/$module_name";
  }

  return $data_dir;
}