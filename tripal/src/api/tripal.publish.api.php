<?php

use \Drupal\tripal\Services\TripalPublish;
/**
 * @file
 * Provides an application programming interface (API) to manage
 * publishing content in Tripal.
 */

/**
 * Publish content of a specified type. Uses a Tripal service.
 *
 * @param $bundle
 *   The bundle type to be published.
 *
 * @return
 *   The number of entities published, FALSE on failure.
 */
function tripal_publish($bundle) {

  // @TODO: remove this hardcoding once the $datastore argument is working.
  $datastore = 'chado_storage';


  // Load the Publish service.
  /** @var \Drupal\tripal\Services\TripalPublish $publish */
  $publish = \Drupal::service('tripal.publish');
  $publish->init($bundle, $datastore);
  $publish->publish();
}
