<?php

namespace Drupal\tripal\Plugin\Menu;

use Drupal\Core\Menu\LocalTaskDefault;
use Drupal\Core\Routing\RouteMatchInterface;

/**
 * Provides a destination on a menu item directing back to the calling page.
 */
class TripalDynamicDestinationMenuLink extends LocalTaskDefault {

  /**
   * {@inheritdoc}
   */
  public function getOptions(RouteMatchInterface $route_match) {
    $options = parent::getOptions($route_match);

    // Dynamically inject the active current path as the destination.
    $options['query']['destination'] = \Drupal::destination()->get();

    return $options;
  }

  /**
   * {@inheritdoc}
   *
   * Prevents the menu system from permanently caching a single page's
   *  destination.
   */
  public function getCacheMaxAge() {
    return 0;
  }

}
