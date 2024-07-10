<?php

namespace Drupal\tripal\TripalVocabTerms\PluginManagers;

use Drupal\Core\Plugin\DefaultPluginManager;
use Drupal\Core\Cache\CacheBackendInterface;
use Drupal\Core\Extension\ModuleHandlerInterface;
use \RuntimeException;

/**
 * Provides a tripal collection plugin manager.
 */
class TripalCollectionPluginManager extends DefaultPluginManager {

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
   * @return Drupal\tripal\TripalVocabTerms\TripalCollectionPluginBase
   *   The new collection or NULL if an error occured.
   */
  public function createCollection($name, $pluginId) {
    if (!is_string($name)) {
      return NULL;
    }

    $clist = $this->getCollectionList();
    if (in_array($name, $clist)) {
      return NULL;
    }
    $db = \Drupal::database();
    $collection = $this->createInstance($pluginId, ["collection_name" => $name]);
    if ($collection->isValid()) {
      $result = $db->insert($this->table)->fields(["name" => $name, "plugin_id" => $pluginId])->execute();
      if ($collection->recordExists() == False) {
        $collection->createRecord();
      }
      return $collection;
    }
    else {
      return NULL;
    }
  }

  /**
   * Removes the collection with the given name in this manager. If no such
   * collection exists with the given name then this does nothing.
   * !!!WARNING!!!
   * If the data in the removed collection is referenced by other collections or
   * entities this could cause data loss. This method must be used with extreme
   * caution!
   *
   * @param string $name
   *   The collection name.
   *
   * @return bool
   *   True if the matching collection was removed or false otherwise.
   */
  public function removeCollection($name) {
    if (!is_string($name)) {
      return NULL;
    }
    $db = \Drupal::database();
    $result = $db->select($this->table,'n')
      ->fields('n', ['name', 'plugin_id'])
      ->condition('n.name', $name)
      ->execute();
    $record = $result->fetchObject();
    if (!$record) {
      return FALSE;
    }
    $num_deleted = $db->delete($this->table)->condition('name', $name)->execute();
    if ($num_deleted < 1) {
      return FALSE;
    }
    $collection = $this->createInstance($record->plugin_id, ["collection_name" => $name]);
    if ($collection->recordExists() == True) {
      $collection->destroy();
    }
    return TRUE;
  }

  /**
   * Returns an array of collection names of all existing collections.
   *
   * @return array
   *   Collection names.
   */
  public function getCollectionList() {
    $names = [];
    $db = \Drupal::database();
    $result = $db->select($this->table, 'n')->fields('n', ['name'])->execute();
    foreach ($result as $record) {
      $names[] = $record->name;
    }
    return $names;
  }

  /**
   * Loads and returns an existing collection plugin with the given name. If
   * the given name does not exist then NULL is returned.
   *
   * @param string $name
   *   The name.
   *
   * @return Drupal\tripal\TripalVocabTerms\TripalCollectionPluginBase|NULL
   *   The loaded collection plugin or NULL.
   */
  public function loadCollection($name) {
    if (!is_string($name)) {
      return NULL;
    }
    $db = \Drupal::database();
    $result = $db->select($this->table, 'n')
      ->condition('n.name', $name)
      ->fields('n', ['name', 'plugin_id'])
      ->execute();
    $first = $result->fetchAssoc();
    if (!$first) {
      return NULL;
    }
    $collection = $this->createInstance($first["plugin_id"], ["collection_name" => $name]);

    if ($collection->isValid() and $collection->recordExists() == False) {
      $collection->createRecord();
    }

    return $collection;
  }

  /**
   * The collection name table.
   *
   * @var string
   */
  private $table;

}
