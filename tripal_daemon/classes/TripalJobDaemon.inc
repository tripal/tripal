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

  // Whether or not the current process is a Daemon (ie: child) or not (ie: parent)
  protected $is_daemon = FALSE;

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

    // Whether or not the current process is a Daemon (ie: child) or not (ie: parent)
    // There are VERY FEW cases where it's a good idea to pass this option in yourself
    if (isset($args['is_daemon'])) {
      $this->is_daemon = $args['is_daemon'];
    }
  }

  /**
   * Start the Daemon
   */
  public function start() {

    // If we are not currently dealing with the daemon/child process then we need to first
    // create it via forking
    if (!$this->is_daemon) {
      print "Starting the Tripal Daemon...\n";
      $this->fork();
    }


    // If we are a Daemon, we need to implement the never-ending daemonizing loop
    if ($this->is_daemon) {

      //Now, we detach from the terminal window, so that we stay alive when it is closed.
      if ( posix_setsid() == -1 ) {
	      echo "\n Error: Unable to detach from the terminal window. \n";
      }

      for ($i=0; $i<=10; $i++) {
        print posix_getpid() . " Mua" . str_repeat("ha",rand(1,15)) . "\n";
      }
    }
    // Otherwise, we are going to be an irresponsible parent and let our child run amok
    else {
      print posix_getpid() . " Letting my child run amok :)\n";
      exit;
    }
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

  /**
   * Fork off a new process to be the daemon since nobody wants to be the daemon themselves ;-)
   */
  protected function fork() {
    $pid = pcntl_fork();
    pcntl_signal(SIGCHLD, SIG_IGN);

    // Problem launching the job
    if ($pid == -1){
      error_log('Could not launch new job, exiting');
      return FALSE;
    }
    // Parent Process since children are not self-aware
    else if ($pid) {
      print "Daemon Process ID: $pid.\n";
    }
    // Child Process (we should never get here b/c of the is_daemon logic in start)
    else {
      $this->is_daemon = TRUE;
    }
  }
}