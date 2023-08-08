<?php

namespace Drupal\tripal\TripalStorage;

use Drupal\Core\Plugin\PluginBase;
use Drupal\tripal\TripalStorage\Interfaces\TripalStorageInterface;

use Drupal\tripal\Services\TripalLogger;
use Drupal\Core\Plugin\ContainerFactoryPluginInterface;
use Symfony\Component\DependencyInjection\ContainerInterface;

abstract class TripalStorageBase extends PluginBase implements TripalStorageInterface, ContainerFactoryPluginInterface {

  /**
   * The logger for reporting progress, warnings and errors to admin.
   *
   * @var Drupal\tripal\Services\TripalLogger
   */
  protected $logger;

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
      $container->get('tripal.logger'),
    );
  }

  /**
   * Implements __contruct().
   *
   * Since we have implemented the ContainerFactoryPluginInterface, the constructor
   * will be passed additional parameters added by the create() function. This allows
   * our plugin to use dependency injection without our plugin manager service needing
   * to worry about it.
   *
   * @param array $configuration
   * @param string $plugin_id
   * @param mixed $plugin_definition
   * @param Drupal\tripal\Services\TripalLogger $logger
   */
  public function __construct(array $configuration, $plugin_id, $plugin_definition, TripalLogger $logger) {
    parent::__construct($configuration, $plugin_id, $plugin_definition);

    $this->logger = $logger;
  }
}
