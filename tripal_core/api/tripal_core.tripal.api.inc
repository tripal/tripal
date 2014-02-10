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
 * Provide better error notice for Tripal
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
    print $severity_string . ' (' . strtoupper($type) . '):' . $message . "\n";
  }
}

/**
 * Use this function to encapsulate text intended to be
 * visible only by the site administrator. A small tripal logo
 * appears alongside the text.  Do not call this function directly, but
 * rather, use the theme() function:
 *
 *   theme('tripal_admin_message', array('message' => $my_message));
 *
 * @param $message
 *   The message to be displayed to the site administrator
 *
 * @ingroup tripal_api
 */
function theme_tripal_admin_message($variables) {

  $message = $variables['message'];

  if (!user_access('access administration pages')) {
    return '';
  }
  return "
    <div class=\"tripal-site-admin-only\">
      <div class=\"tripal-site-admin-message\">$message</div>
    </div>";
}