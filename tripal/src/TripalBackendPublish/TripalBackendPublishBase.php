<?php

namespace Drupal\tripal\TripalBackendPublish;

use Drupal\tripal\Services\TripalTokenParser;
use Drupal\tripal\Services\TripalEntityLookup;
use Drupal\tripal\TripalStorage\PluginManager\TripalStorageManager;
use Drupal\tripal\Services\TripalLogger;
use Drupal\Core\Field\FieldTypePluginManager;
use Drupal\Core\Entity\EntityFieldManagerInterface;
use Drupal\Core\Entity\EntityTypeManagerInterface;
use Drupal\Core\Database\Connection;
use Drupal\tripal\TripalBackendPublish\Interfaces\TripalBackendPublishInterface;
use Drupal\Component\Plugin\PluginBase;
use Drupal\Core\Plugin\ContainerFactoryPluginInterface;
use Symfony\Component\DependencyInjection\ContainerInterface;

/**
 * Base class for Storage Backend-specific publish plugin implementations.
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
   * @var \Drupal\Core\Entity\EntityTypeManagerInterface
   */
  protected $entity_type_manager = NULL;

  /**
   * The Drupal entity field manager service.
   *
   * @var \Drupal\Core\Entity\EntityFieldManagerInterface
   */
  protected $entity_field_manager = NULL;

  /**
   * The Drupal field type manager service.
   *
   * @var \Drupal\Core\Field\FieldTypePluginManager
   */
  protected $field_type_manager = NULL;

  /**
   * The TripalLogger object.
   *
   * @var \Drupal\tripal\Services\TripalLogger
   */
  protected $logger = NULL;

  /**
   * The Tripal Storage service.
   *
   * @var \Drupal\tripal\TripalStorage\PluginManager\TripalStorageManager
   */
  protected $storage_manager = NULL;

  /**
   * The Entity Lookup service.
   *
   * @var \Drupal\tripal\Services\TripalEntityLookup
   */
  protected $entity_lookup_manager = NULL;

  /**
   * The Tripal Token Parser service.
   *
   * @var \Drupal\tripal\Services\TripalTokenParser
   */
  protected $token_parser = NULL;

  /**
   * Specifies the maximum number of records to publish at one time.
   *
   * This limits memory consumption if there are many thousands of
   * records, for example gene records in the feature table.
   *
   * @var int
   */
  protected $batch_size = 1000;

  /**
   * The TripalJob object.
   *
   * @var \Drupal\tripal\Services\TripalJob
   */
  protected $job = NULL;

  /**
   * The id of the entity type (bundle)
   *
   * @var string
   */
  protected $bundle = '';

  /**
   * The id of the TripalStorage plugin.
   *
   * @var string
   */
  protected $datastore = '';

  /**
   * The database schema to publish, e.g. 'chado'.
   *
   * @var string
   */
  protected $schema_name = '';

  /**
   * Controls publishing of either new or all entities.
   *
   * If TRUE, publish all entities, existing entities are republished.
   * If FALSE, just publish new entities.
   * Republish is needed when new fields have been added, or when
   * the entity title format has been changed.
   *
   * @var bool
   */
  protected $republish = FALSE;

  /**
   * Valid HTML tags for titles.
   *
   * This is configurable through entity settings at
   * /admin/tripal/config/entity_settings.
   *
   * @var array
   */
  protected array $allowed_title_tags = [];

  /**
   * Implements ContainerFactoryPluginInterface->create().
   *
   * Since we have implemented the ContainerFactoryPluginInterface this static
   * function will be called behind the scenes when a Plugin Manager uses
   * createInstance(). Specifically, this method is used to determine the
   * parameters to pass to the constructor.
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
      $container->get('database'),
      $container->get('entity_type.manager'),
      $container->get('entity_field.manager'),
      $container->get('plugin.manager.field.field_type'),
      $container->get('tripal.logger'),
      $container->get('tripal.storage'),
      $container->get('tripal.tripal_entity.lookup'),
      $container->get('tripal.token_parser')
      );
  }

  /**
   * {@inheritdoc}
   */
  public function __construct(
    array $configuration,
    $plugin_id,
    $plugin_definition,
    Connection $connection,
    EntityTypeManagerInterface $entity_type_manager,
    EntityFieldManagerInterface $entity_field_manager,
    FieldTypePluginManager $field_type_manager,
    TripalLogger $logger,
    TripalStorageManager $storage_manager,
    TripalEntityLookup $entity_lookup_manager,
    TripalTokenParser $token_parser,
  ) {
    parent::__construct($configuration, $plugin_id, $plugin_definition);
    $this->connection = $connection;
    $this->entity_type_manager = $entity_type_manager;
    $this->entity_field_manager = $entity_field_manager;
    $this->field_type_manager = $field_type_manager;
    $this->logger = $logger;
    $this->storage_manager = $storage_manager;
    $this->entity_lookup_manager = $entity_lookup_manager;
    $this->token_parser = $token_parser;
  }

}
