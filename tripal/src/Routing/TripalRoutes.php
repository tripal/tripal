<?php

namespace Drupal\tripal\Routing;

use Symfony\Component\Routing\Route;

/**
 * Defines dynamic routes.
 */
class TripalRoutes {
  /**
   * {@inheritdoc}
   */
  public function importerRoutes() {
    $routes = [];

    // Add routes for the TripalImporter Plugins.
    $importer_manager = \Drupal::service('tripal.importer');
    $importer_defs = $importer_manager->getDefinitions();
    foreach ($importer_defs as $plugin_id => $def) {
      $menu_path = array_key_exists('menu_path', $def) ? $def['menu_path'] : 'admin/tripal/loaders/' . $plugin_id . '_form';
      $defaults = [
        '_title' => $def['label']->getUntranslatedString()
      ];
      $requirements  = [
        '_permission' => 'use ' . $plugin_id . ' importer'
      ];
      $options = [];
      $routes[$menu_path] = new Route($menu_path, $defaults, $requirements, $options);
    }
    return $routes;
  }
}
