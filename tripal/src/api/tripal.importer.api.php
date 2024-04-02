<?php

use Drupal\tripal\Services\TripalJob;
use Drupal\tripal\TripalImporter\TripalImporterBase;
use Drupal\tripal\Services\TripalLogger;

/**
 * @file
 * Provides an application programming interface (API) for working with
 * data file importers using the TripalImporter class.
 *
 */

/**
 * @defgroup tripal_importer_api Data Importing
 * @ingroup tripal_api
 * @{
 * Provides an application programming interface (API) for working with
 * data file importers using the TripalImporter class into a chado database.
 * @}
 *
 */

/**
 * Implements hook_handle_uploaded_file().
 *
 * This is a Tripal hook that allows the module to set the proper
 * parameters for a file uploaded via the Tripal HTML5 uploader.
 *
 * @param object $file
 *   The Drupal file object of the newly uploaded file.
 * @param string $type
 *   The category or type of file.
 *
 * @return int
 *   A Drupal managed file ID.
 *
 * @ingroup tripal_importer_api
 */
function hook_handle_uploaded_file($file, $type) {

}

/**
 * Implements hook_importer_finish().
 *
 * This hook is executed before a TripalImporter has started.  This allows
 * modules to implement specific actions prior to execution.
 *
 * @param Drupal\tripal\TripalImporter\TripalImporterBase $importer
 *   The instance of the TripalImporter class that just completed its run.
 */
function hook_importer_start($importer) {

}

/**
 * Implements hook_importer_finish().
 *
 * This hook is executed once a TripalImporter has completed both its run
 * and post run activities.  This allows modules to implement specific actions
 * once loaders are completed.
 *
 * @param Drupal\tripal\TripalImporter\TripalImporterBase $importer
 *   The instance of the TripalImporter plugin that just completed its run.
 */
function hook_importer_finish($importer) {

}


/**
 * Imports data into the database.
 *
 * Tripal provides the TripalImporter class to allow site developers to
 * create their own data loaders.  Site users can then use any data loader
 * implemented for the site by submitting the form that comes with the
 * TripalImporter implementation.  This function runs the importer using the
 * arguments provided by the user.
 *
 * @param int $import_id
 *  The ID of the import record.
 *
 * @param \Drupal\tripal\Services\TripalJob $job
 *  An optional Job object.
 *
 * @throws Exception
 *
 * @ingroup tripal_importer_api
 */
function tripal_run_importer($import_id, TripalJob $job = NULL) {

  // Initialize the logger.
  $logger = \Drupal::service('tripal.logger');

  // Get the record for this importer.
  $public = \Drupal::database();
  $query = $public->select('tripal_import', 'ti');
  $query->fields('ti');
  $query->condition('ti.import_id', $import_id);
  $result = $query->execute();
  if (!$result) {
    throw new \Exception("Cannot find the requested import job using the given ID");
  }
  $tripal_import = $result->fetchObject();

  $importer_manager = \Drupal::service('tripal.importer');
  $importer = $importer_manager->createInstance($tripal_import->class);
  $importer_def = $importer_manager->getDefinitions()[$tripal_import->class];
  $importer->load($import_id);
  $importer->setJob($job);
  $importer->prepareFiles();


  $logger->notice("Running '" . $importer_def['label'] . "' importer");
  $logger->notice("NOTE: Loading of this file is performed using a database transaction. " .
    "If it fails or is terminated prematurely then all insertions and " .
    "updates are rolled back and will not be found in the database");

  try {
    // Call the hook_importer_start functions.
    $hook = 'importer_start';
    \Drupal::moduleHandler()->invokeAllWith($hook, function (callable $hook, string $module) {
      $hook($importer);
    });

    // Run the loader
    tripal_run_importer_run($importer, $logger);

    // Handle the post run.
    tripal_run_importer_post_run($importer, $logger);

    // Call the hook_importer_finish functions.
    $hook = 'importer_finish';
    \Drupal::moduleHandler()->invokeAllWith($hook, function (callable $hook, string $module) {
      $hook($importer);
    });

    // @todo uncomment the section below once these functions are available.

    // Check for tables with new cvterms
//     $logger->notice("Remapping Chado Controlled vocabularies to Tripal Terms...");
//     tripal_chado_map_cvterms();

//     // Check for new fields and notify the user.
//     tripal_tripal_cron_notification();

    // Clear the Drupal cache
    //cache_clear_all();
  }
  catch (Exception $e) {
    if ($job) {
      // $job->logMessage($e->getMessage(), [], TRIPAL_ERROR);
      $logger->error(
        $e->getMessage()
      );
    }
    if ($importer) {
      $importer->cleanFile();
    }
  }
}

/**
 * First step of the tripal_run_importer.
 *
 * @param Drupal\tripal\TripalImporter\TripalImporterBase $loader
 *   The TripalImporter object.
 * @param Drupal\tripal\Services\TripalLogger $logger
 *   The TripalLogger object.
 *
 * @throws Exception
 *
 * @ingroup tripal_importer_api
 */
function tripal_run_importer_run($loader, $logger) {

  // Tell the loader class to start a transaction.
  $transactions = $loader->startTransactions();
  try {
    $loader->run();
    $logger->notice("Done.");
  }
  catch (\Exception $e) {
    // Rollback and re-throw the error.
    foreach ($transactions as $transaction) {
      $transaction->rollback();
    }
    $loader->rollbackTransaction('run');
    throw $e;
  }
}

/**
 * Second step of the tripal_run_importer.
 *
 * @param Drupal\tripal\TripalImporter\TripalImporterBase $loader
 *   The TripalImporter object.
 * @param Drupal\tripal\Services\TripalLogger $logger
 *   The TripalLogger object.
 *
 * @ingroup tripal_importer_api
 */
function tripal_run_importer_post_run($loader, $logger) {

  // Tell the loader class to start a transaction.
  $transactions = $loader->startTransactions();
  try {
    $loader->postRun();
  }
  catch (\Exception $e) {
    // Rollback and re-throw the error.
    foreach ($transactions as $transaction) {
      $transaction->rollback();
    }
    $loader->rollbackTransaction('postRun');
    throw $e;
  }
}
