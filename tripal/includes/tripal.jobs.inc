<?php
/**
 * @file
 * Contains functions related to the display of Tripal jobs in a Tripal website.
 */


/**
 * Provides a landing page for tripal jobs admin
 *
 */
function tripal_jobs_admin_view() {
  $output = '';

  // set the breadcrumb
  $breadcrumb = [];
  $breadcrumb[] = l('Home', '<front>');
  $breadcrumb[] = l('Administration', 'admin');
  $breadcrumb[] = l('Tripal', 'admin/tripal');
  $breadcrumb[] = l('Jobs', 'admin/tripal/tripal_jobs');
  drupal_set_breadcrumb($breadcrumb);

  // Add the view
  $view = views_embed_view('tripal_admin_jobs', 'default');
  if (isset($view)) {
    $output .= $view;
  }
  else {
    $output .= '<p>The Tripal Jobs management system uses primarily views to provide an '
      . 'administrative interface. Currently one or more views needed for this '
      . 'administrative interface are disabled. <strong>Go to '
      . l('Administration > Structure > Views', 'admin/structure/views')
      . ' and enable the view titled "Tripal Jobs (Admin)"';
  }

  return $output;
}

/**
 * NO LONGER USED: REPLACED BY VIEW
 *
 */
function tripal_jobs_report_form($form, &$form_state = NULL) {
  $form = [];

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


  $form['job_status'] = [
    '#type' => 'select',
    '#title' => t('Filter by Job Status'),
    '#default_value' => $default_status,
    '#options' => [
      0 => 'All Jobs',
      'Running' => 'Running',
      'Waiting' => 'Waiting',
      'Completed' => 'Completed',
      'Cancelled' => 'Cancelled',
      'Error' => 'Error',
    ],
  ];
  $form['job_name'] = [
    '#type' => 'textfield',
    '#title' => t('Filter by Job Name'),
    '#description' => t('The jobs will be filtered if text provided is contained in the job name'),
    '#default_value' => $default_job_name,
  ];

  $form['submit'] = [
    '#type' => 'submit',
    '#value' => t('Filter'),
  ];
  return $form;
}

/**
 * NO LONGER USED: REPLACED BY VIEW
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
 */
function tripal_jobs_report() {

  // run the following function which will
  // change the status of jobs that have errored out
  tripal_get_running_jobs();

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
  $args = [];
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
  $rows = [];
  foreach ($jobs as $job) {
    $submit = tripal_get_job_submit_date($job);
    $start = tripal_get_job_start($job);
    $end = tripal_get_job_end($job);
    $cancel_link = '';
    $execute_link = '';
    if ($job->start_time == 0 and $job->end_time == 0) {
      $cancel_link = "<a href=\"" . url("admin/tripal/tripal_jobs/cancel/" . $job->job_id) . "\">Cancel</a><br />";
      $execute_link = "<a href=\"" . url("admin/tripal/tripal_jobs/execute/" . $job->job_id) . "\">Execute</a><br />";
    }
    $rerun_link = "<a href=\"" . url("admin/tripal/tripal_jobs/rerun/" . $job->job_id) . "\">Re-run</a><br />";
    $view_link = "<a href=\"" . url("admin/tripal/tripal_jobs/view/" . $job->job_id) . "\">View</a>";
    $rows[] = [
      $job->job_id,
      $job->username,
      $job->job_name,
      "Submit Date: $submit<br>Start Time: $start<br>End Time: $end",
      $job->priority,
      $job->progress . '%',
      $job->job_status,
      "$execute_link $cancel_link $rerun_link $view_link",
    ];
  }

  // the header for the jobs table
  $header = [
    'Job ID',
    'User',
    'Job Name',
    ['data' => 'Dates', 'style' => "white-space: nowrap"],
    'Priority',
    'Progress',
    'Status',
    'Action',
  ];
  $table = [
    'header' => $header,
    'rows' => $rows,
    'attributes' => ['class' => 'tripal-data-table'],
    'sticky' => FALSE,
    'caption' => '',
    'colgroups' => [],
    'empty' => 'No jobs have been submitted',
  ];

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
 */
function tripal_jobs_view($job_id) {

  // set the breadcrumb
  $breadcrumb = [];
  $breadcrumb[] = l('Home', '<front>');
  $breadcrumb[] = l('Administration', 'admin');
  $breadcrumb[] = l('Tripal', 'admin/tripal');
  $breadcrumb[] = l('Jobs', 'admin/tripal/tripal_jobs');
  drupal_set_breadcrumb($breadcrumb);

  // get the job record
  $sql =
    "SELECT TJ.job_id,TJ.uid,TJ.job_name,TJ.modulename,TJ.progress,
            TJ.status as job_status, TJ,submit_date,TJ.start_time,
            TJ.end_time,TJ.priority,U.name as username,TJ.arguments,
            TJ.callback,TJ.error_msg,TJ.pid
     FROM {tripal_jobs} TJ
       INNER JOIN users U on TJ.uid = U.uid
     WHERE TJ.job_id = :job_id";
  $results = db_query($sql, [':job_id' => $job_id]);
  $job = $results->fetchObject();

  // We do not know what the arguments are for and we want to provide a
  // meaningful description to the end-user. So we use a callback function
  // defined in the module that created the job to describe in an array
  // the arguments provided.  If the callback fails then just use the
  // arguments as they are.  Historically, job arguments were separated with
  // two colon. We now store them as a serialized array. So, we need to handle
  // both cases.
  if (preg_match("/::/", $job->arguments)) {
    $args = preg_split("/::/", $job->arguments);
  }
  else {
    $args = unserialize($job->arguments);
  }
  $arg_hook = $job->modulename . "_job_describe_args";
  if (is_callable($arg_hook)) {
    $new_args = call_user_func_array($arg_hook, [$job->callback, $args]);
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
    if (is_array($value)) {
      $value = print_r($value, TRUE);
    }
    $arguments .= "$key: $value<br>";
  }

  // build the links
  $items = [];
  $items[] = l('Return to jobs list', "admin/tripal/tripal_jobs/");
  $items[] = l('Re-run this job', "admin/tripal/tripal_jobs/rerun/" . $job->job_id);
  if ($job->start_time == 0 and $job->end_time == 0) {
    $items[] = l('Cancel this job', "admin/tripal/tripal_jobs/cancel/" . $job->job_id);
    $items[] = l('Execute this job', "admin/tripal/tripal_jobs/execute/" . $job->job_id);
  }
  $links = theme_item_list([
    'items' => $items,
    'title' => '',
    'type' => 'ul',
    'attributes' => [
      'class' => ['action-links'],
    ],
  ]);

  // make our start and end times more legible
  $job->submit_date = tripal_get_job_submit_date($job);
  $job->start_time = tripal_get_job_start($job);
  $job->end_time = tripal_get_job_end($job);

  // construct the table headers
  $header = ['Detail', 'Value'];

  // construct the table rows
  $rows[] = ['Job Description', $job->job_name];
  $rows[] = ['Submitting Module', $job->modulename];
  $rows[] = ['Callback function', $job->callback];
  $rows[] = ['Arguments', $arguments];
  $rows[] = ['Progress', $job->progress . "%"];
  $rows[] = ['Status', $job->job_status];
  $rows[] = ['Process ID', $job->pid];
  $rows[] = ['Submit Date', $job->submit_date];
  $rows[] = ['Start time', $job->start_time];
  $rows[] = ['End time', $job->end_time];
  $rows[] = ['Priority', $job->priority];
  $rows[] = ['Submitting User', $job->username];

  $table = [
    'header' => $header,
    'rows' => $rows,
    'attributes' => ['class' => 'tripal-data-table'],
    'sticky' => FALSE,
    'caption' => '',
    'colgroups' => [],
    'empty' => '',
  ];

  $content['links'] = [
    '#type' => 'markup',
    '#markup' => $links,
  ];
  $content['job_title'] = [
    '#type' => 'item',
    '#title' => t('Job Title'),
    '#markup' => $job->job_name,
  ];
  $content['job_status'] = [
    '#type' => 'item',
    '#title' => t('Status'),
    '#markup' => $job->job_status,
  ];
  $content['details_fset'] = [
    '#type' => 'fieldset',
    '#title' => t('Job Details'),
    '#collapsed' => TRUE,
    '#collapsible' => TRUE,
    '#attributes' => [
      'class' => ['collapsible'],
    ],
    '#attached' => [
      'js' => ['misc/collapse.js', 'misc/form.js'],
    ],
  ];
  $content['details_fset']['job_details'] = [
    '#type' => 'markup',
    '#markup' => theme_table($table),
  ];
  $content['log_fset'] = [
    '#type' => 'fieldset',
    '#title' => t('Job Logs'),
    '#collapsed' => TRUE,
    '#collapsible' => TRUE,
    '#attributes' => [
      'class' => ['collapsible'],
    ],
    '#attached' => [
      'js' => ['misc/collapse.js', 'misc/form.js'],
    ],
  ];
  $content['log_fset']['job_logs'] = [
    '#type' => 'markup',
    '#markup' => '<pre class="tripal-job-logs">' . $job->error_msg . '</pre>',
  ];
  return $content;
}

/**
 * Runs a Tripal job from within the request.
 *
 * @param $job_id
 */
function tripal_jobs_status_view($job_id) {
  // set the breadcrumb
  $breadcrumb = [];
  $breadcrumb[] = l('Home', '<front>');
  $breadcrumb[] = l('Administration', 'admin');
  $breadcrumb[] = l('Tripal', 'admin/tripal');
  $breadcrumb[] = l('Jobs', 'admin/tripal/tripal_jobs');
  drupal_set_breadcrumb($breadcrumb);

  $job = tripal_get_job($job_id);

  drupal_add_css(drupal_get_path('module', 'tripal') . '/theme/css/tripal_jobs.css');
  drupal_add_js(drupal_get_path('module', 'tripal') . '/theme/js/tripal_jobs.js');

  $markup = "<h2>Job Progress</h2>";
  $markup .= "<p>Job: " . $job->job_name . ' (' . $job->progress . '% Complete)';
  $markup .= '<br>Status: ' . $job->status . '</p>';
  $markup .= '<div id="tripal-jobs-progress-bar"><div></div></div>';
  $markup .= '<p>' . l('Refresh Page', 'admin/tripal/tripal_jobs/status/' . $job_id) . '</p>';

  drupal_add_js('var progress_percent = ' . $job->progress . ';', ['type' => 'inline']);

  // Reload the page every 30 seconds.
  $meta = [
    '#tag' => 'meta',
    '#attributes' => [
      'http-equiv' => 'refresh',
      'content' => '30',
    ],
  ];
  drupal_add_html_head($meta, 'tripal_job_status_page');

  $page = [
    '#type' => 'markup',
    '#markup' => $markup,
  ];

  return $page;
}

