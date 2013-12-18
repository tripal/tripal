<?php
/**
 * Administrative settings form
 *
 * @ingroup tripal_contact
 */
function tripal_contact_admin() {
  $form = array();

  // before proceeding check to see if we have any
  // currently processing jobs. If so, we don't want
  // to give the opportunity to sync maps
  $active_jobs = FALSE;
  if (tripal_get_module_active_jobs('tripal_contact')) {
    $active_jobs = TRUE;
  }

  // add the field set for syncing maps
  if (!$active_jobs) {
    get_tripal_contact_admin_form_cleanup_set($form);
// TODO: complete coding of indexing and taxonomy assignment to features.    
//    get_tripal_contact_admin_form_reindex_set($form);
//    get_tripal_contact_admin_form_taxonomy_set($form);
  }
  else {
    $form['notice'] = array(
     '#type' => 'fieldset',
     '#title' => t('Feature Map Management Temporarily Unavailable')
    );
    $form['notice']['message'] = array(
        '#value' => t('Currently, feature map management jobs are waiting or are running. Managemment features have been hidden until these jobs complete.  Please check back later once these jobs have finished.  You can view the status of pending jobs in the Tripal jobs page.'),
    );
  }

  return system_settings_form($form);
}
/**
 *
 *
 * @ingroup tripal_contact
 */
function get_tripal_contact_admin_form_cleanup_set(&$form) {
  $form['cleanup'] = array(
    '#type' => 'fieldset',
    '#title' => t('Clean Up')
  );
  $form['cleanup']['description'] = array(
     '#type' => 'item',
     '#value' => t("With Drupal and chado residing in different databases ".
        "it is possible that nodes in Drupal and maps in Chado become ".
        "\"orphaned\".  This can occur if an map node in Drupal is ".
        "deleted but the corresponding chado map is not and/or vice ".
        "versa. Click the button below to resolve these discrepancies."),
     '#weight' => 1,
  );
  $form['cleanup']['button'] = array(
    '#type' => 'submit',
    '#value' => t('Clean up orphaned maps'),
    '#weight' => 2,
  );
}

/**
 *
 * @ingroup tripal_contact
 */
function get_tripal_contact_admin_form_reindex_set(&$form) {
   // define the fieldsets
  $form['reindex'] = array(
    '#type' => 'fieldset',
    '#title' => t('Reindex Map Features')
  );

  // get the list of maps
  $sql = "SELECT * FROM {contact} ORDER BY name";
  $lib_rset = chado_query($sql);

  // iterate through all of the maps
  $lib_boxes = array();
  while ($contact = db_fetch_object($lib_rset)) {
    $lib_boxes[$contact->contact_id] = "$contact->name";
  }
  $form['reindex']['description'] = array(
     '#type' => 'item',
     '#value' => t("This option allows for reindexing of only those features that belong to the selected maps below. All other features will be unaffected.  To reindex all features in the site see the Feature Administration page."),
   '#weight' => 1,
  );

  $form['reindex']['re-maps'] = array(
   '#title'       => t('Contacts'),
   '#type'        => t('checkboxes'),
   '#description' => t("Check the maps whoee features you want to reindex. Note: this list contains all maps, even those that may not be synced."),
   '#required'    => FALSE,
   '#prefix'      => '<div id="lib_boxes">',
   '#suffix'      => '</div>',
   '#options'     => $lib_boxes,
   '#weight' => 2,
  );
  $form['reindex']['re-button'] = array(
    '#type' => 'submit',
    '#value' => t('Reindex Features'),
    '#weight' => 3,
  );
}
/**
 *
 * @ingroup tripal_contact
 */
function tripal_contact_admin_validate($form, &$form_state) {
  global $user;  // we need access to the user info
  $job_args = array();

  // -------------------------------------
  // Submit the Reindex Job if selected
  if ($form_state['values']['op'] == t('Reindex Features')) {
    $contacts = $form_state['values']['re-maps'];
    foreach ($contacts as $contact_id) {
      if ($contact_id and preg_match("/^\d+$/i", $contact_id)) {
        // get the map info
        $sql = "SELECT * FROM {contact} WHERE contact_id = :contact_id";
        $contact = db_fetch_object(chado_query($sql, array(':contact_id' => $contact_id)));
        $job_args[0] = $contact_id;
        tripal_add_job("Reindex features for map: $contact->name", 'tripal_contact',
         'tripal_contact_reindex_features', $job_args, $user->uid);
      }
    }
  }
  
  // -------------------------------------
  // Submit the Cleanup Job if selected
  if ($form_state['values']['op'] == t('Clean up orphaned maps')) {
    tripal_add_job('Cleanup orphaned maps', 'tripal_contact',
       'tripal_contact_cleanup', $job_args, $user->uid);
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
 * @ingroup tripal_contact
 */
function tripal_contact_cleanup($dummy = NULL, $job_id = NULL) {

  return tripal_core_clean_orphaned_nodes('contact', $job_id);
  
}
/**
 * Add the map as a taxonomy term for associating with map_features
 *
 * @ingroup tripal_contact
 */
function tripal_contact_add_taxonomy($node, $contact_id) {

}