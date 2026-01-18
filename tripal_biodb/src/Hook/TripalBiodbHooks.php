<?php

namespace Drupal\tripal_biodb\Hook;

use Drupal\Core\Hook\Attribute\Hook;
use Drupal\Core\Routing\RouteMatchInterface;
use Drupal\Core\StringTranslation\StringTranslationTrait;

/**
 * Hook implementations for the Tripal Biodb module.
 */
class TripalBiodbHooks {

  use StringTranslationTrait;

  /**
   * Implements hook_help().
   */
  #[Hook('help')]
  public function help($route_name, RouteMatchInterface $route_match): string|array|null {
    switch ($route_name) {
      // Main module help for the tripal_biodb module.
      case 'help.page.tripal_biodb':
        $output = '<h3>' . $this->t('About') . '</h3>';
        $output .= '<p>' . $this->t('Biological database abstraction layer for Tripal.') . '</p>';
        return ['#markup' => $output];
    }
    return NULL;
  }

}
