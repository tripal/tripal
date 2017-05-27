# Tripal Daemon

This module is meant to provide a simple means of creating a robust
command-line-driven, fully bootstrapped PHP Daemon. It uses the PHP-Daemon
(https://github.com/shaneharter/PHP-Daemon) Library to create the Daemon (via
the Libraries API) in order to not re-invent the wheel ;-).


## FEATURES
* Provides a Drush interface to start/stop your Daemon.
* Your daemon starts in the background and is detached from the current
   terminal.
* Daemon will run all Tripal Jobs submitted within 20 seconds.
* A log including the number of jobs executed, their identifiers and results.
* Lock Files, Automatic restart (8hrs default) and Built-in Signal Handling &
   Event Logging are only a few of the features provided by the Daemon API
   making this a fully featured & robust Daemon.

## REQUIREMENTS
* Libraries API (https://www.drupal.org/project/libraries)
* PHP-Daemon Library version 2.0 (https://github.com/shaneharter/PHP-Daemon)
* Drush 5.x (https://github.com/drush-ops/drush)
* Drush Daemon API (https://www.drupal.org/project/drushd)

## INSTALLATION
* Install all required modules as per their instructions.
* Install this module as you would normally install a contributed drupal
   module. See:https://drupal.org/documentation/install/modules-themes/modules-7
   for further information.
* Download the PHP-Daemon Library and extract it in your sites/all/libraries
   directory. The folder must be named "PHP-Daemon".

## TRIPAL DAEMON USAGE
* Start Daemon
    drush trpjob-daemon start
* Stop Daemon
    drush trpjob-daemon stop
* Check the Status
    drush trpjob-daemon status
* Show the Log
   * List the last 10 lines of the log file:
      drush trpjob-daemon show-log
   * List the last N lines of the log file:
      drush trpjob-daemon show-log --num_lines=N
