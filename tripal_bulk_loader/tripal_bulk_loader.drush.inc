<?php

/**
 * Implements drush integration for this module
 */

/**
 * Implements hook_drush_command().
 */
function tripal_bulk_loader_drush_command() {
  $items = array();
  $items['tripal-loader-progress'] = array(
    'description' => dt('Display the progress of any running tripal bulk loading job.'),
    'aliases' => array('trpload-%'),
  );
  return $items;
}

/**
 * Code ran for the tripal-loader-progress drush command
 * Display the progress of any running tripal bulk loading job.
 */
function drush_tripal_bulk_loader_tripal_loader_progress () {

  // determine the progress of any loading jobs
  $sql = "SELECT t.loader_name, t.file, t.job_id FROM tripal_bulk_loader t WHERE job_status='Loading...'";
  $resource = db_query($sql);

  while ($r = db_fetch_object($resource)) {
    if ($r->job_id) {
      $progress = tripal_bulk_loader_progess_file_get_progress($r->job_id);
      if ($progress->num_records > 0 AND $progress->total_percent < 100) {
        drush_print(
          $r->loader_name . "\n"
          . str_repeat("-", 40) . "\n"
          . "File:" . $r->file . "\n"
          . "Current Constant Set:\n"
          . "\tLines processed: " . $progress->num_lines . "\n"
          . "\tRecord Inserted: " .$progress->num_records . "\n"
          . "\tPercent Complete: " . $progress->percent_file . "\n"
          . "Number of Constant Sets fully loaded: " . $progress->num_constant_sets_loaded . "\n"
          . "Job Percent Complete: " . $progress->total_percent . "\n"
        );
      }
    }
  }
}