<?php
/**
 * @file
 * @todo Add file header description
 */

function tripal_eimage_admin($form_state = NULL) {
  $form = array();

  // before proceeding check to see if we have any
  // currently processing jobs. If so, we don't want
  // to give the opportunity to sync libraries
  $active_jobs = FALSE;
  if (tripal_get_module_active_jobs('tripal_eimage')) {
    $active_jobs = TRUE;
  }

  // add the field set for syncing libraries
  if (!$active_jobs) {
    get_tripal_eimage_admin_form_sync_set($form);
    get_tripal_eimage_admin_form_cleanup_set($form);

  }
  else {
    $form['notice'] = array(
    '#type' => 'fieldset',
    '#title' => t('Eimage Management Temporarily Unavailable')
    );
    $form['notice']['message'] = array(
      '#value' => t('Currently, eimage management jobs are waiting or are running. . Managemment features have been hidden until these jobs complete.  Please check back later once these jobs have finished.  You can view the status of pending jobs in the Tripal jobs page.'),
    );
  }

  return system_settings_form($form);
}
/**
 *
 *
 * @ingroup tripal_eimage
 */
function get_tripal_eimage_admin_form_cleanup_set(&$form) {
  $form['cleanup'] = array(
    '#type' => 'fieldset',
    '#title' => t('Clean Up')
  );
  $form['cleanup']['description'] = array(
    '#type' => 'item',
    '#value' => t("With Drupal and chado residing in different databases ".
    "it is possible that nodes in Drupal and eimages in Chado become ".
    "\"orphaned\".  This can occur if an eimage node in Drupal is ".
    "deleted but the corresponding chado eimage is not and/or vice ".
    "versa. Click the button below to resolve these discrepancies."),
    '#weight' => 1,
  );
  $form['cleanup']['button'] = array(
    '#type' => 'submit',
    '#value' => t('Clean up orphaned eimages'),
    '#weight' => 2,
  );
}
/**
 *
 * @ingroup tripal_eimage
 */
function get_tripal_eimage_admin_form_sync_set(&$form) {
  // define the fieldsets
  $form['sync'] = array(
    '#type' => 'fieldset',
    '#title' => t('Sync Eimages')
  );

  // before proceeding check to see if we have any
  // currently processing jobs. If so, we don't want
  // to give the opportunity to sync libraries
  $active_jobs = FALSE;
  if (tripal_get_module_active_jobs('tripal_eimage')) {
    $active_jobs = TRUE;
  }

  if (!$active_jobs) {

    $eimage_boxes = array();

    // get the list of eimages
    $sql = "SELECT E.* FROM {eimage} E LEFT JOIN chado_eimage CE ON E.eimage_id = CE.eimage_id WHERE CE.eimage_id IS NULL LIMIT 100";
    $org_rset = chado_query($sql);

    // if we've added any eimages to the list that can be synced
    // then we want to build the form components to allow the user
    // to select one or all of them.  Otherwise, just present
    // a message stating that all eimages are currently synced.

    $added = 0;
    $eimage_boxes = array();
    while ($eimage = db_fetch_object($org_rset)) {
        $eimage_boxes[$eimage->eimage_id] = "[" . $eimage->eimage_type . "] " . $eimage->image_uri;
        $added++;
    }
    
    // if we have eimages we need to add to the checkbox then
    // build that form element
    if ($added > 0) {
      $eimage_boxes['all'] = "All Eimages";

      $form['sync']['eimages'] = array(
        '#title'       => t('Available Eimages (First 100 eimages listed if there are too many)'),
        '#type'        => t('checkboxes'),
        '#description' => t("Check the eimages you want to sync.  Drupal content will be created for each of the eimages listed above.  Select 'All Eimages' to sync all of them."),
        '#required'    => FALSE,
        '#prefix'      => '<div id="org_boxes">',
        '#suffix'      => '</div>',
        '#options'     => $eimage_boxes,
      );
      $form['sync']['button'] = array(
        '#type' => 'submit',
        '#value' => t('Submit Sync Job')
      );
    }
    // we don't have any eimages to select from
    else {
    $form['sync']['value'] = array(
        '#value' => t('All eimages in Chado are currently synced with Drupal.')
    );
    }
  }
  // we don't want to present a form since we have an active job running
  else {
    $form['sync']['value'] = array(
        '#value' => t('Currently, jobs exist related to chado eimages. Please check back later for eimages that can by synced once these jobs have finished.  You can view the status of pending jobs in the Tripal jobs page.')
    );
  }
}
/**
 *
 * @ingroup tripal_eimage
 */
function tripal_eimage_admin_validate($form, &$form_state) {
  global $user;  // we need access to the user info
  $job_args = array();

  if ($form_state['values']['op'] == t('Submit Sync Job')) {

    // check to see if the user wants to sync chado and drupal.  If
    // so then we need to register a job to do so with tripal
    $eimages = $form_state['values']['eimages'];
    $do_all = FALSE;
    $to_sync = array();

    foreach ($eimages as $eimage_id) {
      if (preg_match("/^all$/i" , $eimage_id)) {
        $do_all = TRUE;
      }
      if ($eimage_id and preg_match("/^\d+$/i" , $eimage_id)) {
        // get the list of eimages
        $sql = "SELECT * FROM {Eimage} WHERE eimage_id = %d";
        $eimage = db_fetch_object(chado_query($sql, $eimage_id));
        $to_sync[$eimage_id] = "[" . $eimage->eimage_type . "] $eimage->image_uri";
      }
    }

    // submit the job the tripal job manager
    if ($do_all) {
      tripal_add_job('Sync all eimages' , 'tripal_eimage',
      'tripal_eimage_sync_eimages' , $job_args , $user->uid);
    }
    else{
      foreach ($to_sync as $eimage_id => $name) {
        $job_args[0] = $eimage_id;
        tripal_add_job("Sync eimage: $name" , 'tripal_eimage',
          'tripal_eimage_sync_eimages' , $job_args , $user->uid);
      }
    }
  }

  // -------------------------------------
  // Submit the Cleanup Job if selected
  if ($form_state['values']['op'] == t('Clean up orphaned eimages')) {
    tripal_add_job('Cleanup orphaned eimages', 'tripal_eimage',
      'tripal_eimage_cleanup', $job_args, $user->uid);
  }
}
/**
 * Synchronize eimages from chado to drupal
 *
 * @ingroup tripal_eimage
 */
function tripal_eimage_sync_eimages($eimage_id = NULL, $job_id = NULL) {
  global $user;
  $page_content = '';

  if (!$eimage_id) {
    $sql = "SELECT * FROM {eimage} P";
    $results = chado_query($sql);
  }
  else {
    $sql = "SELECT * FROM {eimage} P WHERE eimage_id = %d";
    $results = chado_query($sql, $eimage_id);
  }

  // We'll use the following SQL statement for checking if the eimage
  // already exists as a drupal node.
  $sql = "SELECT * FROM {chado_eimage} ".
         "WHERE eimage_id = %d";

  while ($eimage = db_fetch_object($results)) {

    // check if this eimage already exists in the drupal database. if it
    // does then skip this eimage and go to the next one.
    if (!db_fetch_object(db_query($sql, $eimage->eimage_id))) {

      $new_node = new stdClass();
      $new_node->type = 'chado_eimage';
      $new_node->uid = $user->uid;
      $new_node->title = "$eimage->image_uri";
      $new_node->eimage_id = $eimage->eimage_id;
      $new_node->eimage_type = $eimage->eimage_type;
      $new_node->eimage_data = $eimage->eimage_data;
      node_validate($new_node);
      if (!form_get_errors()) {
        $node = node_submit($new_node);
        node_save($node);
        if ($node->nid) {
          print "Added [" . $eimage->eimage_type . "] $eimage->image_uri\n";
        }
      }
      else {
        print "Failed to insert eimage [" . $eimage->eimage_type . "] $eimage->image_uri\n";
      }
    }
    else {
      print "Skipped [" . $eimage->eimage_type . "] $eimage->image_uri\n";
    }
  }
  return $page_content;
}

/**
 * Remove orphaned drupal nodes
 *
 * @param $dummy
 *   Not Used -kept for backwards compatibility
 * @param $job_id
 *   The id of the tripal job executing this function
 *
 * @ingroup tripal_eimage
 */
function tripal_eimage_cleanup($dummy = NULL, $job_id = NULL) {

  return tripal_core_clean_orphaned_nodes('eimage', $job_id);
}