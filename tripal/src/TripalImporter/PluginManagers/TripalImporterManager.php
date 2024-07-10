<?php

namespace Drupal\tripal\TripalImporter\PluginManagers;

use Drupal\Core\Plugin\DefaultPluginManager;
use Drupal\Core\Cache\CacheBackendInterface;
use Drupal\Core\Extension\ModuleHandlerInterface;

/**
 * Provides a tripal importer plugin manager.
 */
class TripalImporterManager extends DefaultPluginManager {

  /**
   * Constructs a new tripal importer manager.
   *
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
      \Traversable $namespaces
      ,CacheBackendInterface $cache_backend
      ,ModuleHandlerInterface $module_handler
  ) {
    parent::__construct(
        "Plugin/TripalImporter"
        ,$namespaces
        ,$module_handler
        ,'Drupal\tripal\TripalImporter\Interfaces\TripalImporterInterface'
        ,'Drupal\tripal\TripalImporter\Annotation\TripalImporter'
    );
    $this->alterInfo("tripal_importer_info");
    $this->setCacheBackend($cache_backend, "tripal_importer_plugins");
  }
}