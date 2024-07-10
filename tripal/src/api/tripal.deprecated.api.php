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
    // drupal_set_message($e->getMessage(), 'error');
    $messenger = \Drupal::messenger();
    $messenger->addError($e->getMessage());
    return FALSE;
  }
}


/**
 * DEPRECATED
 *
 * Provide better error notice for Tripal.
 *
 * This function is deprecated in Tripal v4.  It will be removed in a future
 * release of Tripal. Please use the \Drupal\tripal\Services\TripalLogger
 * class instead.
 *
 * Please be sure to set the $options array as desired. For example, by default
 * this function sends all messages to the Drupal logger. If a long running
 * job uses this function and prints status messages you may not want to have
 * those go to the logger as it can dramatically affect performance.
 *
 * If the environment variable 'TRIPAL_DEBUG' is set to 1 then this function
 * will add backtrace information to the message.
 *
 * @param $type
 *   The catagory to which this message belongs. Can be any string, but the
 *   general practice is to use the name of the module.
 * @param $severity
 *   The severity of the message; one of the following values:
 *     - TRIPAL_CRITICAL: Critical conditions.
 *     - TRIPAL_ERROR: Error conditions.
 *     - TRIPAL_WARNING: Warning conditions.
 *     - TRIPAL_NOTICE: (default) Normal but significant conditions.
 *     - TRIPAL_INFO: Informational messages.
 *     - TRIPAL_DEBUG: Debug-level messages.
 * @param $message
 *   The message to store in the log. Keep $message translatable by not
 *   concatenating dynamic values into it! Variables in the message should be
 *   added by using placeholder strings alongside the variables argument to
 *   declare the value of the placeholders. See t() for documentation on how
 *   $message and $variables interact.
 * @param $variables
 *   Array of variables to replace in the message on display or NULL if message
 *   is already translated or not possible to translate.
 * @param $options
 *   An array of options. Some available options include:
 *     - print: prints the error message to the terminal screen. Useful when
 *       display is the command-line
 *     - drupal_set_message:  set to TRUE then send the message to the
 *       drupal_set_message function.
 *     - logger:  set to FALSE to disable logging to Drupal's logger.
 *     - job: The jobs management object for the job if this function is run
 *       as a job. Adding the job object here ensures that any status or error
 *       messages are also logged with the job.
 *
 * @ingroup tripal_notify_api
 */
function tripal_report_error($type, $severity, $message, $variables = [], $options = []) {

  $logger = \Drupal::service('tripal.logger');
  $logger->warning("DEPRECATED: the '@function' function will be removed from the API in " .
      "a future release. Please use the TripalLogger service for logging.",
      ['@function' => 'tripal_report_error']);

  $suppress = getenv('TRIPAL_SUPPRESS_ERRORS');

  if (strtolower($suppress) === 'true') {
    return;
  }

  // Get human-readable severity string
  $severity_string = '';
  switch ($severity) {
    case TRIPAL_CRITICAL:
      $severity_string = 'CRITICAL';
      break;
    case TRIPAL_ERROR:
      $severity_string = 'ERROR';
      break;
    case TRIPAL_WARNING:
      $severity_string = 'WARNING';
      break;
    case TRIPAL_NOTICE:
      $severity_string = 'NOTICE';
      break;
    case TRIPAL_INFO:
      $severity_string = 'INFO';
      break;
    case TRIPAL_DEBUG:
      $severity_string = 'DEBUG';
      break;
  }

  // If we are not set to return debugging information and the severity string
  // is debug then don't report the error.
  if (($severity == TRIPAL_DEBUG) AND (getenv('TRIPAL_DEBUG') != 1)) {
    return FALSE;
  }

  // Get the backtrace and include in the error message, but only if the
  // TRIPAL_DEBUG environment variable is set.
  if (getenv('TRIPAL_DEBUG') == 1) {
    $backtrace = debug_backtrace();
    $message .= "\nBacktrace:\n";
    $i = 1;
    for ($i = 1; $i < count($backtrace); $i++) {
      $function = $backtrace[$i];
      $message .= "  $i) " . $function['function'] . "\n";
    }
  }

  // Send to logger if the user wants.
  if (array_key_exists('logger', $options) and $options['logger'] !== FALSE) {
    try {
      if (in_array($severity, [TRIPAL_CRITICAL, TRIPAL_ERROR])) {
        \Drupal::logger($type)->error($message);
      }
      elseif ($severity == TRIPAL_WARNING) {
        \Drupal::logger($type)->warning($message);
      }
      else {
        \Drupal::logger($type)->notice($message);
      }
    } catch (Exception $e) {
      print "CRITICAL (TRIPAL): Unable to add error message with logger: " . $e->getMessage() . "\n.";
      $options['print'] = TRUE;
    }
  }

  // Format the message for printing (either to the screen, log or both).
  if (sizeof($variables) > 0) {
    $print_message = str_replace(array_keys($variables), $variables, $message);
  }
  else {
    $print_message = $message;
  }

  // If print option supplied then print directly to the screen.
  if (isset($options['print'])) {
    print $severity_string . ' (' . strtoupper($type) . '): ' . $print_message . "\n";
  }

  if (isset($options['drupal_set_message'])) {
    if (in_array($severity, [TRIPAL_CRITICAL, TRIPAL_ERROR])) {
      $status = \Drupal\Core\Messenger\MessengerInterface::TYPE_ERROR;
    }
    elseif ($severity == TRIPAL_WARNING) {
      $status = \Drupal\Core\Messenger\MessengerInterface::TYPE_WARNING;
    }
    else {
      $status = \Drupal\Core\Messenger\MessengerInterface::TYPE_STATUS;
    }
    \Drupal::messenger()->addMessage($print_message, $status);
  }

  // Print to the Tripal error log but only if the severity is not info.
  if (($severity != TRIPAL_INFO)) {
    tripal_log('[' . strtoupper($type) . '] ' . $print_message . "\n", $severity_string);
  }

  if (array_key_exists('job', $options) and is_a($options['job'], 'TripalJob')) {
    $options['job']->logMessage($message, $variables, $severity);
  }
}


/**
 * DEPRECATED
 *
 * File-based error logging for Tripal.
 *
 * This function is deprecated in Tripal v4.  It will be removed in a future
 * release of Tripal. Please use the \Drupal\tripal\Services\TripalLogger
 * class instead.
 *
 * Consider using the tripal_report_error function rather than
 * calling this function directly, as that function calls this one for non
 * INFO messages and has greater functionality.
 *
 * @param $message
 *   The message to be logged. Need not contain date/time information.
 * @param $log_type
 *   The type of log. Should be one of 'error' or 'job' although other types
 *   are supported.
 * @param $options
 *   An array of options where the following keys are supported:
 *     - first_progress_bar: this should be used for the first log call for a
 *       progress bar.
 *     - is_progress_bar: this option should be used for all but the first print
 *       of a progress bar to allow it all to be printed on the same line
 *       without intervening date prefixes.
 *
 * @return
 *   The number of bytes that were written to the file, or FALSE on failure.
 *
 * @ingroup tripal_notify_api
 */
function tripal_log($message, $type = 'error', $options = []) {

  $logger = \Drupal::service('tripal.logger');
  $logger->warning("DEPRECATED: the '@function' function will be removed from the API in " .
      "a future release. Please use the TripalLogger service for logging.",
      ['@function' => 'tripal_log']);


  global $base_url;
  $prefix = '[site ' . $base_url . '] [TRIPAL ' . strtoupper($type) . '] ';

  if (!isset($options['is_progress_bar'])) {
    $message = $prefix . str_replace("\n", "", trim($message));
  }

  if (isset($options['first_progress_bar'])) {
    $message = $prefix . trim($message);
  }

  return error_log($message);

}
