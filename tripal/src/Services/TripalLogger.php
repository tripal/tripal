<?php

namespace Drupal\tripal\Services;

class TripalLogger {

  /**
   * The drupal logger object.
   */
  protected $logger;

  /**
   * The module for which messages should be logged
   */
  protected $module = 'tripal';

  /**
   * Holds the Job object
   */
  protected $job = NULL;


  /**
   * Intiailizes the Drupal logger.
   */
  protected function initLogger() {
    $this->logger = \Drupal::logger($this->module);
  }

  /**
   * Checks if error suppression is enabled.
   *
   * For backwards compatibility with Tripal v3 this function checks the
   * TRIPAL_SUPPRESS_ERRORS environment variable. If it is set then
   * all logging is suppressed even if it is not an "error" message.
   */
  protected function isSuppressed() {
    $suppress = getenv('TRIPAL_SUPPRESS_ERRORS');

    if (strtolower($suppress) === 'true') {
      return TRUE;
    }
    return FALSE;
  }

  /**
   * Constructor: initialize connections.
   */
  public function __construct() {

    // Initialize the logger.
    $this->initLogger();
  }

  /**
   * Set the name of the module that should be used for logging.
   *
   * @param $module
   *   The module name.
   */
  public function setModule($module) {
    $this->module = $module;
    $this->initLogger();
  }

  /**
   * A setter for the job object if this class is being run using a Tripal job.
   */
  public function setJob(\Drupal\tripal\Services\TripalJob $job) {
    $this->job = $job;
  }

  /**
   * Logs a message directly to the job.
   *
   * @param $message
   *   The message MUST be a string or object implementing __toString().
   * @param $context
   *   The message MAY contain placeholders in the form: {foo} where foo will
   *   be replaced by the context data in key "foo". The context array can
   *   contain arbitrary data. The only assumption that can be made by
   *   implementors is that if an Exception instance is given to produce a
   *   stack trace, it MUST be in a key named "exception".
   */
  protected function log2Job($message, $context = []) {
    if (!is_null($this->job)) {
      $this->job->log($message, $context);
    }
  }

  /**
   * Sends the log message to the webserver server logs.
   *
   * @param $message
   *   The message MUST be a string or object implementing __toString().
   * @param $context
   *   The message MAY contain placeholders in the form: {foo} where foo will
   *   be replaced by the context data in key "foo". The context array can
   *   contain arbitrary data. The only assumption that can be made by
   *   implementors is that if an Exception instance is given to produce a
   *   stack trace, it MUST be in a key named "exception".
   */
  protected function log2Server($message, $context, $options) {

    $message_str = $this->messageString($message, $context);

    global $base_url;
    $prefix = '[site ' . $base_url . '] [TRIPAL] ';

    if (!isset($options['is_progress_bar'])) {
      $message_str = $prefix . str_replace("\n", "", trim($message_str));
    }

    if (isset($options['first_progress_bar'])) {
      $message_str = $prefix . trim($message_str);
    }

    // In test environements, we are not seeing these messages.
    // To fix this, we set a global variable in the TripalTestBrowserBase
    // which we will use here to detect if we should print directly to the terminal.
    $is_a_test_environment = \Drupal::state()->get('is_a_test_environment', FALSE);
    if ($is_a_test_environment === TRUE) {
      print "\n    [TRIPAL LOGGER] " . $this->messageString($message, $context) . "\n";
    }

    error_log($message_str);
  }

  /**
   * Prints the message as a Drupal status message to the page.
   *
   * @param $level
   *   The level of the message: critical, error, emergency, alert, warning,
   *   info or notice.
   * @param $message
   *   The message MUST be a string or object implementing __toString().
   * @param $context
   *   The message MAY contain placeholders in the form: {foo} where foo will
   *   be replaced by the context data in key "foo". The context array can
   *   contain arbitrary data. The only assumption that can be made by
   *   implementors is that if an Exception instance is given to produce a
   *   stack trace, it MUST be in a key named "exception".
   */
  protected function log2Message($level, $message, $context = []) {

    if (in_array($level, ['emergency', 'alert', 'critical', 'error'])) {
      $status = \Drupal\Core\Messenger\MessengerInterface::TYPE_ERROR;
    }
    else if (in_array($level, ['warning'])) {
      $status = \Drupal\Core\Messenger\MessengerInterface::TYPE_WARNING;
    }
    else if (in_array($level, ['notice', 'info', 'debug'])) {
      $status = \Drupal\Core\Messenger\MessengerInterface::TYPE_STATUS;
    }
    else {
      // Any other type of status we just won't handle.
      return;
    }

    $message_str = $this->messageString($message, $context);
    \Drupal::messenger()->addMessage($message_str, $status);
  }


  /**
   * Converts the logged message into a full string
   *
   * Performs replacmeent of the tokens in the message with the values in the
   * context array.
   *
   * @param $message
   *   The message MUST be a string or object implementing __toString().
   * @param $context
   *   The message MAY contain placeholders in the form: {foo} where foo will
   *   be replaced by the context data in key "foo". The context array can
   *   contain arbitrary data. The only assumption that can be made by
   *   implementors is that if an Exception instance is given to produce a
   *   stack trace, it MUST be in a key named "exception".
   */
  protected function messageString($message, $context) {
    if (sizeof($context) > 0) {
      $message_str = str_replace(array_keys($context), $context, $message);
    }
    else {
      $message_str = $message;
    }
    return $message_str;
  }

  /**
   * Logs a notice message.
   *
   * Logs to:
   *   - Drupal Logger (unless specified in options)
   *   - Tripal Job log
   *   - Drupal Message (if specified in options)
   *
   * @param $message
   *   The message MUST be a string or object implementing __toString().
   * @param $context
   *   The message MAY contain placeholders in the form: {foo} where foo will
   *   be replaced by the context data in key "foo". The context array can
   *   contain arbitrary data. The only assumption that can be made by
   *   implementors is that if an Exception instance is given to produce a
   *   stack trace, it MUST be in a key named "exception".
   * @param $options
   *   An array of options where the following keys are supported:
   *     - drupal_set_message: set to TRUE if this message should also be
   *       shown as a message for the user to see on the page.
   *     - logger: set to FALSE if this message should not be sent to the
   *       Drupal logger.
   */
  public function notice($message, $context = [], $options=[]) {
    if ($this->isSuppressed()) return;

    $this->log2Job($message, $context);

    if (!array_key_exists('logger', $options) or $options['logger'] !== FALSE) {
      $message_str = $this->messageString($message, $context);
      $this->logger->notice($message_str);
    }

    if (isset($options['drupal_set_message'])) {
      $this->log2Message('notice', $message, $context);
    }
  }

  /**
   * Logs an info message.
   *
   * Logs to:
   *   - Drupal Logger (unless specified in options)
   *   - Tripal Job log
   *   - Drupal Message (if specified in options)
   *
   * @param $message
   *   The message MUST be a string or object implementing __toString().
   * @param $context
   *   The message MAY contain placeholders in the form: {foo} where foo will
   *   be replaced by the context data in key "foo". The context array can
   *   contain arbitrary data. The only assumption that can be made by
   *   implementors is that if an Exception instance is given to produce a
   *   stack trace, it MUST be in a key named "exception".
   * @param $options
   *   An array of options where the following keys are supported:
   *     - drupal_set_message: set to TRUE if this message should also be
   *       shown as a message for the user to see on the page.
   *     - logger: set to FALSE if this message should not be sent to the
   *       Drupal logger.
   */
  public function info($message, $context = [], $options=[]) {
    if ($this->isSuppressed()) return;

    $this->log2Job($message, $context);

    if (!array_key_exists('logger', $options) or $options['logger'] !== FALSE) {
      $message_str = $this->messageString($message, $context);
      $this->logger->info($message_str);
    }

    if (isset($options['drupal_set_message'])) {
      $this->log2Message('info', $message, $context);
    }
  }

  /**
   * Logs an error message.
   *
   * A prefix of "ERROR: " is added to the message to Tripal jobs.
   *
   * Logs to:
   *   - Drupal Logger (unless specified in options)
   *   - Tripal Job log
   *   - Server log
   *   - Drupal Message (if specified in options)
   *
   * @param $message
   *   The message MUST be a string or object implementing __toString().
   * @param $context
   *   The message MAY contain placeholders in the form: {foo} where foo will
   *   be replaced by the context data in key "foo". The context array can
   *   contain arbitrary data. The only assumption that can be made by
   *   implementors is that if an Exception instance is given to produce a
   *   stack trace, it MUST be in a key named "exception".
   * @param $options
   *   An array of options where the following keys are supported:
   *     - drupal_set_message: set to TRUE if this message should also be
   *       shown as a message for the user to see on the page.
   *     - logger: set to FALSE if this message should not be sent to the
   *       Drupal logger.
   *     - first_progress_bar: this should be used for the first log call for a
   *       progress bar.
   *     - is_progress_bar: this option should be used for all but the first
   *       print of a progress bar to allow it all to be printed on the
   *       same line without intervening date prefixes.
   */
  public function error($message, $context = [], $options=[]) {
    if ($this->isSuppressed()) return;

    $message = 'ERROR: ' . $message;
    $this->log2Job($message, $context);

    if (!array_key_exists('logger', $options) or $options['logger'] !== FALSE) {
      $message_str = $this->messageString($message, $context);
      $this->logger->error($message_str);
    }

    if (isset($options['drupal_set_message'])) {
      $this->log2Message('error', $message, $context);
    }

    $this->log2Server($message, $context, $options);
  }

  /**
   * Logs a warning message.
   *
   * A prefix of "WARNING: " is added to the message to Tripal jobs.
   *
   * Logs to:
   *   - Drupal Logger (unless specified in options)
   *   - Tripal Job log
   *   - Server log
   *   - Drupal Message (if specified in options)
   *
   * @param $message
   *   The message MUST be a string or object implementing __toString().
   * @param $context
   *   The message MAY contain placeholders in the form: {foo} where foo will
   *   be replaced by the context data in key "foo". The context array can
   *   contain arbitrary data. The only assumption that can be made by
   *   implementors is that if an Exception instance is given to produce a
   *   stack trace, it MUST be in a key named "exception".
   * @param $options
   *   An array of options where the following keys are supported:
   *     - drupal_set_message: set to TRUE if this message should also be
   *       shown as a message for the user to see on the page.
   *     - logger: set to FALSE if this message should not be sent to the
   *       Drupal logger.
   *     - first_progress_bar: this should be used for the first log call for a
   *       progress bar.
   *     - is_progress_bar: this option should be used for all but the first
   *       print of a progress bar to allow it all to be printed on the
   *       same line without intervening date prefixes.
   */
  public function warning($message, $context = [], $options=[]) {
    if ($this->isSuppressed()) return;

    $message = 'WARNING: ' . $message;
    $this->log2Job($message, $context);

    if (!array_key_exists('logger', $options) or $options['logger'] !== FALSE) {
      $message_str = $this->messageString($message, $context);
      $this->logger->warning($message_str);
    }

    if (isset($options['drupal_set_message'])) {
      $this->log2Message('warning', $message, $context);
    }

    $this->log2Server($message, $context, $options);
  }

  /**
   * Logs an emergency message.
   *
   * A prefix of "EMERGENCY: " is added to the message to Tripal jobs.
   *
   * Logs to:
   *   - Drupal Logger (unless specified in options)
   *   - Tripal Job log
   *   - Server log
   *   - Drupal Message (if specified in options)
   *
   * @param $message
   *   The message MUST be a string or object implementing __toString().
   * @param $context
   *   The message MAY contain placeholders in the form: {foo} where foo will
   *   be replaced by the context data in key "foo". The context array can
   *   contain arbitrary data. The only assumption that can be made by
   *   implementors is that if an Exception instance is given to produce a
   *   stack trace, it MUST be in a key named "exception".
   * @param $options
   *   An array of options where the following keys are supported:
   *     - drupal_set_message: set to TRUE if this message should also be
   *       shown as a message for the user to see on the page.
   *     - logger: set to FALSE if this message should not be sent to the
   *       Drupal logger.
   *     - first_progress_bar: this should be used for the first log call for a
   *       progress bar.
   *     - is_progress_bar: this option should be used for all but the first
   *       print of a progress bar to allow it all to be printed on the
   *       same line without intervening date prefixes.
   */
  public function emergency($message, $context = [], $options=[]) {
    if ($this->isSuppressed()) return;

    $message = 'EMERGENCY: ' . $message;
    $this->log2Job($message, $context);

    if (!array_key_exists('logger', $options) or $options['logger'] !== FALSE) {
      $message_str = $this->messageString($message, $context);
      $this->logger->emergency($message_str);
    }

    if (isset($options['drupal_set_message'])) {
      $this->log2Message('emergency', $message, $context);
    }

    $this->log2Server($message, $context, $options);
  }

  /**
   * Logs an alert message.
   *
   * A prefix of "ALERT: " is added to the message to Tripal jobs.
   *
   * Logs to:
   *   - Drupal Logger (unless specified in options)
   *   - Tripal Job log
   *   - Server log
   *   - Drupal Message (if specified in options)
   *
   * @param $message
   *   The message MUST be a string or object implementing __toString().
   * @param $context
   *   The message MAY contain placeholders in the form: {foo} where foo will
   *   be replaced by the context data in key "foo". The context array can
   *   contain arbitrary data. The only assumption that can be made by
   *   implementors is that if an Exception instance is given to produce a
   *   stack trace, it MUST be in a key named "exception".
   * @param $options
   *   An array of options where the following keys are supported:
   *     - drupal_set_message: set to TRUE if this message should also be
   *       shown as a message for the user to see on the page.
   *     - logger: set to FALSE if this message should not be sent to the
   *       Drupal logger.
   *     - first_progress_bar: this should be used for the first log call for a
   *       progress bar.
   *     - is_progress_bar: this option should be used for all but the first
   *       print of a progress bar to allow it all to be printed on the
   *       same line without intervening date prefixes.
   */
  public function alert($message, $context = [], $options=[]) {
    if ($this->isSuppressed()) return;

    $message = 'ALERT: ' . $message;
    $this->log2Job($message, $context);

    if (!array_key_exists('logger', $options) or $options['logger'] !== FALSE) {
      $message_str = $this->messageString($message, $context);
      $this->logger->alert($message_str);
    }

    if (isset($options['drupal_set_message'])) {
      $this->log2Message('alert', $message, $context);
    }

    $this->log2Server($message, $context, $options);
  }

  /**
   * Logs a crtical message.
   *
   * A prefix of "CRITICAL: " is added to the message to Tripal jobs.
   *
   * Logs to:
   *   - Drupal Logger (unless specified in options)
   *   - Tripal Job log
   *   - Server log
   *   - Drupal Message (if specified in options)
   *
   * @param $message
   *   The message MUST be a string or object implementing __toString().
   * @param $context
   *   The message MAY contain placeholders in the form: {foo} where foo will
   *   be replaced by the context data in key "foo". The context array can
   *   contain arbitrary data. The only assumption that can be made by
   *   implementors is that if an Exception instance is given to produce a
   *   stack trace, it MUST be in a key named "exception".
   * @param $options
   *   An array of options where the following keys are supported:
   *     - drupal_set_message: set to TRUE if this message should also be
   *       shown as a message for the user to see on the page.
   *     - logger: set to FALSE if this message should not be sent to the
   *       Drupal logger.
   *     - first_progress_bar: this should be used for the first log call for a
   *       progress bar.
   *     - is_progress_bar: this option should be used for all but the first
   *       print of a progress bar to allow it all to be printed on the
   *       same line without intervening date prefixes.
   */
  public function critical($message, $context = [], $options=[]) {
    if ($this->isSuppressed()) return;

    $message = 'CRITICAL: ' . $message;
    $this->log2Job($message, $context);

    if (!array_key_exists('logger', $options) or $options['logger'] !== FALSE) {
      $message_str = $this->messageString($message, $context);
      $this->logger->critical($message_str);
    }

    if (isset($options['drupal_set_message'])) {
      $this->log2Message('critical', $message, $context);
    }

    $this->log2Server($message, $context, $options);
  }

  /**
   * Logs a debug message.
   *
   * A prefix of "DEBUG: " is added to the message to Tripal jobs.
   *
   * Logs to:
   *   - Drupal Logger (unless specified in options)
   *   - Tripal Job log
   *   - Server log
   *   - Drupal Message (if specified in options)
   *
   * For Tripal 3, no debug messages were logged unless the TRIPAL_DEBUG
   * environment variable is set. Under Tripal 4, debug messages are
   * always printed.
   *
   * @param $message
   *   The message MUST be a string or object implementing __toString().
   * @param $context
   *   The message MAY contain placeholders in the form: {foo} where foo will
   *   be replaced by the context data in key "foo". The context array can
   *   contain arbitrary data. The only assumption that can be made by
   *   implementors is that if an Exception instance is given to produce a
   *   stack trace, it MUST be in a key named "exception".
   * @param $options
   *   An array of options where the following keys are supported:
   *     - logger: set to FALSE if this message should not be sent to the
   *       Drupal logger.
   *     - first_progress_bar: this should be used for the first log call for a
   *       progress bar.
   *     - is_progress_bar: this option should be used for all but the first
   *       print of a progress bar to allow it all to be printed on the
   *       same line without intervening date prefixes.
   */
  public function debug($message, $context = [], $options=[]) {
    if ($this->isSuppressed()) return;

    // If we want to implement a toggle for debug messages in the
    // future, it could go here. Tripal 3 had an environment variable
    // TRIPAL_DEBUG to perform this function.
    if (true) {
      $backtrace = debug_backtrace();
      $message .= "\nBacktrace:\n";
      $i = 1;
      for ($i = 1; $i < count($backtrace); $i++) {
        $function = $backtrace[$i];
        $message .= "  $i) " . $function['function'] . "\n";
      }
      $this->log2job('DEBUG: ' . $message, $context);
      if (!array_key_exists('logger', $options) or $options['logger'] !== FALSE) {
        $message_str = $this->messageString($message, $context);
        $this->logger->debug($message_str);
      }

      if (isset($options['drupal_set_message'])) {
        $this->log2Message('debug', $message, $context);
      }

      $this->log2Server('DEBUG: ' . $message, $context, $options);
    }
  }

  /**
   * A wrapper for the Drupal::Logger log function
   *
   * Logs to:
   *   - Drupal Logger (unless specified in options)
   *   - Tripal Job log
   *   - Server log if not 'info' or 'notice'
   *   - Drupal Message (if specified in options)
   *
   * A prefix indicating the level (other than info and notice messages) is
   * added to the message to Tripal jobs.
   *
   * @param $level
   *   The level to log: emergency, critical, alert, error, warning, notice,
   *   info, debug.
   * @param $message
   *   The message MUST be a string or object implementing __toString().
   * @param $context
   *   The message MAY contain placeholders in the form: {foo} where foo will
   *   be replaced by the context data in key "foo". The context array can
   *   contain arbitrary data. The only assumption that can be made by
   *   implementors is that if an Exception instance is given to produce a
   *   stack trace, it MUST be in a key named "exception".
   * @param $options
   *   An array of options where the following keys are supported:
   *     - drupal_set_message: set to TRUE if this message should also be
   *       shown as a message for the user to see on the page.
   *     - logger: set to FALSE if this message should not be sent to the
   *       Drupal logger.
   *     - first_progress_bar: this should be used for the first log call for a
   *       progress bar.
   *     - is_progress_bar: this option should be used for all but the first
   *       print of a progress bar to allow it all to be printed on the
   *       same line without intervening date prefixes.
   */
  public function log($level, $message, $context = [], $options=[]) {
    if ($this->isSuppressed()) return;

    $level = strtolower($level);
    if ($level != 'info' and $level != 'notice') {
      $this->log2Job(ucwords($level) . ': ' . $message, $context);
    }
    else {
      $this->log2Job($message, $context);
    }

    if (!array_key_exists('logger', $options) or $options['logger'] !== FALSE) {
      $message_str = $this->messageString($message, $context);
      $this->logger->log($level, $message_str);
    }

    if (isset($options['drupal_set_message'])) {
      $this->log2Message($level, $message, $context);
    }

    if (!in_array($level, ['notice', 'info'])) {
      $this->log2Server($message, $context, $options);
    }
  }

}
