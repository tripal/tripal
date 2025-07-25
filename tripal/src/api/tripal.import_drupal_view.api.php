<?php

use Drupal\Core\Config\FileStorage;

/**
 * @file
 * Functions handling backwards compatibility.
 */

/**
 * Import a drupal view from a yaml file.
 *
 * This includes a backwards compatibility fix for a new views css option.
 * See https://www.drupal.org/node/3499943 for the new option.
 * Because this new option requires a schema update, having it in the view
 * for Drupal versions < 11.2 will cause errors. This function removes the
 * new setting if the Drupal version is less than 11.2.
 *
 * Because the view will be created automatically from the yaml file upon
 * module install, this function will always reload it if the drupal
 * version is < 11.2, and this can be used by install() to fix this
 * incompatibility.
 *
 * @param string $view_name
 *   The name of the view.
 * @param string $module_name
 *   The name of the module.
 * @param string $file_name
 *   The name of the yaml configuration file.
 * @param bool $legacy
 *   Remove incompatible option and loads the view even if it already
 *   exists. Defaults to FALSE.
 *
 * @return void
 *   No return value.
 */
function import_drupal_view(string $view_name, string $module_name, string $file_name, bool $legacy = FALSE): void {
  $drupal_version = floatval(\Drupal::VERSION);
  $view_storage = \Drupal::entityTypeManager()->getStorage('view');
  $existing_view = $view_storage->load($view_name);

  if (!$existing_view || $legacy) {
    $dir = \Drupal::service('extension.list.module')->getPath($module_name);
    $fileStorage = new FileStorage($dir);
    $config = $fileStorage->read($file_name);
    if ($legacy && array_key_exists('class', $config['display']['default']['display_options']['style']['options'] ?? [])) {
      // Removes the non-backwards-compatible option from the config.
      unset($config['display']['default']['display_options']['style']['options']['class']);
    }
    if ($existing_view) {
      $existing_view->delete();
    }
    $view = $view_storage->create($config);
    $view->save();
  }
}
