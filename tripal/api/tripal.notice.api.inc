<?php
/**
 * @file
 * Provides an application programming interface (API) for improved user
 * notifications.  These API functions can be used to set messages for
 * end-users, administrators, or simple logging.
 */

/**
 * @defgroup tripal_notify_api Notify
 * @ingroup tripal_api
 * @{
 * Provides an application programming interface (API) for improved user
 * notivications.  These API functions can be used to set messages for
 * end-users, administrators, or simple logging.
 *
 * @}
 */

// Globals used by Tripal's Error catching functions
// Should match those defined by watchdog
define('TRIPAL_CRITICAL', 2);
define('TRIPAL_ERROR', 3);
define('TRIPAL_WARNING', 4);
define('TRIPAL_NOTICE', 5);
define('TRIPAL_INFO', 6);
define('TRIPAL_DEBUG', 7);


/**
 * Provide better error notice for Tripal.
 *
 * Please be sure to set the $options array as desired. For example, by default
 * this function sends all messages to the Drupal watchdog. If a long running
 * job uses this function and prints status messages you may not want to have
 * those go to the watchdog as it can dramatically affect performance.
 *
 * If the environment variable 'TRIPAL_DEBUG' is set to 1 then this function
 * will add backtrace information to the message.
 *
 * @param $type
 *   The category to which this message belongs. Can be any string, but the
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
 *     - watchdog:  set to FALSE to disable logging to Drupal's watchdog.
 *     - job: The jobs management object for the job if this function is run
 *       as a job. Adding the job object here ensures that any status or error
 *       messages are also logged with the job.
 *
 * @ingroup tripal_notify_api
 */
function tripal_report_error($type, $severity, $message, $variables = [], $options = []) {

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

  // Send to watchdog if the user wants.
  if (array_key_exists('watchdog', $options) and $options['watchdog'] !== FALSE) {
    try {
      watchdog($type, $message, $variables, $severity);
    } catch (Exception $e) {
      print "CRITICAL (TRIPAL): Unable to add error message with watchdog: " . $e->getMessage() . "\n.";
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
      $status = 'error';
    }
    elseif ($severity == TRIPAL_WARNING) {
      $status = 'warning';
    }
    else {
      $status = 'status';
    }
    drupal_set_message($print_message, $status);
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
 * Display messages to tripal administrators.
 *
 * This can be used instead of drupal_set_message when you want to target
 * tripal administrators.
 *
 * @param $message
 *   The message to be displayed to the tripal administrators.
 * @param $importance
 *   The level of importance for this message. In the future this will be used
 *   to allow administrators to filter some of these messages. It can be one of
 *   the following:
 *     - TRIPAL_CRITICAL: Critical conditions.
 *     - TRIPAL_ERROR: Error conditions.
 *     - TRIPAL_WARNING: Warning conditions.
 *     - TRIPAL_NOTICE: Normal but significant conditions.
 *     - TRIPAL_INFO: (default) Informational messages.
 *     - TRIPAL_DEBUG: Debug-level messages.
 * @param $options
 *   Any options to apply to the current message. Supported options include:
 *     - return_html: return HTML instead of setting a drupal message. This can
 *       be used to place a tripal message in a particular place in the page.
 *       The default is FALSE.
 *
 * @ingroup tripal_notify_api
 */
function tripal_set_message($message, $importance = TRIPAL_INFO, $options = []) {
  global $user;

  // Only show the message to the users with 'view dev helps' permission.
  if (!user_access('view dev helps')) {
    return '';
  }

  // Set defaults.
  $options['return_html'] = (isset($options['return_html'])) ? $options['return_html'] : FALSE;

  // Get human-readable severity string.
  $importance_string = '';
  switch ($importance) {
    case TRIPAL_CRITICAL:
      $importance_string = 'CRITICAL';
      break;
    case TRIPAL_ERROR:
      $importance_string = 'ERROR';
      break;
    case TRIPAL_WARNING:
      $importance_string = 'WARNING';
      break;
    case TRIPAL_NOTICE:
      $importance_string = 'NOTICE';
      break;
    case TRIPAL_INFO:
      $importance_string = 'INFO';
      break;
    case TRIPAL_DEBUG:
      $importance_string = 'DEBUG';
      break;
  }

  // Mark-up the Message.
  $full_message =
    '<div class="tripal-site-admin-message">' .
    '<span class="tripal-severity-string ' . strtolower($importance_string) . '">' . $importance_string . ': </span>' .
    $message .
    '</div>';

  // Handle whether to return the HTML & let the caller deal with it
  // or to use drupal_set_message to put it near the top of the page  & let the 
  // theme deal with it.
  if ($options['return_html']) {
    return '<div class="messages tripal-site-admin-only">' . $full_message . '</div>';
  }
  else {
    drupal_set_message($full_message, 'tripal-site-admin-only');
  }
}

/**
 * File-based error logging for Tripal.
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
  global $base_url;
  $prefix = '[site ' . $base_url . '] [TRIPAL ' . strtoupper(check_plain($type)) . '] ';

  if (!isset($options['is_progress_bar'])) {
    $message = $prefix . str_replace("\n", "", trim($message));
  }

  if (isset($options['first_progress_bar'])) {
    $message = $prefix . trim($message);
  }

  return error_log($message);

}
