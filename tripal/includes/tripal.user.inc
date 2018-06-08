<?php


/**
 * Provides the page with a list of files uploaded by the user.
 *
 * @param $uid
 *   The user ID.
 *
 * @return
 *   A Drupal render array.
 */
 function tripal_user_files_page($uid) {

   // Get all of the files that have been uploaded by the user.
   // TODO: we should make this a paged query in case a user has a huge
   // numbef of uploaded files.
   $sql = "
     SELECT FM.fid, FM.filename, TGEF.expiration_date
     FROM {file_managed} FM
       INNER JOIN {file_usage} FU on FM.fid = FU.fid and FM.uid = :user_id
       LEFT JOIN {tripal_expiration_files} TGEF on TGEF.fid = FU.fid
     WHERE FU.module = 'tripal'
     GROUP BY FM.fid, TGEF.expiration_date
     ORDER BY FM.filename
   ";
   $files = db_query($sql, array(':user_id' => $uid));
   $rows = array();
   While ($entry = $files->fetchObject()) {
    $file = file_load($entry->fid);

    // Don't list files that don't exist on the file system.
    if (!file_exists($file->uri)) {
      continue;
    }

    $date_uploaded = date('Y-m-d H:i:s', $file->timestamp);
    $expiration = $entry->expiration_date ? date('Y-m-d H:i:s', $entry->expiration_date) : '';
    $actions = l('Delete', "user/$uid/files/$file->fid/delete") . ' | ' .
               l('Renew', "user/$uid/files/$file->fid/renew");

    $rows[] = array(
      $entry->fid,
      l($file->filename,"/user/$uid/files/$file->fid"),
      $date_uploaded,
      $expiration,
      tripal_format_bytes($file->filesize),
      $actions,
    );
  }
  $header = array('ID', 'File Name', 'Upload Date', 'Expiration', 'Size', 'Actions');

  // Get the user quota settings.
  $quota = tripal_get_user_quota($uid);
  $usage = tripal_get_user_usage($uid);

  $content = array(
    'page_title' => array(
      '#type' => 'markup',
      '#markup' => '<h2>Your Uploaded Files</h2>',
    ),
    'page_description' => array(
      '#type' => 'markup',
      '#markup' => '<p>' . t('Each user is allowed to consume a limited amount of space with uploaded files. This page provides details about your current usage, your limits and files you\'ve uploaded.') . '</p>',
    ),
    'usage' => array(
      '#type' => 'item',
      '#title' => 'Current Usage',
      '#markup' => tripal_format_bytes($usage),
      '#description' => t('The total number of bytes you currently consume.'),
    ),
    'quota' => array(
      '#type' => 'item',
      '#title' => 'Current Quota',
      '#markup' => tripal_format_bytes($quota->custom_quota),
      '#description' => t('The maximum number of bytes of files you can upload.')
    ),
    'expiration' => array(
      '#type' => 'item',
      '#title' => 'Current Days to Expire',
      '#markup' => $quota->custom_expiration,
      '#description' => t('The number of days a file will remain on the server before deletion. The expiration of date of a file can be renewed using the "Renew" link in the table below.')
    ),
    'file_list' => array(
      '#type' => 'item',
      '#title' => 'Uploaded Files',
      '#markup' => theme_table(array(
        'header' => $header,
        'rows' => $rows,
        'attributes' => array(),
        'caption' => t('Click a file name for more details.'),
        'colgroups' => array(),
        'sticky' => TRUE,
        'empty' => 'You currently have no uploaded files.',
      )),
    )
  );


  if ($usage < $quota->custom_quota) {
    drupal_set_message('Your file usage is currently below the file quota limit.');
  }
  else {
    drupal_set_message('Your file usage is currently over your file quota limit. Please remove some files before uploading more', 'warning');
  }

  return $content;
}

/**
 * User action to renew the expiration of a file.
 *
 * Adds the current time and the expiration date (either from default or if
 * the user has a custom expiration date) to tripal_expiration_files
 * table.
 *
 **/
function tripal_renew_file($fid) {
  $file = file_load($fid);
  $success = tripal_reset_file_expiration($fid);

  if ($success) {
    drupal_set_message('Successfully updated expiration date.');
  }
  drupal_goto('user/' . $file->uid . '/files/');
}

/**
 * Downloads a file.
 *
 * @param $fid
 *   The File ID of the file to be downloaded.
 */
function tripal_download_file($fid) {
  $file = file_load($fid);
  if (file_exists($file->uri)) {
    $headers = array();
    $headers['Content-Type'] = $file->filemime;
    $headers['Content-Disposition']  = 'attachment; filename=' . $file->filename;
    $headers['Content-Length'] = $file->filesize;
    file_transfer($file->uri, $headers);
  }
  else {
    drupal_set_message('Can not download. The file no longer exists on the server.', 'error');
    drupal_goto('user/' . $file->uid . '/files/');
  }
}

/**
 * Provides a confirmation form for deleting an uploaded file.
 */
function tripal_delete_file_form($form, $form_state, $uid, $fid) {
  $form = array();

  $file = file_load($fid);

  $form['uid'] = array(
    '#type' => 'value',
    '#value' => $uid,
  );
  $form['fid'] = array(
    '#type' => 'value',
    '#value' => $fid,
  );

  return confirm_form($form,
      t('Confirm deletion of the file named "' . $file->filename . '"?'),
      'user/' . $uid . '/files/'
  );
}

/**
 * Implements a form submit for deleting an uploaded file.
 */
function tripal_delete_file_form_submit($form, &$form_state) {
  $fid = $form_state['values']['fid'];
  $uid = $form_state['values']['uid'];
  $file = file_load($fid);

  // Remove the file from the file_usage table for all entries that link
  // to the tripal module.
  file_usage_delete($file, 'tripal', NULL, NULL, 0);

  // Get any remaining usage for other modules
  $file_usage = file_usage_list($file);

  // If this file is still used by the tripal module then something
  // didn't work right.
  if (in_array('tripal', $file_usage)) {
    drupal_set_message('The file could not be removed.  Please contact the site administrator.', 'error');
  }

  // If there is no other usage of this file from other modules then delete it.
  if (count(array_keys($file_usage)) == 0) {
    if (file_unmanaged_delete($file->uri)) {

      // Also remove the md5 checksum.
      if (file_exists(file_unmanaged_delete($file->uri . '.md5'))) {
        file_unmanaged_delete($file->uri . '.md5');
      }
      drupal_set_message('The file has been fully removed.');
    }
    else {
      drupal_set_message('The file has removed from this list and does not count against your quota, but other components of this site rely on this file. Thus it has not been fully removed.');
    }
  }
  drupal_goto('user/' . $file->uid . '/files/');
}

/**
 * Provides details about a file.
 */
function tripal_view_file($uid, $fid) {
  $file = file_load($fid);

  $headers = array();
  $rows = array();

  $actions = l('Delete', "user/$uid/files/$file->fid/delete") . '<br>' .
             l('Download', "user/$uid/files/$file->fid/download");

  // Name row
  $rows[] = array(
    array(
      'data' => 'File Name',
      'header' => TRUE,
      'width' => '20%',
    ),
    $file->filename
  );

  $date_uploaded = date('Y-m-d H:i:s', $file->timestamp);
  $rows[] = array(
    array(
      'data' => 'Upload Date',
      'header' => TRUE,
      'width' => '20%',
    ),
    $date_uploaded
  );

  $expiration_date = db_select('tripal_expiration_files', 'tgef')
    ->fields('tgef', array('expiration_date'))
    ->condition('fid', $fid)
    ->execute()
    ->fetchField();
  $expiration = $expiration_date ? date('Y-m-d H:i:s', $expiration_date) : '';
  $rows[] = array(
    array(
      'data' => 'Expiration Date',
      'header' => TRUE,
      'width' => '20%',
    ),
    $expiration
  );

  $rows[] = array(
    array(
      'data' => 'Actions',
      'header' => TRUE,
      'width' => '20%',
    ),
    $actions
  );

  $content = array(
    'description' => array(
      '#type' => 'markup',
      '#markup' => '<p>' . t('The following file has been uploaded.') . '</p>',
    ),
    'return' => array(
      '#type' => 'markup',
      '#markup' => '<p>' . l('View all Uploaded Files', "user/$uid/files") . '</p>',
    ),
    'file_details' => array(
      '#type' => 'markup',
      '#markup' => theme_table(array(
        'header' => $headers,
        'rows' => $rows,
        'attributes' => array(),
        'sticky' => FALSE,
        'caption' => '',
        'colgroups' => array(),
        'empty' => '',
      )),
    ),
  );
  return $content;
}


