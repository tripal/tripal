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

  /**
   * Retrieve a Tripal Pub Library Query record
   *
   * This method will look for a pub library query from the 'tripal_pub_library_query'
   * table in the public schema of the database.
   *
   * @param $query_id
   *   Unique ID of the query (primary key)
   * @return object
   *   A row object with the pub query data from the database
   *
   * @ingroup pub_loader
   */
  public function getSearchQuery(int $query_id) {
    $public = \Drupal::database();
    $row = $public->select('tripal_pub_library_query', 'tpi')
        ->fields('tpi')
        ->condition('pub_library_query_id', $query_id, '=')
        ->execute()
        ->fetchObject();
    return $row;
  }


  /**
   * Retrieve all Tripal Pub Library Query records
   *
   * This method will look for all pub library queries from the 'tripal_pub_library_query'
   * table in the public schema of the database. It will return these records as an array.
   *
   * @return array
   *   An array containing all pub library queries as row objects
   *
   * @ingroup pub_loader
   */
  public function getSearchQueries() {
    $public = \Drupal::database();
    $pub_library_main_query = $public->select('tripal_pub_library_query','tpi');
    $results = $pub_library_main_query->fields('tpi')->orderBy('pub_library_query_id', 'ASC')->execute()->fetchAll();
    return $results;
  }

  /**
   * Add a new Tripal Pub Library Query 
   *
   * This method will add a new pub library query to the 'tripal_pub_library_query'
   * table in the public schema of the database. 
   *
   * @param $query
   *  The query data in the form of an array
   * 
   * @ingroup pub_loader
   */  
  public function addSearchQuery(array $query) {
    $public = \Drupal::database();
    $public->insert('tripal_pub_library_query')->fields($query)->execute();
  }

   /**
   * Update a Tripal Pub Library Query record
   *
   * This method will update a pub library query from the tripal_pub_library_query
   * table in the public schema of the database if it matches the query_id parameter.
   *
   * @param $query_id
   *  The query_id which matches the existing query_id of the table
   * @param $query
   *  An array containing the updated query data
   *
   * @ingroup pub_loader
   */ 
  public function updateSearchQuery(int $query_id, array $query) {
    $public = \Drupal::database();
    $public->update('tripal_pub_library_query')
    ->fields($query)
    ->condition('pub_library_query_id', $query_id)
    ->execute();
  }


  /**
   * Delete a Tripal Pub Library Query record
   *
   * This method will delete a pub library query from the tripal_pub_library_query
   * table in the public schema of the database if it matches the query_id parameter.
   *
   * @param $query_id
   *  The query_id which matches the existing query_id of the table
   *
   * @ingroup pub_loader
   */ 
  public function deleteSearchQuery(int $query_id) {
    $public = \Drupal::database();
    $public->delete('tripal_pub_library_query')
    ->condition('pub_library_query_id', $query_id, '=')
    ->execute();
  }

}