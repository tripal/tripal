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
  public function dataLoaders() {
    $routes = [];
    $default_module_path = $file_path = \Drupal::service('extension.list.module')->getPath('tripal');

    $importers = \tripal_get_importers();
    foreach ($importers as $class_name) {
      tripal_load_include_importer_class($class_name);
      if (class_exists($class_name)) {
        $machine_name = $class_name::$machine_name;
        $name = $class_name::$name;
        $menu_path = 'admin/tripal/loaders/' . $machine_name;
        $callback = $class_name::$callback;
        $callback_path = $class_name::$callback_path;
        $callback_module = $class_name::$callback_module;
        $page_args = [];
        if ($class_name::$menu_path) {
          $menu_path = $class_name::$menu_path;
        }
        if (!$callback) {
          $callback = 'drupal_get_form';
          $page_args = ['tripal_get_importer_form', $class_name];
        }
        if (!$callback_path) {
          $callback_path = 'includes/tripal.importer.inc';
        }
        $file_path = $default_module_path;
        if ($callback_path and $callback_module) {
          $file_path = \Drupal::service('extension.list.module')->getPath($callback_module);
        }

        $routes[$menu_path] = new Route(
          // Path to attach this route to:
          $menu_path,
          // Route defaults:
          [
            '_form' => '\Drupal\tripal\Form\TripalImporterForm',
            '_title' => $class_name::$name
          ],
          // Route requirements:
          [
            '_permission' => 'allow tripal importer ' . $machine_name,
          ]
        );
      }
    }
    return $routes;
  }


}
