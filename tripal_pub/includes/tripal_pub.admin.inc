<?php 

/**
 * Administrative settings form
 *
 * @ingroup tripal_pub
 */
function tripal_pub_admin() {
  $form = array();

  // before proceeding check to see if we have any
  // currently processing jobs. If so, we don't want
  // to give the opportunity to sync publications
  $active_jobs = FALSE;
  if (tripal_get_module_active_jobs('tripal_pub')) {
    $active_jobs = TRUE;
  }

  // add the field set for syncing publications
  if (!$active_jobs) {
    get_tripal_pub_admin_form_cleanup_set($form);
  }
  else {
    $form['notice'] = array(
     '#type' => 'fieldset',
     '#title' => t('Publication Management Temporarily Unavailable')
    );
    $form['notice']['message'] = array(
        '#value' => t('Currently, publication management jobs are waiting or are running. . Managemment features have been hidden until these jobs complete.  Please check back later once these jobs have finished.  You can view the status of pending jobs in the Tripal jobs page.'),
    );
  }

  return system_settings_form($form);
}


/**
 *
 *
 * @ingroup tripal_pub
 */
function get_tripal_pub_admin_form_cleanup_set(&$form) {
  $form['cleanup'] = array(
    '#type' => 'fieldset',
    '#title' => t('Clean Up')
  );
  $form['cleanup']['description'] = array(
     '#type' => 'item',
     '#value' => t("With Drupal and chado residing in different databases ".
        "it is possible that nodes in Drupal and publications in Chado become ".
        "\"orphaned\".  This can occur if an pub node in Drupal is ".
        "deleted but the corresponding chado pub is not and/or vice ".
        "versa. Click the button below to resolve these discrepancies."),
     '#weight' => 1,
  );
  $form['cleanup']['button'] = array(
    '#type' => 'submit',
    '#value' => t('Clean up orphaned publications'),
    '#weight' => 2,
  );
}

/**
 *
 * @ingroup tripal_pub
 */
function tripal_pub_admin_validate($form, &$form_state) {
  global $user;  // we need access to the user info
  $job_args = array();


  // -------------------------------------
  // Submit the Cleanup Job if selected
  if ($form_state['values']['op'] == t('Clean up orphaned publications')) {
    tripal_add_job('Cleanup orphaned publications', 'tripal_pub',
       'tripal_pub_cleanup', $job_args, $user->uid);
  }
}

/**
 * Remove orphaned drupal nodes
 *
 * @param $dummy
 *   Not Used -kept for backwards compatibility
 * @param $job_id
 *   The id of the tripal job executing this function
 *
 * @ingroup tripal_pub
 */
function tripal_pub_cleanup($dummy = NULL, $job_id = NULL) {

  return tripal_core_clean_orphaned_nodes('pub', $job_id);
  
}

