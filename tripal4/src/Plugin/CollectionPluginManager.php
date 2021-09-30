<?php

namespace Drupal\tripal4\Plugin;

use Drupal\Core\Plugin\DefaultPluginManager;
use Drupal\Core\Cache\CacheBackendInterface;
use Drupal\Core\Extension\ModuleHandlerInterface;

/**
 * Provides a tripal collection plugin manager.
 */
class CollectionPluginManager extends DefaultPluginManager {

  /**
   * Constructs a new tripal collection plugin manager.
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
   * @param string $table
   *   The table name used to store the collection of saved collection plugins
   *   in the drupal database.
   */
  public function __construct(
      $subdir
      ,\Traversable $namespaces
      ,CacheBackendInterface $cache_backend
      ,ModuleHandlerInterface $module_handler
      ,$pluginInterface
      ,$pluginDefinitionAnnotation
      ,$table
  ) {
    parent::__construct(
        $subdir
        ,$namespaces
        ,$module_handler
        ,$pluginInterface
        ,$pluginDefinitionAnnotation
    );
    $this->table = $table;
  }

  /**
   * Creates and returns a new collection with the given name and plugin id.
   * The given name must not already exist in this plugin manager's existing
   * collection of plugins. The given plugin id must be a valid plugin
   * implementation.
   *
   * @param string $name
   *   The collection name.
   *
   * @param string $pluginId
   *   The plugin id.
   *
   * @return \Drupal\tripal4\Plugin\TripalCollectionPluginBase
   *   The new collection.
   */
  public function createCollection($name,$pluginId) {
  }

  /**
   * Removes the collection with the given name in this manager. If no such
   * collection exists with the given name then this does nothing.
   * !!!WARNING!!!
   * If the data in the removed collection is referenced by other collections or
   * entities this could break data integrity. This method must be used with
   * extreme caution!
   *
   * @param string $name
   *   The collection name.
   *
   * @return bool
   *   True if the matching collection was removed or false otherwise.
   */
  public function removeCollection($name) {
  }

  /**
   * Returns an array of names of all existing collection plugins.
   *
   * @return array
   *   Collection plugin names.
   */
  public function getCollectionList() {
  }

  /**
   * Loads and returns an existing collection plugin with the given name. If
   * the given name does not exist then NULL is returned.
   *
   * @param string $name
   *   The name.
   *
   * @return \Drupal\tripal4\Plugin\TripalCollectionPluginBase|NULL
   *   The loaded collection plugin or NULL.
   */
  public function loadCollection($name) {
  }

  /**
   * The collection name table.
   *
   * @var string
   */
  private $collectionTable;

}
