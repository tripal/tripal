<?php

namespace Drupal\tripal\TripalVocabTerms\PluginManagers;

use Drupal\tripal\TripalVocabTerms\PluginManagers\TripalCollectionPluginManager;
use Drupal\Core\Cache\CacheBackendInterface;
use Drupal\Core\Extension\ModuleHandlerInterface;

/**
 * Provides the tripal id space plugin manager.
 */
class TripalIdSpaceManager extends TripalCollectionPluginManager {

  /**
   * Constructs a new tripal id space plugin manager.
   *
   * @param \Traversable $namespaces
   *   An object that implements \Traversable which contains the root paths
   *   keyed by the corresponding namespace to look for plugin implementations.
   * @param \Drupal\Core\Cache\CacheBackendInterface $cache_backend
   *   Cache backend instance to use.
   * @param \Drupal\Core\Extension\ModuleHandlerInterface $module_handler
   *   The module handler to invoke the alter hook with.
   */
  public function __construct(
      \Traversable $namespaces
      ,CacheBackendInterface $cache_backend
      ,ModuleHandlerInterface $module_handler
  ) {
    parent::__construct(
        'Plugin/TripalIdSpace'
        ,$namespaces
        ,$cache_backend
        ,$module_handler
        ,'Drupal\tripal\TripalVocabTerms\Interfaces\TripalIdSpaceInterface'
        ,'Drupal\tripal\TripalVocabTerms\Annotation\TripalIdSpace'
        ,'tripal_id_space_collection'
    );
    $this->alterInfo('tripal_id_space_info');
    $this->setCacheBackend($cache_backend,'tripal_id_space_plugins');
  }

}
