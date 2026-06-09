<?php

namespace Drupal\tripal_search_views\Hook;

use Drupal\Core\Hook\Attribute\Hook;
use Drupal\Core\Routing\RouteMatchInterface;
use Drupal\Core\StringTranslation\StringTranslationTrait;
use Drupal\tripal_search_views\Plugin\views\filter\InformedStringFilter;

/**
 * Hook implementations for the Tripal Views module.
 */
class TripalSearchViewsHooks {

  use StringTranslationTrait;

  /**
   * Implements hook_help().
   */
  #[Hook('help')]
  public function help($route_name, RouteMatchInterface $route_match): string|array|null {
    switch ($route_name) {
      // Main module help for the tripal_search_views module.
      case 'help.page.tripal_search_views':
        $output = '<h3>' . $this->t('About') . '</h3>';
        $output .= '<p>' . $this->t('Custom Views for Tripal.') . '</p>';
        return ['#markup' => $output];
    }
    return NULL;
  }

  /**
   * Implements hook_views_plugins_filter_alter().
   */
  #[Hook('views_plugins_filter_alter')]
  public function viewsPluginsFilterAlter(array &$plugins): void {
    // Override the default StringFilter class with our own.
    // @todo can we restrict this override to only apply to Tripal views?
    if (isset($plugins['string'])) {
      $plugins['string']['class'] = InformedStringFilter::class;
    }
  }

}
