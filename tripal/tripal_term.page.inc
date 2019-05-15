<?php

/**
 * @file
 * Contains tripal_term.page.inc.
 *
 * Page callback for Controlled Vocabulary Term entities.
 */

use Drupal\Core\Render\Element;
use Drupal\Core\Link;
use Drupal\Core\Url;

/**
 * Prepares variables for Controlled Vocabulary Term templates.
 *
 * Default template: tripal_term.html.twig.
 *
 * @param array $variables
 *   An associative array containing:
 *   - elements: An associative array containing the user information and any
 *   - attributes: HTML attributes for the containing element.
 */
function template_preprocess_tripal_term(array &$variables) {
  // Fetch TripalTerm Entity Object.
  $tripal_term = $variables['elements']['#tripal_term'];

  // Helpful $content variable for templates.
  foreach (Element::children($variables['elements']) as $key) {
    $variables['content'][$key] = $variables['elements'][$key];
  }
}
