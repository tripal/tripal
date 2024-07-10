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
 * notifications.  These API functions can be used to set messages for
 * end-users, administrators, or simple logging.
 *
 * @}
 */

// Globals used by Tripals Error catching functions
define('TRIPAL_CRITICAL', 2);
define('TRIPAL_ERROR', 3);
define('TRIPAL_WARNING', 4);
define('TRIPAL_NOTICE', 5);
define('TRIPAL_INFO', 6);
define('TRIPAL_DEBUG', 7);


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
  $user = \Drupal::currentUser();
  global $user;

  // Only show the message to the users with 'view dev helps' permission.
  if (!$user->hasPermission('view dev helps')) {
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
    \Drupal::messenger()->addMessage($full_message, 'tripal-site-admin-only');
  }
}
