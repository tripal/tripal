<?php

namespace Drupal\tripal\TripalBackendPublish\PluginManager;

use Drupal\Core\Plugin\DefaultPluginManager;
use Drupal\Core\Cache\CacheBackendInterface;
use Drupal\Core\Extension\ModuleHandlerInterface;

/**
 * Provides a TripalBackendPublish plugin manager.
 */
class TripalBackendPublishManager extends DefaultPluginManager {

  /**
   * Constructs a new TripalBackendPublish manager.
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
    \Traversable $namespaces,
    CacheBackendInterface $cache_backend,
    ModuleHandlerInterface $module_handler
  ) {
print "CP02 TripalBackendPublishManager __construct()\n"; //@@@
    parent::__construct(
      "Plugin/TripalBackendPublish",
      $namespaces,
      $module_handler,
      'Drupal\tripal\TripalBackendPublish\Interfaces\TripalBackendPublishInterface',
      'Drupal\tripal\TripalBackendPublish\Annotation\TripalBackendPublish'
    );
    $this->alterInfo("tripal_backend_publish_info");
    $this->setCacheBackend($cache_backend, "tripal_backend_publish_plugins");
  }

  /**
   * Publish content of a specified type. Uses a Tripal service.
   *
   * @param string $bundle
   *   The entity type id (bundle) to be published.
   *
   * @param string $datastore
   *   The plugin id for the TripalStorage backend to publish from.
   *
   * @param \Drupal\tripal\Services\TripalJob $job
   *  An optional TripalJob object.
   */
  public static function runTripalJob($bundle, $datastore, $options = [], TripalJob $job = NULL) {
    print "CP01 TripalBackendPublishManager runTripalJob()\n"; //@@@
    try {
      // Initialize the logger.
      /** @var \Drupal\tripal\Services\TripalLogger $logger **/
      $logger = \Drupal::service('tripal.logger');

      // Load the appropriate plugin. Currently only chado is supported, $datastore = 'chado_storage'
      /** @var \Drupal\tripal\Services\TripalPublish $publish_service */
      $publish_service = \Drupal::service('tripal.backend_publish');
      $publish_instance = $publish_service->createInstance('chado_publish', []);

      $publish_instance->init($bundle, $datastore, $options, $job);
      $publish_instance->publish();
    }
    catch (Exception $e) {
      if ($job) {
        $logger->error($e->getMessage());
      }
    }
  }

}
