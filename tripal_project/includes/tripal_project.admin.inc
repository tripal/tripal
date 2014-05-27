<?php
/**
 * @file
 * @todo Add file header description
 */

function tripal_project_admin($form_state = NULL) {
  $form = array();

  // before proceeding check to see if we have any
  // currently processing jobs. If so, we don't want
  // to give the opportunity to sync libraries
  $active_jobs = FALSE;
  if (tripal_get_module_active_jobs('tripal_project')) {
    $active_jobs = TRUE;
  }

  // add the field set for syncing libraries
  if (!$active_jobs) {
    get_tripal_project_admin_form_sync_set($form);
    get_tripal_project_admin_form_cleanup_set($form);
//    get_tripal_project_admin_form_reindex_set($form);

  }
  else {
    $form['notice'] = array(
    '#type' => 'fieldset',
    '#title' => t('Project Management Temporarily Unavailable')
    );
    $form['notice']['message'] = array(
      '#value' => t('Currently, project management jobs are waiting or are running. . Managemment features have been hidden until these jobs complete.  Please check back later once these jobs have finished.  You can view the status of pending jobs in the Tripal jobs page.'),
    );
  }

  return system_settings_form($form);
}
/**
 *
 *
 * @ingroup tripal_project
 */
function get_tripal_project_admin_form_cleanup_set(&$form) {
  $form['cleanup'] = array(
    '#type' => 'fieldset',
    '#title' => t('Clean Up')
  );
  $form['cleanup']['description'] = array(
    '#type' => 'item',
    '#value' => t("With Drupal and chado residing in different databases ".
    "it is possible that nodes in Drupal and projects in Chado become ".
    "\"orphaned\".  This can occur if an project node in Drupal is ".
    "deleted but the corresponding chado project is not and/or vice ".
    "versa. Click the button below to resolve these discrepancies."),
    '#weight' => 1,
  );
  $form['cleanup']['button'] = array(
    '#type' => 'submit',
    '#value' => t('Clean up orphaned projects'),
    '#weight' => 2,
  );
}
/**
 *
 * @ingroup tripal_project
 */
function get_tripal_project_admin_form_sync_set(&$form) {
  // define the fieldsets
  $form['sync'] = array(
    '#type' => 'fieldset',
    '#title' => t('Sync Projects')
  );

  // before proceeding check to see if we have any
  // currently processing jobs. If so, we don't want
  // to give the opportunity to sync libraries
  $active_jobs = FALSE;
  if (tripal_get_module_active_jobs('tripal_project')) {
    $active_jobs = TRUE;
  }

  if (!$active_jobs) {

    // get the list of projects
    $sql = "SELECT * FROM {project} ORDER BY name";
    $org_rset = chado_query($sql);

    // if we've added any projects to the list that can be synced
    // then we want to build the form components to allow the user
    // to select one or all of them.  Otherwise, just present
    // a message stating that all projects are currently synced.
    $proj_boxes = array();
    $added = 0;
    while ($project = db_fetch_object($org_rset)) {
      // check to see if the project is already present as a node in drupal.
      // if so, then skip it.
      $sql = "SELECT * FROM {chado_project} WHERE project_id = %d";
      if (!db_fetch_object(db_query($sql, $project->project_id))) {
        $proj_boxes[$project->project_id] = $project->name;
        $added++;
      }
    }

    // if we have projects we need to add to the checkbox then
    // build that form element
    if ($added > 0) {
      $proj_boxes['all'] = "All Projects";

      $form['sync']['projects'] = array(
        '#title'       => t('Available Projects'),
        '#type'        => t('checkboxes'),
        '#description' => t("Check the projects you want to sync.  Drupal content will be created for each of the projects listed above.  Select 'All Projects' to sync all of them."),
        '#required'    => FALSE,
        '#prefix'      => '<div id="org_boxes">',
        '#suffix'      => '</div>',
        '#options'     => $proj_boxes,
      );
      $form['sync']['button'] = array(
        '#type' => 'submit',
        '#value' => t('Submit Sync Job')
      );
    }
    // we don't have any projects to select from
    else {
    $form['sync']['value'] = array(
        '#value' => t('All projects in Chado are currently synced with Drupal.')
    );
    }
  }
  // we don't want to present a form since we have an active job running
  else {
    $form['sync']['value'] = array(
        '#value' => t('Currently, jobs exist related to chado projects. Please check back later for projects that can by synced once these jobs have finished.  You can view the status of pending jobs in the Tripal jobs page.')
    );
  }
}
/**
 *
 * @ingroup tripal_project
 */
function tripal_project_admin_validate($form, &$form_state) {
  global $user;  // we need access to the user info
  $job_args = array();

  if ($form_state['values']['op'] == t('Submit Sync Job')) {

    // check to see if the user wants to sync chado and drupal.  If
    // so then we need to register a job to do so with tripal
    $projects = $form_state['values']['projects'];
    $do_all = FALSE;
    $to_sync = array();

    foreach ($projects as $project_id) {
      if (preg_match("/^all$/i" , $project_id)) {
        $do_all = TRUE;
      }
      if ($project_id and preg_match("/^\d+$/i" , $project_id)) {
        // get the list of projects
        $sql = "SELECT * FROM {Project} WHERE project_id = %d";
        $project = db_fetch_object(chado_query($sql, $project_id));
        $to_sync[$project_id] = "$project->genus $project->species";
      }
    }

    // submit the job the tripal job manager
    if ($do_all) {
      tripal_add_job('Sync all projects' , 'tripal_project',
      'tripal_project_sync_projects' , $job_args , $user->uid);
    }
    else{
      foreach ($to_sync as $project_id => $name) {
        $job_args[0] = $project_id;
        tripal_add_job("Sync project: $name" , 'tripal_project',
          'tripal_project_sync_projects' , $job_args , $user->uid);
      }
    }
  }

  // -------------------------------------
  // Submit the Reindex Job if selected
  if ($form_state['values']['op'] == t('Reindex Features')) {
    $projects = $form_state['values']['re-projects'];
    foreach ($projects as $project_id) {
      if ($project_id and preg_match("/^\d+$/i" , $project_id)) {
        // get the project info
        $sql = "SELECT * FROM {project} WHERE project_id = %d";
        $project = db_fetch_object(chado_query($sql , $project_id));
        $job_args[0] = $project_id;
        tripal_add_job("Reindex features for project: $project->genus ".
         "$project->species", 'tripal_project' ,
         'tripal_project_reindex_features', $job_args, $user->uid);
      }
    }
  }

  // -------------------------------------
  // Submit the taxonomy Job if selected
  if ($form_state['values']['op'] == t('Set Feature Taxonomy')) {
    $projects = $form_state['values']['tx-projects'];
    foreach ($projects as $project_id) {
      if ($project_id and preg_match("/^\d+$/i", $project_id)) {
        // get the project info
        $sql = "SELECT * FROM {project} WHERE project_id = %d";
        $project = db_fetch_object(chado_query($sql , $project_id));
        $job_args[0] = $project_id;
        tripal_add_job("Set taxonomy for features in project: ".
          "$project->genus $project->species" , 'tripal_project',
          'tripal_project_taxonify_features', $job_args, $user->uid);
      }
    }
  }

  // -------------------------------------
  // Submit the Cleanup Job if selected
  if ($form_state['values']['op'] == t('Clean up orphaned projects')) {
    tripal_add_job('Cleanup orphaned projects', 'tripal_project',
      'tripal_project_cleanup', $job_args, $user->uid);
  }
}
/**
 * Synchronize projects from chado to drupal
 *
 * @ingroup tripal_project
 */
function tripal_project_sync_projects($project_id = NULL, $job_id = NULL) {
  global $user;
  $page_content = '';

  if (!$project_id) {
    $sql = "SELECT * FROM {project} P";
    $results = chado_query($sql);
  }
  else {
    $sql = "SELECT * FROM {project} P WHERE project_id = %d";
    $results = chado_query($sql, $project_id);
  }

  // We'll use the following SQL statement for checking if the project
  // already exists as a drupal node.
  $sql = "SELECT * FROM {chado_project} ".
         "WHERE project_id = %d";

  while ($project = db_fetch_object($results)) {

    // check if this project already exists in the drupal database. if it
    // does then skip this project and go to the next one.
    if (!db_fetch_object(db_query($sql, $project->project_id))) {

      $new_node = new stdClass();
      $new_node->type = 'chado_project';
      $new_node->uid = $user->uid;
      $new_node->title = "$project->name";
      $new_node->project_id = $project->project_id;
      //$new_node->name = $project->name;
      $new_node->description = $project->description;
      node_validate($new_node);
      if (!form_get_errors()) {
        $node = node_submit($new_node);
        node_save($node);
        if ($node->nid) {
          print "Added $project->name\n";
        }
      }
      else {
        print "Failed to insert project $project->name\n";
      }
    }
    else {
      print "Skipped $project->name\n";
    }
  }
  return $page_content;
}
/*
 * 
 */
function tripal_project_sync_projects_form_submit($form, &$form_state) {
  global $user;

  //sync'ing is done by a tripal_job that is added here
  $job_id = tripal_add_job('Sync Projects', 'tripal_project',
  'tripal_project_sync_all_projects', array(), $user->uid);

}

/*
 * 
 */
function tripal_project_sync_all_projects() {

  //retrieve all projects in drupal
  $resource = db_query('SELECT project_id FROM {chado_project}');
  $drupal_projects = array();
  while ($r = db_fetch_object($resource)) {
    $drupal_projects[$r->project_id] = $r->project_id;
  }

  // retrieve all projects in chado
  $chado_projects = array();
  $resource = chado_query('SELECT project_id FROM {project}');
  while ($r = db_fetch_object($resource)) {
    // if not already in drupal add to list to be sync'd
    if (!isset($drupal_projects[$r->project_id])) {
      $chado_projects[$r->project_id] = $r->project_id;
    }
  }

  print 'Number of Projects to Sync: ' . sizeof($chado_projects) . "\n";

  foreach ($chado_projects as $project_id) {
    $project = tripal_core_chado_select('project', array('name', 'description'), array('project_id' => $project_id));

    // create node
    $new_node = new stdClass();
    $new_node->type = 'chado_project';
    $new_node->uid = $user->uid;
    $new_node->title = $project[0]->name;
    $new_node->project_id = $project_id;
    $new_node->description = $project[0]->description;
    node_validate($new_node);
    $errors = form_get_errors();
    if (!$errors) {
      $node = node_submit($new_node);
      node_save($node);
      if ($node->nid) {
        print "Added " . $project[0]->name . " (Node ID:" . $node->nid . ")\n";
      }
    }
    else {
      print "Failed to insert project: " . $project[0]->name . "\n";
      print "Errors: " . print_r($errors, TRUE) . "\n";
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
 * @ingroup tripal_project
 */
function tripal_project_cleanup($dummy = NULL, $job_id = NULL) {

  return tripal_core_clean_orphaned_nodes('project', $job_id);
}