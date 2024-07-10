<?php

use Drupal\Core\Extension\ExtensionDiscovery;
use Drupal\Core\Config\InstallStorage;
use Drupal\Core\Config\FileStorage;


/**
 * Loads a Tripal managed Configuration Entity.
 *
 * This function is useful if the configuration entity has been deleted and
 * needs to be reloaded. By deafult, the configurtion entities are populated
 * using the YAML files in the <module>/config/install folder. If they
 * get deleted and need to be restored, this function will do that.
 *
 * @param string $module
 *   The module that establisehd the configuration.
 * @param string $config_type
 *   The ID of a Configuration Entity type.
 */
function tripal_load_configuration($module, $config_type) {

  // Get the config entity storage class and definition class.
  $config_storage = \Drupal::entityTypeManager()->getStorage($config_type);
  $definition = \Drupal::entityTypeManager()->getDefinition($config_type);

  // Get the list of Tripal configurations that are currently installed.
  $config_factory = \Drupal::service('config.factory');
  $config_list = $config_factory->listAll($module . '.' . $config_type);

  // Get the list of installed modules.
  $listing = new ExtensionDiscovery(\Drupal::root());
  $modules = $listing->scan('module');

  // Iterate through the list of modules and look for configurations.
  // If any are found but not installed, then install those.
  foreach ($modules as $module) {
    $extension_path = $module->getPath();
    $config_path = $extension_path . '/' . InstallStorage::CONFIG_INSTALL_DIRECTORY;
    if (is_dir($config_path)) {
      $file_storage = new FileStorage($config_path);
      $configs = $file_storage->listAll($definition->getConfigPrefix());
      foreach ($configs as $config_file) {

        // If a configuration has been found that matches the requested type
        // then add it only if it doesn't already exist.
        if (!in_array($config_file, $config_list)) {
          $config = $file_storage->read($config_file);
          $mapping = $config_storage->create($config);
          $mapping->save();
        }
      }
    }
  }
}

/**
 * Returns the current version of Tripal as defined in the tripal.info.yml file.
 *
 * @return string
 *   The version string of Tripal, e.g. "4.0"
 */
function tripal_version() {
  $tripal_module = \Drupal::service('module_handler')->getModule('tripal');
  $tripal_info = \Drupal::service('info_parser')->parse($tripal_module->getPathname());
  return $tripal_info['version'];
}
