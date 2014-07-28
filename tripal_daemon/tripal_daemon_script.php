<?php

require_once 'classes/TripalDaemon.inc';

/**
 * This is the script that is actually Daemonized.
 *
 * Arguments expected to be passed to this script:
 *  - action: One of 'start','stop','restart',status','show-log'. Meant to indicate what
 *       you want the daemon to do.
 *  - log_file: the full path & filename of the log file. If it doesn't exist this script will
 *       create it.
 */

// Get Command-line Variables
parse_str(implode('&', array_slice($argv, 1)), $args);
$action = $argv[1];
if (!$action) {
  die('You need to specify what you want the Daemon to do. This should be one of: start, stop, restart, status, show-log');
}

$Daemon = new TripalJobDaemon($args);

print "\nTripal Jobs Daemon\n".str_repeat("=",60)."\n";
print "Memory Threshold: " . ($Daemon->get_memory_threshold() * 100) . "%\n";
print "Wait Time: ". $Daemon->get_wait_time() . " seconds\n";
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