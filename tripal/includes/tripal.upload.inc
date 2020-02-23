<?php

function tripal_file_upload($type, $filename, $action = NULL, $chunk = 0) {
  global $user;

  $module = array_key_exists('module', $_GET) ? $_GET['module'] : '';
  $file_size = array_key_exists('file_size', $_GET) ? $_GET['file_size'] : '';
  $chunk_size = array_key_exists('chunk_size', $_GET) ? $_GET['chunk_size'] : '';

  $user_dir = tripal_get_user_files_dir($user);
  if (!tripal_is_user_files_dir_writeable($user)) {
    $message = 'The user\'s data directory is not writeable: !user_dir.';
    watchdog('tripal', $message, ['!user_dir' => $user_dir], WATCHDOG_ERROR);
    drupal_json_output([
      'status' => 'failed',
      'message' => $message,
      'file_id' => '',
    ]);
    return;
  }

  // Make sure we don't go over the user's quota, but only do this check
  // before loading the first chunk so we don't repeat it over and over again.
  if ($action == 'check' and $chunk == 0) {
    $usage = tripal_get_user_usage($user->uid);
    $quota = tripal_get_user_quota($user->uid);
    $quota_size = $quota->custom_quota;
    if ($file_size + $usage > $quota_size) {
      drupal_json_output([
        'status' => 'failed',
        'message' => t("Unfortunately, you can not upload this file as the size exceeds the remainder of your quota. See your account page under the 'Uploads' tab to manage your uploaded files."),
        'file_id' => '',
      ]);
      return;
    }

    // Make sure we don't go over the max file upload size.
    $upload_max = variable_get('tripal_upload_max_size', 10000000000);
    if ($file_size > $upload_max) {
      $message = t("Unfortunately, you can not upload this file as the size exceeds the the maximum file size allowed by this site: " . tripal_format_bytes($upload_max) . '. ');

      if (user_access('administer tripal')) {
        $message .= t('You can manage the file upload by visiting: Home » Administration » Tripal » User File Management.');
      }
      drupal_json_output([
        'status' => 'failed',
        'message' => $message,
        'file_id' => '',
      ]);
      return;
    }
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
      drupal_json_output([
        'status' => 'failed',
        'message' => $message,
        'file_id' => '',
      ]);
      return;
    }
  }

  switch ($action) {
    // If the action is 'save' then the callee is sending a chunk of the file
    case 'save':
      tripal_file_upload_put($filename, $chunk, $user_dir);
      break;
    case 'check':
      tripal_file_upload_verify($filename, $chunk, $user_dir);
      break;
    case 'merge':
      tripal_file_upload_merge($filename, $type, $user_dir);
      break;
  }
}

/**
 * Merges all chunks into a single file
 */
function tripal_file_upload_merge($filename, $type, $user_dir) {
  global $user;

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
        // If so then break out of the loop and fail. Otherwise concatentate
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
    $fid = db_select('file_managed', 'fm')
      ->fields('fm', ['fid'])
      ->condition('uri', $merge_file)
      ->execute()
      ->fetchField();

    // Add the file if it is not already managed.
    if (!$fid) {
      $file = new stdClass();
      $file->uri = $merge_file;
      $file->filename = $filename;
      $file->filemime = file_get_mimetype($merge_file);
      $file->uid = $user->uid;
      $file->status = FILE_STATUS_PERMANENT;
      $file = file_save($file);
      $fid = $file->fid;
    }

    // Reload the file object to get a full object.
    $file_id = $fid;
    $file = file_load($fid);

    // Set the file as being managed by Tripal.
    file_usage_add($file, 'tripal', $type, 0);

    // Set the file expiration.
    tripal_reset_file_expiration($fid);

    // Generate an md5 file the uploaded file.
    $full_path = drupal_realpath($file->uri);
    $md5sum = md5_file($full_path);
    $md5sum_file = fopen("$full_path.md5", "w");
    fwrite($md5sum_file, $md5sum);
    fclose($md5sum_file);

    // Remove the temporary directory.
    file_unmanaged_delete_recursive($temp_dir);

    // Now let the submitting module deal with it.
    $function = $module . '_handle_uploaded_file';
    if (function_exists($function)) {
      $function($file, $type);
    }
    $status = 'completed';
  }

  if ($status == 'failed') {
    watchdog('tripal', $message, [], WATCHDOG_ERROR);
  }
  drupal_json_output([
    'status' => $status,
    'message' => $message,
    'file_id' => $file_id,
  ]);
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

  drupal_json_output([
    'status' => 'success',
    'message' => '',
    'curr_chunk' => $i,
  ]);
}

/**
 * Saves the contents of the file being sent to the server.
 *
 * The file is saved using the filename the chunk number as an
 * extension.  This function uses file locking to prevent two
 * jobs from writing to the same file at the same time.
 */
function tripal_file_upload_put($filename, $chunk, $user_dir) {

  // Get the HTTP PUT data.
  $putdata = fopen("php://input", "r");
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
      while ($data = fread($putdata, 1024)) {
        fwrite($fh, $data);
      }
      flock($fh, LOCK_UN);
      fclose($fh);
    }
  }
  fclose($putdata);

  // Get the current log, updated and re-write it.
  $log = tripal_file_upload_read_log($temp_dir);
  $log['chunks_written'][$chunk] = $size;
  tripal_file_upoad_write_log($temp_dir, $log);
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
function tripal_file_upoad_write_log($temp_dir, $log) {

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
