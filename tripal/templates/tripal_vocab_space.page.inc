<?php

/**
 * @file
 * Contains tripal_vocab_space.page.inc.
 *
 * Page callback for Tripal Vocabulary IDSpace entities.
 */

use Drupal\Core\Render\Element;

/**
 * Prepares variables for Tripal Vocabulary IDSpace templates.
 *
 * Default template: tripal_vocab_space.html.twig.
 *
 * @param array $variables
 *   An associative array containing:
 *   - elements: An associative array containing the user information and any
 *   - attributes: HTML attributes for the containing element.
 */
function template_preprocess_tripal_vocab_space(array &$variables) {
  // Fetch TripalVocabSpace Entity Object.
  $tripal_vocab_space = $variables['elements']['#tripal_vocab_space'];
  $tripal_vocab = $tripal_vocab_space->getVocab();

  // Add the main properties into the content variable.
  $variables['content']['IDSpace'] = [
    '#markup' => ($tripal_vocab_space) ? $tripal_vocab_space->getIDSpace() : '',
  ];
  $variables['content']['URLPrefix'] = [
    '#markup' => ($tripal_vocab_space) ? $tripal_vocab_space->getURLPrefix() : '',
  ];
  $variables['content']['vocab']['name'] = [
    '#markup' => ($tripal_vocab_space) ? $tripal_vocab->getName() : '',
  ];
  $variables['content']['vocab']['namespace'] = [
    '#markup' => ($tripal_vocab_space) ? $tripal_vocab->getNamespace() : '',
  ];

  // Helpful $content variable for templates.
  foreach (Element::children($variables['elements']) as $key) {
    $variables['content'][$key] = $variables['elements'][$key];
  }
}
