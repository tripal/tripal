<?php
/**
 *  @file
 *  This file contains the basic functions for views integration of
 *  chado/tripal phylogeny tables
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
 * @ingroup tripal_phylogeny
 */
function tripal_phylogeny_views_handlers() {
  return array(
    'info' => array(
      'path' => drupal_get_path('module', 'tripal_phylogeny') . '/views_handlers',
    ),
    'handlers' => array(
      
    ),
  );
}
