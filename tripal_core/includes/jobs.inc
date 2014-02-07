<?php
/**
 * @file
 * Contains functions related to the display of Tripal jobs in a Tripal website.
 */

/**
 * @defgroup tripal_jobs Jobs
 * @ingroup tripal_core
 * @{
 * Contains functions related to the display of Tripal jobs in a Tripal website.
 * @}
 */

/**
 * Provides a landing page for tripal jobs admin
 *
 * @ingroup tripal_jobs
 */
function tripal_jobs_admin_view() {
  $output = '';

  // set the breadcrumb
  $breadcrumb = array();
  $breadcrumb[] = l('Home', '<front>');
  $breadcrumb[] = l('Administration', 'admin');
  $breadcrumb[] = l('Tripal', 'admin/tripal');
  $breadcrumb[] = l('Jobs', 'admin/tripal/tripal_jobs');
  drupal_set_breadcrumb($breadcrumb);

  // Add the view
  $view = views_embed_view('tripal_core_admin_jobs','default');
  if (isset($view)) {
    $output .= $view;
  }
  else {
    $output .= '<p>The Tripal Jobs management system uses primarily views to provide an '
      . 'administrative interface. Currently one or more views needed for this '
      . 'administrative interface are disabled. <strong>Click each of the following links to '
      . 'enable the pertinent views</strong>:</p>';
    $output .= '<ul>';
      $output .= '<li>'.l('Jobs View', 'admin/tripal/tripal_jobs/views/jobs/enable').'</li>';
    $output .= '</ul>';
  }

  return $output;
}

/**
 * NO LONGER USED: REPLACED BY VIEW
 *
 * @ingroup tripal_jobs
 */
function tripal_jobs_report_form($form, &$form_state = NULL) {
  $form = array();

  // set the default values
  $default_status = '';
  $default_job_name = '';
  if (array_key_exists('values', $form_state)) {
    $default_status = array_key_exists('job_status', $form_state['values']) ? $form_state['values']['job_status'] : '';
    $default_job_name = array_key_exists('job_name', $form_state['values']) ? $form_state['values']['job_name'] : '';
  }

  if (!$default_status and array_key_exists('tripal_job_filter', $_SESSION)) {
    $job_status = array_key_exists('job_status', $_SESSION['tripal_job_filter']) ? $_SESSION['tripal_job_filter']['job_status'] : '';
  }
  if (!$default_job_name and array_key_exists('tripal_job_filter', $_SESSION)) {
    $default_job_name = array_key_exists('job_name', $_SESSION['tripal_job_filter']) ? $_SESSION['tripal_job_filter']['job_name'] : '';
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
  $form['job_name'] = array(
    '#type'          => 'textfield',
    '#title'         => t('Filter by Job Name'),
    '#description'   => t('The jobs will be filtered if text provided is contained in the job name'),
    '#default_value' => $default_job_name,
  );

  $form['submit'] = array(
    '#type'         => 'submit',
    '#value'        => t('Filter'),
  );
  return $form;
}

/**
 * NO LONGER USED: REPLACED BY VIEW
 *
 * @ingroup tripal_jobs
 */
function tripal_jobs_report_form_submit($form, &$form_state = NULL) {

  $job_status = $form_state['values']['job_status'];
  $job_name = $form_state['values']['job_name'];

  $_SESSION['tripal_job_filter']['job_status'] = $job_status;
  $_SESSION['tripal_job_filter']['job_name'] = $job_name;
}
/**
 * Returns the Tripal Job Report
 *
 * @return
 *   The HTML to be rendered which describes the job report
 *
 * @ingroup tripal_jobs
 */
function tripal_jobs_report() {

  // run the following function which will
  // change the status of jobs that have errored out
  tripal_is_job_running();

  $job_status = '';
  $job_name = '';
  if (array_key_exists('tripal_job_filter', $_SESSION)) {
    $job_status = array_key_exists('job_status', $_SESSION['tripal_job_filter']) ? $_SESSION['tripal_job_filter']['job_status'] : '';
    $job_name = array_key_exists('job_name', $_SESSION['tripal_job_filter']) ? $_SESSION['tripal_job_filter']['job_name'] : '';
  }

  // build the SQL for getting the jobs
  $sql = "
    SELECT
      TJ.job_id,TJ.uid,TJ.job_name,TJ.modulename,TJ.progress,
      TJ.status as job_status, TJ,submit_date,TJ.start_time,
      TJ.end_time,TJ.priority,U.name as username
    FROM {tripal_jobs} TJ
      INNER JOIN {users} U on TJ.uid = U.uid
    WHERE 1=1
  ";
  $args = array();
  if ($job_status) {
    $sql .= "AND TJ.status = :status ";
    $args[':status'] = $job_status;
  }
  if ($job_name) {
    $sql .= "AND TJ.job_name like :job_name";
    $args[':job_name'] = "%$job_name%";
  }
  $sql .= " ORDER BY job_id DESC ";

  // create the SQL that returns the total number of records
  $count_sql = "SELECT count(*) as total FROM ($sql) as t1";
  $result = db_query($count_sql, $args);
  $total_jobs = $result->fetchObject();

  // initialize the pager
  $num_per_page = 25;
  $page = pager_find_page();
  pager_default_initialize($total_jobs->total, $num_per_page);

  // get results
  $pager_sql = "$sql LIMIT  :number OFFSET :offset";
  $args[':number'] = $num_per_page;
  $args[':offset'] = $num_per_page * $page;
  $jobs = db_query($pager_sql, $args);


  // iterate through the jobs and build the table rows
  $rows = array();
  foreach ($jobs as $job) {
    $submit = tripal_get_job_submit_date($job);
    $start = tripal_get_job_start($job);
    $end = tripal_get_job_end($job);
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

  // the header for the jobs table
  $header = array(
    'Job ID',
    'User',
    'Job Name',
    array('data' => 'Dates', 'style' => "white-space: nowrap"),
    'Priority',
    'Progress',
    'Status',
    'Action'
  );
  $table = array(
    'header' => $header,
    'rows' => $rows,
    'attributes' => array(),
    'sticky' => FALSE,
    'caption' => '',
    'colgroups' => array(),
    'empty' => 'No jobs have been submitted',
  );

  // create the report page
  $output = "Waiting jobs are executed first by priority level (the lower the " .
            "number the higher the priority) and second by the order they " .
            "were entered";
  $report_form = drupal_get_form('tripal_jobs_report_form');
  $output .= drupal_render($report_form);
  $output .= theme_table($table);
  $output .= theme('pager');
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
 *
 * @ingroup tripal_jobs
 */
function tripal_jobs_view($job_id) {
  // get the job record
  $sql =
    "SELECT TJ.job_id,TJ.uid,TJ.job_name,TJ.modulename,TJ.progress,
            TJ.status as job_status, TJ,submit_date,TJ.start_time,
            TJ.end_time,TJ.priority,U.name as username,TJ.arguments,
            TJ.callback,TJ.error_msg,TJ.pid
     FROM {tripal_jobs} TJ
       INNER JOIN users U on TJ.uid = U.uid
     WHERE TJ.job_id = :job_id";
  $results = db_query($sql, array(':job_id' => $job_id));
  $job = $results->fetchObject();

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
  // generate the list of arguments for display
  $arguments = '';
  foreach ($job->arguments as $key => $value) {
     $arguments .= "$key: $value<br>";
  }

  // build the links
  $links  = l('Return to jobs list', "admin/tripal/tripal_jobs/") . ' | ' .
  $links .= l('Re-run this job', "admin/tripal/tripal_jobs/rerun/" . $job->job_id) . ' | ';
  if ($job->start_time == 0 and $job->end_time == 0) {
    $links .= l('Cancel this job', "admin/tripal/tripal_jobs/cancel/" . $job->job_id) . ' | ';
  }

  // make our start and end times more legible
  $job->submit_date = tripal_get_job_submit_date($job);
  $job->start_time = tripal_get_job_start($job);
  $job->end_time = tripal_get_job_end($job);

  // construct the table headers
  $header = array('Detail', 'Value');

  // construct the table rows
  $rows[] = array('Job Description', $job->job_name);
  $rows[] = array('Submitting Module', $job->modulename);
  $rows[] = array('Callback function', $job->callback);
  $rows[] = array('Arguments', $arguments);
  $rows[] = array('Progress', $job->progress . "%");
  $rows[] = array('Status', $job->job_status);
  $rows[] = array('Process ID', $job->pid);
  $rows[] = array('Submit Date', $job->submit_date);
  $rows[] = array('Start time', $job->start_time);
  $rows[] = array('End time', $job->end_time);
  $rows[] = array('Error Message', $job->error_msg);
  $rows[] = array('Priority', $job->priority);
  $rows[] = array('Submitting User', $job->username);

  $table = array(
    'header' => $header,
    'rows' => $rows,
    'attributes' => array(),
    'sticky' => FALSE,
    'caption' => '',
    'colgroups' => array(),
    'empty' => '',
  );

  $output = '<p>' . substr($links, 0, -2) . '</p>'; // remove trailing |
  $output .= theme_table($table);
  return $output;
}
