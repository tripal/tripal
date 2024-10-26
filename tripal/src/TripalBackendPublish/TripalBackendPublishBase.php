<?php

namespace Drupal\tripal\TripalBackendPublish;

use Drupal\tripal\TripalBackendPublish\Interfaces\TripalBackendPublishInterface;
use Drupal\Component\Plugin\PluginBase;
use Drupal\Core\Plugin\ContainerFactoryPluginInterface;
use Symfony\Component\DependencyInjection\ContainerInterface;

/**
 * Defines a base class for Storage Backend-specific publish plugin implementations.
 */
abstract class TripalBackendPublishBase extends PluginBase implements TripalBackendPublishInterface, ContainerFactoryPluginInterface {

  /**
   * The Drupal entity type manager service.
   *
   * @var \Drupal\Core\Entity\EntityTypeManagerInterface $entity_type_manager
   */
  protected $entity_type_manager = NULL;

  /**
   * The TripalLogger object.
   *
   * @var \Drupal\tripal\Services\TripalLogger $logger
   */
  protected $logger = NULL;

  /**
   * The Tripal Storage service.
   *
   * @var \Drupal\tripal\TripalStorage\PluginManager\TripalStorageManager $storage_manager
   */
  protected $storage_manager = NULL;

  /**
   * The Entity Lookup service.
   *
   * @var \Drupal\tripal\Services\TripalEntityLookup $entity_lookup_manager
   */
  protected $entity_lookup_manager = NULL;

  /**
   * Implements ContainerFactoryPluginInterface->create().
   *
   * Since we have implemented the ContainerFactoryPluginInterface this static function
   * will be called behind the scenes when a Plugin Manager uses createInstance(). Specifically
   * this method is used to determine the parameters to pass to the contructor.
   *
   * @param \Symfony\Component\DependencyInjection\ContainerInterface $container
   * @param array $configuration
   * @param string $plugin_id
   * @param mixed $plugin_definition
   *
   * @return static
   */
  public static function create(ContainerInterface $container, array $configuration, $plugin_id, $plugin_definition) {
print "CP03 TripalBackendPublishBase create()\n"; //@@@
    return new static(
      $configuration,
      $plugin_id,
      $plugin_definition,
      $container->get('entity_type.manager'),
      $container->get('tripal.logger'),
      $container->get('tripal.storage'),
      $container->get('tripal.tripal_entity.lookup')
      );
  }

  /**
   * {@inheritdoc}
   */
  public function __construct(array $configuration, $plugin_id, $plugin_definition,
    \Drupal\Core\Entity\EntityTypeManagerInterface $entity_type_manager,
    \Drupal\tripal\Services\TripalLogger $logger,
    \Drupal\tripal\TripalStorage\PluginManager\TripalStorageManager $storage_manager,
    \Drupal\tripal\Services\TripalEntityLookup $entity_lookup_manager) {
print "CP04 TripalBackendPublishBase __construct()\n"; //@@@
    parent::__construct($configuration, $plugin_id, $plugin_definition);
    $this->entity_type_manager = $entity_type_manager;
    $this->logger = $logger;
    $this->storage_manager = $storage_manager;
    $this->entity_lookup_manager = $entity_lookup_manager;
  }

}
