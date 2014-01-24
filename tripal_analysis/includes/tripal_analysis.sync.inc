<?php 

/**
 * 
 */
function tripal_analysis_sync_form () {
  $form = array();
  
  // define the fieldsets
  $form['sync'] = array(
    '#type' => 'fieldset',
    '#title' => t('Sync Analyses')
  );

  // get the list of analyses
  $sql = "SELECT * FROM {analysis} ORDER BY name";
  $ana_rset = chado_query($sql);

  // if we've added any analyses to the list that can be synced
  // then we want to build the form components to allow the user
  // to select one or all of them.  Otherwise, just present
  // a message stating that all analyses are currently synced.
  $ana_boxes = array();
  $added = 0;
  while ($analysis = $ana_rset->fetchObject()) {
    // check to see if the analysis is already present as a node in drupal.
    // if so, then skip it.
    $sql = "SELECT * FROM {chado_analysis} WHERE analysis_id = :analysis_id";
    if (!db_query($sql, array(':analysis_id' => $analysis->analysis_id))->fetchObject()) {
      $ana_boxes[$analysis->analysis_id] = "$analysis->name";
      $added++;
    }
  }

  // if we have analyses we need to add to the checkbox then
  // build that form element
  if ($added > 0) {
    $ana_boxes['all'] = "All analyses";

    $form['sync']['analyses'] = array(
      '#title'       => t('Available analyses'),
      '#type'        => t('checkboxes'),
      '#description' => t("Check the analyses you want to sync.  Drupal " .
          "content will be created for each of the analyses listed above. " .
          "Select 'All analyses' to sync all of them."),
      '#required'    => FALSE,
      '#prefix'      => '<div id="ana_boxes">',
      '#suffix'      => '</div>',
      '#options'     => $ana_boxes,
    );
    $form['sync']['button'] = array(
      '#type' => 'submit',
      '#value' => t('Submit Sync Job')
    );
  }
  // we don't have any analyses to select from
  else {
    $form['sync']['value'] = array(
      '#markup' => t('All analyses in Chado are currently synced with Drupal.')
    );
  }
  
  $form['cleanup'] = array(
    '#type' => 'fieldset',
    '#title' => t('Clean Up')
  );
  $form['cleanup']['description'] = array(
    '#markup' => t("With Drupal and chado residing in different databases " .
        "it is possible that nodes in Drupal and analyses in Chado become " .
        "\"orphaned\".  This can occur if an analysis node in Drupal is " .
        "deleted but the corresponding chado analysis is not and/or vice " .
        "versa. Click the button below to resolve these discrepancies."),
    '#weight' => 1,
  );
  $form['cleanup']['button'] = array(
    '#type' => 'submit',
    '#value' => t('Clean up orphaned analyses'),
    '#weight' => 2,
  );
  
  return $form;
}

/**
 * Validate the administrative form
 *
 * @param $form
 *   The form API array of the form to be validated
 * @form_state
 *   The user submitted values
 *
 * @ingroup tripal_analysis
 */
function tripal_analysis_sync_form_submit($form, &$form_state) {
  global $user;  // we need access to the user info
  $job_args = array();

  if ($form_state['values']['op'] == t('Submit Sync Job')) {

    // check to see if the user wants to sync chado and drupal.  If
    // so then we need to register a job to do so with tripal
    $analyses = $form_state['values']['analyses'];
    $do_all = FALSE;
    $to_sync = array();

    foreach ($analyses as $analysis_id) {
      if (preg_match("/^all$/i", $analysis_id)) {
        $do_all = TRUE;
      }
      if ($analysis_id and preg_match("/^\d+$/i", $analysis_id)) {
        // get the list of analyses
        $sql = "SELECT * FROM {analysis} WHERE analysis_id = :analysis_id";
        $analysis = chado_query($sql, array(':analysis_id' => $analysis_id))->fetchObject();
        $to_sync[$analysis_id] = $analysis->name;
      }
    }

    // submit the job the tripal job manager
    if ($do_all) {
      tripal_add_job('Sync all analyses', 'tripal_analysis', 'tripal_analysis_sync_analyses', $job_args, $user->uid);
    }
    else{
      foreach ($to_sync as $analysis_id => $name) {
        $job_args[0] = $analysis_id;
        tripal_add_job("Sync analysis: $name", 'tripal_analysis', 'tripal_analysis_sync_analyses', $job_args, $user->uid);
      }
    }
  }  
  // -------------------------------------
  // Submit the Cleanup Job if selected
  if ($form_state['values']['op'] == t('Clean up orphaned analyses')) {
    tripal_add_job('Cleanup orphaned analyses', 'tripal_analysis',
    'tripal_analyses_cleanup', $job_args, $user->uid);
  }
}
/**
 * Synchronize analyses from chado to drupal
 *
 * @ingroup tripal_analysis
 */
function tripal_analysis_sync_analyses($analysis_id = NULL, $job_id = NULL) {
  global $user;
  $page_content = '';

  if (!$analysis_id) {
    $sql = "SELECT * FROM {analysis}";
    $results = chado_query($sql);
  }
  else {
    $sql = "SELECT * FROM {analysis} WHERE analysis_id = :analysis_id";
    $results = chado_query($sql, array(':analysis_id' => $analysis_id));
  }

  // We'll use the following SQL statement for checking if the analysis
  // already exists as a drupal node.
  $sql = "SELECT * FROM {chado_analysis} WHERE analysis_id = :analysis_id";

  foreach ($results as $analysis) {

    // check if this analysis already exists in the drupal database. if it
    // does then skip this analysis and go to the next one.
    if (!db_query($sql, array(':analysis_id' => $analysis->analysis_id))->fetchObject()) {

      $new_node = new stdClass();
      $new_node->type = 'chado_analysis';
      $new_node->uid = $user->uid;
      $new_node->analysis_id    = $analysis->analysis_id;
      $new_node->analysisname   = $analysis->name;
      $new_node->description    = $analysis->description;
      $new_node->program        = $analysis->program;
      $new_node->programversion = $analysis->programversion;
      $new_node->algorithm      = $analysis->algorithm;
      $new_node->sourcename     = $analysis->sourcename;
      $new_node->sourceversion  = $analysis->sourceversion;
      $new_node->sourceuri      = $analysis->sourceuri;
      $new_node->timeexecuted   = $analysis->timeexecuted;

      // If the analysis has a name, use it as the node title. If not,
      // construct the title using program, programversion, and sourcename
      if ($new_node->analysisname) {
        $new_node->title = $new_node->analysisname;
      }
      else {
        // Construct node title as "program (version)"
        $new_node->title = "$analysis->program ($analysis->programversion)";
      }
      
      $form = array(); // dummy variable
      $form_state = array(); // dummy variable
      node_validate($new_node, $form, $form_state);
      if (!form_get_errors()) {
        $node = node_submit($new_node);
        node_save($node);
        if ($node->nid) {
          print "Added $new_node->title\n";
        }
      }
      else {
        print "Failed to insert analysis $analysis->name\n";
        watchdog('tanalysis_sync', "Unable to create analysis node. ID: %analysis_id, Name: %name.",
         array('%analysis_id' => $analysis->analysis_id, '%name' => $analysis->name), WATCHDOG_WARNING);
      }
    }
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
 * @ingroup tripal_analysis
 */
function tripal_analyses_cleanup($dummy = NULL, $job_id = NULL) {

  return tripal_core_chado_node_cleanup_orphaned('analysis', $job_id);

}
