<?php

namespace Drupal\tripal_chado\ChadoBuddy\PluginManagers;

use Drupal\Core\Cache\CacheBackendInterface;
use Drupal\Core\Extension\ModuleHandlerInterface;
use Drupal\Core\Plugin\DefaultPluginManager;
use Drupal\tripal_chado\ChadoBuddy\Annotation\ChadoBuddy;
use Drupal\tripal_chado\ChadoBuddy\Interfaces\ChadoBuddyInterface;
use Drupal\tripal_chado\Database\ChadoConnection;

/**
 * ChadoBuddy plugin manager.
 */
final class ChadoBuddyPluginManager extends DefaultPluginManager {

  public ChadoConnection $connection;

  /**
   * Constructs the object.
   */
  public function __construct(\Traversable $namespaces, CacheBackendInterface $cache_backend, ModuleHandlerInterface $module_handler, ChadoConnection $connection) {
    parent::__construct('Plugin/ChadoBuddy', $namespaces, $module_handler, ChadoBuddyInterface::class, ChadoBuddy::class);
    $this->alterInfo('chado_buddy_info');
    $this->setCacheBackend($cache_backend, 'chado_buddy_plugins');
    $this->connection = $connection;
  }

}
