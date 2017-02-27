<?php
/**
 * Returns the end time for a given job
 *
 * @param $job
 *   An object describing the job
 *
 * @return
 *   The end time of the job if it was already run and empty otherwise
 *
 * @ingroup tripal_jobs_api
 */
function tripal_get_job_end($job) {
  tripal_report_error('tripal_deprecated', TRIPAL_NOTICE,
    "DEPRECATED: %function has been removed from the API the end date " .
    "is now accessible via the %property property. Please update your code.",
    array(
      '%old_function' => 'tripal_jobs_get_end_time',
      '%property' => '\$job->end_time_string',
    )
  );

  if ($job->end_time > 0) {
    $end = format_date($job->end_time);
  }
  else {
    $end = '';
  }

  return $end;
}

/**
 * Returns the start time for a given job
 *
 * @param $job
 *   An object describing the job
 *
 * @return
 *   The start time of the job if it was already run and either "Cancelled" or "Not Yet Started" otherwise
 *
 * @ingroup tripal_jobs_api
 */
function tripal_get_job_start($job) {

  tripal_report_error('tripal_deprecated', TRIPAL_NOTICE,
    "DEPRECATED: %function has been removed from the API the end date " .
    "is now accessible via the %property property. Please update your code.",
    array(
      '%old_function' => 'tripal_get_job_start',
      '%property' => '\$job->start_time_string',
    )
  );

  if ($job->start_time > 0) {
    $start = format_date($job->start_time);
  }
  else {
    if (strcmp($job->job_status, 'Cancelled')==0) {
      $start = 'Cancelled';
    }
    else {
      $start = 'Not Yet Started';
    }
  }
  return $start;
}

/**
 * Returns the date the job was added to the queue
 *
 * @param $job
 *   An object describing the job
 *
 * @return
 *   The date teh job was submitted
 *
 * @ingroup tripal_jobs_api
 */
function tripal_get_job_submit_date($job) {

  tripal_report_error('tripal_deprecated', TRIPAL_NOTICE,
      "DEPRECATED: %function has been removed from the API the end date " .
      "is now accessible via the %property property. Please update your code.",
      array(
        '%old_function' => 'tripal_get_job_submit_date',
        '%property' => '\$job->submit_date_string',
      )
  );
  return format_date($job->submit_date);
}
