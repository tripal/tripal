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
if (sizeof($argv) < 2) {
  print_usage($stdout);
  exit;
}

$drupal_base_url = parse_url('http://www.example.com');
$_SERVER['HTTP_HOST'] = $drupal_base_url['host'];
//  $_SERVER['PHP_SELF'] = $drupal_base_url['path'].'/index.php';
$_SERVER['REQUEST_URI'] = $_SERVER['SCRIPT_NAME'] = $_SERVER['PHP_SELF'];
$_SERVER['REMOTE_ADDR'] = NULL;
$_SERVER['REQUEST_METHOD'] = NULL;

require_once 'includes/bootstrap.inc';
drupal_bootstrap(DRUPAL_BOOTSTRAP_FULL);

// check to make sure the username is valid
$username = $argv[1];
$do_parallel = $argv[2];
$results = db_query("SELECT * FROM {users} WHERE name = :name", array(':name' => $username));
$u = $results->fetchObject();
if (!$u) {
  fwrite($stdout, "'$username' is not a valid Drupal username. exiting...\n");
  exit;
}

global $user;
$user = user_load($u->uid);


fwrite($stdout, "Tripal Job Launcher\n");
fwrite($stdout, "Running as user ' . $username . '\n");
fwrite($stdout, "-------------------\n");

tripal_launch_job($do_parallel);

/**
 * Print out the usage instructions if they are not followed correctly
 *
 * @ingroup tripal_core
 */
function print_usage($stdout) {
  fwrite($stdout, "Usage:\n");
  fwrite($stdout, "  php ./sites/all/modules/tripal_core/tripal_launch_jobs <username> \n\n");
  fwrite($stdout, "    where <username> is a Drupal user name\n\n");
}
