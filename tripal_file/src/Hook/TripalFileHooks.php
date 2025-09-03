<?php

namespace Drupal\tripal_file\Hook;

use Drupal\Core\Hook\Attribute\Hook;
use Drupal\Core\Routing\RouteMatchInterface;
use Drupal\Core\StringTranslation\StringTranslationTrait;

/**
 * Hook implementations for the TripalFile module.
 */
class TripalFileHooks {

  use StringTranslationTrait;

  /**
   * Implements hook_help().
   */
  #[Hook('help')]
  public function help($route_name, RouteMatchInterface $route_match) {
    switch ($route_name) {
      // Main module help for the tripal_file module.
      case 'help.page.tripal_file':
        $output = '<h3>' . $this->t('About') . '</h3>';
        $output .= '<p>' . $this->t('A module for associating files with Tripal content and for accessing files via web services.') . '</p>';
        return $output;

      default:
    }
  }

}
