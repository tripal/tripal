<?php
/**
 * @file
 * Provides an application programming interface (API) for managing files within
 * the Tripal data directory structure.
 */

/**
 * @defgroup tripal_files_api Files
 * @ingroup tripal_api
 * @{
 * Provides an application programming interface (API) for managing files within
 * the Tripal data directory structure.
 * @}
 *
 */

/**
 * Creates a directory for a module in the Drupal's public files directory.
 *
 * Previously it was recommended that this function be called during
 * installation of the module in the .install file.  However this causes
 * permission problems if the module is installed via drush with a
 * user account that is not the same as the web user.  Therefore, this
 * function should not be called in a location accessiblve via a drush
 * command.  The tripal_get_files_dir() and tripal_get_files_stream()
 * will automatically create the directory if it doesn't exist so there is
 * little need to call this function directly.
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
      tripal_report_error('tripal', TRIPAL_ERROR, $message, array());
    }
  }
  else {
    // make sure the module data directory exists, we make a recursive call
    // but without the path
    tripal_create_files_dir($module_name);

    // now make sure the sub dir exists
    $sub_dir = tripal_get_files_dir() . '/' . $module_name . '/' . $path;
    if (!file_prepare_directory($sub_dir, FILE_CREATE_DIRECTORY | FILE_MODIFY_PERMISSIONS)) {
      $message = "Can not create directory $sub_dir. ";
      drupal_set_message(check_plain(t($message)), 'error');
      tripal_report_error('tripal', TRIPAL_ERROR, $message, array());
    }
  }
}

/**
 * Retrieves the Drupal relative directory for a Tripal module.
 *
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

  // Build the directory path.
  $data_dir = variable_get('file_public_path', conf_path() . '/files/tripal');

  // If a module name is provided then append the module directory.
  if ($module_name) {
    $data_dir .= "/$module_name";

    // Make sure the directory exists.
    tripal_create_files_dir($module_name);

  }

  return $data_dir;
}

/**
 * Retrieves the Drupal stream (e.g. public://...) for a Tripal module.
 *
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
function tripal_get_files_stream($module_name = FALSE) {
  // Build the directory path.
  $stream =  'public://tripal';

  // If a module name is provided then append the module directory.
  if ($module_name) {
    $stream .= "/$module_name";

    // Make sure the directory exists.
    tripal_create_files_dir($module_name);
  }

  return $stream;
}
