<?php

/**
 * Admin launchpad
 */
function tripal_featuremap_admin_featuremaps_listing() {
  $output = '';

  // set the breadcrumb
  $breadcrumb = array();
  $breadcrumb[] = l('Home', '<front>');
  $breadcrumb[] = l('Administration', 'admin');
  $breadcrumb[] = l('Tripal', 'admin/tripal');
  $breadcrumb[] = l('Chado', 'admin/tripal/chado');
  $breadcrumb[] = l('Maps', 'admin/tripal/chado/tripal_featuremap');
  drupal_set_breadcrumb($breadcrumb);

  // Add the view
  $view = views_embed_view('tripal_featuremap_admin_featuremaps','default');
  if (isset($view)) {
    $output .= $view;
  }
  else {
    $output .= '<p>The Tripal Feature Map Module uses primarily views to provide an '
      . 'administrative interface. Currently one or more views needed for this '
      . 'administrative interface are disabled. <strong>Click each of the following links to '
      . 'enable the pertinent views</strong>:</p>';
    $output .= '<ul>';
      $output .= '<li>'.l('Feature Map Admin', 'admin/tripal/chado/tripal_featuremap/views/featuremaps/enable').'</li>';
    $output .= '</ul>';
  }

  return $output;
}

/**
 * Administrative settings form
 *
 * @ingroup tripal_featuremap
 */
function tripal_featuremap_admin() {
  $form = array();

  // before proceeding check to see if we have any
  // currently processing jobs. If so, we don't want
  // to give the opportunity to sync maps
  $active_jobs = FALSE;
  if (tripal_get_module_active_jobs('tripal_featuremap')) {
    $active_jobs = TRUE;
  }

  // add the field set for syncing maps
  if (!$active_jobs) {
    get_tripal_featuremap_admin_form_sync_set($form);
    get_tripal_featuremap_admin_form_cleanup_set($form);
// TODO: complete coding of indexing and taxonomy assignment to features.
//    get_tripal_featuremap_admin_form_reindex_set($form);
//    get_tripal_featuremap_admin_form_taxonomy_set($form);
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
 * @ingroup tripal_featuremap
 */
function get_tripal_featuremap_admin_form_cleanup_set(&$form) {
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
 *
 * @ingroup tripal_featuremap
 */
function get_tripal_featuremap_admin_form_taxonomy_set(&$form) {
  $form['taxonify'] = array(
    '#type' => 'fieldset',
    '#title' => t('Assign Drupal Taxonomy to Map Features')
  );

  // get the list of maps
  $sql = "SELECT * FROM {featuremap} ORDER BY name";
  $lib_rset = chado_query($sql);

  // iterate through all of the maps
  $lib_boxes = array();
  while ($featuremap = $lib_rset->fetchObject()) {
    $lib_boxes[$featuremap->featuremap_id] = "$featuremap->name";
  }

  $form['taxonify']['description'] = array(
     '#type' => 'item',
     '#value' => t("Drupal allows for assignment of \"taxonomy\" or catagorical terms to " .
                   "nodes. These terms allow for advanced filtering during searching. This option allows ".
                   "for setting taxonomy only for features that belong to the selected maps below.  All other features will be unaffected.  To set taxonomy for all features in the site see the Feature Administration page."),
   '#weight' => 1,
  );

  $form['taxonify']['tx-maps'] = array(
   '#title'       => t('Maps'),
   '#type'        => t('checkboxes'),
   '#description' => t("Check the maps whose features you want to reset taxonomy.  Note: this list contains all maps, even those that may not be synced."),
   '#required'    => FALSE,
   '#prefix'      => '<div id="lib_boxes">',
   '#suffix'      => '</div>',
   '#options'     => $lib_boxes,
   '#weight'      => 2
  );
  $form['taxonify']['tx-button'] = array(
    '#type' => 'submit',
    '#value' => t('Set Feature Taxonomy'),
    '#weight'      => 3
  );
}
/**
 *
 * @ingroup tripal_featuremap
 */
function get_tripal_featuremap_admin_form_reindex_set(&$form) {
   // define the fieldsets
  $form['reindex'] = array(
    '#type' => 'fieldset',
    '#title' => t('Reindex Map Features')
  );

  // get the list of maps
  $sql = "SELECT * FROM {featuremap} ORDER BY name";
  $lib_rset = chado_query($sql);

  // iterate through all of the maps
  $lib_boxes = array();
  while ($featuremap = $lib_rset->fetchObject()) {
    $lib_boxes[$featuremap->featuremap_id] = "$featuremap->name";
  }
  $form['reindex']['description'] = array(
     '#type' => 'item',
     '#value' => t("This option allows for reindexing of only those features that belong to the selected maps below. All other features will be unaffected.  To reindex all features in the site see the Feature Administration page."),
   '#weight' => 1,
  );

  $form['reindex']['re-maps'] = array(
   '#title'       => t('Maps'),
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
 * @ingroup tripal_featuremap
 */
function get_tripal_featuremap_admin_form_sync_set(&$form) {
   // define the fieldsets
  $form['sync'] = array(
    '#type' => 'fieldset',
    '#title' => t('Sync Maps')
  );


  // get the list of maps
  $sql = "SELECT * FROM {featuremap} ORDER BY name";
  $lib_rset = chado_query($sql);

  // if we've added any maps to the list that can be synced
  // then we want to build the form components to allow the user
  // to select one or all of them.  Otherwise, just present
  // a message stating that all maps are currently synced.
  $lib_boxes = array();
  $added = 0;
  while ($featuremap = $lib_rset->fetchObject()) {
    // check to see if the map is already present as a node in drupal.
    // if so, then skip it.
    $sql = "SELECT * FROM {chado_featuremap} WHERE featuremap_id = :featuremap_id";
    if (!db_query($sql, array(':featuremap_id' => $featuremap->featuremap_id))->fetchObject()) {
      $lib_boxes[$featuremap->featuremap_id] = "$featuremap->name";
      $added++;
    }
  }

  // if we have maps we need to add to the checkbox then
  // build that form element
  if ($added > 0) {
    $lib_boxes['all'] = "All Maps";

    $form['reindex']['description'] = array(
     '#type' => 'item',
     '#value' => t("This option allows for the creation of Drupal content for maps in chado. Only the selected maps will be synced."),
   '#weight' => 1,
    );


    $form['sync']['featuremaps'] = array(
      '#title'       => t('Available Maps'),
      '#type'        => t('checkboxes'),
      '#description' => t("Check the maps you want to sync.  Drupal content will be created for each of the maps listed above.  Select 'All Maps' to sync all of them."),
      '#required'    => FALSE,
      '#prefix'      => '<div id="lib_boxes">',
      '#suffix'      => '</div>',
      '#options'     => $lib_boxes,
    '#weight' => 2,
    );
    $form['sync']['button'] = array(
       '#type' => 'submit',
       '#value' => t('Sync Maps'),
     '#weight' => 3,
    );
  }
   // we don't have any maps to select from
  else {
    $form['sync']['value'] = array(
       '#value' => t('All maps in Chado are currently synced with Drupal.')
    );
  }
}
/**
 *
 * @ingroup tripal_featuremap
 */
function tripal_featuremap_admin_validate($form, &$form_state) {
  global $user;  // we need access to the user info
  $job_args = array();

  // Submit the Sync Job if selected
  if ($form_state['values']['op'] == t('Sync Maps')) {

    // check to see if the user wants to sync chado and drupal.  If
    // so then we need to register a job to do so with tripal
    $featuremaps = $form_state['values']['featuremaps'];
    $do_all = FALSE;
    $to_sync = array();

  foreach ($featuremaps as $featuremap_id) {
    if (preg_match("/^all$/i", $featuremap_id)) {
      $do_all = TRUE;
    }
    if ($featuremap_id and preg_match("/^\d+$/i", $featuremap_id)) {
      // get the map info
      $sql = "SELECT * FROM {featuremap} WHERE featuremap_id = :featuremap_id";
      $featuremap = chado_query($sql, array(':featuremap_id' => $featuremap_id))->fetchObject();
      $to_sync[$featuremap_id] = $featuremap->name;
    }
  }

  // submit the job to the tripal job manager
  if ($do_all) {
    tripal_add_job('Sync all maps', 'tripal_featuremap', 'tripal_featuremap_sync_featuremaps', $job_args, $user->uid);
  }
  else{
    foreach ($to_sync as $featuremap_id => $name) {
      $job_args[0] = $featuremap_id;
      tripal_add_job("Sync map: $name", 'tripal_featuremap', 'tripal_featuremap_sync_featuremaps', $job_args, $user->uid);
      }
    }
  }

  // -------------------------------------
  // Submit the Reindex Job if selected
  if ($form_state['values']['op'] == t('Reindex Features')) {
    $featuremaps = $form_state['values']['re-maps'];
    foreach ($featuremaps as $featuremap_id) {
      if ($featuremap_id and preg_match("/^\d+$/i", $featuremap_id)) {
        // get the map info
        $sql = "SELECT * FROM {featuremap} WHERE featuremap_id = :featuremap_id";
        $featuremap = chado_query($sql, array(':featuremap_id' => $featuremap_id))->fetchObject();
        $job_args[0] = $featuremap_id;
        tripal_add_job("Reindex features for map: $featuremap->name", 'tripal_featuremap',
         'tripal_featuremap_reindex_features', $job_args, $user->uid);
      }
    }
  }

  // -------------------------------------
  // Submit the Taxonomy Job if selected
  if ($form_state['values']['op'] == t('Set Feature Taxonomy')) {
    $featuremaps = $form_state['values']['tx-maps'];
    foreach ($featuremaps as $featuremap_id) {
      if ($featuremap_id and preg_match("/^\d+$/i", $featuremap_id)) {
        // get the map info
        $sql = "SELECT * FROM {featuremap} WHERE featuremap_id = :featuremap_id";
        $featuremap = chado_query($sql, array(':featuremap_id' => $featuremap_id))->fetchObject();
        $job_args[0] = $featuremap_id;
        tripal_add_job("Set taxonomy for features in map: $featuremap->name", 'tripal_featuremap',
         'tripal_featuremap_taxonify_features', $job_args, $user->uid);
      }
    }
  }
    // -------------------------------------
    // Submit the Cleanup Job if selected
    if ($form_state['values']['op'] == t('Clean up orphaned maps')) {
      tripal_add_job('Cleanup orphaned maps', 'tripal_featuremap',
         'tripal_featuremap_cleanup', $job_args, $user->uid);
    }
}

/**
 *
 *
 * @ingroup tripal_featuremap
 */
function tripal_featuremap_sync_featuremaps($featuremap_id = NULL, $job_id = NULL) {

  global $user;
  $page_content = '';

  // get the list of featuremaps and create new nodes
  if (!$featuremap_id) {
    $sql = "SELECT * FROM {featuremap} F";
    $results = chado_query($sql);
  }
  else {
    $sql = "SELECT * FROM {featuremap} F WHERE featuremap_id = :featuremap_id";
    $results = chado_query($sql, array(':featuremap_id' => $featuremap_id));
  }

  // We'll use the following SQL statement for checking if the map
  // already exists as a drupal node.
  $sql = "SELECT * FROM {chado_featuremap} ".
        "WHERE featuremap_id = :featuremap_id";

  while ($featuremap = $results->fetchObject()) {

    // check if this map already exists in the drupal database. if it
    // does then skip this map and go to the next one.
    if (!db_query($sql, array(':featuremap_id' => $featuremap->featuremap_id))->fetchObject()) {

    $new_node = new stdClass();
    $new_node->type = 'chado_featuremap';
    $new_node->uid = $user->uid;
    $new_node->title = "$featuremap->name";
    $new_node->featuremap_id = $featuremap->featuremap_id;

    node_validate($new_node);
    $errors = form_get_errors();
    if (!$errors) {
      $node = node_submit($new_node);
      node_save($node);
      if ($node->nid) {
        print "Added " . $featuremap->name . "\n";
      }
      else {
        print "ERROR: Unable to create " . $featuremap->name . "\n";
      }
    }
    else {
      print "ERROR: Unable to create " . $featuremap->name . "\n" . print_r($errors, TRUE) . "\n";
    }
    }
    else {
      print "Skipped " . $featuremap->name . "\n";
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
 * @ingroup tripal_featuremap
 */
function tripal_featuremap_cleanup($dummy = NULL, $job_id = NULL) {

  return tripal_core_clean_orphaned_nodes('featuremap', $job_id);

}
/**
 * Add the map as a taxonomy term for associating with map_features
 *
 * @ingroup tripal_featuremap
 */
function tripal_featuremap_add_taxonomy($node, $featuremap_id) {

}