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
function template_preprocess_tripal_term(array &$variables) {

  // Fetch TripalEntity Entity Object.
  $tripal_term = $variables['elements']['#tripal_term'];
  $tripal_idspace = $tripal_term->getIDSpace();
  $tripal_vocab = $tripal_idspace->getVocab();
  $accessory_vocabs = $tripal_term->getVocab();

  // Add the main properties into the content variable.
  $variables['content']['name'] = [
    '#markup' => ($tripal_term) ? $tripal_term->getName() : '',
  ];
  $variables['content']['accession'] = [
    '#markup' => ($tripal_term) ? $tripal_term->getAccession() : '',
  ];
  $variables['content']['definition'] = [
    '#markup' => ($tripal_term) ? $tripal_term->getDefinition() : '',
  ];
  $variables['content']['idspace'] = [
    '#markup' => ($tripal_idspace) ? $tripal_idspace->getIDSpace() : '',
  ];
  $variables['content']['vocab']['name'] = [
    '#markup' => ($tripal_vocab) ? $tripal_vocab->getName() : '',
  ];
  $variables['content']['vocab']['namespace'] = [
    '#markup' => ($tripal_vocab) ? $tripal_vocab->getNamespace() : '',
  ];
  $variables['content']['vocab']['route'] = 'entity.tripal_vocab.canonical';
  $variables['content']['vocab']['id'] = $tripal_vocab->id();

  if (sizeof($accessory_vocabs) > 1) {
    $variables['content']['allvocabs'] = [];
    foreach ($accessory_vocabs as $vocab) {
      if ($vocab->id() != $tripal_vocab->id()) {
        $variables['content']['allvocabs'][] = [
          'name' => [ '#markup' => $vocab->getName() ],
          'namespace' => [ '#markup' => $vocab->getNamespace() ],
          'route' => 'entity.tripal_vocab.canonical',
          'id' => $vocab->id(),
        ];
      }
    }
  }

  // Helpful $content variable for templates.
  foreach (Element::children($variables['elements']) as $key) {
    $variables['content'][$key] = $variables['elements'][$key];
  }
}
