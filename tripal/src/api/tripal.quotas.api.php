<?php

use \Drupal\file\Entity\File;

/**
 * Retrieves the user's quote and default days to expire.
 *
 * @param int $uid
 *   The User ID.
 *
 * @return \stdClass
 *  An object containing the quota and default days to expire.
 */
function tripal_get_user_quota($uid) {
  $quota = Drupal::database()
    ->select('tripal_custom_quota', 'tgcq')
    ->fields('tgcq', [
      'custom_quota',
      'custom_expiration',
    ])
    ->condition('uid', $uid)
    ->execute()
    ->fetchObject();

  if (!$quota) {
    $quota = new stdClass();
    $quota->custom_quota = Drupal::state()
      ->get('tripal_default_file_quota', pow(20, 6));
    $quota->custom_expiration = Drupal::state()
      ->get('tripal_default_file_expiration', '60');
  }

  return $quota;
}

/**
 * Sets a user's file space quota and default file expiration.
 * If a quota already exists, it will be replaced.
 *
 * @param int $uid
 *  The User ID for whom the quota will be set.
 * @param $quota
 *  The quota
 * @param int $expiration
 *   The expiration timestamp
 *
 * @return int The inserted record.
 * @throws \Exception
 */
function tripal_set_user_quota($uid, $quota, $expiration) {
  $values = [
    'uid' => $uid,
    'custom_quota' => $quota,
    'custom_expiration' => $expiration,
  ];

  Drupal::database()
    ->delete('tripal_custom_quota')
    ->condition('uid', $uid)
    ->execute();

  return Drupal::database()
    ->insert('tripal_custom_quota')
    ->fields($values)
    ->execute();
}

/**
 * Removes a user's file space and default file expiration.
 *
 * @param int $uid
 *  The User ID for whom the quota will be removed.
 *
 * @return void
 */
function tripal_remove_user_quota($uid) {
  Drupal::database()
    ->delete('tripal_custom_quota')
    ->condition('uid', $uid)
    ->execute();
}

/**
 * Retrieves the current size of all files uploaded by the user.
 *
 * @param int $uid The
 *          User ID.
 *
 * @return int The total number of bytes currently used.
 */
function tripal_get_user_usage($uid) {
  // Get the user's current file usage
  $db = \Drupal::database();

  $query = $db->select('file_usage', 'fu');
  $query->join('file_managed', 'fm', 'fm.fid = fu.fid');
  $query->addExpression('sum(filesize)', 'total_size');
  $query->condition('fu.module', 'tripal');
  $query->condition('fm.uid', $uid);
  $query = $query->execute();

  return (int) $query->fetchObject()->total_size;
}

/**
 * Checks if a file needs to be expired.
 */
// TODO: TripalJob class needs to be defined before implementing this function.
//function tripal_expire_files(TripalJob $job = NULL) {
//  $results = db_select('tripal_expiration_files', 'tgfe')
//    ->fields('tgfe')
//    ->execute();
//  while ($result = $results->fetchObject()) {
//    if (time() > $result->expiration_date) {
//
//      $file = file_load($result->fid);
//      if ($file) {
//        if ($job) {
//          $job->logMessage('File "' . $file->filename . '" has expired. Removing...');
//        }
//        // First remove the file from the file system.
//        file_delete($file, TRUE);
//
//        // Remove the file from our file expiration table.
//        $query = db_delete('tripal_expiration_files');
//        $query->condition('fid', $result->fid);
//        $query->execute();
//      }
//    }
//  }
//}

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
  $file = File::load($fid);

  try {
    $quota = tripal_get_user_quota($file->getOwnerId());
    $custom_expiration = $quota->custom_expiration;
    $expiration_date = time() + $custom_expiration * 24 * 60 * 60;

    Drupal::database()
      ->delete('tripal_expiration_files')
      ->condition('fid', $fid)
      ->execute();

    Drupal::database()->insert('tripal_expiration_files')->fields([
      'fid' => $file->id(),
      'expiration_date' => $expiration_date,
    ])->execute();
  } catch (Exception $e) {
    // TODO: Uncomment once tripal_report_error is implemented
    //tripal_report_error('trp_quota', TRIPAL_ERROR, $e->getMessage());
    return FALSE;
  }

  return $expiration_date;
}
