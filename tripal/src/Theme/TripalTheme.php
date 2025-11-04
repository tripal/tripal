<?php

namespace Drupal\tripal\Theme;

use Drupal\Core\Theme\ThemeExtensionInterface;

/**
 * Provides theme hooks for the Tripal module.
 */
class TripalTheme implements ThemeExtensionInterface {

  /**
   * {@inheritdoc}
   */
  public function getThemeHooks() {
    $path = drupal_get_path('module', 'tripal') . '/templates';

    return [
      'tripal_entity_type' => [
        'render element' => 'elements',
        'template' => 'tripal_entity_type',
        'path' => $path,
      ],
      'tripal_entity' => [
        'render element' => 'elements',
        'template' => 'tripal_entity',
        'path' => $path,
      ],
      'tripal_entity_edit_form' => [
        'render element' => 'form',
        'template' => 'tripal-entity-edit-form',
        'path' => $path,
      ],
      'tripal_entity_content_add_list' => [
        'render element' => 'types',
        'variables' => ['types' => NULL],
        'template' => 'tripal_entity',
        'path' => $path,
      ],
    ];
  }

}
