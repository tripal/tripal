<?php

/**
 * @file
 * This script can be used to launch jobs on a multi-site Drupal installation
 */

include_once './includes/bootstrap.inc';

fwrite(STDOUT, "Running Tripal Job Launcher\n");

/**
 * SETTINGS
 */

//the location of the 'sites' directory relative to this script.
$sites_dir = 'sites';
$debug=0;

/**
 * END SETTINGS
 */

//error_reporting(E_ALL);

include ("Console/Getopt.php");

// initialize object
$cg = new Console_Getopt();

/* define list of allowed options - p = h:sitename, u:username  */
$allowed_short_options = "h:u:";

// read the command line
$args = $cg->readPHPArgv();

// get the options
$ret = $cg->getopt($args, $allowed_short_options);

// check for errors and die with an error message if there was a problem
if (PEAR::isError($ret)) {
    die("Error in command line: " . $ret->getMessage() . "\n");
}

ini_set('include_path', ini_get('include_path') . PATH_SEPARATOR . './scripts');

/*
 * This doesn't work in every case: getopt function is not always available
  $options = getopt("h:r:");
  var_dump($options);
 */

$hostname = "";
$username = "";

// parse the options array
$opts = $ret[0];
if (sizeof($opts) > 0) {
    // if at least one option is present
    foreach ($opts as $opt) {
        switch ($opt[0]) {
            case 'h':
                $hostname = $opt[1];
                break;
            case 'u':
                $username = $opt[1];
                break;
            default:
                fwrite(STDOUT, 'Usage: \n');
                fwrite(STDOUT, '- h hostname\n');
                fwrite(STDOUT, " -u username\n");
                break;

        }
    }
}
else {
  fwrite(STDOUT, "Usage: \n");
  fwrite(STDOUT, " -h hostname\n");
  fwrite(STDOUT, " -u username\n");

}

runjob($hostname, $username);

/**
 * Runs tripal_launch_jobs() as the specified user
 *
 * @ingroup tripal_core
 */
function runjob($sitename, $username) {
  global $user;

  $_SERVER['SCRIPT_NAME'] = '/sites/all/modules/tripal_jobs/tripal_launch_jobs_multi.php';
  $_SERVER['SCRIPT_FILENAME'] = '/sites/all/modules/tripal_jobs/tripal_launch_jobs_multi.php';
  $_SERVER['HTTP_HOST'] = $sitename;
  $_SERVER['REMOTE_ADDR'] = 'localhost';
  $_SERVER['REQUEST_METHOD'] = 'GET';

  drupal_bootstrap(DRUPAL_BOOTSTRAP_FULL);

  // check to make sure the username is valid
  $results = db_query("SELECT * FROM {users} WHERE name = :name", array(':name' => $username));
  $u = $results->fetchObject();
  if (!$u) {
    fwrite($stdout, "'$username' is not a valid Drupal username. exiting...\n");
    exit;
  }

  global $user;
  $user = user_load($u->uid);

  tripal_launch_job();
}
