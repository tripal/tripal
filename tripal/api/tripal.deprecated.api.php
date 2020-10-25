<?php


/**
 * DEPRECATED
 *
 * Use the TripalJob::load function instead
 *
 * Retrieve information regarding a tripal job
 *
 * @param $job_id
 *   The unique identifier of the job
 *
 * @return
 *   An object representing a record from the tripal_job table or FALSE on
 *   failure.
 *
 * @ingroup tripal_jobs_api
 */
function tripal_get_job($job_id) {
  $logger = \Drupal::service('tripal.logger');
  $logger->warning("DEPRECATED: the '@old_function' function will be removed " .
      "from the API in a future release. Please use '@new_function' instead.",
      ['@old_function' => 'tripal_get_job',
        '@new_function' => 'TripalJob::load'
      ]
      );

  try {
    $job = new TripalJob();
    $job->load($job_id);
    return $job->getJob();
  }
  catch (Exception $e) {
    tripal_report_error('tripal', TRIPAL_ERROR, $e->getMessage());
    drupal_set_message($e->getMessage(), 'error');
    return FALSE;
  }
}