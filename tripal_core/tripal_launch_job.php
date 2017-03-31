<?php

/**
 * @file
 * This script will launch any waiting tripal jobs in succession.
 *
 * This script must be run at the base directory level of the drupal installation
 * in order to pick up all necessary dependencies
 */

$stdout = fopen('php://stdout', 'w');

// we require one command-line argument
if (sizeof($argv) < 5) {
  print_usage($stdout);
  exit;
}

$job_id = $argv[1];
$root = $argv[2];
$username = $argv[3];
$do_parallel = $argv[4];
$max_jobs = (isset($argv[5]) ? $argv[5] : -1;  // -1 = don't limit number of consecutive jobs

/**
 * Root directory of Drupal installation.
 */
define('DRUPAL_ROOT', getcwd());

require_once DRUPAL_ROOT . '/includes/bootstrap.inc';
drupal_bootstrap(DRUPAL_BOOTSTRAP_FULL);
menu_execute_active_handler();

$drupal_base_url = parse_url('http://www.example.com');
$_SERVER['HTTP_HOST'] = $drupal_base_url['host'];
//  $_SERVER['PHP_SELF'] = $drupal_base_url['path'].'/index.php';
$_SERVER['REQUEST_URI'] = $_SERVER['SCRIPT_NAME'] = $_SERVER['PHP_SELF'];
$_SERVER['REMOTE_ADDR'] = NULL;
$_SERVER['REQUEST_METHOD'] = NULL;


$results = db_query("SELECT * FROM {users} WHERE name = :name", array(':name' => $username));
$u = $results->fetchObject();
if (!$u) {
  fwrite($stdout, "'$username' is not a valid Drupal username. exiting...\n");
  exit;
}

global $user;
$user = user_load($u->uid);

fwrite($stdout, date('Y-m-d' H:i:s) . "\n");
fwrite($stdout, "Tripal Job Launcher\n");
fwrite($stdout, "Running as user ' . $username . '\n");
fwrite($stdout, "-------------------\n");

tripal_launch_job($do_parallel, null, $max_jobs);

/**
 * Print out the usage instructions if they are not followed correctly
 *
 * @ingroup tripal_core
 */
function print_usage($stdout) {
  fwrite($stdout, "Usage:\n");
  fwrite($stdout, "  php tripal_launch_job <job_id> <drupal_root_path> <username> <do parallel>\n\n");
  fwrite($stdout, "    where <username> is a Drupal user name\n\n");
}
