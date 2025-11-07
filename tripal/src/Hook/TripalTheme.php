<?php

namespace Drupal\tripal\Hook;

use Symfony\Component\HttpKernel\Exception\AccessDeniedHttpException;
use Drupal\Core\Form\FormStateInterface;
use Drupal\Core\Hook\Attribute\Hook;
use Drupal\Core\Render\Markup;
use Drupal\Core\Routing\RouteMatchInterface;
use Drupal\Core\StringTranslation\StringTranslationTrait;
use Drupal\Core\Url;
use Drupal\tripal\TripalField\TripalFieldItemBase;
use Drupal\views\ViewEntityInterface;
use Drupal\views\ViewExecutable;
use Drupal\views\ViewsConfigUpdater;

/**
 * Hook implementations for the Tripal module.
 */
class TripalHooks {

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
  public function tripalTheme() {
    $theme = [];

    $theme['tripal_entity_type'] = [
      'render element' => 'elements',
      'file' => 'templates/tripal_entity_type.page.php',
      'template' => 'tripal_entity_type',
    ];

    $theme['tripal_entity'] = [
      'render element' => 'elements',
      'file' => 'templates/tripal_entity.page.php',
      'template' => 'tripal_entity',
    ];

    $theme['tripal_entity_edit_form'] = [
      'render element' => 'form',
      'template' => 'tripal-entity-edit-form',
    ];

    $theme['tripal_entity_content_add_list'] = [
      'render element' => 'types',
      'variables' => ['types' => NULL],
      'file' => 'templates/tripal_entity.page.php',
    ];

    return $theme;
  }

}
