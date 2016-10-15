<?php

/**
 * @file
 * Contains tripal_vocab.page.inc.
 *
 * Page callback for Controlled Vocabulary entities.
 */

use Drupal\Core\Render\Element;
use Drupal\Core\Link;
use Drupal\Core\Url;

/**
 * Prepares variables for Controlled Vocabulary templates.
 *
 * Default template: tripal_vocab.html.twig.
 *
 * @param array $variables
 *   An associative array containing:
 *   - elements: An associative array containing the user information and any
 *   - attributes: HTML attributes for the containing element.
 */
function template_preprocess_tripal_vocab(array &$variables) {
  // Fetch TripalVocab Entity Object.
  $tripal_vocab = $variables['elements']['#tripal_vocab'];

  // Helpful $content variable for templates.
  foreach (Element::children($variables['elements']) as $key) {
    $variables['content'][$key] = $variables['elements'][$key];
  }
}
