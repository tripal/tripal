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
      //'status'   => 'Display status information about the daemon.',
      'stop'     => 'Stop the daemon.',
      //'restart'  => 'Restart the daemon',
      //'show-log' => 'Show the log file.',
    ),
    'options' => array(
      'num_lines' => 'The number of lines of the log file to show.',
      'deamonize' => 'Pass this option to push the daemon into the background'
    ),
    'examples' => array(
      'drush trpjob-daemon start'                            => 'Start the daemon.',
      //'drush trpjob-daemon start --feedback="100 items"'     => 'Log a status message every 100 nodes.',
      //'drush trpjob-daemon start --feedback="60 seconds"'    => 'Log a status message every 60 seconds.',
      //'drush trpjob-daemon start --verbose'                  => 'Log verbosely.',
      //' '   => '',
      //'drush trpjob-daemon status' => 'Show the current status of the daemon.',
      '  '  => '',
      'drush trpjob-daemon stop'              => 'Stop the daemon.',
      //'drush trpjob-daemon stop --timeout=10' => 'Allow 10 seconds for processing.',
      //'drush trpjob-daemon stop --queue'      => 'Queue the node access rebuild daemon to stop.',
      //'   ' => '',
      //'drush trpjob-daemon restart'              => 'Restart the daemon.',
      //'drush trpjob-daemon restart --timeout=10' => 'Allow 10 seconds for processing.',
      //'drush trpjob-daemon restart --queue'      => 'Queue the node access rebuild daemon to restart.',
      //'    ' => '',
      //'drush trpjob-daemon show-log' => 'Show the log file, using less.',
      //'drush trpjob-daemon show-log --watch' => 'Watch the log file.',
      //'drush trpjob-daemon show-log --tail' => 'Show just the tail of the log file, to see recent messages.',
    ),
    'aliases' => array('trpjob-daemon'),
  );

  return $items;
}

/**
 * Drush Command for Daemonized management of Tripal Jobs.
 * Simply plugs into the Daemon API for easier running. This is the equivalent of
 *   drush jobs-daemon $action tripal_daemon
 *
 * @param $action
 *   One of 'start','stop','restart',status','show-log'. Meant to indicate what you want
 *   the daemon to do.
 */
function drush_tripal_daemon_tripal_jobs_daemon($action) {
  drush_daemon_api_jobs_daemon($action, 'tripal_daemon');
}