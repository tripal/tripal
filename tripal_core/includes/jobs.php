<?php

/**
 * @file
 * Contains functions related to the display of Tripal jobs in a Tripal website.
 *
 */

/**
 *
 * @ingroup tripal_core
 */
function tripal_jobs_report_form($form, &$form_state = NULL) {
  $form = array();

  // set the default values
  $default_status = $form_state['values']['job_status'];

  if (!$default_status) {
    $default_status = $_SESSION['tripal_job_status_filter'];
  }

  $form['job_status'] = array(
    '#type'          => 'select',
    '#title'         => t('Filter by Job Status'),
    '#default_value' => $default_status,
    '#options' => array(
	    0           => 'All Jobs',
	    'Running'   => 'Running',
	    'Waiting'   => 'Waiting',
	    'Completed' => 'Completed',
	    'Cancelled' => 'Cancelled',
	    'Error'     => 'Error',
	  ),
  );

  $form['submit'] = array(
    '#type'         => 'submit',
    '#value'        => t('Filter'),
  );
  return $form;
}
/**
 *
 * @ingroup tripal_core
 */
function tripal_jobs_report_form_submit($form, &$form_state = NULL) {
  $job_status = $form_state['values']['job_status'];
  $_SESSION['tripal_job_status_filter'] = $job_status;
}
/**
 * Returns the Tripal Job Report
 *
 * @return
 *   The HTML to be rendered which describes the job report
 *
 * @ingroup tripal_core
 */
function tripal_jobs_report() {

  // run the following function which will
  // change the status of jobs that have errored out
  tripal_jobs_check_running();

	$jobs_status_filter = $_SESSION['tripal_job_status_filter'];

  $sql = "
    SELECT
      TJ.job_id,TJ.uid,TJ.job_name,TJ.modulename,TJ.progress,
      TJ.status as job_status, TJ,submit_date,TJ.start_time,
      TJ.end_time,TJ.priority,U.name as username
    FROM {tripal_jobs} TJ
      INNER JOIN {users} U on TJ.uid = U.uid ";
  if ($jobs_status_filter) {
    $sql .= "WHERE TJ.status = '%s' ";
  }
  $sql .= "ORDER BY job_id DESC";

  $jobs = pager_query($sql, 25, 0, "SELECT count(*) FROM ($sql) as t1", $jobs_status_filter);
  $header = array(
    'Job ID',
    'User',
    'Job Name',
    array('data' => 'Dates', 'style'=> "white-space: nowrap"),
    'Priority',
    'Progress',
    'Status',
    'Action');
  $rows = array();

  // iterate through the jobs
  while ($job = db_fetch_object($jobs)) {
    $submit = tripal_jobs_get_submit_date($job);
    $start = tripal_jobs_get_start_time($job);
    $end = tripal_jobs_get_end_time($job);
    $cancel_link = '';
    if ($job->start_time == 0 and $job->end_time == 0) {
      $cancel_link = "<a href=\"" . url("admin/tripal/tripal_jobs/cancel/" . $job->job_id) . "\">Cancel</a><br />";
    }
    $rerun_link = "<a href=\"" . url("admin/tripal/tripal_jobs/rerun/" . $job->job_id) . "\">Re-run</a><br />";
    $view_link ="<a href=\"" . url("admin/tripal/tripal_jobs/view/" . $job->job_id) . "\">View</a>";
    $rows[] = array(
      $job->job_id,
      $job->username,
      $job->job_name,
      "Submit Date: $submit<br>Start Time: $start<br>End Time: $end",
      $job->priority,
      $job->progress . '%',
      $job->job_status,
      "$cancel_link $rerun_link $view_link",
    );
  }

  // create the report page
  $output .= "Waiting jobs are executed first by priority level (the lower the ".
             "number the higher the priority) and second by the order they ".
             "were entered";
  $output .= drupal_get_form('tripal_jobs_report_form');
  $output .= theme('table', $header, $rows);
  $output .= theme_pager();
  return $output;
}
/**
 * Returns the HTML code to display a given job
 *
 * @param $job_id
 *   The job_id of the job to display
 *
 * @return
 *   The HTML describing the indicated job
 * @ingroup tripal_core
 */
function tripal_jobs_view($job_id) {
  return theme('tripal_core_job_view', $job_id);
}

/**
 * Registers variables for the tripal_core_job_view themeing function
 *
 * @param $variables
 *   An array containing all variables supplied to this template
 *
 * @ingroup tripal_core
 */
function tripal_core_preprocess_tripal_core_job_view(&$variables) {

  // get the job record
  $job_id = $variables['job_id'];
  $sql =
    "SELECT TJ.job_id,TJ.uid,TJ.job_name,TJ.modulename,TJ.progress,
            TJ.status as job_status, TJ,submit_date,TJ.start_time,
            TJ.end_time,TJ.priority,U.name as username,TJ.arguments,
            TJ.callback,TJ.error_msg,TJ.pid
     FROM {tripal_jobs} TJ
       INNER JOIN users U on TJ.uid = U.uid
     WHERE TJ.job_id = %d";
  $job = db_fetch_object(db_query($sql, $job_id));

  // we do not know what the arguments are for and we want to provide a
  // meaningful description to the end-user. So we use a callback function
  // deinfed in the module that created the job to describe in an array
  // the arguments provided.  If the callback fails then just use the
  // arguments as they are
  $args = preg_split("/::/", $job->arguments);
  $arg_hook = $job->modulename . "_job_describe_args";
  if (is_callable($arg_hook)) {
    $new_args = call_user_func_array($arg_hook, array($job->callback, $args));
    if (is_array($new_args) and count($new_args)) {
      $job->arguments = $new_args;
    }
    else {
      $job->arguments = $args;
    }
  }
  else {
    $job->arguments = $args;
  }

  // make our start and end times more legible
  $job->submit_date = tripal_jobs_get_submit_date($job);
  $job->start_time = tripal_jobs_get_start_time($job);
  $job->end_time = tripal_jobs_get_end_time($job);

  // add the job to the variables that get exported to the template
  $variables['job'] = $job;
}


