<?php
/**
 * @file
 * @todo Add file header description
 */

function tripal_natural_diversity_admin($form_state = NULL) {
  $form = array();

  // before proceeding check to see if we have any
  // currently processing jobs. If so, we don't want
  // to give the opportunity to sync nd_geolocation
  $active_jobs = FALSE;
  if (tripal_get_module_active_jobs('tripal_nd_geolocation')) {
    $active_jobs = TRUE;
  }

  // add the field set for syncing libraries
  if (!$active_jobs) {
    get_tripal_natural_diversity_admin_form_sync_set($form);
    get_tripal_natural_diversity_admin_form_cleanup_set($form);
  }
  else {
    $form['notice'] = array(
    '#type' => 'fieldset',
    '#title' => t('Natural Diversity Management Temporarily Unavailable')
    );
    $form['notice']['message'] = array(
      '#value' => t('Currently, natural_diversity management jobs are waiting or are running. . Managemment features have been hidden until these jobs complete.  Please check back later once these jobs have finished.  You can view the status of pending jobs in the Tripal jobs page.'),
    );
  }

  return system_settings_form($form);
}
/**
 *
 *
 * @ingroup tripal_natural_diversity
 */
function get_tripal_natural_diversity_admin_form_cleanup_set(&$form) {
  $form['cleanup'] = array(
    '#type' => 'fieldset',
    '#title' => t('Clean Up')
  );
  $form['cleanup']['description'] = array(
    '#type' => 'item',
    '#value' => t("With Drupal and chado residing in different databases ".
    "it is possible that nodes in Drupal and nd_geolocation in Chado become ".
    "\"orphaned\".  This can occur if a nd_geolocation node in Drupal is ".
    "deleted but the corresponding chado natural_diversity is not and/or vice ".
    "versa. Click the button below to resolve these discrepancies."),
    '#weight' => 1,
  );
  $form['cleanup']['button'] = array(
    '#type' => 'submit',
    '#value' => t('Clean up orphaned nd_geolocation'),
    '#weight' => 2,
  );
}
/**
 *
 * @ingroup tripal_natural_diversity
 */
function get_tripal_natural_diversity_admin_form_sync_set(&$form) {
  // define the fieldsets
  $form['sync'] = array(
    '#type' => 'fieldset',
    '#title' => t('Sync ND Geolocation')
  );

  // before proceeding check to see if we have any
  // currently processing jobs. If so, we don't want
  // to give the opportunity to sync libraries
  $active_jobs = FALSE;
  if (tripal_get_module_active_jobs('tripal_nd_geolocation')) {
    $active_jobs = TRUE;
  }

  if (!$active_jobs) {

    // get the list of nd_geolocation
    $sql = "SELECT * FROM {nd_geolocation} ORDER BY description";
    $org_rset = chado_query($sql);

    // if we've added any nd_geolocation to the list that can be synced
    // then we want to build the form components to allow the user
    // to select one or all of them.  Otherwise, just present
    // a message stating that all nd_geolocation are currently synced.
    $nd_geo_boxes = array();
    $added = 0;
    while ($nd_geolocation = db_fetch_object($org_rset)) {
      // check to see if the nd_geolocation is already present as a node in drupal.
      // if so, then skip it.
      $sql = "SELECT * FROM {chado_nd_geolocation} WHERE nd_geolocation_id = %d";
      if (!db_fetch_object(db_query($sql, $nd_geolocation->nd_geolocation_id))) {
        $nd_geo_boxes[$nd_geolocation->nd_geolocation_id] = $nd_geolocation->description;
        $added++;
      }
    }

    // if we have nd_geolocation we need to add to the checkbox then
    // build that form element
    if ($added > 0) {
      $nd_geo_boxes['all'] = "All ND Geolocation";

      $form['sync']['nd_geolocation'] = array(
        '#title'       => t('Available ND Geolocation'),
        '#type'        => t('checkboxes'),
        '#description' => t("Check the nd_geolocation you want to sync.  Drupal content will be created for each of the nd_geolocation listed above.  Select 'All Natural Diversity' to sync all of them."),
        '#required'    => FALSE,
        '#prefix'      => '<div id="org_boxes">',
        '#suffix'      => '</div>',
        '#options'     => $nd_geo_boxes,
      );
      $form['sync']['button'] = array(
        '#type' => 'submit',
        '#value' => t('Submit Sync Job')
      );
    }
    // we don't have any nd_geolocation to select from
    else {
    $form['sync']['value'] = array(
        '#value' => t('All nd_geolocation in Chado are currently synced with Drupal.')
    );
    }
  }
  // we don't want to present a form since we have an active job running
  else {
    $form['sync']['value'] = array(
        '#value' => t('Currently, jobs exist related to chado nd_geolocation. Please check back later for nd_geolocation that can by synced once these jobs have finished.  You can view the status of pending jobs in the Tripal jobs page.')
    );
  }
}
/**
 *
 * @ingroup tripal_natural_diversity
 */
function tripal_natural_diversity_admin_validate($form, &$form_state) {
  global $user;  // we need access to the user info
  $job_args = array();

  if ($form_state['values']['op'] == t('Submit Sync Job')) {

    // check to see if the user wants to sync chado and drupal.  If
    // so then we need to register a job to do so with tripal
    $nd_geolocation = $form_state['values']['nd_geolocation'];
    $do_all = FALSE;
    $to_sync = array();

    foreach ($nd_geolocation as $nd_geolocation_id) {
      if (preg_match("/^all$/i" , $nd_geolocation_id)) {
        $do_all = TRUE;
      }
      if ($nd_geolocation_id and preg_match("/^\d+$/i" , $nd_geolocation_id)) {
        // get the list of nd_geolocation
        $sql = "SELECT * FROM {nd_geolocation} WHERE nd_geolocation_id = %d";
        $nd_geolocation = db_fetch_object(chado_query($sql, $nd_geolocation_id));
        $to_sync[$nd_geolocation_id] = "$nd_geolocation->description";
      }
    }

    // submit the job the tripal job manager
    if ($do_all) {
      tripal_add_job('Sync all nd_geolocation' , 'tripal_natural_diversity',
      'tripal_natural_diversity_sync_nd_geolocation' , $job_args , $user->uid);
    }
    else{
      foreach ($to_sync as $nd_geolocation_id => $name) {
        $job_args[0] = $nd_geolocation_id;
        tripal_add_job("Sync nd_geolocation: $name" , 'tripal_natural_diversity',
          'tripal_natural_diversity_sync_nd_geolocation' , $job_args , $user->uid);
      }
    }
  }

  // -------------------------------------
  // Submit the Cleanup Job if selected
  if ($form_state['values']['op'] == t('Clean up orphaned nd_geolocation')) {
    tripal_add_job('Cleanup orphaned nd_geolocation', 'tripal_natural_diversity',
      'tripal_natural_diversity_cleanup', $job_args, $user->uid);
  }
}
/**
 * Synchronize natural_diversity from chado to drupal
 *
 * @ingroup tripal_natural_diversity
 */
function tripal_natural_diversity_sync_nd_geolocation($nd_geolocation_id = NULL, $job_id = NULL) {
  global $user;
  $page_content = '';

  if (!$nd_geolocation_id) {
    $sql = "SELECT * FROM {nd_geolocation} P";
    $results = chado_query($sql);
  }
  else {
    $sql = "SELECT * FROM {nd_geolocation} P WHERE nd_geolocation_id = %d";
    $results = chado_query($sql, $nd_geolocation_id);
  }

  // We'll use the following SQL statement for checking if the nd_geolocation
  // already exists as a drupal node.
  $sql = "SELECT * FROM {chado_nd_geolocation} ".
         "WHERE nd_geolocation_id = %d";

  while ($nd_geolocation = db_fetch_object($results)) {

    // check if this nd_geolocation already exists in the drupal database. if it
    // does then skip this nd_geolocation and go to the next one.
    if (!db_fetch_object(db_query($sql, $nd_geolocation->nd_geolocation_id))) {

      $new_node = new stdClass();
      $new_node->type = 'chado_nd_geolocation';
      $new_node->uid = $user->uid;
      $new_node->nd_geolocation_id = $nd_geolocation->nd_geolocation_id;
      $new_node->title = $nd_geolocation->description;
      $new_node->latitude = $nd_geolocation->latitude;
      $new_node->longitude = $nd_geolocation->longitude;
      $new_node->altitude = $nd_geolocation->altitude;
      $new_node->geodetic_datum = $nd_geolocation->geodetic_datum;
      node_validate($new_node);
      if (!form_get_errors()) {
        $node = node_submit($new_node);
        node_save($node);
        db_query("UPDATE node SET title = '%s' WHERE nid = %d", $nd_geolocation->description, $node->nid); // Use description as node title
        if ($node->nid) {
          print "Added $nd_geolocation->description\n";
        }
      }
      else {
        print "Failed to insert nd_geolocation $nd_geolocation->description\n";
      }
    }
    else {
      print "Skipped $nd_geolocation->description\n";
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
 * @ingroup tripal_natural_diversity
 */
function tripal_natural_diversity_cleanup($dummy = NULL, $job_id = NULL) {

  return tripal_core_clean_orphaned_nodes('nd_geolocation', $job_id);
}