<?php
/**
 * @file
 * Provides an application programming interface (API) for Tripal
 *
 * The Tripal API currently provides generic insert/update/select functions for all chado content as
 * well as some module specific functions that insert/update/delete/select specific chado content.
 *
 * This API is currently in its infancy and some necessary functions might be missing. If you find
 * a missing function that you think should be included go to the sourceforge feature request
 * page and request it's inclusion in the API. Such feature requests with a working function
 * definition will be given priority.
 */

/**
 * @defgroup tripal_api Tripal API
 * @{
 * Provides an application programming interface (API) for Tripal
 *
 * The Tripal API currently provides generic insert/update/select functions for all chado content as
 * well as some module specific functions that insert/update/delete/select specific chado content.
 *
 * This API is currently in its infancy and some necessary functions might be missing. If you find
 * a missing function that you think should be included go to the sourceforge feature request
 * page and request it's inclusion in the API. Such feature requests with a working function
 * definition will be given priority.
 * @}
 */

// Globals used by Tripals Error catching functions
// Should match those defined by watchdog
define('TRIPAL_CRITICAL',2);
define('TRIPAL_ERROR',3);
define('TRIPAL_WARNING',4);
define('TRIPAL_NOTICE',5);
define('TRIPAL_INFO',6);
define('TRIPAL_DEBUG',7);

/**
 * Provide better error notice for Tripal. If the environment variable 
 * 'TRIPAL_DEBUG' is set to 1 then this function will add backtrace
 * information to the message.
 * 
 * @param $type
 *   The catagory to which this message belongs. Can be any string, but the general
 *   practice is to use the name of the module.
 * @param $message
 *   The message to store in the log. Keep $message translatable by not concatenating
 *   dynamic values into it! Variables in the message should be added by using placeholder
 *   strings alongside the variables argument to declare the value of the placeholders.
 *   See t() for documentation on how $message and $variables interact.
 * @param $variables
 *   Array of variables to replace in the message on display or NULL if message is
 *   already translated or not possible to translate.
 * @param $severity
 *   The severity of the message; one of the following values:
 *     - TRIPAL_CRITICAL: Critical conditions.
 *     - TRIPAL_ERROR: Error conditions.
 *     - TRIPAL_WARNING: Warning conditions.
 *     - TRIPAL_NOTICE: (default) Normal but significant conditions.
 *     - TRIPAL_INFO: Informational messages.
 *     - TRIPAL_DEBUG: Debug-level messages.
 * @param $options
 *   An array of options. Some available options include:
 *     - print: prints the error message to the terminal screen. Useful when display is the command-line
 *
 * @ingroup tripal_api
 */
function tripal_report_error($type, $severity, $message, $variables = array(), $options = array()) {

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
  
  // get the backtrace and include in the error message, but only if the
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

  // Send to watchdog
  try {
    watchdog($type, $message, $variables, $severity);
  }
  catch (Exception $e) {
    print "CRITICAL (TRIPAL_CORE): Unable to register error message with watchdog";
    $options['print'] = TRUE;
  }

  // If print option supplied then print directly to the screen
  if (isset($options['print'])) {
    if (sizeof($variables) > 0) {
      $message = str_replace(array_keys($variables), $variables, $message);
    }
    print $severity_string . ' (' . strtoupper($type) . '): ' . $message . "\n";
  }
}

/**
 * Display messages to tripal administrators. This can be used instead of
 * drupal_set_message when you want to target tripal administrators.
 *
 * @param $message
 *   The message to be displayed to the tripal administrators
 * @param $importance
 *   The level of importance for this message. In the future this will be used to allow
 *   administrators to filter some of these messages. It can be one of the following:
 *     - TRIPAL_CRITICAL: Critical conditions.
 *     - TRIPAL_ERROR: Error conditions.
 *     - TRIPAL_WARNING: Warning conditions.
 *     - TRIPAL_NOTICE: Normal but significant conditions.
 *     - TRIPAL_INFO: (default) Informational messages.
 *     - TRIPAL_DEBUG: Debug-level messages.
 * @param $options
 *   Any options to apply to the current message. Supported options include:
 *     - return_html: return HTML instead of setting a drupal message. This can be
 *         used to place a tripal message in a particular place in the page.
 *         The default is FALSE.
 */
function tripal_set_message($message, $importance = TRIPAL_INFO, $options = array()) {
  global $user;

  // Only show the message to the administrator user (uid=1)
  // thus if the current user is not the administrator then return nothing
  if (!user_access('administer tripal')) {
    return '';
  }

  // set defaults
  $options['return_html'] = (isset($options['return_html'])) ? $options['return_html'] : FALSE;

  // Get human-readable severity string
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

  // Mark-up the Message
  $full_message =
     '<div class="tripal-site-admin-message">'
       . '<span class="tripal-serverity-string ' . strtolower($importance_string) . '">' . $importance_string . ': </span>'
       . $message
   . '</div>';

  // Handle whether to return the HTML & let the caller deal with it
  // or to use drupal_set_message to put it near the top of the page  & let the theme deal with it
  if ($options['return_html']) {
    return '<div class="messages tripal-site-admin-only">' . $full_message . '</div>';
  }
  else {
    drupal_set_message($full_message, 'tripal-site-admin-only');
  }
}