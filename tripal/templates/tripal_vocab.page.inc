<?php

/**
 * @file
 * Contains tripal_vocab.page.inc.
 *
 * Page callback for Tripal Content entities.
 */

use Drupal\Core\Render\Element;
use Drupal\Core\Link;
use Drupal\Core\Url;

/**
 * Prepares variables for Tripal Content templates.
 *
 * Default template: tripal_vocab.html.twig.
 *
 * @param array $variables
 *   An associative array containing:
 *   - elements: An associative array containing the user information and any
 *   - attributes: HTML attributes for the containing element.
 */
function template_preprocess_tripal_vocab(array &$variables) {

  // Fetch TripalEntity Entity Object.
  $tripal_vocab = $variables['elements']['#tripal_vocab'];

  // Add the main properties into the content variable.
  $variables['content']['name'] = [
    '#markup' => ($tripal_vocab) ? $tripal_vocab->getName() : '',
  ];
  $variables['content']['namespace'] = [
    '#markup' => ($tripal_vocab) ? $tripal_vocab->getNamespace() : '',
  ];
  $variables['content']['url'] = [
    '#markup' => ($tripal_vocab) ? $tripal_vocab->getURL() : '',
  ];
  $variables['content']['description'] = [
    '#markup' => ($tripal_vocab) ? $tripal_vocab->getDescription(): '',
  ];

  // Helpful $content variable for templates.
  foreach (Element::children($variables['elements']) as $key) {
    $variables['content'][$key] = $variables['elements'][$key];
  }
}
