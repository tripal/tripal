<?php

use \Drupal\tripal\Services\TripalPublish;
use \Drupal\tripal\Services\TripalJob;
/**
 * @file
 * Provides an application programming interface (API) to manage
 * publishing content in Tripal.
 */

/**
 * Publish content of a specified type. Uses a Tripal service.
 *
 * @param string $bundle
 *   The entity type id (bundle) to be published.
 *
 * @param string $datastore
 *   The plugin id for the TripalStorage backend to publish from.
 *
 * @param \Drupal\tripal\Services\TripalJob $job
 *  An optional TripalJob object.
 */
function tripal_publish($bundle, $datastore, $options = [], TripalJob $job = NULL) {

  // Initialize the logger.
  /** @var \Drupal\tripal\Services\TripalLogger $logger **/
  $logger = \Drupal::service('tripal.logger');

  // Load the Publish service.
  /** @var \Drupal\tripal\Services\TripalPublish $publish */
  $publish = \Drupal::service('tripal.publish');

  try {
    $publish->init($bundle, $datastore, $options, $job);
    $publish->publish();
  }
  catch (Exception $e) {
    if ($job) {
      $logger->error($e->getMessage());
    }
  }
}
