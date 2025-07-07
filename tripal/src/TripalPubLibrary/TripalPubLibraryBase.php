<?php

namespace Drupal\tripal\TripalPubLibrary;
use Drupal\tripal\TripalPubLibrary\Interfaces\TripalPubLibraryInterface;
use Drupal\Component\Plugin\PluginBase;
use Drupal\Core\Plugin\ContainerFactoryPluginInterface;
use Symfony\Component\DependencyInjection\ContainerInterface;
use Drupal\Core\Database\Connection;
use Drupal\tripal\Services\TripalFileRetriever;

/**
 * Defines the base class for the tripal pub parser plugins.
 */
abstract class TripalPubLibraryBase extends PluginBase implements TripalPubLibraryInterface, ContainerFactoryPluginInterface {

  /**
   * The public database connection
   */
  protected $public;

  /**
   * The logger for reporting progress, warnings and errors to admin.
   *
   * @var \Drupal\tripal\Services\TripalLogger
   */
  protected $logger;

  /**
   * An instance of the Tripal file retriever service
   *
   * @var object \Drupal\tripal\Services\TripalFileRetriever
   */
  protected $fileretriever = NULL;

  /**
   * The Tripal Citation generation service.
   *
   * @var \Drupal\tripal\Services\TripalCitationManager $citation_manager
   */
  protected $citation_manager = NULL;

  /**
   * The ID of this plugin.
   *
   * @var string
   */
  protected $plugin_id;

  /**
   * The plugin definition
   *
   * @var array
   */
  protected $plugin_definition;

  /**
   * Implements ContainerFactoryPluginInterface->create().
   *
   * Since we have implemented the ContainerFactoryPluginInterface this static function
   * will be called behind the scenes when a Plugin Manager uses createInstance(). Specifically
   * this method is used to determine the parameters to pass to the constructor.
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
      $container->get('tripal.logger'),
      $container->get('tripal.citation'),
      $container->get('tripal.fileretriever'),
    );
  }

  /**
   * {@inheritdoc}
   */
  public function __construct(array $configuration, $plugin_id, $plugin_definition,
                              Connection $public,
                              \Drupal\tripal\Services\TripalLogger $logger,
                              \Drupal\tripal\Services\TripalCitationManager $citation_manager,
                              TripalFileRetriever $fileretriever) {

    parent::__construct($configuration, $plugin_id, $plugin_definition);

    // Dependency injection for public schema, tripal logger, citation generator, and file retriever
    $this->public = $public;
    $this->logger = $logger;
    $this->citation_manager = $citation_manager;
    $this->fileretriever = $fileretriever;
  }

}
