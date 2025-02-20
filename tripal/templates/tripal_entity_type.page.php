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

  // Fetch TripalEntityType Object.
  $tripal_entity_type = $variables['elements']['#tripal_entity_type'];

  // Take information from the entity and add it to the content.
  $variables['content']['label'] = [
    '#type' => 'item',
    '#title' => 'Label',
    '#markup' => $tripal_entity_type->getLabel(),
    '#wrapper_attributes' => [
      'class' => ['container-inline'],
    ],
  ];
  $variables['content']['term'] = [
    '#type' => 'item',
    '#title' => 'Term',
    '#markup' => $tripal_entity_type->getTermIdSpace() . ':' . $tripal_entity_type->getTermAccession(),
    '#wrapper_attributes' => [
      'class' => ['container-inline'],
    ],
  ];
  $variables['content']['category'] = [
    '#type' => 'item',
    '#title' => 'Category',
    '#markup' => $tripal_entity_type->getCategory(),
    '#wrapper_attributes' => [
      'class' => ['container-inline'],
    ],
  ];
  $variables['content']['description'] = [
    '#type' => 'item',
    '#title' => 'Help Text for Curators',
    '#markup' => $tripal_entity_type->getHelpText(),
  ];

  // Helpful $content variable for templates.
  // Only adds fields which TripalEntityType may not have.
  foreach (Element::children($variables['elements']) as $key) {
    $variables['content'][$key] = $variables['elements'][$key];
  }

}
