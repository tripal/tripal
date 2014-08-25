<?php

/**
 * @file
 * Implementation of the Tripal Daemon Drush commands.
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
      'show-log' => 'Show the log file.',
    ),
    'options' => array(
      'num_lines' => 'The number of lines of the log file to show.',
      'child' => array(
        'hidden' => TRUE,
        'description' => 'This option should only be passed via '
        . 'drush_invoke_process and essentially just allows my command '
        . 'to not fork bomb',
      ),
    ),
    'examples' => array(
      'drush trpjob-daemon start' => 'Start the daemon.',
      'drush trpjob-daemon status' => 'Show the current status of the daemon.',
      'drush trpjob-daemon stop'              => 'Stop the daemon.',
      'drush trpjob-daemon show-log' => 'Show the last 10 lines of the log file.',
      'drush trpjob-daemon show-log --num_lines=50' => 'Show the last 10 lines of the log file.',
    ),
    'aliases' => array('trpjob-daemon'),
  );

  return $items;
}

/**
 * Drush Command for Daemonized management of Tripal Jobs.
 *
 * Simply plugs into the Daemon API for easier running. This is equivalent to
 *   drush jobs-daemon $action tripal_daemon.
 *
 * @param string $action
 *   One of 'start','stop','restart',status','show-log'. Meant to indicate what
 *   you want the daemon to do.
 */
function drush_tripal_daemon_tripal_jobs_daemon($action) {
  drush_drushd_daemon($action, 'tripal_daemon');
}
