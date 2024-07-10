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
 * function should not be called in a location accessible via a drush
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
  $fs = \Drupal::service('file_system');
  $messenger = \Drupal::messenger();
  $flags = \Drupal\Core\File\FileSystemInterface::CREATE_DIRECTORY | \Drupal\Core\File\FileSystemInterface::MODIFY_PERMISSIONS;

  // if the path is not supplied then assume they want to create the base files
  // directory for the specified module
  if (!$path) {
    // make the data directory for this module
    $data_dir = tripal_get_files_dir() . "/$module_name";
    if (!$fs->prepareDirectory($data_dir, $flags)) {
      $message = "Cannot create directory $data_dir. This module may not " .
        "behave correctly without this directory.  Please  create " .
        "the directory manually or fix the problem and reinstall.";
    }
    else {
      return;
    }
  }
  else {
    // make sure the module data directory exists, we make a recursive call
    // but without the path
    tripal_create_files_dir($module_name);

    // now make sure the sub dir exists
    $sub_dir = tripal_get_files_dir() . '/' . $module_name . '/' . $path;
    if (!$fs->prepareDirectory($sub_dir, $flags)) {
      $message = "Can not create directory $sub_dir. ";
    }
  }
  $messenger->addMessage(t($message), \Drupal\Core\Messenger\MessengerInterface::TYPE_ERROR);
  tripal_report_error('tripal', TRIPAL_ERROR, $message, []);
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
  $default_scheme = \Drupal::config('system.file')->get('default_scheme');
  $data_dir = \Drupal::service('file_system')->realpath($default_scheme . "://");

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
  $stream = 'public://tripal';

  // If a module name is provided then append the module directory.
  if ($module_name) {
    $stream .= "/$module_name";

    // Make sure the directory exists.
    tripal_create_files_dir($module_name);
  }

  return $stream;
}

/**
 * Formats a size (in bytes) in human readable format.
 *
 * Function taken from php.net
 *
 * @param $bytes
 *   The size of the file in bytes
 * @param $precision
 *   The number of decimal places to use in the final number if needed
 *
 * @return string
 *   A formatted string indicating the size of the file
 *
 *
 * @ingroup tripal_files_api
 */
function tripal_format_bytes($bytes, $precision = 2) {
  $units = ['B', 'KB', 'MB', 'GB', 'TB'];

  $bytes = max($bytes, 0);
  $pow = floor(($bytes ? log($bytes) : 0) / log(1024));
  $pow = min($pow, count($units) - 1);

  // Uncomment one of the following alternatives
  $bytes /= pow(1000, $pow);
  // $bytes /= (1 << (10 * $pow));

  return round($bytes, $precision) . '' . $units[$pow];
}

/**
 * Retrieves the list of files uploaded by a user.
 *
 * @param $uid
 *   The ID of the user whose files should be retrieved.
 * @param $allowed_types
 *   A list of valid extensions to restrict the files to.
 * @param $module
 *   The name of the module that is managing the file.
 *
 * @return
 *   A list of file objects.
 *
 * @ingroup tripal_files_api
 */
function tripal_get_user_uploads($uid, $allowed_types = [], $module = 'tripal') {
  $db = \Drupal::database();

  $query = $db->select('file_managed', 'FM');
  $query->fields('FM', ['fid']);
  $query->distinct();
  $query->fields('FM', ['filename']);
  $query->condition('FM.uid', $uid);
  $query->innerJoin('file_usage', 'FU', "\"FU\".fid = \"FM\".fid");
  $query->condition('FU.module', $module);
  $query->orderBy('FM.filename');
  $files = $query->execute();

  $files_list = [];
  while ($fid = $files->fetchField()) {
    $file = \Drupal\file\Entity\File::load($fid);
    foreach ($allowed_types as $type) {
      // if (preg_match('/\.' . $type . '$/', $file->filename)) { // old
      if (preg_match('/\.' . $type . '$/', $file->getFilename())) {  
        $files_list[$fid] = $file;
      }
    }
  }

  return $files_list;
}

/**
 * Retrieves the URI for the dedicated directory for a user's files.
 *
 * This directory is used by the file uploader and by data collections.
 *
 * @param int $uid
 *   A Drupal user id.
 *
 * @return
 *   The URI of the directory.
 *
 * @ingroup tripal_files_api
 */
function tripal_get_user_files_dir($uid) {

  $user_dir = 'public://tripal/users/' . $uid;

  return $user_dir;
}

/**
 * Checks if the user's dedicated directory is accessible and writeable.
 *
 * @param $uid
 *   A Drupal user id.
 *
 * @return
 *   TRUE if the user's directory is writeable. FALSE otherwise.
 *
 * @ingroup tripal_files_api
 */
function tripal_is_user_files_dir_writeable($uid) {
  $user_dir = tripal_get_user_files_dir($uid);
  $fs = \Drupal::service('file_system');

  // First, make sure the directory exists.
  if (!$fs->prepareDirectory($user_dir, \Drupal\Core\File\FileSystemInterface::CREATE_DIRECTORY)) {
    return FALSE;
  }

  // It has been reported that file_prepare_directory is not properly
  // detecting if the directory is writeable, so we'll do another
  // round of checks to be sure.
  if (!is_dir($user_dir)) {
    return FALSE;
  }
  if (!is_writeable($user_dir)) {
    return FALSE;
  }
  return TRUE;
}
