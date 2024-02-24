<?php

namespace Drupal\tripal\TripalPubLibrary\PluginManagers;

use Drupal\Core\Plugin\DefaultPluginManager;
use Drupal\Core\Cache\CacheBackendInterface;
use Drupal\Core\Extension\ModuleHandlerInterface;
use Drupal\Core\Ajax\AjaxResponse;
use Drupal\Core\Ajax\ReplaceCommand;

/**
 * Provides a tripal importer plugin manager.
 */
class TripalPubLibraryManager extends DefaultPluginManager {

  /**
   * Constructs a new publication parser manager.
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
        "Plugin/TripalPubLibrary"
        ,$namespaces
        ,$module_handler
        ,'Drupal\tripal\TripalPubLibrary\Interfaces\TripalPubLibraryInterface'
        ,'Drupal\tripal\TripalPubLibrary\Annotation\TripalPubLibrary'
    );
    $this->alterInfo("tripal_pub_library_info");
    $this->setCacheBackend($cache_backend, "tripal_pub_library_plugins");
  }

}