<?php

/**
 * @file
 * Implementation of the Tripal Daemon Drush commands
 */

/**
 * Implements hook_drush_help().
 */
function tripal_daemon_drush_help($command) {
  switch ($command) {
    case 'drush:tripal-jobs-daemon':
      return dt('Use Tripal Jobs Deamon to manage Tripal Job execution.');
  }
}

/**
 * Implements hook_drush_command().
 */
function tripal_daemon_drush_command() {
  $items = array();
  $items['tripal-jobs-daemon'] = array(
    'description' => dt('Use Tripal Jobs Deamon to manage Tripal Job execution.'),
    'arguments' => array(
      'start'    => 'Start the daemon.',
      'status'   => 'Display status information about the daemon.',
      'stop'     => 'Stop the daemon.',
      'restart'  => 'Restart the daemon',
      'show-log' => 'Show the log file.',
    ),
    'options' => array(
      'memory_threshold' => 'The maximum memory usage allowed before the daemon should kill itself. Default: 0.85',
      'wait_time' => 'The amount of seconds to wait before checking whether a Tripal Job has been submitted. Default: 60.',
      'file_path' => 'The filepath to store the log and status files to. This should be
        either the sites/default/files directory for your Drupal site or the /tmp directory. Default: /tmp.',
      'log_filename' => 'The name of the logfile. Default: tripaljobs_daemon.log',
      'status_filename' => array(
        'description' => 'The name of the file containing a JSON array describing the status of the Daemon. Default: tripaljobs_daemon.status.json',
        'hidden' => TRUE
      )
    ),
    'examples' => array(
      'drush trpjob-daemon start'                            => 'Start the daemon.',
      //'drush trpjob-daemon start --feedback="100 items"'     => 'Log a status message every 100 nodes.',
      //'drush trpjob-daemon start --feedback="60 seconds"'    => 'Log a status message every 60 seconds.',
      //'drush trpjob-daemon start --verbose'                  => 'Log verbosely.',
      ' '   => '',
      'drush trpjob-daemon status' => 'Show the current status of the daemon.',
      '  '  => '',
      'drush trpjob-daemon stop'              => 'Stop the daemon.',
      //'drush trpjob-daemon stop --timeout=10' => 'Allow 10 seconds for processing.',
      //'drush trpjob-daemon stop --queue'      => 'Queue the node access rebuild daemon to stop.',
      '   ' => '',
      'drush trpjob-daemon restart'              => 'Restart the daemon.',
      //'drush trpjob-daemon restart --timeout=10' => 'Allow 10 seconds for processing.',
      //'drush trpjob-daemon restart --queue'      => 'Queue the node access rebuild daemon to restart.',
      '    ' => '',
      'drush trpjob-daemon show-log' => 'Show the log file, using less.',
      //'drush trpjob-daemon show-log --watch' => 'Watch the log file.',
      //'drush trpjob-daemon show-log --tail' => 'Show just the tail of the log file, to see recent messages.',
    ),
    'aliases' => array('trpjob-daemon'),
  );

  return $items;
}

/**
 * Drush Command for Daemonized management of Tripal Jobs
 *
 * @param $action
 *   One of 'start','stop','restart',status','show-log'. Meant to indicate what you want
 *   the daemon to do.
 */
function drush_tripal_daemon_tripal_jobs_daemon($action) {

  $details = tripal_daemon_drush_command();
  $supported_options = array_keys($details['tripal-jobs-daemon']['options']);

  $script_path = drupal_get_path('module','tripal_daemon');
  $module_path = DRUPAL_ROOT . '/' . drupal_get_path('module','tripal_daemon');

  $output = NULL;
  $return_value = NULL;
  $options = array();
  foreach ($supported_options as $opt) {
    $val = drush_get_option($opt);
    if ($val) {
      $options[] = $opt . '=' . $val;
    }
  }
  $command = "php $script_path/tripal_daemon_script.php $action module_path=$module_path " . implode(" ",$options) . " &";
  exec($command); //, $output, $return_value);
  /**
  drush_print(implode("\n",$output));

  switch($return_value) {
    case 0:
      drush_log("Daemon started successfully.",'ok');
      break;
    default:
      drush_log("Unable to $action the Daemon.",'error');
      break;
  }
  */
}