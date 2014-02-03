<?php

/**
 *  @file
 *  This file contains the basic functions for views integration of
 *  chado/tripal organism tables. Supplementary functions can be found in
 *  ./views/
 *
 *  Documentation on views integration can be found at
 *  http://views2.logrus.com/doc/html/index.html.
 */

/**
 * Implements hook_views_handlers()
 *
 * Purpose: Register all custom handlers with views
 *   where a handler describes either "the type of field",
 *   "how a field should be filtered", "how a field should be sorted"
 *
 * @return: An array of handler definitions
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
