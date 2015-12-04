<?php
/**
 *  @file
 *  This file contains the basic functions for views integration of
 *  chado/tripal feature tables
 */

/**
 * Implements hook_views_handlers().
 *
 * Purpose: Register all custom handlers with views
 *   where a handler describes either "the type of field",
 *   "how a field should be filtered", "how a field should be sorted"
 *
 * @return: An array of handler definitions
 *
 * @ingroup tripal_feature
 */
function tripal_feature_views_handlers() {
  return array(
    'info' => array(
      'path' => drupal_get_path('module', 'tripal_feature') . '/views_handlers',
    ),
    'handlers' => array(
      'views_handler_field_residues' => array(
        'parent' => 'views_handler_field',
      ),
    ),
  );
}
