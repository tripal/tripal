<?php 


/**
 * Administrative settings form
 *
 * @ingroup tripal_library
 */
function tripal_library_admin() {
  $form = array();

  // before proceeding check to see if we have any
  // currently processing jobs. If so, we don't want
  // to give the opportunity to sync libraries
  $active_jobs = FALSE;
  if (tripal_get_module_active_jobs('tripal_library')) {
    $active_jobs = TRUE;
  }

  // add the field set for syncing libraries
  if (!$active_jobs) {
    get_tripal_library_admin_form_sync_set($form);
    get_tripal_library_admin_form_reindex_set($form);
    get_tripal_library_admin_form_taxonomy_set($form);
    get_tripal_library_admin_form_cleanup_set($form);
  }
  else {
    $form['notice'] = array(
     '#type' => 'fieldset',
     '#title' => t('Library Management Temporarily Unavailable')
    );
    $form['notice']['message'] = array(
        '#value' => t('Currently, library management jobs are waiting or are running. . Managemment features have been hidden until these jobs complete.  Please check back later once these jobs have finished.  You can view the status of pending jobs in the Tripal jobs page.'),
    );
  }

  return system_settings_form($form);
}


/**
 *
 *
 * @ingroup tripal_library
 */
function get_tripal_library_admin_form_cleanup_set(&$form) {
  $form['cleanup'] = array(
    '#type' => 'fieldset',
    '#title' => t('Clean Up')
  );
  $form['cleanup']['description'] = array(
     '#type' => 'item',
     '#value' => t("With Drupal and chado residing in different databases ".
        "it is possible that nodes in Drupal and libraries in Chado become ".
        "\"orphaned\".  This can occur if an library node in Drupal is ".
        "deleted but the corresponding chado library is not and/or vice ".
        "versa. Click the button below to resolve these discrepancies."),
     '#weight' => 1,
  );
  $form['cleanup']['button'] = array(
    '#type' => 'submit',
    '#value' => t('Clean up orphaned libraries'),
    '#weight' => 2,
  );
}

/**
 *
 *
 * @ingroup tripal_library
 */
function get_tripal_library_admin_form_taxonomy_set(&$form) {
  $form['taxonify'] = array(
    '#type' => 'fieldset',
    '#title' => t('Assign Drupal Taxonomy to Library Features')
  );

  // get the list of libraries
  $sql = "SELECT * FROM {Library} ORDER BY uniquename";
  $lib_rset = chado_query($sql);

  // iterate through all of the libraries
  $lib_boxes = array();
  while ($library = db_fetch_object($lib_rset)) {
    $lib_boxes[$library->library_id] = "$library->name";
  }

  $form['taxonify']['description'] = array(
     '#type' => 'item',
     '#value' => t("Drupal allows for assignment of \"taxonomy\" or catagorical terms to " .
                   "nodes. These terms allow for advanced filtering during searching. This option allows ".
                   "for setting taxonomy only for features that belong to the selected libraries below.  All other features will be unaffected.  To set taxonomy for all features in the site see the Feature Administration page."),
   '#weight' => 1,
  );

  $form['taxonify']['tx-libraries'] = array(
   '#title'       => t('Libraries'),
   '#type'        => t('checkboxes'),
   '#description' => t("Check the libraries whose features you want to reset taxonomy.  Note: this list contains all libraries, even those that may not be synced."),
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
 * @ingroup tripal_library
 */
function get_tripal_library_admin_form_reindex_set(&$form) {
   // define the fieldsets
  $form['reindex'] = array(
    '#type' => 'fieldset',
    '#title' => t('Reindex Library Features')
  );

  // get the list of libraries
  $sql = "SELECT * FROM {Library} ORDER BY uniquename";
  $lib_rset = chado_query($sql);

  // iterate through all of the libraries
  $lib_boxes = array();
  while ($library = db_fetch_object($lib_rset)) {
    $lib_boxes[$library->library_id] = "$library->name";
  }
  $form['reindex']['description'] = array(
     '#type' => 'item',
     '#value' => t("This option allows for reindexing of only those features that belong to the selected libraries below. All other features will be unaffected.  To reindex all features in the site see the Feature Administration page."),
   '#weight' => 1,
  );

  $form['reindex']['re-libraries'] = array(
   '#title'       => t('Libraries'),
   '#type'        => t('checkboxes'),
   '#description' => t("Check the libraries whoee features you want to reindex. Note: this list contains all libraries, even those that may not be synced."),
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
 * @ingroup tripal_library
 */
function get_tripal_library_admin_form_sync_set(&$form) {
   // define the fieldsets
  $form['sync'] = array(
    '#type' => 'fieldset',
    '#title' => t('Sync Libraries')
  );


  // get the list of libraries
  $sql = "SELECT * FROM {Library} ORDER BY uniquename";
  $lib_rset = chado_query($sql);

  // if we've added any libraries to the list that can be synced
  // then we want to build the form components to allow the user
  // to select one or all of them.  Otherwise, just present
  // a message stating that all libraries are currently synced.
  $lib_boxes = array();
  $added = 0;
  while ($library = db_fetch_object($lib_rset)) {
    // check to see if the library is already present as a node in drupal.
    // if so, then skip it.
    $sql = "SELECT * FROM {chado_library} WHERE library_id = %d";
    if (!db_fetch_object(db_query($sql, $library->library_id))) {
      $lib_boxes[$library->library_id] = "$library->name";
      $added++;
    }
  }

  // if we have libraries we need to add to the checkbox then
  // build that form element
  if ($added > 0) {
    $lib_boxes['all'] = "All Libraries";

    $form['reindex']['description'] = array(
     '#type' => 'item',
     '#value' => t("This option allows for the creation of Drupal content for libraries in chado. Only the selected libraries will be synced."),
   '#weight' => 1,
    );


    $form['sync']['libraries'] = array(
      '#title'       => t('Available Libraries'),
      '#type'        => t('checkboxes'),
      '#description' => t("Check the libraries you want to sync.  Drupal content will be created for each of the libraries listed above.  Select 'All Libraries' to sync all of them."),
      '#required'    => FALSE,
      '#prefix'      => '<div id="lib_boxes">',
      '#suffix'      => '</div>',
      '#options'     => $lib_boxes,
    '#weight' => 2,
    );
    $form['sync']['button'] = array(
       '#type' => 'submit',
       '#value' => t('Sync Libraries'),
     '#weight' => 3,
    );
  }
   // we don't have any libraries to select from
  else {
    $form['sync']['value'] = array(
       '#value' => t('All libraries in Chado are currently synced with Drupal.')
    );
  }
}
/**
 *
 * @ingroup tripal_library
 */
function tripal_library_admin_validate($form, &$form_state) {
  global $user;  // we need access to the user info
  $job_args = array();

  // Submit the Sync Job if selected
  if ($form_state['values']['op'] == t('Sync Libraries')) {

    // check to see if the user wants to sync chado and drupal.  If
    // so then we need to register a job to do so with tripal
    $libraries = $form_state['values']['libraries'];
    $do_all = FALSE;
    $to_sync = array();

  foreach ($libraries as $library_id) {
    if (preg_match("/^all$/i", $library_id)) {
      $do_all = TRUE;
    }
    if ($library_id and preg_match("/^\d+$/i", $library_id)) {
      // get the library info
      $sql = "SELECT * FROM {Library} WHERE library_id = %d";
      $library = db_fetch_object(chado_query($sql, $library_id));
      $to_sync[$library_id] = $library->name;
    }
  }

  // submit the job to the tripal job manager
  if ($do_all) {
    tripal_add_job('Sync all libraries', 'tripal_library', 'tripal_library_sync_libraries', $job_args, $user->uid);
  }
  else{
    foreach ($to_sync as $library_id => $name) {
      $job_args[0] = $library_id;
      tripal_add_job("Sync library: $name", 'tripal_library', 'tripal_library_sync_libraries', $job_args, $user->uid);
      }
    }
  }

  // -------------------------------------
  // Submit the Reindex Job if selected
  if ($form_state['values']['op'] == t('Reindex Features')) {
    $libraries = $form_state['values']['re-libraries'];
    foreach ($libraries as $library_id) {
      if ($library_id and preg_match("/^\d+$/i", $library_id)) {
        // get the library info
        $sql = "SELECT * FROM {Library} WHERE library_id = %d";
        $library = db_fetch_object(chado_query($sql, $library_id));
        $job_args[0] = $library_id;
        tripal_add_job("Reindex features for library: $library->name", 'tripal_library',
         'tripal_library_reindex_features', $job_args, $user->uid);
      }
    }
  }

  // -------------------------------------
  // Submit the Taxonomy Job if selected
  if ($form_state['values']['op'] == t('Set Feature Taxonomy')) {
    $libraries = $form_state['values']['tx-libraries'];
    foreach ($libraries as $library_id) {
      if ($library_id and preg_match("/^\d+$/i", $library_id)) {
        // get the library info
        $sql = "SELECT * FROM {Library} WHERE library_id = %d";
        $library = db_fetch_object(chado_query($sql, $library_id));
        $job_args[0] = $library_id;
        tripal_add_job("Set taxonomy for features in library: $library->name", 'tripal_library',
         'tripal_library_taxonify_features', $job_args, $user->uid);
      }
    }
  }
    // -------------------------------------
    // Submit the Cleanup Job if selected
    if ($form_state['values']['op'] == t('Clean up orphaned libraries')) {
      tripal_add_job('Cleanup orphaned libraries', 'tripal_library',
         'tripal_library_cleanup', $job_args, $user->uid);
    }
}


/**
 * Add the library as a taxonomy term for associating with library_features
 *
 * @ingroup tripal_library
 */
function tripal_library_add_taxonomy($node, $library_id) {

    //include the file containing the required functions.  We only have to
    // do this because Drupal 6 fails to do this globally for us and
    // the drupal_execute function below won't work
    module_load_include('inc', 'taxonomy', 'taxonomy.admin');

   /*   // get the vocabulary id
    $vocabularies = taxonomy_get_vocabularies();
    $vid = NULL;
    foreach($vocabularies as $vocab){
    if($vocab->name == 'DNA Libraries'){
    $vid = $vocab->vid;
    }
    }

    if(!$vid){  */
   // add the vocabulary
  $vocab_form['values']['name'] = 'DNA Libraries';
  $vocab_form['values']['description'] = 'Allows for associating/searching of library features by library name';
  $vocab_form['values']['help'] = '';
  $vocab_form['values']['module'] = 'taxonomy';
  drupal_execute('taxonomy_form_vocabulary', $vocab_form);
  return;
   //   }

   // make sure this term doesn't already exist.  If it doesn't then add it
  if ($vid) {
    $tree = taxonomy_get_tree($vid);
    $found = 0;
    foreach ($tree as $term) {
      if ($term->name == $node->title) {
        $found = 1;
      }
    }

      // add the term to the vocabulary
    if (!$found) {
      $form_state = array();
      $form_state['values']['name'] = $node->title;
      $form_state['values']['description'] = $library_id;
      drupal_execute('taxonomy_form_term', $form_state, $vid);
    }
  }
}


/**
 *
 *
 * @ingroup tripal_library
 */
function tripal_library_sync_libraries($library_id = NULL, $job_id = NULL) {

  global $user;
  $page_content = '';

  // get the list of libraries and create new nodes
  if (!$library_id) {
    $sql = "SELECT * FROM {Library} L";
    $results = chado_query($sql);
  }
  else {
    $sql = "SELECT * FROM {Library} L WHERE library_id = %d";
    $results = chado_query($sql, $library_id);
  }

  // We'll use the following SQL statement for checking if the library
  // already exists as a drupal node.
  $sql = "SELECT * FROM {chado_library} ".
        "WHERE library_id = %d";

  while ($library = db_fetch_object($results)) {

    // check if this library already exists in the drupal database. if it
    // does then skip this library and go to the next one.
    if (!db_fetch_object(db_query($sql, $library->library_id))) {

    $new_node = new stdClass();
    $new_node->type = 'chado_library';
    $new_node->uid = $user->uid;
    $new_node->title = "$library->name";
    $new_node->library_id = $library->library_id;
    $new_node->organism_id = $library->organism_id;
    $new_node->uniquename = $library->uniquename;
    $new_node->type_id = $library->type_id;

    node_validate($new_node);
    $errors = form_get_errors();
    if (!$errors) {
      $node = node_submit($new_node);
      node_save($node);
      if ($node->nid) {
        print "Added " . $library->name . "\n";
      }
      else {
        print "ERROR: Unable to create " . $library->name . "\n";
      }
    }
    else {
      print "ERROR: Unable to create " . $library->name . "\n" . print_r($errors, TRUE) . "\n";
    }
    }
    else {
      print "Skipped " . $library->name . "\n";
    }
  }
  return $page_content;
}


/**
 *
 * @ingroup tripal_library
 */
function tripal_library_feature_set_taxonomy($library_id = NULL) {

   //TO DO : return usable error if vocabs don't exist
   // get the list of vocabularies and find our two vocabularies of interest
  $vocabularies = taxonomy_get_vocabularies();
  $vid = NULL;
  foreach ($vocabularies as $vocab) {
    if ($vocab->name == 'Library') {
      $vid = $vocab->vid;
    }
  }
  if (!$vid) {
    return;
  }

  // We'll use the following SQL statement for getting the node info
  if ($library_id) {
    print "Finding features for library with ID: $library_id\n";
    $sql = "SELECT LF.feature_id, L.library_id, L.name as libname ".
           "FROM {library_feature} LF ".
           "INNER JOIN Library L ON LF.library_id = L.library_id ".
           "WHERE L.library_id = $library_id ".
           "ORDER BY LF.feature_id";
    $features = chado_query($sql);
  }
  else {
    print "Finding features for all libraries\n";
    $sql = "SELECT LF.feature_id, L.library_id, L.name as libname ".
           "FROM {library_feature} LF ".
           "INNER JOIN Library L ON LF.library_id = L.library_id ".
           "ORDER BY LF.feature_id";
    $features = chado_query($sql);
  }

  $node_sql = "SELECT * FROM {chado_feature} CF ".
             "  INNER JOIN {node} N ON CF.nid = N.nid ".
             "WHERE feature_id = %d";

  // iterate through the features and add the taxonomy
  while ($feature = db_fetch_object($features)) {
    $node = db_fetch_object(db_query($node_sql, $feature->feature_id));
    $tags["$vid"] = $feature->libname;
    $terms['tags'] = $tags;
    taxonomy_node_save($node, $terms);
    print "Updated $feature->feature_id as $feature->libname\n";
  }
}
/**
 *
 * @ingroup tripal_library
 */
function tripal_library_reindex_features($library_id = NULL, $job_id = NULL) {
  $i = 0;

  // if the caller provided a library_id then get all of the features
  // associated with the library. Otherwise get all sequences assoicated
  // with all libraries.
  if ($library_id) {
    $sql = "SELECT LF.feature_id, L.library_id, L.name as libname ".
           " FROM {library_feature} LF ".
           "  INNER JOIN Library L ON LF.library_id = L.library_id ".
           "WHERE L.library_id = $library_id ".
           "ORDER BY LF.feature_id";
    $results = chado_query($sql);
  }
  else {
    $sql = "SELECT LF.feature_id, L.library_id, L.name as libname ".
           " FROM {library_feature} LF ".
           "  INNER JOIN Library L ON LF.library_id = L.library_id ".
           "ORDER BY LF.feature_id";
    $results = chado_query($sql);
  }

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
    if ($job_id and $i % interval == 0) {
      tripal_job_set_progress($job_id, intval(($i/$count)*100));
    }
    $i++;
  }
}
/**
 *
 * @ingroup tripal_library
 */
function tripal_library_taxonify_features($library_id = NULL, $job_id = NULL) {
  $i = 0;

  // if the caller provided a library_id then get all of the features
  // associated with the library. Otherwise get all sequences assoicated
  // with all libraries.
  if ($library_id) {
    $sql = "SELECT LF.feature_id, L.library_id, L.name as libname ".
           " FROM {library_feature} LF ".
           "  INNER JOIN Library L ON LF.library_id = L.library_id ".
           "WHERE L.library_id = $library_id ".
           "ORDER BY LF.feature_id";
    $results = chado_query($sql);
  }
  else {
    $sql = "SELECT LF.feature_id, L.library_id, L.name as libname ".
           " FROM {library_feature} LF ".
           "  INNER JOIN Library L ON LF.library_id = L.library_id ".
           "ORDER BY LF.feature_id";
    $results = chado_query($sql);
  }

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
    if ($job_id and $i % interval == 0) {
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
 * @ingroup tripal_library
 */
function tripal_library_cleanup($dummy = NULL, $job_id = NULL) {

  return tripal_core_clean_orphaned_nodes('library', $job_id);
  
}

