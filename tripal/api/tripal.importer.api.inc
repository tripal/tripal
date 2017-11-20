<?php
/**
 * Implements hook_handle_uplaoded_file().
 *
 * This is a Tripal hook that allows the module to set the proper
 * parameters for a file uploaded via the Tripal HTML5 uploader.
 *
 * @param $filename
 *   The name of the file uploaded
 * @param $filepath
 *   The path to the file
 * @param $type
 *   The category or type of file.
 *
 * @return
 *   A Drupal managed file ID.
 */
function hook_handle_uploaded_file($filename, $filepath, $type) {

}

/**
 * Retrieves a list of TripalImporter Importers.
 *
 * The TripalImporter classes can be added by a site developer that wishes
 * to create a new data loader.  The class file should
 * be placed in the [module]/includes/TripalImporter directory.  Tripal will
 * support any loader as long as it is in this directory and extends the
 * TripalImporter class.
 *
 * @return
 *   A list of TripalImporter names.
 */
function tripal_get_importers() {
  $importers = array();

  $modules = module_list(TRUE);
  foreach ($modules as $module) {
    // Find all of the files in the tripal_chado/includes/fields directory.
    $loader_path = drupal_get_path('module', $module) . '/includes/TripalImporter';
    $loader_files = file_scan_directory($loader_path, '/.inc$/');
    // Iterate through the fields, include the file and run the info function.
    foreach ($loader_files as $file) {
      $class = $file->name;
      module_load_include('inc', $module, 'includes/TripalImporter/' . $class);
      if (class_exists($class) and is_subclass_of($class, 'TripalImporter')) {
        $importers[] = $class;
      }
    }
  }
  return $importers;
}

/**
 * Loads the TripalImporter class file into scope.
 *
 * @param $class
 *   The TripalImporter class to include.
 *
 * @return
 *   TRUE if the field type class file was found, FALSE otherwise.
 */
function tripal_load_include_importer_class($class) {

  $modules = module_list(TRUE);
  foreach ($modules as $module) {
    $file_path = realpath(".") . '/' . drupal_get_path('module', $module) . '/includes/TripalImporter/' . $class . '.inc';
    if (file_exists($file_path)) {
      module_load_include('inc', $module, 'includes/TripalImporter/' . $class);
      if (class_exists($class)) {
        return TRUE;
      }
    }
  }
  return FALSE;
}

/**
 * Imports data into the database.
 *
 * Tripal provides the TripalImporter class to allow site developers to
 * create their own data loaders.  Site users can then use any data loader
 * implemented for the site by submitting the form that comes with the
 * TripalImporter impelmentation.  This function runs the importer using the
 * arguments provided by the user.
 *
 * @param $import_id
 *   The ID of the import record.
 * @throws Exception
 */
function tripal_run_importer($import_id, TripalJob $job = NULL) {

  $loader = NULL;
  try {
    // begin the transaction
    $transaction = db_transaction();
    print "\nNOTE: Loading of this file is performed using a database transaction. \n" .
        "If the load fails or is terminated prematurely then the entire set of \n" .
        "insertions/updates is rolled back and will not be found in the database\n\n";

    $loader = TripalImporter::byID($import_id);
    $loader->setJob($job);
    $loader->prepareFiles();
    $loader->run();
    $loader->cleanFile();

    if ($job) {
      $job->logMessage("Done");
    }

    // Remove the temp file
    if (!empty($details->arguments['file_url'])) {
      $loader->logMessage('Removing downloaded file...');
      unlink($temp);
    }
  }
  catch (Exception $e) {
    $transaction->rollback();
    if ($job) {
      $job->logMessage($e->getMessage(), array(), TRIPAL_ERROR);
    }
    if ($loader) {
      $loader->cleanFile();
    }
  }
  unset($transaction);
  try {
    // the transaction
    $transaction = db_transaction();
    $loader->postRun();
  } 
  catch (Exception $e) {
    $transaction->rollback();
    if ($job) {
      $job->logMessage($e->getMessage(), array(), TRIPAL_ERROR);
    }
    if ($loader) {
      $loader->cleanFile();
    }
  }
  // Check for tables with new cvterms
  tripal_chado_map_cvterms();
  
  // Check for new fields and notify the user.
  tripal_tripal_cron_notification();
  
  // Clear the Drpual chace
  cache_clear_all();

  print "\nDone\n";
}

