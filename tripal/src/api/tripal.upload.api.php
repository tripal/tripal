<?php

/**
 * @file
 * Provides an application programming interface (API) for working with
 * file uploads.
 */

/**
 * @defgroup tripal_upload_api File Upload
 * @ingroup tripal_api
 * @{
 * Tripal provides a convenient HTML5 Javascript uploader. It is automatically
 * embedded into the TripalImporter class.  This application programing
 * interface (API) provides support for working with uploaded files.
 *
 * If you want to use the TripalUploader JavaScript in your own form the
 * following must be performed:
 *
 * 1) Add a Drupal form to your code that contains the following:
 *   * A Drupal-style table with 4 or 8 columns.  See the initialize
 *     function in this class for a description of the columns.
 *   * A button for submitting a file for upload.
 *
 * @code
 * $headers = array(
 *    array('data' => 'Sequence File'),
 *    array('data' => 'Size', 'width' => '10%'),
 *    array('data' => 'Upload Progress', 'width' => '20%'),
 *    array('data' => 'Action', 'width' => '10%')
 *  );
 *  $rows = array();
 *  $table_vars = array(
 *    'header'      => $headers,
 *    'rows'        => $rows,
 *    'attributes'  => array('id' => 'sequence-file-upload-table'),
 *    'sticky'      => TRUE,
 *    'colgroups'   => array(),
 *    'empty'       => t('There are currently no files added.'),
 *  );
 *  $form['upload']['sequence_file'] = array(
 *    '#markup' => theme('table', $table_vars)
 *  );
 *  $form['upload']['sequence_fid'] = array(
 *    '#type' => 'hidden',
 *    '#value' => 0,
 *    '#attributes' => array('id' => 'sequence-fid')
 *  );
 *  $form['upload']['sequence_file_submit'] = array(
 *    '#type'     => 'submit',
 *    '#value'    => 'Upload Sequence File',
 *    '#name' => 'sequence_file_submit',
 *    // We don't want this button to submit as the file upload
 *    // is handled by the JavaScript code.
 *    '#attributes' => array('onclick' => 'return (false);')
 *  );
 * @endcode
 *
 *
 * 2)  Edit the theme/js/[module_name].js and in the "Drupal.behaviors.[module]"
 * section add a JQuery show function to the form that converts the table
 * created in the Drupal form to a TripalUploader table.  The 'table_id' must be
 * the same as the 'id' attribute set for the table in the Drupal code above.
 * The 'submit_id' must be the id of the upload button added in the Drupal
 * code above.  The 'category' for the files.  This is the category that
 * will be saved in Tripal for the file.  See the addUploadTable function
 * for additional options.  Include a 'cardinality' setting to indicate
 * the number of allowed files per upload, and set the 'target_id' to the
 * name of the field that will contain the file ID (fid) after uploading.
 *
 * @code
 *  // The TripalUploader object used for uploading of files using the
 *  // HTML5 File API. Large files are uploaded as chunks and a progress
 *  // bar is provided.
 *  var uploader = new TripalUploader();
 *
 *  $('#tripal-sequences-panel-form').show(function() {
 *    uploader.addUploadTable('sequence_file', {
 *      'table_id' : '#sequence-file-upload-table',
 *      'submit_id': '#edit-sequence-file-submit',
 *      'category' : ['sequence_file'],
 *      'cardinality' : 1,
 *      'target_id' : 'sequence-fid',
 *    });
 *  });
 * @endcode
 *
 *
 * 3) Files are uploaded automatically to Tripal.  Files are saved in the
 * Tripal user's directory.  You can retrieve information about the
 * file by querying for the file category for the current project.
 *
 * @code
 *   $seq_files = TripalFeature::getFilesByTypes($user->uid,
 *                   array('sequence_file'), $project_id);
 * @endcode
 *
 * 4) If the 'target_id' was used in array for step #2 above, then the
 * file ID can be retrieved in the hook_validate() and hook_submit() functions
 * via the $form_state['input'] array (not the $form_state['values'] array.
 *
 * @}
 */

use Symfony\Component\HttpFoundation\JsonResponse;

/**
 * Allows a module to interact with the Tripal file uploader during upload.
 *
 * This function is called prior to an 'action' occurring and allows the
 * module that is responsible for the file upload to halt an upload if
 * needed.
 *
 * @param $action
 *   The current action that is being performed during the upload process. The
 *   actions are:  'save', 'check' and 'merge'.
 * @param $details
 *   An associative array containing details about the upload process. Valid
 *   keys include:
 *     - filename:  The name of the file.
 *     - file_size:  The total size of the file.
 *     - chunk_size:  The size of the chunk.
 *     - fid:  The file ID of the file.
 * @param $message
 *   An error message to report to the user if the function returns FALSE.
 *
 * @return
 *   TRUE if the upload should continue. FALSE if a problem occurs and the
 *   upload should be terminated.
 *
 * @ingroup tripal_upload_api
 */
function hook_file_upload_check($action, $details, &$message) {
  switch ($action) {
    case 'save':
      // Place code here when chunks are being saved.
      break;
    case 'check':
      // Place code here when a chunk is being checked if the upload
      // completed successfully.
      break;
    case 'merge':
      // Place code here when all chunks will be merged.
      break;
  }
  return TRUE;
}

/**
 *
 */
function tripal_file_upload($type, $filename, $action = NULL, $chunk = 0) {
  $user = \Drupal::currentUser();
  $uid = $user->id();

  $module = \Drupal::request()->get('module', '');
  $file_size = \Drupal::request()->get('file_size', '');
  $chunk_size = \Drupal::request()->get('chunk_size', '');

  // \Drupal::service('file_system')->realpath('');
  // $user_dir = tripal_get_user_files_dir($uid); //old
  $user_dir = \Drupal::service('file_system')->realpath(tripal_get_user_files_dir($uid));
  if (!tripal_is_user_files_dir_writeable($uid)) {
    $message = "The user's data directory is not writeable: !user_dir";
    \Drupal::logger('tripal')->error($message, [
      '!user_dir' => $user_dir,
    ]);
    $result = [
      'status' => 'failed',
      'message' => $message,
      'file_id' => '',
    ];
    return new JsonResponse($result);
  }

  // Make sure we don't go over the user's quota, but only do this check
  // before loading the first chunk so we don't repeat it over and over again.
  if ($action == 'check' and $chunk == 0) {
    $usage = tripal_get_user_usage($uid);
    $quota = tripal_get_user_quota($uid);
    $quota_size = $quota->custom_quota;
    if ($file_size + $usage > $quota_size) {
      $result = [
        'status' => 'failed',
        'message' => t("Unfortunately, you can not upload this file as the size exceeds the remainder of your quota. See your account page under the 'Uploads' tab to manage your uploaded files."),
        'file_id' => '',
      ];
      return new JsonResponse($result);
    }
  }

  // Make sure we don't go over the max file upload size.
  $upload_max = \Drupal\Component\Utility\Bytes::toNumber(ini_get('upload_max_filesize'));
  if ($file_size > $upload_max) {
    $message = t("Unfortunately, you can not upload this file as the size exceeds the maximum file size allowed by this site: " . tripal_format_bytes($upload_max) . '. ');

    if ($user->hasPermission('administer tripal')) {
      $message .= t('You can manage the maximum file upload size by changing the upload_max_filesize in your php.ini file.');
    }
    $result = [
      'status' => 'failed',
      'message' => $message,
      'file_id' => '',
    ];
    return new JsonResponse($result);
  }

  $chunk_max = \Drupal\Component\Utility\Bytes::toNumber(ini_get('post_max_size'));
  if ($chunk_size > $chunk_max) {
    $message = t("Unfortunately, you can not upload this file as the chunk size exceeds the maximum file chunk size allowed by this site: " . tripal_format_bytes($chunk_max) . '. ');

    if ($user->hasPermission('administer tripal')) {
      $message .= t('You can manage the maximum file chunk size by changing the post_max_size value in your php.ini file.');
    }
    $result = [
      'status' => 'failed',
      'message' => $message,
      'file_id' => '',
    ];
    return new JsonResponse($result);
  }

  // Allow the module that will own the file to make some checks. The module
  // is allowed to stop the upload as needed.
  $hook_name = $module . '_file_upload_check';
  if (function_exists($hook_name)) {
    $details = [
      'filename' => $filename,
      'file_size' => $file_size,
      'chunk_size' => $chunk_size,
    ];
    $message = '';
    $status = $hook_name($action, $details, $message);
    if ($status === FALSE) {
      $result = [
        'status' => 'failed',
        'message' => $message,
        'file_id' => '',
      ];
      return new JsonResponse($result);
    }
  }

  switch ($action) {
    // If the action is 'save' then the callee is sending a chunk of the file
    case 'save':
      return tripal_file_upload_post($filename, $chunk, $user_dir);

    case 'check':
      return tripal_file_upload_verify($filename, $chunk, $user_dir);

    case 'merge':
      return tripal_file_upload_merge($filename, $type, $user_dir);
  }
}

/**
 * Saves the contents of the file being sent to the server.
 *
 * The file is saved using the filename the chunk number as an
 * extension.  This function uses file locking to prevent two
 * jobs from writing to the same file at the same time.
 */
function tripal_file_upload_post($filename, $chunk, $user_dir) {
  // Get the HTTP POST data.
  $postdata = fopen("php://input", "r");
  $size = $_SERVER['CONTENT_LENGTH'];

  // Store the chunked file in a temp folder.
  $temp_dir = $user_dir . '/temp/' . $filename;
  if (!file_exists($temp_dir)) {
    mkdir($temp_dir, 0700, TRUE);
  }

  // Open the file for writing if doesn't already exist with the proper size.
  $chunk_file = $temp_dir . '/' . $filename . '.chunk.' . $chunk;
  if (!file_exists($chunk_file) or filesize($chunk_file) != $size) {
    // Read the data 1 KB at a time and write to the file
    $fh = fopen($chunk_file, "w");
    // Lock the file for writing. We don't want two different
    // processes trying to write to the same file at the same time.
    if (flock($fh, LOCK_EX)) {
      while ($data = fread($postdata, 1024)) {
        fwrite($fh, $data);
      }
      flock($fh, LOCK_UN);
      fclose($fh);
    }
  }
  fclose($postdata);

  // Get the current log, updated and re-write it.
  $log = tripal_file_upload_read_log($temp_dir);
  $log['chunks_written'][$chunk] = $size;
  tripal_file_upload_write_log($temp_dir, $log);
  return new JsonResponse([]);
}

/**
 * Reads the upload log file.
 *
 * The log file is used to keep track of which chunks have been uploaded.
 * The format is an array with a key of 'chunks_written' which each element
 * a key/value pair containing the chunk index as the key and the chunk size
 * as the value.
 *
 * @param $temp_dir
 *   The directory where the log file will be written. It must be a unique
 *   directory where only chunks from a single file are kept.
 */
function tripal_file_upload_read_log($temp_dir) {

  $log_file = $temp_dir . '/tripal_upload.log';
  $log = NULL;

  if (file_exists($log_file)) {
    $fh = fopen($log_file, "r");

    if ($fh and flock($fh, LOCK_EX)) {
      $contents = '';
      while ($data = fread($fh, 1024)) {
        $contents .= $data;
      }
      $log = unserialize($contents);
    }
    flock($fh, LOCK_UN);
    fclose($fh);
  }
  if (!is_array($log)) {
    $log = [
      'chunks_written' => [],
    ];
  }
  return $log;
}

/**
 * Writes the upload log file.
 *
 * The log file is used to keep track of which chunks have been uploaded.
 * The format is an array with a key of 'chunks_written' which each element
 * a key/value pair containing the chunk index as the key and the chunk size
 * as the value.
 *
 * @param $temp_dir
 *   The directory where the log file will be written. It must be a unique
 *   directory where only chunks from a single file are kept.
 * @param $log
 *   The log array, that is serialized and then written to the file.
 */
function tripal_file_upload_write_log($temp_dir, $log) {

  $log_file = $temp_dir . '/tripal_upload.log';

  if (!$log or !is_array($log)) {
    $log = [
      'chunks_written' => [],
    ];
  }

  // Get the last chunk read
  $fh = fopen($log_file, "w");
  if ($fh and flock($fh, LOCK_EX)) {
    fwrite($fh, serialize($log));
  }
  flock($fh, LOCK_UN);
  fclose($fh);
}

/**
 * Checks the size of a chunk to see if is fully uploaded.
 *
 * @return
 *   returns a JSON array with a status, message and the
 *   current chunk.
 */
function tripal_file_upload_verify($filename, $chunk, $user_dir) {

  $chunk_size = $_GET['chunk_size'];

  // Store the chunked file in a temp folder.
  $temp_dir = $user_dir . '/temp' . '/' . $filename;
  if (!file_exists($temp_dir)) {
    mkdir($temp_dir, 0700, TRUE);
  }

  // Get the upload log.
  $log = tripal_file_upload_read_log($temp_dir);
  $chunks_written = $log['chunks_written'];
  $max_chunk = 0;
  if ($chunks_written) {
    $max_chunk = max(array_keys($chunks_written));
  }

  // Iterate through the chunks in order and see if any are missing.
  // If so then break out of the loop and this is the chunk to start
  // on.
  for ($i = 0; $i <= $max_chunk; $i++) {
    if (!array_key_exists($i, $chunks_written)) {
      break;
    }
  }

  $result = [
    'status' => 'success',
    'message' => '',
    'curr_chunk' => $i,
  ];
  return new JsonResponse($result);
}

/**
 * Merges all chunks into a single file
 */
function tripal_file_upload_merge($filename, $type, $user_dir) {
  $uid = \Drupal::currentUser()->id();
  $db = \Drupal::database();

  $module = $_GET['module'];

  $status = 'merging';
  $message = '';

  // Build the paths to the temp directory and merged file.
  $temp_dir = $user_dir . '/temp' . '/' . $filename;
  $merge_file = $user_dir . '/' . $filename;

  // If the temp directory where the chunks are found does not exist and the
  // client is requesting merge then most likely the file has already been
  // merged and the user hit the upload button again.
  if (file_exists($temp_dir)) {
    // Get the upload log.
    $log = tripal_file_upload_read_log($temp_dir);

    // Keep up with the expected file size.
    $merge_size = 0;

    // Open the new merged file.
    $merge_fh = fopen($merge_file, "w");
    if ($merge_fh) {
      if (flock($merge_fh, LOCK_EX)) {
        $chunks_written = $log['chunks_written'];
        $max_chunk = max(array_keys($chunks_written));
        // Iterate through the chunks in order and see if any are missing.
        // If so then break out of the loop and fail. Otherwise concatenate
        // them together.
        for ($i = 0; $i <= $max_chunk; $i++) {
          if (!array_key_exists($i, $chunks_written)) {
            $status = 'failed';
            $message = 'Missing some chunks. Cannot complete file merge.';
            break;
          }
          $merge_size += $chunks_written[$i];
          $chunk_file = $temp_dir . '/' . $filename . '.chunk.' . $i;
          $cfh = fopen($chunk_file, 'r');
          while ($data = fread($cfh, 1024)) {
            fwrite($merge_fh, $data);
          }
          fclose($cfh);
        } // end for ($i = 0; $i <= $max_chunk; $i++) { ...

        if (filesize($merge_file) != $merge_size) {
          $status = 'failed';
          $message = 'File was uploaded but final merged size is incorrect: ' . $merge_file . '.';
        }
      }
      else {
        $status = 'failed';
        $message = 'Cannot lock merged file for writing: ' . $merge_file . '.';
      }
    }
    else {
      $status = 'failed';
      $message = 'Cannot open merged file: ' . $merge_file . '.';
    }
    flock($merge_fh, LOCK_UN);
    fclose($merge_fh);
  }

  // Make sure the merged file exists
  if (!file_exists($merge_file)) {
    $status = 'failed';
    $message = 'Merge file is missing after upload ' . $merge_file . '.';
  }

  $file_id = NULL;

  // If the file has been successfully merged then do a few other things...
  if ($status != 'failed') {

    // See if this file is already managed if so, then it has been uploaded
    // before and we don't need to add a managed item again.
    $fid = $db->select('file_managed', 'fm')
      ->fields('fm', ['fid'])
      ->condition('uri', $merge_file)
      ->execute()
      ->fetchField();

    // Add the file if it is not already managed.
    if (!$fid) {
      $file = \Drupal\file\Entity\File::create([
        'uri' => $merge_file,
        'uid' => $uid,
        'filename' => $filename,
      ]);
      $file->setPermanent();
      $file->save();
      $fid = $file->id();
    }

    // Reload the file object to get a full object.
    $file_id = $fid;
    $file = \Drupal\file\Entity\File::load($fid);

    // Set the file as being managed by Tripal.
    $file_usage = \Drupal::service('file.usage');
    $file_usage->add($file, 'tripal', $type, $file_id);

    // Set the file expiration.
    tripal_reset_file_expiration($fid);

    // Generate an md5 file the uploaded file.
    // $full_path = \Drupal::service('file_system')->realpath($file->uri); // old
    $full_path = \Drupal::service('file_system')->realpath($file->getFileUri());
    $md5sum = md5_file($full_path);
    $md5sum_file = fopen("$full_path.md5", "w");
    fwrite($md5sum_file, $md5sum);
    fclose($md5sum_file);

    // Remove the temporary directory.
    \Drupal::service('file_system')->deleteRecursive($temp_dir);

    // Now let the submitting module deal with it.
    $function = $module . '_handle_uploaded_file';
    if (function_exists($function)) {
      $function($file, $type);
    }
    $status = 'completed';
  }

  if ($status == 'failed') {
    \Drupal::logger('tripal')->error($message);
  }
  $result = [
    'status' => $status,
    'message' => $message,
    'file_id' => $file_id,
  ];
  return new JsonResponse($result);
}
