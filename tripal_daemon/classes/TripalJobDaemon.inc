<?php

/**
 *
 */
class TripalJobDaemon {

  // Maximum Memory Usage Allowed
  // During Health checks, if the memory usage is over this % usage for the system
  // then the child process will kill re-fork and kill itself
  protected $memory_threshold = 0.85;
  public function get_memory_threshold() { return $this->memory_threshold; }

  // How often to check for more Tripal Jobs (in seconds)
  // Default is 1 minute (60 seconds)
  protected $wait_time = 60;
  public function get_wait_time() { return $this->wait_time; }

  // The daemon command, such as start, stop, restart
  protected $action = '';
  public function get_action() { return $this->action; }

  // The filepath to store the log and status files to
  // This should be either the sites/default/files directory for your Drupal site
  // or the /tmp directory and should be set upon creation of the Tripal Daemon
  protected $file_path;

  // The filename & path of the log file
  public $log_filename;

  // The filename & path of the status file
  public $status_filename;

  /**
   * Creates a new TripalDaemon object
   */
  public function __construct($args = array()) {

    // Set the log/status files
    $this->file_path = (isset($args['file_path'])) ? $args['file_path'] : '/tmp';

    $this->log_filename = (isset($args['log_filename'])) ? $args['log_filename'] : 'tripaljobs_daemon.log';
    $this->log_filename = $this->file_path . '/' . $this->log_filename;

    $this->status_filename = (isset($args['status_filename'])) ? $args['status_filename'] : 'tripaljobs_daemon.status.json';
    $this->status_filename = $this->file_path . '/' . $this->status_filename;

    // If the wait time is provided then set it to the provided value
    if (isset($args['wait_time'])) {
      $this->wait_time = $args['wait_time'];
    }

    // If the memory threshold is provided then use it instead of our default
    if (isset($args['memory_threshold'])) {
      $this->memory_threshold = $args['memory_threshold'];
    }
  }

  /**
   * Start the Daemon
   */
  public function start() {
    print "Starting the Tripal Daemon...\n";
  }

  /**
   * Stop the Daemon
   */
  public function stop() {
    print "Stopping the Tripal Daemon...\n";
  }

  /**
   * Restart the Daemon
   */
  public function restart() {
    print "Re-Starting the Tripal Daemon...\n";
  }

  /**
   * Show the Status of the Daemon
   */
  public function status() {
    print "Checking the Status of the Tripal Daemon...\n";
  }

  /**
   * Show the Log of the Daemon
   */
  public function log() {
    print "Showing the Tripal Daemon Log...\n";
  }
}