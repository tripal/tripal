<?php

namespace Drupal\tripal\Theme;

use Drupal\Core\Theme\ThemeHookSuggestionAltererInterface;

/**
 * Implements theme suggestions for the Tripal module.
 */
class TripalThemeSuggestions implements ThemeHookSuggestionAltererInterface {

  /**
   * {@inheritdoc}
   */
  public function alter(array &$suggestions, array $variables, $hook) {
    if ($hook === 'tripal_entity') {
      if (isset($variables['elements']['#tripal_entity'])) {
        $entity = $variables['elements']['#tripal_entity'];
        $sanitized_view_mode = strtr($variables['elements']['#view_mode'], '.', '_');

        $suggestions[] = 'tripal_entity__' . $sanitized_view_mode;
        $suggestions[] = 'tripal_entity__' . $entity->bundle();
        $suggestions[] = 'tripal_entity__' . $entity->bundle() . '__' . $sanitized_view_mode;
        $suggestions[] = 'tripal_entity__' . $entity->id();
        $suggestions[] = 'tripal_entity__' . $entity->id() . '__' . $sanitized_view_mode;
      }
    }
  }

}
