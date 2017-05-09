<?php


/**
 * Build the form for a TripalImporter implementation.
 */
function tripal_get_importer_form($form, &$form_state, $class) {

  tripal_load_include_importer_class($class);

  $form['importer_class'] = array(
    '#type' => 'value',
    '#value' => $class,
  );
  $form['file'] = array(
    '#type' => 'fieldset',
    '#title' => t($class::$upload_title),
    '#description' => t($class::$upload_description) . ' Either upload the file or if the file exists on the Tripal server please provide the full path.'
  );
  $form['file']['file_upload']= array(
    '#type' => 'html5_file',
    '#title' => '',
    '#description' => 'Remember to click the "Upload" button below to send ' .
      'your file to the server.  This interface is capable of uploading very ' .
      'large files.  If you are disconnected you can return, reload the file and it ' .
      'will resume where it left off.  Once the file is uploaded the "Upload '.
      'Progress" will indicate "Complete".  If the file is already present on the server ' .
      'then the status will quickly update to "Complete".',
    '#usage_type' => 'tripal_importer',
    '#usage_id' => 0,
  );

  $form['file']['file_local']= array(
    '#type'          => 'textfield',
    '#title'         => '',
    '#description'   => t('If the file is local to the Tripal server please provide the full path here.'),
  );

  if ($class::$use_analysis) {
    // get the list of analyses
    $sql = "SELECT * FROM {analysis} ORDER BY name";
    $org_rset = chado_query($sql);
    $analyses = array();
    $analyses[''] = '';
    while ($analysis = $org_rset->fetchObject()) {
      $analyses[$analysis->analysis_id] = "$analysis->name ($analysis->program $analysis->programversion, $analysis->sourcename)";
    }
    $form['analysis_id'] = array(
      '#title'       => t('Analysis'),
      '#type'        => t('select'),
      '#description' => t('Choose the analysis to which the uploaded data will be associated. ' .
        'Why specify an analysis for a data load?  All data comes from some place, even if ' .
        'downloaded from a website. By specifying analysis details for all data imports it ' .
        'provides provenance and helps end user to reproduce the data set if needed. At ' .
        'a minimum it indicates the source of the data.'),
      '#required'    => $class::$require_analysis,
      '#options'     => $analyses,
    );
  }


  $importer = new $class();
  $element = array();
  $form['class_elements'] = $importer->form($element, $form_state);

  $form['button'] = array(
    '#type' => 'submit',
    '#value' => t($class::$button_text),
    '#weight' => 10,
  );
  return $form;
}

/**
 * Validate function for the tripal_get_importer_form form().
 */
function tripal_get_importer_form_validate($form, &$form_state) {
  $class = $form_state['values']['importer_class'];

  // Get the form values for the file.
  $file_local = trim($form_state['values']['file_local']);
  $file_upload = trim($form_state['values']['file_upload']);

  // If the file is local make sure it exists on the local filesystem.
  if ($file_local) {
    // check to see if the file is located local to Drupal
    $file_local = trim($file_local);
    $dfile = $_SERVER['DOCUMENT_ROOT'] . base_path() . $file_local;
    if (!file_exists($dfile)) {
      // if not local to Drupal, the file must be someplace else, just use
      // the full path provided
      $dfile = $file_local;
    }
    if (!file_exists($dfile)) {
      form_set_error('file_local', t("Cannot find the file on the system. Check that the file exists or that the web server has permissions to read the file."));
    }
  }

  // The user must provide at least an uploaded file or a local file path.
  if (!$file_upload and !$file_local) {
    form_set_error('file_local', t("You must provide a file either by uploading one or specifying a path on the Tripal server.  Did you click the 'Upload' button?"));
  }

  // Now allow the loader to do validation of it's form additions.
  $importer = new $class();
  $importer->formValidate($form, $form_state);
}

/**
 * Submit function for the tripal_get_importer_form form().
 */
function tripal_get_importer_form_submit($form, &$form_state) {
  global $user;

  $arguments = $form_state['values'];

  // Remove the file_local and file_upload args. We'll add in a new
  // full file path and the fid instead.
  unset($arguments['file_local']);
  unset($arguments['file_upload']);
  unset($arguments['form_build_id']);
  unset($arguments['form_token']);
  unset($arguments['form_id']);
  unset($arguments['op']);
  unset($arguments['button']);

  $file_local = trim($form_state['values']['file_local']);
  $file_upload = trim($form_state['values']['file_upload']);
  $class = $form_state['values']['importer_class'];

  // Sumbit a job for this loader.
  $fname = '';
  $fid = NULL;
  if ($file_local) {
    $fname = preg_replace("/.*\/(.*)/", "$1", $file_local);
    $arguments['file_path'] = $file_local;
    $arguments['fid'] = NULL;
  }
  if ($file_upload) {
    $fid = $file_upload;
    $file = file_load($fid);
    $fname = $file->filename;
    $arguments['file_path'] =  base_path() . drupal_realpath($file->uri);
    $arguments['fid'] = $file_upload;
  }

  try {

    $values = array(
      'uid' => $user->uid,
      'class' => $class,
      'arguments' => serialize($arguments),
      'submit_date' => time(),
    );
    if ($fid) {
      $values['fid'] = $fid;
    }
    // Add the importer to the tripal_import table.
    $import_id = db_insert('tripal_import')
      ->fields($values)
      ->execute();

    // Add a job to run the importer.
    $args = array($import_id);
    $includes = array(
      module_load_include('inc', 'tripal', 'api/tripal.importer.api'),
    );
    $job_id = tripal_add_job("Import " . $class::$upload_title . ": $fname", 'tripal',
        'tripal_run_importer', $args, $user->uid, 10, $includes);

    // Now associate the job_id with the import.
    db_update('tripal_import')
      ->fields(array(
        'job_id' => $job_id,
      ))
      ->condition('import_id', $import_id)
      ->execute();

    // Now allow the loader to do it's own submit if needed.
    $importer = new $class();
    $importer->formSubmit($form, $form_state);
  }
  catch (Exception $e) {
    drupal_set_message('Cannot submit import: ' . $e->getMessage(), 'error');
  }
}