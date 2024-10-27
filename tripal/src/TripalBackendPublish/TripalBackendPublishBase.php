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
   * The database connection for querying Drupal tables.
   *
   * @var \Drupal\Core\Database\Connection
   */
  protected $connection;

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
   * Specifies the maximum number of records to publish at one time.
   * This limits memory consumption if there are many thousands of
   * records, for example gene records in the feature table.
   * @todo We might want to add this as an option on the publish form.
   *
   * @var integer $batch_size
   */
  protected $batch_size = 1000;

  /**
   * The TripalJob object.
   *
   * @var \Drupal\tripal\Services\TripalJob $job
   */
  protected $job = NULL;

  /**
   * The id of the entity type (bundle)
   *
   * @var string $bundle
   */
  protected $bundle = '';

  /**
   * The id of the TripalStorage plugin.
   *
   * @var string $datastore.
   */
  protected $datastore = '';

  /**
   * The republish flag specifies whether to publish all entities
   * if TRUE, or just publish new entities if FALSE.
   * Republish is needed when new fields have been added, or when
   * the entity title format has been changed.
   *
   * @var boolean $republish
   */
  protected $republish = FALSE;

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
    return new static(
      $configuration,
      $plugin_id,
      $plugin_definition,
      $container->get('database'),
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
    \Drupal\Core\Database\Connection $connection,
    \Drupal\Core\Entity\EntityTypeManagerInterface $entity_type_manager,
    \Drupal\tripal\Services\TripalLogger $logger,
    \Drupal\tripal\TripalStorage\PluginManager\TripalStorageManager $storage_manager,
    \Drupal\tripal\Services\TripalEntityLookup $entity_lookup_manager) {
    parent::__construct($configuration, $plugin_id, $plugin_definition);
    $this->connection = $connection;
    $this->entity_type_manager = $entity_type_manager;
    $this->logger = $logger;
    $this->storage_manager = $storage_manager;
    $this->entity_lookup_manager = $entity_lookup_manager;
  }

}
