<?php
/**
 *  @file
 *  This file contains the basic functions for views integration of
 *  chado/tripal feature tables
 */

/**
 * Implements hook_views_api().
 */
function tripal_entities_views_api() {
  return array(
    'api' => 3,
    'path' => drupal_get_path('module', 'tripal_entities') . '/views',
  );
}
