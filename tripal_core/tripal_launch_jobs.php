<?php
//
// Copyright 2009 Clemson University
//

/* 

This script must be run at the base directory level of the drupal installation 
in order to pick up all necessary dependencies 

*/

  $stdout = fopen('php://stdout', 'w');

  // we require one command-line argument
  if(sizeof($argv) < 2){
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

  fwrite($stdout, "Tripal Job Launcher\n");
  fwrite($stdout, "-------------------\n");

  // check to make sure the username is valid
  $username = $argv[1];
  $do_parallel = $argv[2];
  if(!db_fetch_object(db_query("SELECT * FROM {users} WHERE name = '$username'"))){
     fwrite($stdout, "'$username' is not a valid Drupal username. exiting...\n");
     exit;
  }
  global $user;
  $user = user_load(array('name' => $username));

  tripal_jobs_launch($do_parallel);

/**
 *
 *
 * @ingroup tripal_core
 */
function print_usage ($stdout){
  fwrite($stdout,"Usage:\n");
  fwrite($stdout,"  php ./sites/all/modules/tripal_core/tripal_launch_jobs <username> \n\n");
  fwrite($stdout,"    where <username> is a Drupal user name\n\n");
}

?>
