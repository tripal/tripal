<?php

/**
 * Retrieves the user's quote and default days to expire.
 *
 * @param $uid
 *   The User ID.
 *
 * @return
 *   An associative array containing the quota and default days to
 *   expire.
 */
function tripal_get_user_quota($uid) {
  $quota = db_select('tripal_custom_quota', 'tgcq')->fields('tgcq', [
    'custom_quota',
    'custom_expiration',
  ])
    ->condition('uid', $uid)
    ->execute()
    ->fetchObject();
  if (!$quota) {
    $quota = new stdClass();
    $quota->custom_quota = variable_get('tripal_default_file_quota', pow(20, 6));
    $quota->custom_expiration = variable_get('tripal_default_file_expiration', '60');
  }
  return $quota;
}

/**
 * Sets a user's file space quota and default file expiration.
 *
 * @param $uid The
 *          User ID for whom the quota will be set.
 * @param $quota The
 *          quota
 * @param
 *          $expriation
 *
 * @return The inserted record.
 */
function tripal_set_user_quota($uid, $quota, $expriation) {
  $values = [
    'uid' => $uid,
    'custom_quota' => $quota,
    'custom_expiration' => $expriation,
  ];
  return db_insert('tripal_custom_quota')->fields($values)->execute();
}

/**
 * Removes a user's file space and default file expiration.
 *
 * @param $uid The
 *          User ID for whom the quota will be removed.
 *
 * @return
 */
function tripal_remove_user_quota($uid) {
  db_delete('tripal_custom_quota')->condition('uid', $uid)->execute();
}

/**
 * Retrieves the current size of all files uploaded by the user.
 *
 * @param $uid The
 *          User ID.
 *
 * @return The total number of bytes currently used.
 */
function tripal_get_user_usage($uid) {
  // Get the user's current file usage
  $sql = "
    SELECT SUM(filesize) FROM (
    SELECT DISTINCT FM.fid, FM.filename, FM.filesize
      FROM file_usage FU
        INNER JOIN file_managed FM ON FM.fid = FU.fid and FU.module = 'tripal'
      WHERE FM.uid = :uid) AS foo
  ";
  $total_usage = db_query($sql, [':uid' => $uid])->fetchField();
  return ($total_usage);
}

/**
 * Checks if a file needs to be expired.
 */
function tripal_expire_files(TripalJob $job = NULL) {
  $results = db_select('tripal_expiration_files', 'tgfe')
    ->fields('tgfe')
    ->execute();
  while ($result = $results->fetchObject()) {
    if (time() > $result->expiration_date) {

      $file = file_load($result->fid);
      if ($file) {
        if ($job) {
          $job->logMessage('File "' . $file->filename . '" has expired. Removing...');
        }
        // First remove the file from the file system.
        file_delete($file, TRUE);

        // Remove the file from our file expiration table.
        $query = db_delete('tripal_expiration_files');
        $query->condition('fid', $result->fid);
        $query->execute();
      }
    }
  }
}

/**
 * Resets the expiration data of a file managed by Tripal.
 *
 * @param $fid
 *   The file ID of the file to reset.
 *
 * @return
 *   The new expiration date on success, FALSE on failure.
 */
function tripal_reset_file_expiration($fid) {

  $file = file_load($fid);
  try {
    $quota = tripal_get_user_quota($file->uid);
    $custom_expiration = $quota->custom_expiration;
    $expiration_date = time() + $custom_expiration * 24 * 60 * 60;

    db_delete('tripal_expiration_files')
      ->condition('fid', $fid)
      ->execute();
    db_insert('tripal_expiration_files')
      ->fields([
        'fid' => $file->fid,
        'expiration_date' => $expiration_date,
      ])
      ->execute();
  } catch (Exception $e) {
    tripal_report_error('trp_quota', TRIPAL_ERROR, $e->getMessage());
    return FALSE;
  }
  return $expiration_date;
}