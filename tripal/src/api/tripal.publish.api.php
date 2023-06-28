<?php
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

  // Load the Publish service.
  $publish = \Drupal::service('tripal.publish');

  $publish->publish($bundle, 'datastore_ph', ['placeholder']);
}
