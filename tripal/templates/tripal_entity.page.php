<?php

/**
 * @file
 * Contains tripal_entity.page.php.
 *
 * Page callback for Tripal Content entities.
 */

use Drupal\Core\Render\Element;
use Drupal\Core\Link;
use Drupal\Core\Url;

/**
 * Prepares variables for Tripal Content templates.
 *
 * Default template: tripal_entity.html.twig.
 *
 * @param array $variables
 *   An associative array containing:
 *   - elements: An associative array containing the user information and any
 *   - attributes: HTML attributes for the containing element.
 */
function template_preprocess_tripal_entity(array &$variables) {

  // Fetch TripalEntity Entity Object.
  $tripal_entity = $variables['elements']['#tripal_entity'];

  // Helpful $content variable for templates.
  foreach (Element::children($variables['elements']) as $key) {
    $variables['content'][$key] = $variables['elements'][$key];
  }
}
