<?php

/**
 * @file
 * Contains tripal_entity_type.page.php.
 *
 * Page callback for Tripal Entity Types (bundles).
 */

use Drupal\Core\Render\Element;
use Drupal\Core\Link;
use Drupal\Core\Url;

/**
 * Prepares variables for Tripal Entity Type templates.
 *
 * Default template: tripal_entity_type.html.twig.
 *
 * @param array $variables
 *   An associative array containing:
 *   - elements: An associative array containing the user information and any
 *   - attributes: HTML attributes for the containing element.
 */
function template_preprocess_tripal_entity_type(array &$variables) {

  // Fetch TripalEntity Entity Object.
  $tripal_entity_type = $variables['elements']['#tripal_entity_type'];

  // Helpful $content variable for templates.
  foreach (Element::children($variables['elements']) as $key) {
    $variables['content'][$key] = $variables['elements'][$key];
  }

  // See issue #2123 for the history of this
  $variables['content']['warning'] = [
    '#markup' => t('This tripal entity type page is a placeholder and is not currently implemented'),
  ];
}
