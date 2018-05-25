<?php
/**
 * @file
 * Functions to aid in loading ontologies into the chado cv module
 */

/**
 * @defgroup tripal_obo_loader Ontology Loader
 * @ingroup tripal_cv
 * @{
 * Functions to aid in loading ontologies into the chado cv module
 * @}
 */

/**
 * Provides the form to load an already existing controlled
 *  Vocabulary into chado
 *
 * @param $form
 *   The form array
 * @param $form_state
 *   The form state array
 *
 * @return
 *   The form array with new additions
 *
 * @ingroup tripal_obo_loader
 */
function tripal_cv_obo_form($form, &$form_state) {

  // get a list of db from chado for user to choose
  $sql = "SELECT * FROM {tripal_cv_obo} ORDER BY name";
  $results = db_query($sql);

  $obos = array();
  $obos[] = 'Select a Vocabulary';
  foreach ($results as $obo) {
    $obos[$obo->obo_id] = $obo->name;
  }

  $obo_id = '';
  if (array_key_exists('values', $form_state)) {
    $obo_id = array_key_exists('obo_id', $form_state['values']) ? $form_state['values']['obo_id'] : '';
  }

  $form['instructions'] = array(
    '#type' => 'fieldset',
    '#title' => 'instructions',
    '#collapsible' => TRUE,
    '#collapsed' => FALSE,
  );
  $form['instructions']['info'] = array(
    '#type' => 'item',
    '#markup' => t('This page allows you to load vocabularies and ontologies
      that are in OBO format. Once loaded, the terms from these
      vocabularies can be used to categorize data in the database.
      You may use the form below to either reload a vocabulary that is already
      loaded (as when new updates to that vocabulary are available) or load a new
      vocabulary.'),
  );

  $form['obo_existing'] = array(
    '#type' => 'fieldset',
    '#title' => t('Use a Saved Ontology OBO Reference'),
    '#prefix' => '<span id="obo-existing-fieldset">',
    '#suffix' => '</span>'
  );

  $form['obo_existing']['existing_instructions']= array(
    '#type' => 'item',
    '#markup' => t('The vocabularies listed in the select box below have bene pre-populated
      upon installation of Tripal or have been previously loaded.  Select one to edit
      its settings or submit for loading.  You may reload any vocabulary that has
      already been loaded to retrieve any new updates.'),
  );

  $form['obo_existing']['obo_id'] = array(
    '#title' => t('Ontology OBO File Reference'),
    '#type' => 'select',
    '#options' => $obos,
    '#ajax' => array(
      'callback' => 'tripal_cv_obo_form_ajax_callback',
      'wrapper' => 'obo-existing-fieldset',
    ),
  );

  // If the user has selected an OBO ID then get the form elements for
  // updating.
  if ($obo_id) {
    $uobo_name = '';
    $uobo_url = '';
    $uobo_file = '';

    $vocab = db_select('tripal_cv_obo', 't')
      ->fields('t', array('name', 'path'))
      ->condition('obo_id', $obo_id)
      ->execute()
      ->fetchObject();
    $uobo_name = $vocab->name;
    if (preg_match('/^http/', $vocab->path)) {
      $uobo_url = $vocab->path;
    }
    else {
      $uobo_file = trim($vocab->path);
      $matches = array();
      if (preg_match('/\{(.*?)\}/', $uobo_file, $matches)) {
        $modpath = drupal_get_path('module', $matches[1]);
        $uobo_file = preg_replace('/\{.*?\}/', $modpath, $uobo_file);
      }
    }
    // We don't want the previous value to remain. We want the new default to
    // show up, so remove the input values
    unset($form_state['input']['uobo_name']);
    unset($form_state['input']['uobo_url']);
    unset($form_state['input']['uobo_file']);

    $form['obo_existing']['uobo_name']= array(
      '#type'          => 'textfield',
      '#title'         => t('Vocabulary Name'),
      '#description'   => t('Please provide a name for this vocabulary.  After upload, this name will appear in the drop down
                           list above for use again later.'),
      '#default_value'  => $uobo_name,
    );

    $form['obo_existing']['uobo_url']= array(
      '#type'          => 'textfield',
      '#title'         => t('Remote URL'),
      '#description'   => t('Please enter a URL for the online OBO file.  The file will be downloaded and parsed.
                           (e.g. http://www.obofoundry.org/ro/ro.obo'),
      '#default_value' => $uobo_url,
    );

    $form['obo_existing']['uobo_file']= array(
      '#type'          => 'textfield',
      '#title'         => t('Local File'),
      '#description'   => t('Please enter the file system path for an OBO
        definition file. If entering a path relative to
        the Drupal installation you may use a relative path that excludes the
        Drupal installation directory (e.g. sites/default/files/xyz.obo). Note
        that Drupal relative paths have no preceeding slash.
        Otherwise, please provide the full path on the filesystem.  The path
        must be accessible to the web server on which this Drupal instance is running.'),
      '#default_value' => $uobo_file,
    );
    $form['obo_existing']['update_obo_details']= array(
      '#type' => 'submit',
      '#value' => 'Update Ontology Details',
      '#name' => 'update_obo_details'
    );
    $form['obo_existing']['update_load_obo']= array(
      '#type' => 'submit',
      '#value' => 'Load Vocabulary',
      '#name' => 'update_load_obo'
    );
  }

  $form['obo_new'] = array(
    '#type' => 'fieldset',
    '#title' => t('Add a New Ontology OBO Reference'),
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
  );

  $form['obo_new']['path_instructions']= array(
    '#value' => t('Provide the name and path for the OBO file.  If the vocabulary OBO file
                   is stored local to the server provide a file name. If the vocabulry is stored remotely,
                   provide a URL.  Only provide a URL or a local file, not both.'),
  );

  $form['obo_new']['obo_name']= array(
    '#type'          => 'textfield',
    '#title'         => t('New Vocabulary Name'),
    '#description'   => t('Please provide a name for this vocabulary.  After upload, this name will appear in the drop down
                           list above for use again later.'),
  );

  $form['obo_new']['obo_url']= array(
    '#type'          => 'textfield',
    '#title'         => t('Remote URL'),
    '#description'   => t('Please enter a URL for the online OBO file.  The file will be downloaded and parsed.
                           (e.g. http://www.obofoundry.org/ro/ro.obo'),
  );

  $form['obo_new']['obo_file']= array(
    '#type'  => 'textfield',
    '#title'  => t('Local File'),
    '#description' => t('Please enter the file system path for an OBO
        definition file. If entering a path relative to
        the Drupal installation you may use a relative path that excludes the
        Drupal installation directory (e.g. sites/default/files/xyz.obo). Note
        that Drupal relative paths have no preceeding slash.
        Otherwise, please provide the full path on the filesystem.  The path
        must be accessible to the web server on which this Drupal instance is running.'),
  );

  $form['obo_new']['add_new_obo'] = array(
    '#type'  => 'submit',
    '#value' => t('Add this vocabulary'),
    '#name' => 'add_new_obo',
  );

  $form['#redirect'] = 'admin/tripal/tripal_cv/obo_loader';

  return $form;
}

/**
 *
 * @param $form
 * @param $form_state
 */
function tripal_cv_obo_form_validate($form, &$form_state) {
  $obo_id    = $form_state['values']['obo_id'];
  $obo_name  = trim($form_state['values']['obo_name']);
  $obo_url   = trim($form_state['values']['obo_url']);
  $obo_file  = trim($form_state['values']['obo_file']);
  $uobo_name  = array_key_exists('uobo_name', $form_state['values']) ? trim($form_state['values']['uobo_name']) : '';
  $uobo_url   = array_key_exists('uobo_url', $form_state['values']) ? trim($form_state['values']['uobo_url']) : '';
  $uobo_file  = array_key_exists('uobo_file', $form_state['values']) ? trim($form_state['values']['uobo_file']) : '';

  // Make sure if the name is changed it doesn't conflict with another OBO.
  if ($form_state['clicked_button']['#name'] == 'update_obo_details' or
      $form_state['clicked_button']['#name'] == 'update_load_obo') {
    // Get the current record
    $vocab = db_select('tripal_cv_obo', 't')
      ->fields('t', array('obo_id', 'name', 'path'))
      ->condition('name', $uobo_name)
      ->execute()
      ->fetchObject();
    if ($vocab and $vocab->obo_id != $obo_id) {
      form_set_error('uobo_name', 'The vocabulary name must be different from existing vocabularies');
    }
    // Make sure the file exists. First check if it is a relative path
    $dfile = $_SERVER['DOCUMENT_ROOT'] . base_path() . $uobo_file;
    if (!file_exists($dfile)) {
      if (!file_exists($uobo_file)) {
        form_set_error('uobo_file', 'The specified path does not exist or cannot be read.');
      }
    }
    if (!$uobo_url and !$uobo_file) {
      form_set_error('uobo_url', 'Please provide a URL or a path for the vocabulary.');
    }
    if ($uobo_url and $uobo_file) {
      form_set_error('uobo_url', 'Please provide only a URL or a path for the vocabulary, but not both.');
    }
  }
  if ($form_state['clicked_button']['#name'] == 'add_new_obo') {
    // Get the current record
    $vocab = db_select('tripal_cv_obo', 't')
      ->fields('t', array('obo_id', 'name', 'path'))
      ->condition('name', $obo_name)
      ->execute()
      ->fetchObject();
    if ($vocab) {
      form_set_error('obo_name', 'The vocabulary name must be different from existing vocabularies');
    }
    // Make sure the file exists. First check if it is a relative path
    $dfile = $_SERVER['DOCUMENT_ROOT'] . base_path() . $obo_file;
    if (!file_exists($dfile)) {
      if (!file_exists($obo_file)) {
        form_set_error('obo_file', 'The specified path does not exist or cannot be read.');
      }
    }
    if (!$obo_url and !$obo_file) {
      form_set_error('obo_url', 'Please provide a URL or a path for the vocabulary.');
    }
    if ($obo_url and $obo_file) {
      form_set_error('obo_url', 'Please provide only a URL or a path for the vocabulary, but not both.');
    }
  }
}

/**
 * The submit function for the load ontology form. It registers a
 *   tripal job to import the user specified ontology file
 *
 * @param $form
 *   The form array
 * @param $form_state
 *   The form state array
 *
 *
 * @ingroup tripal_obo_loader
 */
function tripal_cv_obo_form_submit($form, &$form_state) {

  $obo_id    = $form_state['values']['obo_id'];
  $obo_name  = trim($form_state['values']['obo_name']);
  $obo_url   = trim($form_state['values']['obo_url']);
  $obo_file  = trim($form_state['values']['obo_file']);
  $uobo_name  = array_key_exists('uobo_name', $form_state['values']) ? trim($form_state['values']['uobo_name']) : '';
  $uobo_url   = array_key_exists('uobo_url', $form_state['values']) ? trim($form_state['values']['uobo_url']) : '';
  $uobo_file  = array_key_exists('uobo_file', $form_state['values']) ? trim($form_state['values']['uobo_file']) : '';

  // If the user requested to alter the details then do that.
  if ($form_state['clicked_button']['#name'] == 'update_obo_details' or
      $form_state['clicked_button']['#name'] == 'update_load_obo') {
    $success = db_update('tripal_cv_obo')
      ->fields(array(
        'name' => $uobo_name,
        'path' => $uobo_url ? $uobo_url : $uobo_file,
      ))
      ->condition('obo_id', $obo_id)
      ->execute();
    if ($success) {
      drupal_set_message(t("The vocabulary %vocab has been updated.", array('%vocab' => $uobo_name)));
    }
    else {
      drupal_set_message(t("The vocabulary %vocab could not be updated.", array('%vocab' => $uobo_name)), 'error');
    }

  }
  // If the user requested to update and load then we've already handled the
  // update now we just need to load.
  if ($form_state['clicked_button']['#name'] == 'update_load_obo') {
    tripal_submit_obo_job(array('obo_id' => $obo_id));
  }
  if ($form_state['clicked_button']['#name'] == 'add_new_obo') {
    $success = db_insert('tripal_cv_obo')
    ->fields(array(
      'name' => $obo_name,
      'path' => $obo_url ? $obo_url : $obo_file,
    ))
    ->execute();
    if ($success) {
      drupal_set_message(t("The vocabulary %vocab has been added.", array('%vocab' => $obo_name)));
    }
    else {
      drupal_set_message(t("The vocabulary %vocab could not be added.", array('%vocab' => $obo_name)), 'error');
    }
  }
}

/**
 * A wrapper function for importing the user specified OBO file into Chado by
 * specifying the obo_id of the OBO. It requires that the file be in OBO v1.2
 * compatible format.  This function is typically executed via the Tripal jobs
 * management after a user submits a job via the Load Onotloies form.
 *
 * @param $obo_id
 *   An obo_id from the tripal_cv_obo file that specifies which OBO file to import
 * @param $job_id
 *   The job_id of the job from the Tripal jobs management system.
 *
 * @ingroup tripal_obo_loader
 */
function tripal_cv_load_obo_v1_2_id($obo_id, $jobid = NULL) {

  // Get the OBO reference.
  $sql = "SELECT * FROM {tripal_cv_obo} WHERE obo_id = :obo_id";
  $obo = db_query($sql, array(':obo_id' => $obo_id))->fetchObject();

  // Convert the module name to the real path if present
  if (preg_match("/\{(.*?)\}/", $obo->path, $matches)) {
    $module = $matches[1];
    $path = drupal_realpath(drupal_get_path('module', $module));
    $obo->path = preg_replace("/\{.*?\}/", $path, $obo->path);
  }

  // if the reference is for a remote URL then run the URL processing function
  if (preg_match("/^http:\/\//", $obo->path) or
      preg_match("/^https:\/\//", $obo->path) or
      preg_match("/^ftp:\/\//", $obo->path)) {
    tripal_cv_load_obo_v1_2_url($obo->name, $obo->path, $jobid, 0);
  }
  // if the reference is for a local file then run the file processing function
  else {
    // check to see if the file is located local to Drupal
    $dfile = $_SERVER['DOCUMENT_ROOT'] . base_path() . $obo->path;
    if (file_exists($dfile)) {
      tripal_cv_load_obo_v1_2_file($obo->name, $dfile , $jobid, 0);
    }
    // if not local to Drupal, the file must be someplace else, just use
    // the full path provided
    else {
      if (file_exists($obo->path)) {
        tripal_cv_load_obo_v1_2_file($obo->name, $obo->path, $jobid, 0);
      }
      else {
        print "ERROR: could not find OBO file: '$obo->path'\n";
      }
    }
  }
}

/**
 * A wrapper function for importing the user specified OBO file into Chado by
 * specifying the filename and path of the OBO. It requires that the file be in OBO v1.2
 * compatible format.  This function is typically executed via the Tripal jobs
 * management after a user submits a job via the Load Onotloies form.
 *
 * @param $obo_name
 *   The name of the OBO (typially the ontology or controlled vocabulary name)
 * @param $file
 *   The path on the file system where the ontology can be found
 * @param $job_id
 *   The job_id of the job from the Tripal jobs management system.
 * @param $is_new
 *   Set to TRUE if this is a new ontology that does not yet exist in the
 *   tripal_cv_obo table.  If TRUE the OBO will be added to the table.
 *
 * @ingroup tripal_obo_loader
 */
function tripal_cv_load_obo_v1_2_file($obo_name, $file, $jobid = NULL, $is_new = TRUE) {
  $newcvs = array();

  if ($is_new) {
    tripal_insert_obo($obo_name, $file);
  }

  $success = tripal_cv_load_obo_v1_2($file, $jobid, $newcvs);
  if ($success) {
    // update the cvtermpath table
    tripal_cv_load_update_cvtermpath($newcvs, $jobid);
    print "\nDone\n";
  }
}

/**
 * A wrapper function for importing the user specified OBO file into Chado by
 * specifying the remote URL of the OBO. It requires that the file be in OBO v1.2
 * compatible format.  This function is typically executed via the Tripal jobs
 * management after a user submits a job via the Load Onotloies form.
 *
 * @param $obo_name
 *   The name of the OBO (typially the ontology or controlled vocabulary name)
 * @param $url
 *   The remote URL of the OBO file.
 * @param $job_id
 *   The job_id of the job from the Tripal jobs management system.
 * @param $is_new
 *   Set to TRUE if this is a new ontology that does not yet exist in the
 *   tripal_cv_obo table.  If TRUE the OBO will be added to the table.
 *
 * @ingroup tripal_obo_loader
 */
function tripal_cv_load_obo_v1_2_url($obo_name, $url, $jobid = NULL, $is_new = TRUE) {

  $newcvs = array();

  // first download the OBO
  $temp = tempnam(sys_get_temp_dir(), 'obo_');
  print "Downloading URL $url, saving to $temp\n";
  $url_fh = fopen($url, "r");
  $obo_fh = fopen($temp, "w");
  if (!$url_fh) {
    tripal_cv_obo_quiterror("Unable to download the remote OBO file at $url. Could a firewall be blocking outgoing connections? " .
      " if you are unable to download the file you may manually downlod the OBO file and use the web interface to " .
      " specify the location of the file on your server.");

  }
  while (!feof($url_fh)) {
    fwrite($obo_fh, fread($url_fh, 255), 255);
  }
  fclose($url_fh);
  fclose($obo_fh);

  if ($is_new) {
    tripal_insert_obo($obo_name, $url);
  }

  // second, parse the OBO
  $success = tripal_cv_load_obo_v1_2($temp, $jobid, $newcvs);
  if ($success) {

    // update the cvtermpath table
    tripal_cv_load_update_cvtermpath($newcvs, $jobid);
    print "Done\n";
  }
  // now remove the temp file
  unlink($temp);
}

/**
 * A function for executing the cvtermpath function of Chado.  This function
 * populates the cvtermpath table of Chado for quick lookup of term
 * relationships
 *
 * @param $newcvs
 *   An associative array of controlled vocabularies to update.  The key must be
 *   the name of the vocabulary and the value the cv_id from the cv table of chado.
 * @param $jobid
 *   The job_id of the job from the Tripal jobs management system.
 *
 * @ingroup tripal_obo_loader
 */
function tripal_cv_load_update_cvtermpath($newcvs, $jobid) {

  print "\nUpdating cvtermpath table.  This may take a while...\n";
  foreach ($newcvs as $namespace => $cvid) {
    tripal_update_cvtermpath($cvid, $jobid);
  }
}

/**
 * Imports a given OBO file into Chado.  This function is usually called by
 * one of three wrapper functions:  tripal_cv_load_obo_v1_2_id,
 * tripal_cv_load_obo_v1_2_file or tirpal_cv_load_obo_v1_2_url. But, it can
 * be called directly if the full path to an OBO file is available on the
 * file system.
 *
 * @param $flie
 *   The full path to the OBO file on the file system
 * @param $jobid
 *   The job_id of the job from the Tripal jobs management system.
 * @param $newcvs
 *   An empty array passed by reference that upon return will contain the list
 *   of newly added vocabularies.  The key will contain the CV name and the
 *   value the new cv_id
 *
 *
 * @ingroup tripal_obo_loader
 */
function tripal_cv_load_obo_v1_2($file, $jobid = NULL, &$newcvs) {

  $transaction = db_transaction();
  print "\nNOTE: Loading of this OBO file is performed using a database transaction. \n" .
      "If the load fails or is terminated prematurely then the entire set of \n" .
      "insertions/updates is rolled back and will not be found in the database\n\n";
  try {
    $header = array();

    // make sure our temporary table exists
    $ret = array();

    // empty the temp table
    $sql = "DELETE FROM {tripal_obo_temp}";
    chado_query($sql);

    print "Step 1: Preloading File $file\n";

    // make sure we have an 'internal' and a '_global' database
    if (!tripal_insert_db(array('name' => 'internal'))) {
      tripal_cv_obo_quiterror("Cannot add 'internal' database");
    }
    if (!tripal_insert_db(array('name' => '_global'))) {
      tripal_cv_obo_quiterror("Cannot add '_global' database");
    }

    // parse the obo file
    $default_db = tripal_cv_obo_parse($file, $header, $jobid);

    // add the CV for this ontology to the database.  The v1.2 definition
    // specifies a 'default-namespace' to be used if a 'namespace' is not
    // present for each stanza.  Some ontologies have adopted the v1.4 method
    // in their v1.2 files and not including it.
    if (array_key_exists('default-namespace', $header)) {
      $defaultcv = tripal_insert_cv($header['default-namespace'][0], '');
      if (!$defaultcv) {
        tripal_cv_obo_quiterror('Cannot add namespace ' . $header['default-namespace'][0]);
      }
      $newcvs[$header['default-namespace'][0]] = $defaultcv->cv_id;
    }
    // if the 'default-namespace' is missing
    else {
      // Grab the first term accession from the file and get the short name for the cv
      $fh = fopen($file, 'r');
      while ($line = fgets($fh)) {
        // Grab the first item's id info to break apart.
        if (preg_match('/^\s*\[/', $line)) {
          $stanza = TRUE;
          continue;
        }
        if ($stanza === TRUE && (substr($line, 0, 3) === "id:")) {
          $parts = explode(':', $line);
          $short_name = strtolower($parts[1]);
          $short_name = preg_replace('/\s+/', '', $short_name);
          break;
        }
      }
      fclose($fh);
      // Check if the EBI ontology search has this ontology:
      try {
        $results = tripal_cv_obo_EBI_lookup($short_name, 'ontology');
        if (array_key_exists('default-namespace', $results['config']['annotations'])) {
          $results = $results['config']['annotations']['default-namespace'];
          if (is_array($results)) {
            $results = $results[0];
          }
        }
        elseif (array_key_exists('namespace', $results['config'])) {
          $results = $results['config']['namespace'];
        }
        else {
          $results = $short_name;
        }
        $defaultcv = tripal_insert_cv(strtoupper($results), '');
        $newcvs[$defaultcv->name] = $defaultcv->cv_id;
      }
      catch (Exception $e) {
        watchdog_exception('OBOImporter no such accession found in EBI Ontology Search', $e);
      }
      
      if (empty($defaultcv)) {
        throw new Exception("Could not find a namespace for this OBO file: $file");
      }
      tripal_report_error('T_obo_loader', TRIPAL_WARNING, "This OBO is missing " . 
        "the 'default-namespace' header. It is therefore, not possible to " .
        "determine which vocabulary a term belongs to if it does not have " . 
        "a 'namespace' key.  Instead, terms without a 'namespace' key " .
        "will be placed in the '!vocab' vocabulary.", 
        array('!vocab' => $defaultcv->name));
    }

    // add any typedefs to the vocabulary first
    print "\nStep 2: Loading type defs...\n";
    tripal_cv_obo_load_typedefs($defaultcv, $newcvs, $default_db, $jobid);

    // next add terms to the vocabulary
    print "\nStep 3: Loading terms...\n";
    if (!tripal_cv_obo_process_terms($defaultcv, $jobid, $newcvs, $default_db)) {
      tripal_cv_obo_quiterror('Cannot add terms from this ontology');
    }
  }
  catch (Exception $e) {
    $transaction->rollback();
    print "\n"; // make sure we start errors on new line
    print "FAILED. Rolling back database changes...\n";
    watchdog_exception('T_obo_loader', $e);
    return FALSE;
  }

  return TRUE;
}

/**
 * Immediately terminates loading of the OBO file.
 *
 * @param $message
 *   The error message to present to the user
 *
 * @ingroup tripal_obo_loader
 */
function tripal_cv_obo_quiterror($message) {

  tripal_report_error("T_obo_loader", TRIPAL_ERROR, $message, array());
  exit;

}

/**
 * OBO files are divided into a typedefs terms section and vocabulary terms section.
 * This function loads the typedef terms from the OBO.
 *
 * @param $defaultcv
 *   A database object containing a record from the cv table for the
 *   default controlled vocabulary
 * @param $newcvs
 *   An associative array of controlled vocabularies for this OBO.  The key must be
 *   the name of the vocabulary and the value the cv_id from the cv table of chado.
 * @param $default_db
 *   The name of the default database.
 * @param $jobid
 *   The job_id of the job from the Tripal jobs management system.
 *
 * @ingroup tripal_obo_loader
 */
function tripal_cv_obo_load_typedefs($defaultcv, $newcvs, $default_db, $jobid) {
  $sql = "SELECT * FROM {tripal_obo_temp} WHERE type = 'Typedef'";
  $typedefs = chado_query($sql);

  $sql = "
    SELECT count(*) as num_terms
    FROM {tripal_obo_temp}
    WHERE type = 'Typedef'
  ";
  $result = chado_query($sql)->fetchObject();
  $count = $result->num_terms;

  // calculate the interval for updates
  $interval = intval($count * 0.0001);
  if ($interval < 1) {
    $interval = 1;
  }
  $i = 0;
  foreach ($typedefs as $typedef) {
    $term = unserialize(base64_decode($typedef->stanza));

    // update the job status every interval
    if ($jobid and $i % $interval == 0) {
      $complete = ($i / $count) * 33.33333333;
      tripal_set_job_progress($jobid, intval($complete + 33.33333333));
      printf("%d of %d records. (%0.2f%%) Memory: %s bytes\r", $i, $count, $complete * 3, number_format(memory_get_usage()));
    }

    tripal_cv_obo_process_term($term, $defaultcv->name, 1, $newcvs, $default_db);

    $i++;
  }
    // set the final status
  if ($jobid) {
    if ($count > 0) {
      $complete = ($i / $count) * 33.33333333;
    }
    else {
      $complete = 33.33333333;
    }
    tripal_set_job_progress($jobid, intval($complete + 33.33333333));
    printf("%d of %d records. (%0.2f%%) Memory: %s bytes\r", $i, $count, $complete * 3, number_format(memory_get_usage()));
  }
  return 1;
}

/**
 * OBO files are divided into a typedefs section and a terms section.  This
 * function loads the typedef terms from the OBO.
 *
 * @param $defaultcv
 *   A database object containing a record from the cv table for the
 *   default controlled vocabulary
 * @param $jobid
 *  The job_id of the job from the Tripal jobs management system.
 * @param $newcvs
 *   An associative array of controlled vocabularies for this OBO.  The key must be
 *   the name of the vocabulary and the value the cv_id from the cv table of chado.
 * @param $default_db
 *   The name of the default database.
 * @ingroup tripal_obo_loader
 */
function tripal_cv_obo_process_terms($defaultcv, $jobid = NULL, &$newcvs, $default_db) {

  $i = 0;

  // iterate through each term from the OBO file and add it
  $sql = "SELECT * FROM {tripal_obo_temp} WHERE type = 'Term' ORDER BY id";
  $terms = chado_query($sql);

  $sql = "
    SELECT count(*) as num_terms
    FROM {tripal_obo_temp}
    WHERE type = 'Term'
  ";
  $result = chado_query($sql)->fetchObject();
  $count = $result->num_terms;

  // calculate the interval for updates
  $interval = intval($count * 0.0001);
  if ($interval < 1) {
    $interval = 1;
  }
  foreach ($terms as $t) {
    $term = unserialize(base64_decode($t->stanza));

    // update the job status every interval
    if ($jobid and $i % $interval == 0) {
      $complete = ($i / $count) * 33.33333333;
      tripal_set_job_progress($jobid, intval($complete + 66.666666));
      printf("%d of %d records. (%0.2f%%) Memory: %s bytes\r", $i, $count, $complete * 3, number_format(memory_get_usage()));
    }

    // add/update this term
    if (!tripal_cv_obo_process_term($term, $defaultcv->name, 0, $newcvs, $default_db)) {
      tripal_cv_obo_quiterror("Failed to process terms from the ontology");
    }

    $i++;
  }

  // set the final status
  if ($jobid) {
    if ($count > 0) {
      $complete = ($i / $count) * 33.33333333;
    }
    else {
      $complete = 33.33333333;
    }
    tripal_set_job_progress($jobid, intval($complete + 66.666666));
    printf("%d of %d records. (%0.2f%%) Memory: %s bytes\r", $i, $count, $complete * 3, number_format(memory_get_usage()));
  }

  return 1;
}

/**
 * Uses the provided term array to add/update information to Chado about the
 * term including the term, dbxref, synonyms, properties, and relationships.
 *
 * @param $term
 *   An array representing the cvterm.
 * @param $defaultcv
 *   The name of the default controlled vocabulary
 * @is_relationship
 *   Set to 1 if this term is a relationship term
 * @default_db
 *   The name of the default database.
 *
 * @ingroup tripal_obo_loader
 */
function tripal_cv_obo_process_term($term, $defaultcv, $is_relationship = 0, &$newcvs, $default_db) {

  // make sure we have a namespace for this term
  if (!array_key_exists('namespace', $term) and !($defaultcv or $defaultcv == '')) {
    tripal_cv_obo_quiterror("Cannot add the term: no namespace defined. " . $term['id'][0]);
  }

  // construct the term array for sending to the tripal_cv_add_cvterm function
  // for adding a new cvterm
  $t = array();
  $t['id'] = $term['id'][0];
  $t['name'] = $term['name'][0];
  if (array_key_exists('def', $term)) {
    $t['definition'] = $term['def'][0];
  }
  if (array_key_exists('subset', $term)) {
    $t['subset'] = $term['subset'][0];
  }
  if (array_key_exists('namespace', $term)) {
    $t['namespace'] = $term['namespace'][0];
  }
  if (array_key_exists('is_obsolete', $term)) {
    $t['is_obsolete'] = $term['is_obsolete'][0];
  }
  
  // Check that the default_cv is in the cv table.
  $sql =  "
    SELECT CV.name
    FROM {cv} CV
    WHERE CV.name = '$defaultcv'
  ";
  $results = chado_query($sql)->fetchObject();
  if (!$results){
    //The controlled vocabulary is not in the cv term table and needs to be added.
    $ontology_info = tripal_cv_obo_EBI_lookup($defaultcv, 'ontology');
    if (!empty($ontology_info)){
      if (array_key_exists('default-namespace', $ontology_info['config']['annotations'])) {
        $results = $ontology_info['config']['annotations']['default-namespace'];
      }
      elseif (array_key_exists('namespace', $ontology_info['config'])) {
        $results = $ontology_info['config']['namespace'];
      }
      $cv_returned = tripal_insert_cv($results[0], '');
      // If name && definition are both empty then look up the term from the ontology you just loaded.
      if($cv_returned) {
        $defaultcv = $cv_returned;
      }
    }
  }

  $t['cv_name'] = $defaultcv;
  $t['is_relationship'] = $is_relationship;
  $t['db_name'] = $default_db;

  // add the cvterm
  $cvterm = tripal_insert_cvterm($t, array('update_existing' => TRUE));
  if (!$cvterm) {
    tripal_cv_obo_quiterror("Cannot add the term " . $term['id'][0]);
  }
  
  // Remove any relationships that this term already has (in case it was
  // updated) and we'll re-add them.
  $sql = "
      DELETE FROM {cvterm_relationship} CVTR
      WHERE CVTR.subject_id = :cvterm_id
    ";
  chado_query($sql, array(':cvterm_id' => $cvterm->cvterm_id));

  if (array_key_exists('namespace', $term)) {
    $newcvs[$term['namespace'][0]] = $cvterm->cv_id;
  }

  // now handle other properites
  if (array_key_exists('is_anonymous', $term)) {
    //print "WARNING: unhandled tag: is_anonymous\n";
  }
  if (array_key_exists('alt_id', $term)) {
    foreach ($term['alt_id'] as $alt_id) {
      if (!tripal_cv_obo_add_cvterm_dbxref($cvterm, $alt_id)) {
        tripal_cv_obo_quiterror("Cannot add alternate id $alt_id");
      }
    }
  }

  if (array_key_exists('subset', $term)) {
    //print "WARNING: unhandled tag: subset\n";
  }
  // add synonyms for this cvterm
  if (array_key_exists('synonym', $term)) {
    if (!tripal_cv_obo_add_synonyms($term, $cvterm)) {
      tripal_cv_obo_quiterror("Cannot add synonyms");
    }
  }

  // reformat the deprecated 'exact_synonym, narrow_synonym, and broad_synonym'
  // types to be of the v1.2 standard
  if (array_key_exists('exact_synonym', $term) or array_key_exists('narrow_synonym', $term) or array_key_exists('broad_synonym', $term)) {
    if (array_key_exists('exact_synonym', $term)) {
      foreach ($term['exact_synonym'] as $synonym) {
        $new = preg_replace('/^\s*(\".+?\")(.*?)$/', '$1 EXACT $2', $synonym);
        $term['synonym'][] = $new;
      }
    }
    if (array_key_exists('narrow_synonym', $term)) {
      foreach ($term['narrow_synonym'] as $synonym) {
        $new = preg_replace('/^\s*(\".+?\")(.*?)$/', '$1 NARROW $2', $synonym);
        $term['synonym'][] = $new;
      }
    }
    if (array_key_exists('broad_synonym', $term)) {
      foreach ($term['broad_synonym'] as $synonym) {
        $new = preg_replace('/^\s*(\".+?\")(.*?)$/', '$1 BROAD $2', $synonym);
        $term['synonym'][] = $new;
      }
    }

    if (!tripal_cv_obo_add_synonyms($term, $cvterm)) {
      tripal_cv_obo_quiterror("Cannot add/update synonyms");
    }
  }

  // add the comment to the cvtermprop table
  if (array_key_exists('comment', $term)) {
    $comments = $term['comment'];
    $j = 0;
    foreach ($comments as $comment) {
      if (!tripal_cv_obo_add_cvterm_prop($cvterm, 'comment', $comment, $j)) {
        tripal_cv_obo_quiterror("Cannot add/update cvterm property");
      }
      $j++;
    }
  }

  // add any other external dbxrefs
  if (array_key_exists('xref', $term)) {
    foreach ($term['xref'] as $xref) {
      if (!tripal_cv_obo_add_cvterm_dbxref($cvterm, $xref)) {
        tripal_cv_obo_quiterror("Cannot add/update cvterm database reference (dbxref).");
      }
    }
  }

  if (array_key_exists('xref_analog', $term)) {
    foreach ($term['xref_analog'] as $xref) {
      if (!tripal_cv_obo_add_cvterm_dbxref($cvterm, $xref)) {
        tripal_cv_obo_quiterror("Cannot add/update cvterm database reference (dbxref).");
      }
    }
  }
  if (array_key_exists('xref_unk', $term)) {
    foreach ($term['xref_unk'] as $xref) {
      if (!tripal_cv_obo_add_cvterm_dbxref($cvterm, $xref)) {
        tripal_cv_obo_quiterror("Cannot add/update cvterm database reference (dbxref).");
      }
    }
  }

  // add is_a relationships for this cvterm
  if (array_key_exists('is_a', $term)) {
    foreach ($term['is_a'] as $is_a) {
      if (!tripal_cv_obo_add_relationship($cvterm, $defaultcv, 'is_a', $is_a, $is_relationship, $default_db)) {
        tripal_cv_obo_quiterror("Cannot add relationship is_a: $is_a");
      }
    }
  }

  if (array_key_exists('intersection_of', $term)) {
    //print "WARNING: unhandled tag: intersection_of\n";
  }
  if (array_key_exists('union_of', $term)) {
    //print "WARNING: unhandled tag: union_on\n";
  }
  if (array_key_exists('disjoint_from', $term)) {
    //print "WARNING: unhandled tag: disjoint_from\n";
  }
  if (array_key_exists('relationship', $term)) {
    foreach ($term['relationship'] as $value) {
      $rel = preg_replace('/^(.+?)\s.+?$/', '\1', $value);
      $object = preg_replace('/^.+?\s(.+?)$/', '\1', $value);
      if (!tripal_cv_obo_add_relationship($cvterm, $defaultcv, $rel, $object, $is_relationship, $default_db)) {
        tripal_cv_obo_quiterror("Cannot add relationship $rel: $object");
      }
    }
  }
  if (array_key_exists('replaced_by', $term)) {
   //print "WARNING: unhandled tag: replaced_by\n";
  }
  if (array_key_exists('consider', $term)) {
    //print "WARNING: unhandled tag: consider\n";
  }
  if (array_key_exists('use_term', $term)) {
    //print "WARNING: unhandled tag: user_term\n";
  }
  if (array_key_exists('builtin', $term)) {
    //print "WARNING: unhandled tag: builtin\n";
  }
  return 1;
}

/**
 * Adds a cvterm relationship
 *
 * @param $cvterm
 *   A database object for the cvterm
 * @param $rel
 *   The relationship name
 * @param $objname
 *   The relationship term name
 * @param $defaultcv
 *   A database object containing a record from the cv table for the
 *   default controlled vocabulary
 * @object_is_relationship
 *   Set to 1 if this term is a relationship term
 * @default_db
 *   The name of the default database.
 *
 * @ingroup tripal_obo_loader
 */
function tripal_cv_obo_add_relationship($cvterm, $defaultcv, $rel,
  $objname, $object_is_relationship = 0, $default_db = 'OBO_REL') {

  // If an accession was passed we need to see if we can find the actual label.
  if (strpos($rel, ':')) {
    $pair = explode(":", $rel);
    $ontology_id = $pair[0];
    $accession_num = $pair[1];
    if (is_numeric($accession_num)) {
      $results = tripal_cv_obo_EBI_lookup($rel, 'query');
      if (!empty($results)) {
        if (array_key_exists('docs', $results)){
          if(!empty($results['docs'])) {
            $rel = $results['docs']['label'];
          }
          else {
            // The first search doesn't work, so let's try a broader one.
            $results = tripal_cv_obo_EBI_lookup($rel, 'query-non-local');
            if (!empty($results)) {
              if (array_key_exists('docs', $results)){
                if(!empty($results['docs'])) {
                  $accession = $rel;
                  $accession_underscore = str_replace(":", "_", $accession);
                  foreach ($results['docs'] as $item) {
                    if ($item['label'] != $accession && $item['label'] != $accession_underscore) {
                      //Found the first place a label is other than the accession is used, so take
                      // that info and then end the loop.
                      $rel = $item['label'];
                      break;
                    }
                  }
                }
              }
            }
          }
        }
      }
    }
  } 
  
  // make sure the relationship cvterm exists
  $term = array(
    'name' => $rel,
    'id' => "$default_db:$rel",
    'definition' => '',
    'is_obsolete' => 0,
    'cv_name' => $defaultcv,
    'is_relationship' => TRUE,
    'db_name' => $default_db
  );
  $relcvterm = tripal_insert_cvterm($term, array('update_existing' => FALSE));

  if (!$relcvterm) {
    // If the relationship term couldn't be found in the default_db provided
    // then do on more check to find it in the relationship ontology
    $term = array(
      'name' => $rel,
      'id' => "OBO_REL:$rel",
      'definition' => '',
      'is_obsolete' => 0,
      'cv_name' => $defaultcv,
      'is_relationship' => TRUE,
      'db_name' => 'OBO_REL'
    );
    $relcvterm = chado_insert_cvterm($term, array('update_existing' => FALSE));
    if (!$relcvterm) {
      tripal_cv_obo_quiterror("Cannot find the relationship term in the current or relationship ontology: $rel\n");
    }    
  }

  // get the object term
  $oterm = tripal_cv_obo_get_term($objname);
  if (!$oterm) {
    tripal_cv_obo_quiterror("Could not find object term $objname\n");
  }

  $objterm = array();
  $objterm['id']            = $oterm['id'][0];
  $objterm['name']          = $oterm['name'][0];
  if (array_key_exists('def', $oterm)) {
    $objterm['definition']           = $oterm['def'][0];
  }
  if (array_key_exists('subset', $oterm)) {
    $objterm['subset']      = $oterm['subset'][0];
  }
  if (array_key_exists('namespace', $oterm)) {
    $objterm['namespace']   = $oterm['namespace'][0];
  }
  if (array_key_exists('is_obsolete', $oterm)) {
    $objterm['is_obsolete'] = $oterm['is_obsolete'][0];
  }

  $objterm['cv_name' ] = $defaultcv;
  $objterm['is_relationship'] = $object_is_relationship;
  $objterm['db_name'] = $default_db;

  $objcvterm = tripal_insert_cvterm($objterm, array('update_existing' => TRUE));
  if (!$objcvterm) {
    tripal_cv_obo_quiterror("Cannot add cvterm " . $oterm['name'][0]);
  }

  // check to see if the cvterm_relationship already exists, if not add it
  $values = array(
    'type_id'    => $relcvterm->cvterm_id,
    'subject_id' => $cvterm->cvterm_id,
    'object_id'  => $objcvterm->cvterm_id
  );
  $result = chado_select_record('cvterm_relationship', array('*'), $values);
  if (count($result) == 0) {
    $options = array('return_record' => FALSE);
    $success = chado_insert_record('cvterm_relationship', $values, $options);
    if (!$success) {
      tripal_cv_obo_quiterror("Cannot add term relationship: '$cvterm->name' $rel '$objcvterm->name'");
    }
  }

  return TRUE;
}

/**
 * Retreives the term array from the temp loading table for a given term id.
 *
 * @param id
 *   The id of the term to retrieve
 *
 * @ingroup tripal_obo_loader
 */
function tripal_cv_obo_get_term($id) {
  $values = array('id' => $id);
  $result = chado_select_record('tripal_obo_temp', array('stanza'), $values);
  if (count($result) == 0) {
    return FALSE;
  }
  return unserialize(base64_decode($result[0]->stanza));
}

/**
 * Adds the synonyms to a term
 *
 * @param term
 *   An array representing the cvterm. It must have a 'synonym' key/value pair.
 * @param cvterm
 *   The database object of the cvterm to which the synonym will be added.
 *
 * @ingroup tripal_obo_loader
 */
function tripal_cv_obo_add_synonyms($term, $cvterm) {

  // make sure we have a 'synonym_type' vocabulary
  $syncv = tripal_insert_cv('synonym_type', 'A vocabulary added by the Tripal CV module OBO loader for storing synonym types.');

  // now add the synonyms
  if (array_key_exists('synonym', $term)) {
    foreach ($term['synonym'] as $synonym) {

      // separate out the synonym definition and the synonym type
      $def = preg_replace('/^\s*"(.*)"\s*.*$/', '\1', $synonym);
      // the scope will be 'EXACT', etc...
      $scope = drupal_strtolower(preg_replace('/^.*"\s+(.*?)\s+.*$/', '\1', $synonym));
      if (!$scope) {  // if no scope then default to 'exact'
        $scope = 'exact';
      }

      // make sure the synonym type exists in the 'synonym_type' vocabulary
      $values = array(
        'name' => $scope,
        'cv_id' => array(
          'name' => 'synonym_type',
        ),
      );
      $syntype = tripal_get_cvterm($values);

      // if it doesn't exist then add it
      if (!$syntype) {
        // build a 'term' object so we can add the missing term
        $term = array(
           'name' => $scope,
           'id' => "internal:$scope",
           'definition' => '',
           'is_obsolete' => 0,
           'cv_name' => $syncv->name,
           'is_relationship' => FALSE
        );
        $syntype = tripal_insert_cvterm($term, array('update_existing' => TRUE));
        if (!$syntype) {
          tripal_cv_obo_quiterror("Cannot add synonym type: internal:$scope");
        }
      }

      // make sure the synonym doesn't already exists
      $values = array(
        'cvterm_id' => $cvterm->cvterm_id,
        'synonym' => $def
      );
      $results = chado_select_record('cvtermsynonym', array('*'), $values);
      if (count($results) == 0) {
        $values = array(
          'cvterm_id' => $cvterm->cvterm_id,
          'synonym' => $def,
          'type_id' => $syntype->cvterm_id
        );
        $options = array('return_record' => FALSE);
        $success = chado_insert_record('cvtermsynonym', $values, $options);
        if (!$success) {
          tripal_cv_obo_quiterror("Failed to insert the synonym for term: $cvtern->name ($def)");
        }
      }

      // now add the dbxrefs for the synonym if we have a comma in the middle
      // of a description then this will cause problems when splitting os lets
      // just change it so it won't mess up our splitting and then set it back
      // later.
      /**
      $synonym = preg_replace('/(".*?),\s(.*?")/','$1,_$2',$synonym);
      $dbxrefs = preg_split("/, /",preg_replace('/^.*\[(.*?)\]$/','\1',$synonym));
      foreach ($dbxrefs as $dbxref) {
       $dbxref = preg_replace('/,_/',", ",$dbxref);
        if ($dbxref) {
          tripal_cv_obo_add_cvterm_dbxref($syn,$dbxref);
        }
      }
      */
    }
  }

  return TRUE;
}

/**
 * Parse the OBO file and populate the templ loading table
 *
 * @param $file
 *   The path on the file system where the ontology can be found
 * @param $header
 *   An array passed by reference that will be populated with the header
 *   information from the OBO file
 * @param $jobid
 *   The job_id of the job from the Tripal jobs management system.
 *
 * @ingroup tripal_obo_loader
 */
function tripal_cv_obo_parse($obo_file, &$header, $jobid) {
  $in_header = 1;
  $stanza = array();
  $default_db = '';
  $line_num = 0;
  $num_read = 0;
  $intv_read = 0;

  $filesize = filesize($obo_file);
  $interval = intval($filesize * 0.01);
  if ($interval < 1) {
    $interval = 1;
  }

  // iterate through the lines in the OBO file and parse the stanzas
  $fh = fopen($obo_file, 'r');
  while ($line = fgets($fh)) {

    $line_num++;
    $size = drupal_strlen($line);
    $num_read += $size;
    $intv_read += $size;
    $line = trim($line);

    // update the job status every 1% features
    if ($jobid and $intv_read >= $interval) {
      $percent = sprintf("%.2f", ($num_read / $filesize) * 100);
      print "Parsing Line $line_num (" . $percent . "%). Memory: " . number_format(memory_get_usage()) . " bytes.\r";
      tripal_set_job_progress($jobid, intval(($num_read / $filesize) * 33.33333333));
      $intv_read = 0;
    }

    // remove newlines
    $line = rtrim($line);

    // remove any special characters that may be hiding
    $line = preg_replace('/[^(\x20-\x7F)]*/', '', $line);

    // skip empty lines
    if (strcmp($line, '') == 0) {
      continue;
    }

    // Remove comments from end of lines
    $line = preg_replace('/^(.*?)\!.*$/', '\1', $line);  

    // at the first stanza we're out of header
    if (preg_match('/^\s*\[/', $line)) {
      $in_header = 0;

      // store the stanza we just finished reading
      if (sizeof($stanza) > 0) {
        // add the term to the temp table
        $values = array(
          'id' => $stanza['id'][0],
          'stanza' => base64_encode(serialize($stanza)),
          'type' => $type,
        );
        $success = chado_insert_record('tripal_obo_temp', $values);
        if (!$success) {
          tripal_report_error('T_obo_loader', "ERROR: Cannot insert stanza into temporary table.", array(), 'error');
          exit;
        }

      }
      // get the stanza type:  Term, Typedef or Instance
      $type = preg_replace('/^\s*\[\s*(.+?)\s*\]\s*$/', '\1', $line);

      // start fresh with a new array
      $stanza = array();
      continue;
    }
    
    // break apart the line into the tag and value but ignore any escaped colons
    preg_replace("/\\:/", "|-|-|", $line); // temporarily replace escaped colons
    $pair = explode(":", $line, 2);
    $tag = $pair[0];
    $value = ltrim(rtrim($pair[1]));// remove surrounding spaces

    // if this is the ID then look for the default DB
    $matches = array();
    if ($tag == 'id' and preg_match('/^(.+?):.*$/', $value, $matches)) {
       $default_db = $matches[1];
    }

    $tag = preg_replace("/\|-\|-\|/", "\:", $tag); // return the escaped colon
    $value = preg_replace("/\|-\|-\|/", "\:", $value);
    if ($in_header) {
      if (!array_key_exists($tag, $header)) {
        $header[$tag] = array();
      }
      $header[$tag][] = $value;
    }
    else {
      if (!array_key_exists($tag, $stanza)) {
        $stanza[$tag] = array();
      }
      $stanza[$tag][] = $value;
    }
  }
  // now add the last term in the file
  if (sizeof($stanza) > 0) {
    $values = array(
      'id' => $stanza['id'][0],
      'stanza' => base64_encode(serialize($stanza)),
      'type' => $type,
    );
    chado_insert_record('tripal_obo_temp', $values);
    if (!$success) {
      tripal_report_error('T_obo_loader', "ERROR: Cannot insert stanza into temporary table.", array(), 'error');
      exit;
    }
    $percent = sprintf("%.2f", ($num_read / $filesize) * 100);
    print "Parsing Line $line_num (" . $percent . "%). Memory: " . number_format(memory_get_usage()) . " bytes.\r";
    tripal_set_job_progress($jobid, intval(($num_read / $filesize) * 33.33333333));
  }
  return $default_db;
}

/**
 * Adds a database reference to a cvterm
 *
 * @param cvterm
 *   The database object of the cvterm to which the synonym will be added.
 * @param xref
 *   The cross refernce.  It should be of the form from the OBO specification
 *
 * @ingroup tripal_obo_loader
 */
function tripal_cv_obo_add_cvterm_dbxref($cvterm, $xref) {

  $dbname = preg_replace('/^(.+?):.*$/', '$1', $xref);
  $accession = preg_replace('/^.+?:\s*(.*?)(\{.+$|\[.+$|\s.+$|\".+$|$)/', '$1', $xref);
  $description = preg_replace('/^.+?\"(.+?)\".*?$/', '$1', $xref);
  $dbxrefs = preg_replace('/^.+?\[(.+?)\].*?$/', '$1', $xref);

  if (!$accession) {
    tripal_cv_obo_quiterror();
    tripal_report_error("T_obo_loader", TRIPAL_WARNING, "Cannot add a dbxref without an accession: '$xref'", NULL);
    return FALSE;
  }

  // if the xref is a database link, handle that specially
  if (strcmp($dbname, 'http') == 0) {
    $accession = $xref;
    $dbname = 'URL';
  }

  // add the database
  $db = tripal_insert_db(array('name' => $dbname));
  if (!$db) {
    tripal_cv_obo_quiterror("Cannot find database '$dbname' in Chado.");
  }

  // now add the dbxref
  $dbxref = tripal_cv_obo_add_dbxref($db->db_id, $accession, '', $description);
  if (!$dbxref) {
    tripal_cv_obo_quiterror("Cannot find or add the database reference (dbxref)");
  }

  // finally add the cvterm_dbxref but first check to make sure it exists
  $values = array(
    'cvterm_id' => $cvterm->cvterm_id,
    'dbxref_id' => $dbxref->dbxref_id,
  );
  $result = chado_select_record('cvterm_dbxref', array('*'), $values);
  if (count($result) == 0) {
    $ins_options = array('return_record' => FALSE);
    $result = chado_insert_record('cvterm_dbxref', $values, $ins_options);
    if (!$result) {
      tripal_cv_obo_quiterror("Cannot add cvterm_dbxref: $xref");
      return FALSE;
    }
  }

  return TRUE;
}

/**
 * Adds a property to a cvterm
 *
 * @param cvterm
 *   A database object for the cvterm to which properties will be added
 * @param $property
 *   The name of the property to add
 * @param $value
 *   The value of the property
 * @param rank
 *   The rank of the property
 *
 * @ingroup tripal_obo_loader
 */
function tripal_cv_obo_add_cvterm_prop($cvterm, $property, $value, $rank) {

  // make sure the 'cvterm_property_type' CV exists
  $cv = tripal_insert_cv('cvterm_property_type', '');
  if (!$cv) {
    tripal_cv_obo_quiterror("Cannot add/find cvterm_property_type cvterm");
  }

  // get the property type cvterm.  If it doesn't exist then we want to add it
  $values = array(
    'name' => $property,
    'cv_id' => $cv->cv_id,
  );
  $results = chado_select_record('cvterm', array('*'), $values);
  if (count($results) == 0) {
    $term = array(
      'name' => $property,
      'id' => "internal:$property",
      'definition' => '',
      'is_obsolete' => 0,
      'cv_name' => $cv->name,
      'is_relationship' => FALSE,
    );
    $cvproptype = tripal_insert_cvterm($term, array('update_existing' => FALSE));
    if (!$cvproptype) {
      tripal_cv_obo_quiterror("Cannot add cvterm property: internal:$property");
      return FALSE;
    }
  }
  else {
    $cvproptype = $results[0];
  }

  // remove any properties that currently exist for this term.  We'll reset them
  if ($rank == 0) {
    $values = array('cvterm_id' => $cvterm->cvterm_id);
    $success = chado_delete_record('cvtermprop', $values);
    if (!$success) {
       tripal_cv_obo_quiterror("Could not remove existing properties to update property $property for term\n");
       return FALSE;
    }
  }

  // now add the property
  $values = array(
    'cvterm_id' => $cvterm->cvterm_id,
    'type_id' => $cvproptype->cvterm_id,
    'value' => $value,
    'rank' => $rank,
  );
  $options = array('return_record' => FALSE);
  $result = chado_insert_record('cvtermprop', $values, $options);
  if (!$result) {
    tripal_cv_obo_quiterror("Could not add property $property for term\n");
    return FALSE;
  }
  return TRUE;
}

/**
 * Adds a database cross reference to a cvterm
 *
 * @param db_id
 *   The database ID of the cross reference
 * @param accession
 *   The cross reference's accession
 * @param $version
 *   The version of the dbxref
 * @param $description
 *   The description of the cross reference
 *
 * @ingroup tripal_obo_loader
 */
function tripal_cv_obo_add_dbxref($db_id, $accession, $version='', $description='') {

  // check to see if the dbxref exists if not, add it
  $values = array(
    'db_id' => $db_id,
    'accession' => $accession,
  );
  $result = chado_select_record('dbxref', array('dbxref_id'), $values);
  if (count($result) == 0) {
    $ins_values = array(
      'db_id'       => $db_id,
      'accession'   => $accession,
      'version'     => $version,
      'description' => $description,
    );
    $ins_options = array('return_record' => FALSE);
    $result = chado_insert_record('dbxref', $ins_values, $ins_options);
    if (!$result) {
      tripal_cv_obo_quiterror("Failed to insert the dbxref record $accession");
      return FALSE;
    }
    $result = chado_select_record('dbxref', array('dbxref_id'), $values);
  }
  return $result[0];
}


function tripal_cv_obo_form_ajax_callback($form, $form_state) {
  return $form['obo_existing'];
}

/**
 * API call to Ontology Lookup Service provided by
 * https://www.ebi.ac.uk/ols/docs/api#resources-terms
 *
 * @param accession
 *   Accession term for query
 * @param type_of_search
 *   Either ontology, term, query, or query-non-local
 *
 * @ingroup tripal_obo_loader
 */
function tripal_cv_obo_EBI_lookup($accession, $type_of_search) {
  
  //Grab just the ontology from the $accession.
  $parts = explode(':', $accession);
  $ontology = strtolower($parts[0]);
  $ontology = preg_replace('/\s+/', '', $ontology);
  if ($type_of_search == 'ontology') {
    $options = array();
    $full_url = 'http://www.ebi.ac.uk/ols/api/ontologies/' . $ontology;
    $response = drupal_http_request($full_url, $options);
    if(!empty($response)){
      $response = drupal_json_decode($response->data);
    }
  }
  elseif ($type_of_search == 'term') {
    //The IRI of the terms, this value must be double URL encoded
    $iri = urlencode(urlencode("http://purl.obolibrary.org/obo/" . str_replace(':' , '_', $accession)));
    $options = array();
    $full_url = 'http://www.ebi.ac.uk/ols/api/ontologies/' . $ontology . '/' . 'terms/' . $iri;
    $response = drupal_http_request($full_url, $options);
    if(!empty($response)){
      $response = drupal_json_decode($response->data);
    }
  }
  elseif($type_of_search == 'query') {
    $options = array();
    $full_url = 'http://www.ebi.ac.uk/ols/api/search?q=' . $accession . '&queryFields=obo_id&local=true';
    $response = drupal_http_request($full_url, $options);
    if(!empty($response)){
      $response = drupal_json_decode($response->data);
    }
  }
  elseif($type_of_search == 'query-non-local') {
    $options = array();
    $full_url = 'http://www.ebi.ac.uk/ols/api/search?q=' . $accession . '&queryFields=obo_id';
    $response = drupal_http_request($full_url, $options);
    if(!empty($response)){
      $response = drupal_json_decode($response->data);
    }
  }
  return $response;
}