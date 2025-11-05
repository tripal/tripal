<?php

namespace Drupal\tripal_layout\Hook;

use Drupal\Core\Hook\Attribute\Hook;
use Drupal\Core\Routing\RouteMatchInterface;
use Drupal\Core\StringTranslation\StringTranslationTrait;

/**
 * Hook implementations for the Tripal Layout module.
 */
class TripalLayoutHooks {

  use StringTranslationTrait;

  /**
   * Implements hook_help().
   */
  #[Hook('help')]
  public function help($route_name, RouteMatchInterface $route_match): string|array|null {
    switch ($route_name) {
      // Main module help for the tripal_layout module.
      case 'help.page.tripal_layout':
        $output = '<h3>' . $this->t('About') . '</h3>';
        $output .= '<p>' . $this->t('Layouts for Tripal entity types.') . '</p>';
        return ['#markup' => $output];
    }
    return NULL;
  }

}
