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
    // @TODO This needs to be built out for all the loaders, this is just an
    // example on how to start.
    // Add in the loaders
    // $importers = tripal_get_importers();
    // foreach ($importers as $class_name) {
    //   tripal_load_include_importer_class($class_name);
    //   if (class_exists($class_name)) {
    //     $machine_name = $class_name::$machine_name;
    //     $menu_path = 'admin/tripal/loaders/' . $machine_name;
    //     $callback = $class_name::$callback;
    //     $callback_path = $class_name::$callback_path;
    //     $callback_module = $class_name::$callback_module;
    //     $page_args = [];
    //     if ($class_name::$menu_path) {
    //       $menu_path = $class_name::$menu_path;
    //     }
    //     if (!$callback) {
    //       $callback = 'drupal_get_form';
    //       $page_args = ['tripal_get_importer_form', $class_name];
    //     }
    //     if (!$callback_path) {
    //       $callback_path = 'includes/tripal.importer.inc';
    //     }
    //     $file_path = \Drupal::service('extension.list.module')->getPath('tripal');
    //     if ($callback_path and $callback_module) {
    //       $file_path = \Drupal::service('extension.list.module')->getPath($callback_module);
    //     }


    //     $routes[$menu_path] = new Route(
    //           // Path to attach this route to:
    //           $menu_path,
    //           // Route defaults:
    //           [
    //             '_controller' => '\Drupal\example\Controller\TripalController::'. $callback,
    //             '_title' => $class_name::$name
    //           ],
    //           // Route requirements:
    //           [
    //             '_permission'  => array('use ' . $machine_name . ' importer'),
    //           ]
    //         );
    //         // $items[$menu_path] = array(
    //         //   'title' => $class_name::$name,
    //         //   'description' =>  $class_name::$description,
    //         //   'page callback' => $callback,
    //         //   'page arguments' => $page_args,
    //         //   'access arguments' => array('use ' . $machine_name . ' importer'),
    //         //   'type' => MENU_NORMAL_ITEM,
    //         //   'file' => $callback_path,
    //         //   'file path' => $file_path,
    //         // );
    //   }
    // }
    return $routes;
  }


}
