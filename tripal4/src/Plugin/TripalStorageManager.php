<?php

namespace Drupal\tripal4\Plugin;

use Drupal\Core\Plugin\DefaultPluginManager;
use Drupal\Core\Cache\CacheBackendInterface;
use Drupal\Core\Extension\ModuleHandlerInterface;

/**
 * Provides a tripal storage plugin manager.
 */
class TripalStorageManager extends DefaultPluginManager {

  /**
   * Constructs a new tripal storage manager.
   *
   * @param string $subdir
   *   The plugin's subdirectory, for example Plugin/views/filter.
   * @param \Traversable $namespaces
   *   An object that implements \Traversable which contains the root paths
   *   keyed by the corresponding namespace to look for plugin implementations.
   * @param \Drupal\Core\Extension\ModuleHandlerInterface $module_handler
   *   The module handler.
   * @param string $plugin_interface
   *   The interface each plugin should implement.
   * @param string $plugin_definition_annotation_name
   *   The name of the annotation that contains the plugin definition.
   */
  public function __construct(
      ,\Traversable $namespaces
      ,CacheBackendInterface $cache_backend
      ,ModuleHandlerInterface $module_handler
  ) {
    parent::__construct(
        "Plugin/TripalStorage"
        $subdir
        ,$namespaces
        ,$module_handler
        ,'Drupal\tripal4\Interface\TripalStorageInterface'
        ,'Drupal\tripal4\Annotation\TripalStorage'
    );
    $this->alterInfo("tripal_storage_info");
    $this->setCacheBackend($cache_backend,"tripal_storage_plugins");
  }
}
