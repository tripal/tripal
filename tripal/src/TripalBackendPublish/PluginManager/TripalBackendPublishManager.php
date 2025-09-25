<?php

namespace Drupal\tripal\TripalBackendPublish\PluginManager;

use Drupal\tripal\Services\TripalJob;
use Drupal\tripal\Services\TripalLogger;
use Drupal\Core\Plugin\DefaultPluginManager;
use Drupal\Core\Cache\CacheBackendInterface;
use Drupal\Core\Extension\ModuleHandlerInterface;

/**
 * Provides a TripalBackendPublish plugin manager.
 */
class TripalBackendPublishManager extends DefaultPluginManager {

  /**
   * The TripalLogger object.
   *
   * @var \Drupal\tripal\Services\TripalLogger
   */
  protected $logger = NULL;

  /**
   * Implements ContainerFactoryPluginInterface->create().
   *
   * Since we have implemented the ContainerFactoryPluginInterface this static
   * function will be called behind the scenes when a Plugin Manager uses
   * createInstance(). Specifically, this method is used to determine the
   * parameters to pass to the contsructor.
   *
   * @param \Symfony\Component\DependencyInjection\ContainerInterface $container
   *   The current container.
   * @param array $configuration
   *   A configuration array.
   * @param string $plugin_id
   *   The plugin identifier.
   * @param mixed $plugin_definition
   *   The definition of the plugin.
   *
   * @return static
   */
  public static function create(ContainerInterface $container, array $configuration, $plugin_id, $plugin_definition) {
    return new static(
      $configuration,
      $plugin_id,
      $plugin_definition,
      $container->get('tripal.logger')
    );
  }

  /**
   * Constructs a new TripalBackendPublish manager.
   *
   * @param \Traversable $namespaces
   *   An object that implements \Traversable which contains the root paths
   *   keyed by the corresponding namespace to look for plugin implementations.
   * @param Drupal\Core\Cache\CacheBackendInterface $cache_backend
   *   The cache backend.
   * @param \Drupal\Core\Extension\ModuleHandlerInterface $module_handler
   *   The module handler.
   * @param \Drupal\tripal\Services\TripalLogger $logger
   *   Dependency injection of the tripal logger.
   */
  public function __construct(
    \Traversable $namespaces,
    CacheBackendInterface $cache_backend,
    ModuleHandlerInterface $module_handler,
    TripalLogger $logger,
  ) {
    parent::__construct(
      "Plugin/TripalBackendPublish",
      $namespaces,
      $module_handler,
      'Drupal\tripal\TripalBackendPublish\Interfaces\TripalBackendPublishInterface',
      'Drupal\tripal\TripalBackendPublish\Attribute\TripalBackendPublish',
      'Drupal\tripal\TripalBackendPublish\Annotation\TripalBackendPublish'
    );
    $this->alterInfo("tripal_backend_publish_info");
    $this->setCacheBackend($cache_backend, "tripal_backend_publish_plugins");
    $this->logger = $logger;
  }

  /**
   * {@inheritdoc}
   *
   * OVERRIDE: We need to override the default implementation here in order to
   * pass in the dependencies cleanly :-)
   */
  public function createInstance($plugin_id, array $configuration = []) {
    return $this->getFactory()->createInstance($plugin_id, $configuration, $this->logger);
  }

  /**
   * Publish content of a specified type. Uses a Tripal service.
   *
   * @param string $bundle
   *   The entity type id (bundle) to be published.
   * @param string $datastore
   *   The plugin id for the TripalStorage backend to publish
   *   from, for example 'chado_storage'.
   * @param array $options
   *   Associative array of additional options to pass to the publish
   *   plugin. Valid keys are 'schema_name', 'republish', 'batch_size'.
   * @param ?\Drupal\tripal\Services\TripalJob $job
   *   An optional TripalJob object.
   */
  public static function runTripalJob($bundle, $datastore, $options = [], ?TripalJob $job = NULL) {
    try {
      // Load the specified plugin. An invalid plugin_id is caught
      // during __construct().
      $publish_service = \Drupal::service('tripal.backend_publish');
      $publish_instance = $publish_service->createInstance($datastore, []);
      $publish_options = $options;
      $publish_options['bundle'] = $bundle;
      $publish_options['datastore'] = $datastore;
      $publish_options['job'] = $job;
      $publish_instance->publish($publish_options);
    }
    catch (Exception $e) {
      if ($job) {
        self::$logger->error($e->getMessage());
      }
    }
  }

}
