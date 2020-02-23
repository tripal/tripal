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
  $items = [];
  $items['tripal-jobs-daemon'] = [
    'description' => dt('Use Tripal Jobs Deamon to manage Tripal Job execution.'),
    'arguments' => [
      'start' => 'Start the daemon.',
      'status' => 'Display status information about the daemon.',
      'stop' => 'Stop the daemon.',
      'show-log' => 'Show the log file.',
    ],
    'options' => [
      'parallel' => dt('Normally jobs are executed one at a time. But if you are certain no conflicts will occur with other currently running jobs you may set this argument to a value of 1 to make the job run in parallel with other running jobs.'),
      'max_jobs' => dt('Indicate the maximum number of concurrent jobs. Default is 3; use -1 (unlimited). Ignore if not running parallel jobs'),
      'num_lines' => 'The number of lines of the log file to show.',
      'child' => [
        'hidden' => TRUE,
        'description' => 'This option should only be passed via '
          . 'drush_invoke_process and essentially just allows my command '
          . 'to not fork bomb',
      ],
    ],
    'examples' => [
      'drush trpjob-daemon start' => 'Start the daemon.',
      'drush trpjob-daemon status' => 'Show the current status of the daemon.',
      'drush trpjob-daemon stop' => 'Stop the daemon.',
      'drush trpjob-daemon show-log' => 'Show the last 10 lines of the log file.',
      'drush trpjob-daemon show-log --num_lines=50' => 'Show the last 10 lines of the log file.',
    ],
    'aliases' => ['trpjob-daemon'],
  ];

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

  $parallel = drush_get_option('parallel', FALSE);
  $max_jobs = drush_get_option('max_jobs', 3);

  // Check if we have the right version of Drush Daemon to support passing
  // options to the daemon class.
  $have_support = FALSE;
  if (function_exists('drushd_instantiate_daemon')) {
    $have_support = TRUE;
  }

  // We need to handle start ourselves in order to handle parallel processing
  if ($parallel AND ($action == 'start')) {

    // Check if we have the right version of Drush Daemon to support passing
    // options to the daemon class.
    if (function_exists('drushd_instantiate_daemon')) {
      // First, instantiate the daemon.
      $daemon = drushd_instantiate_daemon('tripal_daemon');

      // We always start our daemons in daemon-mode. Thus when the daemon is first
      // started from drush, we need to fork the process. However, we don't want
      // our children to fork continuously or we will end up with a fork_bomb.
      // Thus when we start our child process we pass in the "child" option which
      // tells our drush command not to fork again but instead to just run
      // the daemon.
      if (!drush_get_option('child')) {
        drush_invoke_process(
          '@self',
          'tripal-jobs-daemon',
          ['start'],
          ['child' => TRUE, 'parallel' => $parallel, 'max_jobs' => $max_jobs],
          ['fork' => TRUE]
        );
        drush_print(dt("Use 'drush tripal-jobs-daemon status' to check the "
          . "status of the daemon just started and 'drush tripal-jobs-daemon stop' to stop it.\n"));
      }
      else {
        $daemon->setParallel($parallel, $max_jobs);
        $daemon->run();
      }
    }
    else {
      drush_set_error(dt('Error: You need version 2.3 of Drush Daemon to run
        parallel jobs. Either update your version of Drush Daemon or start the
        Tripal Daemon without the parallel option.'));
    }
  }
  // Otherwise just let Drush daemon handle it ;-).
  else {
    drush_drushd_daemon($action, 'tripal_daemon');
  }
}
