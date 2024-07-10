<?php

namespace Drupal\tripal\TripalPubLibrary;
use Drupal\tripal\TripalPubLibrary\Interfaces\TripalPubLibraryInterface;
use Drupal\Component\Plugin\PluginBase;
use Drupal\Core\Plugin\ContainerFactoryPluginInterface;
use Symfony\Component\DependencyInjection\ContainerInterface;
use Drupal\Core\Database\Connection;
/**
 * Defines the base class for the tripal pub parser plugins.
 */
abstract class TripalPubLibraryBase extends PluginBase implements TripalPubLibraryInterface, ContainerFactoryPluginInterface {

  /**
   * The public database connection
   */
  protected $public;

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
    );
  }

  /**
   * {@inheritdoc}
   */
  public function __construct(array $configuration, $plugin_id, $plugin_definition, Connection $public) {
    parent::__construct($configuration, $plugin_id, $plugin_definition);

    // Dependency injection for public schema
    $this->public = $public;
  }

}
