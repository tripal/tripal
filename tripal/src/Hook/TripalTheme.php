<?php

namespace Drupal\tripal\Hook;

use Drupal\Core\Hook\Attribute\Hook;
use Drupal\Core\StringTranslation\StringTranslationTrait;

/**
 * Theme hook implementations for the Tripal module.
 */
class TripalTheme {

  use StringTranslationTrait;

  /**
   * Implements hook_theme_suggestions_HOOK().
   */
  #[Hook('theme_suggestions_tripal_entity')]
  public function themeSuggestionsTripalEntity(array $variables) {
    $suggestions = [];
    $entity = $variables['elements']['#tripal_entity'];
    $sanitized_view_mode = strtr($variables['elements']['#view_mode'], '.', '_');
    $suggestions[] = 'tripal_entity__' . $sanitized_view_mode;
    $suggestions[] = 'tripal_entity__' . $entity->bundle();
    $suggestions[] = 'tripal_entity__' . $entity->bundle() . '__' . $sanitized_view_mode;
    $suggestions[] = 'tripal_entity__' . $entity->id();
    $suggestions[] = 'tripal_entity__' . $entity->id() . '__' . $sanitized_view_mode;
    return $suggestions;
  }

  /**
   * Implements hook_theme().
   */
  #[Hook('theme')]
  public function theme() {
    $theme = [];

    $theme['tripal_entity_type'] = [
      'render element' => 'elements',
      'initial_preprocess' => static::class . ':templatePreprocessTripalEntityType',
      'template' => 'tripal_entity_type',
    ];

    $theme['tripal_entity'] = [
      'render element' => 'elements',
      'initial_preprocess' => static::class . ':templatePreprocessTripalEntity',
      'template' => 'tripal_entity',
    ];

    $theme['tripal_entity_edit_form'] = [
      'render element' => 'form',
      'template' => 'tripal-entity-edit-form',
    ];

    $theme['tripal_entity_content_add_list'] = [
      'render element' => 'types',
      'variables' => ['types' => NULL],
      'initial_preprocess' => static::class . ':templatePreprocessTripalEntity',
    ];

    return $theme;
  }

  /**
   * Prepares variables for Tripal Entity Type templates.
   *
   * Default template: tripal_entity_type.html.twig.
   *
   * @param array &$variables
   *   An associative array containing:
   *   - elements: An associative array containing the user information.
   *   - attributes: HTML attributes for the containing element.
   */
  public function templatePreprocessTripalEntityType(array &$variables): void {

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
  public function templatePreprocessTripalEntity(array &$variables) {

    // Helpful $content variable for templates.
    foreach (Element::children($variables['elements']) as $key) {
      $variables['content'][$key] = $variables['elements'][$key];
    }
  }

}
