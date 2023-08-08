<?php

namespace Drupal\tripal\TripalStorage;

use Drupal\Core\Plugin\PluginBase;
use Drupal\tripal\TripalStorage\Interfaces\TripalStorageInterface;

use Drupal\tripal\Services\TripalLogger;
use Drupal\tripal\TripalField\TripalFieldItemBase;
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
   * An associative array that contains all of the field defitions that
   * have been added to this object. It is indexed by entityType ->
   * fieldName and the value is the FieldType object associated with that
   * field name.
   *
   * @var array
   */
  protected $field_definitions = [];

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

  /**
   * @{inheritdoc}
   */
  public function addFieldDefinition(string $bundle_name, string $field_name, TripalFieldItemBase $field_definition) {
    if (!array_key_exists($bundle_name, $this->field_definitions)) {
      $this->field_definitions[$bundle_name] = [];
    }
    if (!array_key_exists($field_name, $this->field_definitions[$bundle_name])) {
      $this->field_definitions[$bundle_name][$field_name] = [];
    }
    $this->field_definitions[$bundle_name][$field_name] = $field_definition;

    return TRUE;
  }

  /**
   * @{inheritdoc}
   */
  public function getFieldDefinition(string $bundle_name, string $field_name) {
    if (array_key_exists($bundle_name, $this->field_definitions)) {
      if (array_key_exists($field_name, $this->field_definitions[$bundle_name])) {
        if (is_object($this->field_definitions[$bundle_name][$field_name])) {
          return $this->field_definitions[$bundle_name][$field_name];
        }
      }
    }
    return FALSE;
  }
}
