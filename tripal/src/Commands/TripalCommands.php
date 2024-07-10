<?php

namespace Drupal\tripal\Commands;

use Drush\Commands\DrushCommands;
use Drush\Drush;

/**
 * Drush commands
 */
class TripalCommands extends DrushCommands {

  /**
   * Makes sure the proper user is set when running the drush command.
   */
  protected function switchUser($uname) {
    if (!$uname) {
      throw new \Exception(dt('The --username argument is required.'));
    }

    $user = user_load_by_name($uname);
    if (!$user) {
      throw new \Exception(dt('The --username argument does not specify a valid user.'));
    }
    \Drupal::service('account_switcher')->switchTo($user);
  }

  /**
   * Executes one or more jobs in the Tripal Jobs Queue.
   *
   * @command tripal:trp-run-jobs
   * @aliases trp-run-jobs
   * @options parallel
   *   Set to 1 if the job is allowed to run in parallel with other Tripal jobs.
   * @options job_id
   *   The numeric ID of the job. If no job ID is provided then all of the
   *   jobs waiting in the queue will be run.
   * @options max_jobs
   *   The maximum number of jobs that should be run concurrently. If -1 then
   *   unlimited.
   * @options single
   *   Ensures only a single job is run rather then the entire queue.
   * @options username
   *   The name of the user for whom the job run is associated.
   * @usage drush trp-run-jobs --username=[USERNAME]
   *   Executes all jobs waiting in the queue and associates the runs with
   *   the provided user.
   * @usage drush trp-run-jobs --job_id=[JOB_ID] --username=[USERNAME]
   *   Executes a job, using the provided job ID and associates the run with
   *   the provided user.
   */
  public function runJobs($options = ['username' => NULL, 'job_id' => NULL,
    'parallel' => FALSE, 'max_jobs' => -1, 'single' => 0]) {

    $parallel = $options['parallel'];
    $job_id = $options['job_id'];
    $max_jobs = $options['max_jobs'];
    $single = $options['single'];
    $uname = $options['username'];

    $this->switchUser($uname);

    $this->output()->writeln("\n" . date('Y-m-d H:i:s'));
    if ($parallel) {
      $this->output()->writeln("Tripal Job Launcher (in parallel)");
      if ($max_jobs !== -1) {
        $this->output()->writeln("Maximum number of jobs is " . $max_jobs);
      }
      $this->output()->writeln("Running as user '$uname'");
      $this->output()->writeln("-------------------");
      tripal_launch_job($parallel, $job_id, $max_jobs, $single);
    }
    else {
      $this->output()->writeln("Tripal Job Launcher");
      $this->output()->writeln("Running as user '$uname'");
      $this->output()->writeln("-------------------");
      tripal_launch_job(0, $job_id, $max_jobs, $single);
    }
  }

  /**
   * Reruns a jobs in the Tripal Jobs Queue.
   *
   * @command tripal:trp-rerun-job
   * @aliases trp-rerun-job
   * @options parallel
   *   Set to 1 if the job is allowed to run in parallel with other Tripal jobs.
   * @options job_id
   *   The numeric ID of the job. If no job ID is provided then all of the
   *   jobs waiting in the queue will be run.
   * @options max_jobs
   *   The maximum number of jobs that should be run concurrently. If -1 then
   *   unlimited.
   * @options single
   *   Ensures only a single job is run rather then the entire queue.
   * @options username
   *   The name of the user for whom the job run is associated.
   * @usage drush trp-run-job --job_id=[JOB_ID] --username=[USERNAME]
   *   Re-runs a job by first resubmitting it then executing it.
   */
  public function rerunJob($options = ['username' => NULL, 'job_id' => NULL,
    'parallel' => FALSE, 'max_jobs' => -1, 'single' => 0]) {

    $parallel = $options['parallel'];
    $job_id = $options['job_id'];
    $max_jobs = $options['max_jobs'];
    $single = $options['single'];
    $uname = $options['username'];

    if (!$job_id) {
      throw new \Exception(dt('The --job_id argument is required.'));
    }

    $this->switchUser($uname);

    $new_job_id = tripal_rerun_job($job_id, FALSE);

    $this->output()->writeln("\n" . date('Y-m-d H:i:s'));
    if ($parallel) {
      $this->output()->writeln("Tripal Job Launcher (in parallel)");
      $this->output()->writeln("Running as user '$username'");
      $this->output()->writeln("-------------------");
      tripal_launch_job($parallel, $new_job_id, $max_jobs, $single);
    }
    else {
      $this->output()->writeln("Tripal Job Launcher");
      $this->output()->writeln("Running as user '$username'");
      $this->output()->writeln("-------------------");
      tripal_launch_job(0, $new_job_id, $max_jobs, $single);
    }
  }

  /**
   * Returns the current version of Tripal that is installed
   *
   * @command tripal:version
   * @aliases trp-version
   * @usage drush trp-version
   *   Returns the current Tripal version string.
   */
  public function tripalVersion() {
    $this->output()->writeln(tripal_version());
  }

  /**
   * Imports a collection of Tripal Content Types and associated fields
   * for a specific collection id.
   *
   * @command tripal:trp-import-types
   * @aliases trp-import-types
   * @options collection_id
   *   The id specified in the YAML file for the particular TripalEntityType-Collection
   *   you would like to import. Note: fields will also be added automatically if the
   *   TripalField-Collection YAML file has the same id.
   * @options username
   *   The name of the user for whom the content types created are associated.
   * @usage drush trp-import-types --username=[USERNAME] --collection_id=genomic_chado
   *   Runs a job importing the genomic content types focused on a Chado backend.
   */
  public function tripalImportContentTypes($options = ['username' => NULL, 'collection_id' => NULL]) {

    if (!$options['username']) {
      throw new \Exception(dt('The --username argument is required.'));
    }
    if (!$options['collection_id']) {
      throw new \Exception(dt('The --collection_id argument is required.'));
    }

    $content_type_setup = \Drupal::service('tripal.tripalentitytype_collection');


    // Check that the id supplied is valid.
    $collections = $content_type_setup->getTypeCollections();
    if (!array_key_exists($options['collection_id'], $collections)) {
      Drush::logger()->notice('The following are the found collection ids:');
      foreach($collections as $id => $details) {
        Drush::logger()->notice('  - ' . $id . ' (' . $details['description'] . ')');
      }
      throw new \Exception(dt('The collection ID you provided was not valid. Please try again with one of the above listed ids (e.g. general_chado).'));
    }

    $chosen_collection_ids = [ $options['collection_id'] ];

    // Import the content types
    $content_type_setup->install($chosen_collection_ids);

    // Import the fields.
    $fields = \Drupal::service('tripal.tripalfield_collection');
    $fields->install($chosen_collection_ids);

  }
}
