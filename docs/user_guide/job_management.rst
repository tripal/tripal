Job Management (Tripal Daemon)
==============================

.. note::

  Remember you must set the $DRUPAL_HOME environment variable to cut-and-paste the commands below. See see :doc:`./install_tripal/drupal_home`


The Tripal Daemon module is meant to provide a simple means of creating a robust command-line-driven, fully bootstrapped PHP Daemon. It uses the PHP-Daemon (https://github.com/shaneharter/PHP-Daemon) Library to create the Daemon (via the Libraries API) in order to not re-invent the wheel. It allows you to execute Jobs submitted to Tripal without using cron.  It provides a faster user experience for running jobs.  Prior to Tripal v3, the Tripal Daemon module was an extension module. It was integrated into the core Tripal package.

Features
--------

* Provides a Drush interface to start/stop your Daemon.
* Your daemon starts in the background and is detached from the current terminal.
* Daemon will run all Tripal Jobs submitted within 20 seconds.
* A log including the number of jobs executed, their identifiers and results.
* Lock Files, Automatic restart (8hrs default) and Built-in Signal Handling & Event Logging are only a few of the features provided by the Daemon API making this a fully featured & robust Daemon.


Installation
------------

The Tripal Daemon requires the `Libraries API <https://www.drupal.org/project/libraries>`_ module.  You can easily download and install this module using the following drush commands:

.. code-block:: shell

  drush pm-download libraries
  drush pm-enable libraries

Next, we need the `PHP-Daemon Library version 2.0 <https://github.com/shaneharter/PHP-Daemon>`_. You must download the PHP-Daemon Library and extract it in your ``sites/all/libraries`` directory. The folder must be named "PHP-Daemon".  The following commands can be used to do this:

.. code-block:: shell

  cd $DRUPAL_HOME/sites/all/libraries
  wget https://github.com/shaneharter/PHP-Daemon/archive/v2.0.tar.gz
  tar -zxvf v2.0.tar.gz
  mv PHP-Daemon-2.0 PHP-Daemon

Next, install the `Drush Daemon API <https://www.drupal.org/project/drushd>`_ module.

.. code-block:: shell

  drush pm-download drushd
  drush pm-enable drushd

Finally, enable the Tripal Daemon module. This module comes with Tripal v3.

.. code-block:: shell

  drush pm-enable tripal_daemon

Usage
-----

Start the Daemon

.. code-block:: shell

  drush trpjob-daemon start

Stop the Daemon

.. code-block:: shell

  drush trpjob-daemon stop

Check the status

.. code-block:: shell

  drush trpjob-daemon status

List the last 10 lines of the log file:

.. code-block:: shell

  drush trpjob-daemon show-log

List the last N lines of the log file:

.. code-block:: shell

  drush trpjob-daemon show-log --num_lines=N

Set N to the number of lines you want to view.
