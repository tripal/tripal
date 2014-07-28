<?php

/**
 * This is the script that is actually Daemonized.
 * Expected to be run via Drush tripal-jobs-daemon
 */

// Get Command-line Variables
parse_str(implode('&', array_slice($argv, 1)), $args);
$action = $argv[1];
if (!$action) {
  die('You need to specify what you want the Daemon to do. This should be one of: start, stop, restart, status, show-log');
}

require_once $args['module_path'] . '/classes/TripalJobDaemon.inc';
$Daemon = new TripalJobDaemon($args);

print "\nTripal Jobs Daemon\n".str_repeat("=",60)."\n";
print "Memory Threshold: " . ($Daemon->get_memory_threshold() * 100) . "%\n";
print "Wait Time: ". $Daemon->get_wait_time() . " seconds\n";
print "Log File: " . $Daemon->log_filename . "\n";
print "\n";

// Check that the action is valid and then execute it
// Everything else is taken case of by the object :)
$action = strtolower($action);
if (method_exists($Daemon, $action)) {
  $Daemon->{$action}();
}
else {
  die("ERROR: Unable to $action the daemon. This action is not recognized; instead try one of 'start', 'stop', 'restart', 'status' or 'log'.\n\n");
}