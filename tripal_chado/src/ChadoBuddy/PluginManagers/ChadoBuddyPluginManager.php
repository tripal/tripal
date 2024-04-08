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

  /**
   * Provides the TripalDBX connection to chado that ChadoBuddies created by
   * this plugin manager should act upon.
   *
   * @var Drupal\tripal_chado\Database\ChadoConnection
   */
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

  /**
   * {@inheritdoc}
   *
   * OVERRIDE: We need to override the default implementation here in order to
   * pass in the dependencies cleanly :-)
   */
  public function createInstance($plugin_id, array $configuration = []) {

    $all_plugin_definitions = $this->getDefinitions();
    $plugin_definition = $all_plugin_definitions[$plugin_id];
    $plugin_class = $plugin_definition['class'];

    return new $plugin_class($configuration, $plugin_id, $plugin_definition, $this->connection);
  }
}
