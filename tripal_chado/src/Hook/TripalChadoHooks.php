<?php

namespace Drupal\tripal_chado\Hook;

use Drupal\Core\Hook\Attribute\Hook;
use Drupal\Core\Routing\RouteMatchInterface;
use Drupal\Core\StringTranslation\StringTranslationTrait;

/**
 * Hook implementations for the Tripal Chado module.
 */
class TripalChadoHooks {

  use StringTranslationTrait;

  /**
   * Implements hook_help().
   */
  #[Hook('help')]
  public function help($route_name, RouteMatchInterface $route_match): string|array|null {
    switch ($route_name) {
      // Main module help for the tripal_chado module.
      case 'help.page.tripal_chado':
        $output = '<h3>' . $this->t('About') . '</h3>';
        $output .= '<p>' . $this->t('Chado integration for Tripal.') . '</p>';
        return ['#markup' => $output];
    }
    return NULL;
  }

  /**
   * Implements hook_rebuild().
   */
  #[Hook('rebuild')]
  public function onRebuild(): void {
    $rebuild_service = \Drupal::service('tripal_chado.rebuild_service');
    $rebuild_service->executeRebuild();
  }

}
