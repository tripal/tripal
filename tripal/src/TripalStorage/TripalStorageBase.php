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
   * An associative array that contains all of the field defitions that
   * have been added to this object. It is indexed by fieldName
   * and the value is the field configuration object.
   * This can be an instance of:
   *   \Drupal\field\Entity\FieldStorageConfig or
   *   \Drupal\field\Entity\FieldConfig
   *
   * @var array
   */
  protected $field_definitions = [];

  /**
   * An associative array that contains all of the property types that
   * have been added to this object. It is indexed by entityType ->
   * fieldName -> key and the value is the
   * Drupal\tripal\TripalStorage\StoragePropertyValue object.
   *
   * @var array
   */
  protected $property_types = [];

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
  public function addFieldDefinition(string $field_name, object $field_definition) {

    $this->field_definitions[$field_name] = $field_definition;

    return TRUE;
  }

  /**
   * @{inheritdoc}
   */
  public function getFieldDefinition(string $field_name) {

    if (array_key_exists($field_name, $this->field_definitions)) {
      if (is_object($this->field_definitions[$field_name])) {
        return $this->field_definitions[$field_name];
      }
    }

    return FALSE;
  }

  /**
   * @{inheritdoc}
   */
  public function addTypes(string $field_name, array $types) {

    // Index the types by their entity type, field type and key.
    foreach ($types as $index => $type) {
      if (!is_object($type)) {
        $this->logger->error('Type provided must be an object but instead index @index was a @type',
            ['@index' => $index, '@type' => gettype($type)]);
        return FALSE;
      }
      elseif(!is_subclass_of($type, 'Drupal\tripal\TripalStorage\StoragePropertyTypeBase')) {
        $this->logger->error('Type provided must be an object extending StoragePropertyTypeBase. Instead index @index was of type: @type',
            ['@index' => $index, '@type' => get_class($type)]);
        return FALSE;
      }

      $key = $type->getKey();

      if (!array_key_exists($field_name, $this->property_types)) {
        $this->property_types[$field_name] = [];
      }
      $this->property_types[$field_name][$key] = $type;

    }
  }

  /**
   * @{inheritdoc}
   */
  public function getTypes() {
    return $this->property_types;
  }

  /**
   * @{inheritdoc}
   */
  public function getPropertyType(string $field_name, string $key) {

    if (array_key_exists($field_name, $this->property_types)) {
      if (array_key_exists($key, $this->property_types[$field_name])) {
        return $this->property_types[$field_name][$key];
      }
    }

    return NULL;
  }

  /**
   * @{inheritdoc}
   */
  public function removeTypes(string $field_name, array $types) {

    foreach ($types as $type) {
      $key = $type->getKey();

      if (array_key_exists($field_name, $this->property_types)) {
        if (array_key_exists($key, $this->property_types[$field_name])) {
          unset($this->property_types[$field_name][$key]);
        }
      }

    }
  }
}
