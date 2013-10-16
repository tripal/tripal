<?php

/**
 * Administrative settings for chado_orgnism
 *
 * @ingroup tripal_organism
 */
function tripal_organism_admin() {

  $form = array();

  // before proceeding check to see if we have any
  // currently processing jobs. If so, we don't want
  // to give the opportunity to sync libraries
  $active_jobs = FALSE;
  if (tripal_get_module_active_jobs('tripal_organism')) {
    $active_jobs = TRUE;
  }

  // add the field set for syncing libraries
  if (!$active_jobs) {
    get_tripal_organism_admin_form_sync_set($form);
    get_tripal_organism_admin_form_reindex_set($form);
    get_tripal_organism_admin_form_taxonomy_set($form);
    get_tripal_organism_admin_form_cleanup_set($form);
  }
  else {
    $form['notice'] = array(
    '#type' => 'fieldset',
    '#title' => t('Organism Management Temporarily Unavailable')
    );
    $form['notice']['message'] = array(
      '#value' => t('Currently, organism management jobs are waiting or are running. . Managemment features have been hidden until these jobs complete.  Please check back later once these jobs have finished.  You can view the status of pending jobs in the Tripal jobs page.'),
    );
  }

  return system_settings_form($form);
}
/**
 *
 *
 * @ingroup tripal_organism
 */
function get_tripal_organism_admin_form_cleanup_set(&$form) {
  $form['cleanup'] = array(
    '#type' => 'fieldset',
    '#title' => t('Clean Up')
  );
  $form['cleanup']['description'] = array(
    '#type' => 'item',
    '#value' => t("With Drupal and chado residing in different databases ".
    "it is possible that nodes in Drupal and organisms in Chado become ".
    "\"orphaned\".  This can occur if an organism node in Drupal is ".
    "deleted but the corresponding chado organism is not and/or vice ".
    "versa. Click the button below to resolve these discrepancies."),
    '#weight' => 1,
  );
  $form['cleanup']['button'] = array(
    '#type' => 'submit',
    '#value' => t('Clean up orphaned organisms'),
    '#weight' => 2,
  );
}
/**
 *
 *
 * @ingroup tripal_organism
 */
function get_tripal_organism_admin_form_taxonomy_set(&$form) {
  $form['taxonify'] = array(
    '#type' => 'fieldset',
    '#title' => t('Assign Drupal Taxonomy to Organism Features')
  );

  // get the list of libraries
  $sql = "SELECT * FROM {Organism} ORDER BY genus,species";
  $org_rset = chado_query($sql);

  // iterate through all of the libraries
  $org_boxes = array();
  while ($organism = db_fetch_object($org_rset)) {
    $org_boxes[$organism->organism_id] = "$organism->genus $organism->species";
  }

  $form['taxonify']['description'] = array(
    '#type' => 'item',
    '#value' => t("Drupal allows for assignment of \"taxonomy\" or catagorical terms to " .
                 "nodes. These terms allow for advanced filtering during searching. This option allows ".
                 "for setting taxonomy only for features that belong to the selected organisms below.  All other features will be unaffected.  To set taxonomy for all features in the site see the Feature Administration page."),
    '#weight' => 1,
  );

  $form['taxonify']['tx-organisms'] = array(
    '#title'       => t('Organisms'),
    '#type'        => t('checkboxes'),
    '#description' => t("Check the organisms whose features you want to reset taxonomy.  Note: this list contains all organisms, even those that may not be synced."),
    '#required'    => FALSE,
    '#prefix'      => '<div id="lib_boxes">',
    '#suffix'      => '</div>',
    '#options'     => $org_boxes,
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
 * @ingroup tripal_organism
 */
function get_tripal_organism_admin_form_reindex_set(&$form) {
  // define the fieldsets
  $form['reindex'] = array(
    '#type' => 'fieldset',
    '#title' => t('Reindex Organism Features')
  );

  // get the list of libraries
  $sql = "SELECT * FROM {Organism} ORDER BY genus,species";
  $org_rset = chado_query($sql);

  // iterate through all of the libraries
  $org_boxes = array();
  while ($organism = db_fetch_object($org_rset)) {
    $org_boxes[$organism->organism_id] = "$organism->genus $organism->species";
  }
  $form['reindex']['description'] = array(
    '#type' => 'item',
    '#value' => t("This option allows for reindexing of only those features that belong to the selected organisms below. All other features will be unaffected.  To reindex all features in the site see the Feature Administration page."),
    '#weight' => 1,
  );

  $form['reindex']['re-organisms'] = array(
    '#title'       => t('Organisms'),
    '#type'        => t('checkboxes'),
    '#description' => t("Check the organisms whoee features you want to reindex. Note: this list contains all organisms, even those that may not be synced."),
    '#required'    => FALSE,
    '#prefix'      => '<div id="lib_boxes">',
    '#suffix'      => '</div>',
    '#options'     => $org_boxes,
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
 * @ingroup tripal_organism
 */
function get_tripal_organism_admin_form_sync_set(&$form) {
  // define the fieldsets
  $form['sync'] = array(
    '#type' => 'fieldset',
    '#title' => t('Sync Organisms')
  );

  // before proceeding check to see if we have any
  // currently processing jobs. If so, we don't want
  // to give the opportunity to sync libraries
  $active_jobs = FALSE;
  if (tripal_get_module_active_jobs('tripal_organism')) {
    $active_jobs = TRUE;
  }

  if (!$active_jobs) {

    // get the list of organisms
    $sql = "SELECT * FROM {Organism} ORDER BY genus,species";
    $org_rset = chado_query($sql);

    // if we've added any organisms to the list that can be synced
    // then we want to build the form components to allow the user
    // to select one or all of them.  Otherwise, just present
    // a message stating that all organisms are currently synced.
    $org_boxes = array();
    $added = 0;
    while ($organism = db_fetch_object($org_rset)) {
      // check to see if the organism is already present as a node in drupal.
      // if so, then skip it.
      $sql = "SELECT * FROM {chado_organism} WHERE organism_id = %d";
      if (!db_fetch_object(db_query($sql, $organism->organism_id))) {
        $org_boxes[$organism->organism_id] = "$organism->genus $organism->species ($organism->common_name)";
        $added++;
      }
    }

    // if we have organisms we need to add to the checkbox then
    // build that form element
    if ($added > 0) {
      $org_boxes['all'] = "All Organisms";

      $form['sync']['organisms'] = array(
        '#title'       => t('Available Organisms'),
        '#type'        => t('checkboxes'),
        '#description' => t("Check the organisms you want to sync.  Drupal content will be created for each of the organisms listed above.  Select 'All Organisms' to sync all of them."),
        '#required'    => FALSE,
        '#prefix'      => '<div id="org_boxes">',
        '#suffix'      => '</div>',
        '#options'     => $org_boxes,
      );
      $form['sync']['button'] = array(
        '#type' => 'submit',
        '#value' => t('Submit Sync Job')
      );
    }
    // we don't have any organisms to select from
    else {
    $form['sync']['value'] = array(
        '#value' => t('All organisms in Chado are currently synced with Drupal.')
    );
    }
  }
  // we don't want to present a form since we have an active job running
  else {
    $form['sync']['value'] = array(
        '#value' => t('Currently, jobs exist related to chado organisms. Please check back later for organisms that can by synced once these jobs have finished.  You can view the status of pending jobs in the Tripal jobs page.')
    );
  }
}
/**
 *
 * @ingroup tripal_organism
 */
function tripal_organism_admin_validate($form, &$form_state) {
  global $user;  // we need access to the user info
  $job_args = array();

  if ($form_state['values']['op'] == t('Submit Sync Job')) {

    // check to see if the user wants to sync chado and drupal.  If
    // so then we need to register a job to do so with tripal
    $organisms = $form_state['values']['organisms'];
    $do_all = FALSE;
    $to_sync = array();

    foreach ($organisms as $organism_id) {
      if (preg_match("/^all$/i" , $organism_id)) {
        $do_all = TRUE;
      }
      if ($organism_id and preg_match("/^\d+$/i" , $organism_id)) {
        // get the list of organisms
        $sql = "SELECT * FROM {Organism} WHERE organism_id = %d";
        $organism = db_fetch_object(chado_query($sql, $organism_id));
        $to_sync[$organism_id] = "$organism->genus $organism->species";
      }
    }

    // submit the job the tripal job manager
    if ($do_all) {
      tripal_add_job('Sync all organisms' , 'tripal_organism',
      'tripal_organism_sync_organisms' , $job_args , $user->uid);
    }
    else{
      foreach ($to_sync as $organism_id => $name) {
        $job_args[0] = $organism_id;
        tripal_add_job("Sync organism: $name" , 'tripal_organism',
          'tripal_organism_sync_organisms' , $job_args , $user->uid);
      }
    }
  }

  // -------------------------------------
  // Submit the Reindex Job if selected
  if ($form_state['values']['op'] == t('Reindex Features')) {
    $organisms = $form_state['values']['re-organisms'];
    foreach ($organisms as $organism_id) {
      if ($organism_id and preg_match("/^\d+$/i" , $organism_id)) {
        // get the organism info
        $sql = "SELECT * FROM {organism} WHERE organism_id = %d";
        $organism = db_fetch_object(chado_query($sql , $organism_id));
        $job_args[0] = $organism_id;
        tripal_add_job("Reindex features for organism: $organism->genus ".
         "$organism->species", 'tripal_organism' ,
         'tripal_organism_reindex_features', $job_args, $user->uid);
      }
    }
  }

  // -------------------------------------
  // Submit the taxonomy Job if selected
  if ($form_state['values']['op'] == t('Set Feature Taxonomy')) {
    $organisms = $form_state['values']['tx-organisms'];
    foreach ($organisms as $organism_id) {
      if ($organism_id and preg_match("/^\d+$/i", $organism_id)) {
        // get the organism info
        $sql = "SELECT * FROM {organism} WHERE organism_id = %d";
        $organism = db_fetch_object(chado_query($sql , $organism_id));
        $job_args[0] = $organism_id;
        tripal_add_job("Set taxonomy for features in organism: ".
          "$organism->genus $organism->species" , 'tripal_organism',
          'tripal_organism_taxonify_features', $job_args, $user->uid);
      }
    }
  }

  // -------------------------------------
  // Submit the Cleanup Job if selected
  if ($form_state['values']['op'] == t('Clean up orphaned organisms')) {
    tripal_add_job('Cleanup orphaned organisms', 'tripal_organism',
      'tripal_organism_cleanup', $job_args, $user->uid);
  }
}

/**
 * Synchronize organisms from chado to drupal
 *
 * @ingroup tripal_organism
 */
function tripal_organism_sync_organisms($organism_id = NULL, $job_id = NULL) {
  global $user;
  $page_content = '';

  if (!$organism_id) {
    $sql = "SELECT * FROM {Organism} O";
    $results = chado_query($sql);
  }
  else {
    $sql = "SELECT * FROM {organism} L WHERE organism_id = %d";
    $results = chado_query($sql, $organism_id);
  }

  // We'll use the following SQL statement for checking if the organism
  // already exists as a drupal node.
  $sql = "SELECT * FROM {chado_organism} ".
         "WHERE organism_id = %d";

  while ($organism = db_fetch_object($results)) {

    // check if this organism already exists in the drupal database. if it
    // does then skip this organism and go to the next one.
    if (!db_fetch_object(db_query($sql, $organism->organism_id))) {

      $new_node = new stdClass();
      $new_node->type = 'chado_organism';
      $new_node->uid = $user->uid;
      $new_node->title = "$organism->genus $organism->species";
      $new_node->organism_id = $organism->organism_id;
      $new_node->genus = $organism->genus;
      $new_node->species = $organism->species;
      $new_node->description = '';
      node_validate($new_node);
      if (!form_get_errors()) {
        $node = node_submit($new_node);
        node_save($node);
        if ($node->nid) {
          print "Added $organism->common_name\n";
        }
      }
      else {
        print "Failed to insert organism $organism->common_name\n";
      }
    }
    else {
      print "Skipped $organism->common_name\n";
    }
  }
  return $page_content;
}
/**
 *
 * @ingroup tripal_organism
 */
function tripal_organism_reindex_features($organism_id = NULL, $job_id = NULL) {
  $i = 0;

  if (!$organism_id) {
    return;
  }

  $sql = "SELECT * ".
        "FROM {feature} ".
        "WHERE organism_id = $organism_id ".
        "ORDER BY feature_id";
  $results = chado_query($sql);

  // load into ids array
  $count = 0;
  $ids = array();
  while ($id = db_fetch_object($results)) {
    $ids[$count] = $id->feature_id;
    $count++;
  }

  $interval = intval($count * 0.01);
  foreach ($ids as $feature_id) {
    // update the job status every 1% features
    if ($job_id and $i % $interval == 0) {
      tripal_job_set_progress($job_id , intval(($i/$count)*100));
    }
    $i++;
  }
}
/**
 *
 * @ingroup tripal_organism
 */
function tripal_organism_taxonify_features($organism_id = NULL, $job_id = NULL) {
  $i = 0;

  if (!$organism_id) {
    return;
  }

  $sql = "SELECT * ".
        "FROM {feature} ".
        "WHERE organism_id = $organism_id ".
        "ORDER BY feature_id";
  $results = chado_query($sql);

  // load into ids array
  $count = 0;
  $ids = array();
  while ($id = db_fetch_object($results)) {
    $ids[$count] = $id->feature_id;
    $count++;
  }

  // make sure our vocabularies are set before proceeding
  tripal_feature_set_vocabulary();

  // use this SQL for getting the nodes
  $nsql =  "SELECT * FROM {chado_feature} CF ".
          "  INNER JOIN {node} N ON N.nid = CF.nid ".
          "WHERE feature_id = %d";

  // iterate through the features and set the taxonomy
  $interval = intval($count * 0.01);
  foreach ($ids as $feature_id) {
    // update the job status every 1% features
    if ($job_id and $i % $interval == 0) {
      tripal_job_set_progress($job_id, intval(($i/$count)*100));
    }
    $node = db_fetch_object(db_query($nsql, $feature_id));
    tripal_feature_set_taxonomy($node, $feature_id);
    $i++;
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
 * @ingroup tripal_organism
 */
function tripal_organism_cleanup($dummy = NULL, $job_id = NULL) {

  return tripal_core_clean_orphaned_nodes('organism', $job_id);
}