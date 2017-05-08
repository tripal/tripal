<?php

class TripalJob {

  /**
   * A database connection distinct from the transaction used for the job.
   * This allows for updating the job progress without that progress being
   * wiped out if the job fails.
   */
  private $logger;

  /**
   * Instantiates a new TripalJob object.
   *
   * @param $job_id
   */
  public function __construct() {
    $logger =

  }

  public function run() {

  }

  /**
   * Provides a message to the user by logging a system message.
   *
   * This function takes similar arguments as Drupal's watchdog function.
   *
   * @param $message
   *   The message to store in the log. Keep $message translatable by not
   *   concatenating dynamic values into it! Variables in the message should
   *   be added by using placeholder strings alongside the variables argument
   *   to declare the value of the placeholders. See t() for documentation on
   *   how $message and $variables interact.
   * @param $variables
   *   Array of variables to replace in the message on display or NULL if
   *   message is already translated or not possible to translate.
   * @param $severity
   *   The severity of the message; one of the following values:
   *     - TRIPAL_CRITICAL: Critical conditions.
   *     - TRIPAL_ERROR: Error conditions.
   *     - TRIPAL_WARNING: Warning conditions.
   *     - TRIPAL_NOTICE: Normal but significant conditions.
   *     - TRIPAL_INFO: (default) Informational messages.
   *     - TRIPAL_DEBUG: Debug-level messages.
   */
  public function logError($message, $variables = array(), $severity = TRIPAL_INFO) {

  }
  /**
   * @param $message
   *   (optional) The translated message to be displayed to the user. For
   *   consistency with other messages, it should begin with a capital letter
   *   and end with a period.
   * @param $type
   *   (optional) The message's type. Defaults to 'status'. These values are
   *   supported:
   *     - 'status'
   *     - 'warning'
   *     - 'error'
   * @param $repeat
   *   (optional) If this is FALSE and the message is already set, then the
   *   message won't be repeated. Defaults to TRUE.
   */
  public function logOutput($message, $type = 'status', $repeat = FALSE) {

  }
}