<?php

/**
 * @file
 * Implements drush integration for this module
 *
 * @ingroup tripal_bulk_loader
 */

/**
 * Implements hook_drush_command().
 *
 * @ingroup tripal_bulk_loader
 */
function tripal_bulk_loader_drush_command() {
  $items = array();
  $items['tripal-loader-progress'] = array(
    'description' => dt('Display the progress of any running tripal bulk loading job.'),
    'aliases' => array('trpload-%'),
  );
  $items['tripal-loader-view'] = array(
    // used by drush help
    'description' => dt('Returns the status/details of the specified bulk loading job.'),
    'arguments' => array(
      'nid' => dt('The Node ID of the bulk Loading Job')
    ),
    'examples' => array(
      'Standard Example' => 'drush tripal-loader-view 5433',
    ),
    'aliases' => array('trpload-view')
  );
  $items['tripal-loader-cancel'] = array(
    // used by drush help
    'description' => dt('Cancels the specified bulk loading job.'),
    'arguments' => array(
      'nid' => dt('The Node ID of the bulk Loading Job')
    ),
    'examples' => array(
      'Standard Example' => 'drush tripal-loader-cancel 5433',
    ),
    'aliases' => array('trpload-cncl')
  );
  $items['tripal-loader-submit'] = array(
    // used by drush help
    'description' => dt('Submit or Re-submit the given bulk loading job.'),
    'arguments' => array(
      'nid' => dt('The Node ID of the bulk Loading Job')
    ),
    'examples' => array(
      'Standard Example' => 'drush tripal-loader-submit 5433',
    ),
    'aliases' => array('trpload-sbmt')
  );
  $items['tripal-loader-revert'] = array(
    // used by drush help
    'description' => dt('Revert the records loaded by the last run of the specified loading job. This is only available if the specified loading job is keeping track of inserted IDs.'),
    'arguments' => array(
      'nid' => dt('The Node ID of the bulk Loading Job')
    ),
    'examples' => array(
      'Standard Example' => 'drush tripal-loader-revert 5433',
    ),
    'aliases' => array('trpload-revert')
  );

  return $items;
}

/**
 * Code ran for the tripal-loader-progress drush command
 * Display the progress of any running tripal bulk loading job.
 *
 * @ingroup tripal_bulk_loader
 */
function drush_tripal_bulk_loader_tripal_loader_progress() {

  // determine the progress of any loading jobs
  $sql = "SELECT t.loader_name, t.file, t.job_id FROM {tripal_bulk_loader} t WHERE job_status='Loading...'";
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
          . "\tRecord Inserted: " . $progress->num_records . "\n"
          . "\tPercent Complete: " . $progress->percent_file . "\n"
          . "Number of Constant Sets fully loaded: " . $progress->num_constant_sets_loaded . "\n"
          . "Job Percent Complete: " . $progress->total_percent . "\n"
        );
      }
    }
  }
}

/**
 * Returns the status/details of the specified bulk loading job.
 *
 * @param $nid
 *   The Node ID of the bulk Loading Job
 *
 * @ingroup tripal_bulk_loader
 */
function drush_tripal_bulk_loader_tripal_loader_view ($nid) {
  $node = node_load($nid);
  $author = user_load($node->uid);

  drush_print("Job Name: ".$node->loader_name);
  drush_print("Submitted By: ".$author->name);
  drush_print("Job Creation Date: ".format_date($node->created));
  drush_print("Last Updated: ".format_date($node->changed));
  drush_print("Template Name: ".$node->template->name);
  drush_print("Data File: ".$node->file);
  drush_print("Job Status: ".$node->job_status);
}

/**
 * Cancels the specified bulk loading job.
 *
 * @param $nid
 *   The Node ID of the bulk Loading Job
 *
 * @ingroup tripal_bulk_loader
 */
function drush_tripal_bulk_loader_tripal_loader_cancel ($nid) {
  $node = node_load($nid);
  db_query("UPDATE {tripal_bulk_loader} SET job_status='%s' WHERE nid=%d", 'Job Cancelled', $node->nid);
  tripal_cancel_job($node->job_id,FALSE);
}

/**
 * Submit or Re-submit the given bulk loading job.
 *
 * @param $nid
 *   The Node ID of the bulk Loading Job
 *
 * @ingroup tripal_bulk_loader
 */
function drush_tripal_bulk_loader_tripal_loader_submit ($nid) {
  global $user;

  if ($node->job_id) {
    //Re-submit Tripal Job
    tripal_rerun_job($node->job_id);
    db_query("UPDATE {tripal_bulk_loader} SET job_status='%s' WHERE nid=%d", 'Submitted to Queue', $nid);
  }
  else {
    //Submit Tripal Job
    $node= node_load($nid);
    $job_args[1] = $nid;
    if (is_readable($node->file)) {
      $fname = basename($node->file);
      $job_id = tripal_add_job("Bulk Loading Job: $fname", 'tripal_bulk_loader', 'tripal_bulk_loader_load_data', $job_args, $user->uid);

      // add job_id to bulk_loader node
      $success = db_query("UPDATE {tripal_bulk_loader} SET job_id=%d WHERE nid=%d", $job_id, $nid);

      // change status
      db_query("UPDATE {tripal_bulk_loader} SET job_status='%s' WHERE nid=%d", 'Submitted to Queue', $nid);
    }
    else {
      drupal_set_message(t("Can not open %file. Job not scheduled.", array('%file' => $node->file)));
    }
  }

}

/**
 * Revert the records loaded by the last run of the specified loading job. This is only
 * available if the specified loading job is keeping track of inserted IDs.
 *
 * @param $nid
 *   The Node ID of the bulk Loading Job
 *
 * @ingroup tripal_bulk_loader
 */
function drush_tripal_bulk_loader_tripal_loader_revert ($nid) {

  // Remove the records from the database that were already inserted
  $resource = db_query('SELECT * FROM {tripal_bulk_loader_inserted} WHERE nid=%d ORDER BY tripal_bulk_loader_inserted_id DESC', $nid);
  while ($r = db_fetch_object($resource)) {
    $ids = preg_split('/,/', $r->ids_inserted);
    db_query('DELETE FROM {%s} WHERE %s IN (%s)', $r->table_inserted_into, $r->table_primary_key, $r->ids_inserted);
    $result = db_fetch_object(db_query('SELECT true as present FROM {%s} WHERE %s IN (%s)', $r->table_inserted_into, $r->table_primary_key, $r->ids_inserted));
    if (!$result->present) {
      drush_print('Successfully Removed data Inserted into the '.$r->table_inserted_into.' table.');
      db_query('DELETE FROM {tripal_bulk_loader_inserted} WHERE tripal_bulk_loader_inserted_id=%d', $r->tripal_bulk_loader_inserted_id);
    }
    else {
      drush_print('Unable to remove data Inserted into the '.$r->table_inserted_into.' table!');
    }
  }

  // reset status
  db_query("UPDATE {tripal_bulk_loader} SET job_status='%s' WHERE nid=%d", 'Reverted -Data Deleted', $nid);

}
