<?php 

/**
 * 
 */
function tripal_organism_sync () {
  $form = array();
  
  get_tripal_organism_admin_form_sync_set($form);
  get_tripal_organism_admin_form_cleanup_set($form);
  
  return $form;
}

/**
 *
 *
 * @ingroup tripal_organism
 */
function get_tripal_organism_admin_form_cleanup_set(&$form) {
  $form['cleanup'] = array(
    '#type' => 'fieldset',
    '#title' => t('Clean Up'),
    '#description' => t("With Drupal and chado residing in different databases or database schemas " .
        "it is possible that nodes in Drupal and organisms in Chado become " .
        "\"orphaned\".  This can occur if an organism node in Drupal is " .
        "deleted but the corresponding chado organism is not and/or vice " .
        "versa. Click the button below to resolve these discrepancies."),
  );
  $form['cleanup']['button'] = array(
    '#type' => 'submit',
    '#value' => t('Clean up orphaned organisms'),
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

  // get the list of organisms
  $sql = "SELECT * FROM {Organism} ORDER BY genus,species";
  $org_rset = chado_query($sql);

  // if we've added any organisms to the list that can be synced
  // then we want to build the form components to allow the user
  // to select one or all of them.  Otherwise, just present
  // a message stating that all organisms are currently synced.
  $org_boxes = array();
  $added = 0;
  foreach ($org_rset as $organism) {
    // check to see if the organism is already present as a node in drupal.
    // if so, then skip it.
    $sql = "SELECT * FROM {chado_organism} WHERE organism_id = :organism_id";
    if (!db_query($sql, array(':organism_id' => $organism->organism_id))->fetchObject()) {
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
      '#markup' => t('All organisms in Chado are currently synced with Drupal.')
    );
  }
}

/**
 *
 * @ingroup tripal_organism
 */
function tripal_organism_sync_submit($form, &$form_state) {
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
        $sql = "SELECT * FROM {organism} WHERE organism_id = :organism_id";
        $organism = chado_query($sql, array(':organism_id' => $organism_id))->fetchObject();
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
    $sql = "SELECT * FROM {organism} WHERE organism_id = :organism_id";
    $results = chado_query($sql, array(':organism_id' => $organism_id));
  }

  // We'll use the following SQL statement for checking if the organism
  // already exists as a drupal node.
  $sql = "SELECT * FROM {chado_organism} " .
      "WHERE organism_id = :organism_id";

  foreach ($results as $organism) {

    // check if this organism already exists in the drupal database. if it
    // does then skip this organism and go to the next one.
    if (!db_query($sql, array(':organism_id' => $organism->organism_id))->fetchObject()) {

      $new_node = new stdClass();
      $new_node->type = 'chado_organism';
      $new_node->uid = $user->uid;
      $new_node->title = "$organism->genus $organism->species";
      $new_node->organism_id = $organism->organism_id;
      $new_node->genus = $organism->genus;
      $new_node->species = $organism->species;
      $new_node->description = '';
      $form = array(); // dummy variable
      $form_state = array(); // dummy variable
      node_validate($new_node, $form, $form_state);
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