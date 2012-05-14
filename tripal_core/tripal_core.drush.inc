<?php

/**
 * Describes each drush command implemented by the module
 *
 * @return
 *   The first line of description when executing the help for a given command
 */
function tripal_core_drush_help($command) {
  switch ($command) {
    case 'drush:tripal-current-job':
      return dt('Returns details about the currently running tripal job including percent complete.');
    case 'drush:tripal-update-mview':
      return dt('Updates the specified materialized view.');
  }
}

/**
 * Registers a drush command and constructs the full help for that command
 *
 * @return
 *   And array of command descriptions
 */
function tripal_core_drush_command() {
  $items = array();
  $items['tripal-current-job'] = array(
    'description' => dt('Returns details about the currently running tripal job including percent complete.'),
    'arguments'   => array(
    ),
    'examples' => array(
      'Standard example' => 'drush tripal-current-job',
    ),
    'aliases' => array('trpjob'),
  );
  $items['tripal-update-mview'] = array(
    // used by drush help
    'description' => dt('Updates the specified materialized view.'),
    'arguments' => array(
      'mview_id' => dt('The ID of the materialized view to update'),
      'table_name' => dt('The name of the materialized view table to update.'),
    ),
    'examples' => array(
      'By Materialized View ID' => 'drush tripal-update-mview --mview_id=5',
      'By Table Name' => 'drush tripal-update-mview --table_name=organism_feature_count'
    ),
    // supply options
    'options' => array(
      'mview_id',
      'table_name'
    ),
    'alias' => array('trpmv-up')
  );
  return $items;
}

/**
 * Prints details about the current running job
 * 
 * NOTE: The following code is executed when drush tjob or drush tripal-current-job is called
 */
function drush_tripal_core_tripal_current_job () {
  
  $sql =  "SELECT * FROM {tripal_jobs} TJ ".
            "WHERE TJ.end_time IS NULL and NOT TJ.start_time IS NULL ";
  $jobs = db_query($sql);
  while ($job = db_fetch_object($jobs)) {
    $job_pid = $job->pid;

    $output = "Name: ".$job->job_name."\n"
             ."Submitted: ".date(DATE_RFC822,$job->submit_date)."\n"
             ."Started: ".date(DATE_RFC822,$job->start_time)."\n"
             ."Module: ".$job->modulename."\n"
             ."Callback: ".$job->callback."\n"
             ."Process ID: ".$job->pid."\n"
             ."Progress: ".$job->progress."%\n";
    drush_print($output);
  }
  
  if (!$job_pid) {
    drush_print('There are currently no running jobs.');
  }
    
  //log to the command line with an OK status
  drush_log('Running tripal-current-job', 'ok');
}

/**
 * Updates the specified materialized view
 *
 * @param $mview_id
 *   The ID of the materialized view (tripal_mview.mview_id)
 * @param $table_name
 *   The name of the table storing the materialized view (tripal_mview.mv_table)
 *
 * Note: Either $mview_id OR $table_name is required
 */ 
function drush_tripal_core_tripal_update_mview () {
  $mview_id = drush_get_option('mview_id');
  $table_name = drush_get_option('table_name');
  
  // Either table_name or mview is required
  if (!$mview_id) {
    if ($table_name) {
      // if table_name supplied use that to get mview_id
      $sql = "SELECT mview_id FROM {tripal_mviews} WHERE mv_table='%s'";
      $r = db_fetch_object(db_query($sql, $table_name));
      if (!$r->mview_id) {
        drush_set_error('No Materialized View associated with that table_name.');
      }
      $mview_id=$r->mview_id;
    } else {
      drush_set_error('Either mview_id OR table_name are required.');
    }
  }
  
  drush_print('Updating the Materialized View with ID='.$mview_id);  
  $status = tripal_update_mview($mview_id);
  if ($status) {
    drush_log('Materialized View Updated', 'ok');
  } else {
    drush_set_error('Update failed.');
  }
  
}