<?php

namespace Drupal\tripal\TripalStorage;

use Drupal\Core\Plugin\PluginBase;
use Drupal\tripal\TripalStorage\Interfaces\TripalStorageInterface;
use Drupal\Core\Form\FormStateInterface;
use Drupal\tripal\Services\TripalLogger;
use Drupal\Core\Plugin\ContainerFactoryPluginInterface;
use Symfony\Component\DependencyInjection\ContainerInterface;

/**
 * The base class for TripalStorage plugins.
 */
abstract class TripalStorageBase extends PluginBase implements TripalStorageInterface, ContainerFactoryPluginInterface {

  /**
   * The logger for reporting progress, warnings and errors to admin.
   *
   * @var \Drupal\tripal\Services\TripalLogger
   */
  protected $logger;

  /**
   * Keeps track of field definitions.
   *
   * @var array
   *   An associative array that contains all of the field definitions that
   *   have been added to this object. It is indexed by fieldName
   *   and the value is the field configuration object.
   *   This can be an instance of:
   *   - \Drupal\field\Entity\FieldStorageConfig.
   *   - \Drupal\field\Entity\FieldConfig.
   */
  protected $field_definitions = [];

  /**
   * Keepts track of property types for all fields.
   *
   * @var array
   *   An associative array that contains all of the property types that
   *   have been added to this object. It is indexed by fieldName -> key and the
   *   value is the Drupal\tripal\TripalStorage\StoragePropertyValue object.
   */
  protected $property_types = [];

  /**
   * Keeps track of which fields are fully cached by Drupal and which are not.
   *
   * This should be used by the specific storage backend during loadValues()
   * to improve performance by not looking up the values of fields which do
   * not need it.
   *
   * @var array
   *   This array will contain an entry (key) for each field where the value
   *   is either TRUE to indicate that all properties for this field are cached
   *   or FALSE if at least one property in the field is not cached.
   */
  protected $cached_fields = [];

  /**
   * Caches the default value for entity field caching from the configuration.
   *
   * @var bool
   */
  protected bool $default_is_required = TRUE;

  /**
   * Implements ContainerFactoryPluginInterface->create().
   *
   * Since we have implemented the ContainerFactoryPluginInterface this static
   * function will be called behind the scenes when a Plugin Manager uses
   * createInstance(). Specifically this method is used to determine the
   * parameters to pass to the constructor.
   *
   * @param \Symfony\Component\DependencyInjection\ContainerInterface $container
   *   The container holding the services.
   * @param array $configuration
   *   Configuration for the current plugin instance.
   * @param string $plugin_id
   *   The unique ID of the current plugin instance.
   * @param mixed $plugin_definition
   *   The definition of the current plugin instance.
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
   * Implements __construct().
   *
   * Since we have implemented the ContainerFactoryPluginInterface, the
   * constructor will be passed additional parameters added by the create()
   * function. This allows our plugin to use dependency injection without our
   * plugin manager service needing to worry about it.
   *
   * @param array $configuration
   *   The configuration for the current plugin instance.
   * @param string $plugin_id
   *   The unique ID for the current plugin instance.
   * @param mixed $plugin_definition
   *   The definition of the current plugin instance.
   * @param \Drupal\tripal\Services\TripalLogger $logger
   *   An instance of the Tripal logger to log messages to the admin.
   */
  public function __construct(array $configuration, $plugin_id, $plugin_definition, TripalLogger $logger) {
    parent::__construct($configuration, $plugin_id, $plugin_definition);

    $this->logger = $logger;

    $this->default_is_required = \Drupal::config('tripal.settings')
      ->get('tripal_entity_type.default_cache_backend_field_values') ?? TRUE;
  }

  /**
   * Adds the field definition to our storage plugin instance for later use.
   *
   * @{inheritdoc}
   */
  public function addFieldDefinition(string $field_name, object $field_definition) {

    if (!array_key_exists($field_name, $this->field_definitions)) {
      $this->field_definitions[$field_name] = [];
    }
    $this->field_definitions[$field_name] = $field_definition;

    return TRUE;
  }

  /**
   * Gets a previously added field definition.
   *
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
   * Adds property type objects for a specific field.
   *
   * @{inheritdoc}
   */
  public function addTypes(string $field_name, array $types) {

    // Index the types by their field name and key.
    foreach ($types as $index => $type) {
      if (!is_object($type)) {
        $this->logger->error('Type provided must be an object but instead index @index was a @type',
            ['@index' => $index, '@type' => gettype($type)]);
        return FALSE;
      }
      elseif (!is_subclass_of($type, 'Drupal\tripal\TripalStorage\StoragePropertyTypeBase')) {
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
   * Gets property type objects for all fields.
   *
   * @{inheritdoc}
   */
  public function getTypes() {
    return $this->property_types;
  }

  /**
   * Gets property type objects for a specific field.
   *
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
   * Removes property types for a specific field.
   *
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

  /**
   * {@inheritDoc}
   *
   * @see \Drupal\tripal\TripalStorage\Interfaces\TripalStorageInterface::getStoredValues()
   */
  public function getStoredValues() {

    /** @var \Drupal\Core\Field\FieldTypePluginManager $field_type_manager **/
    $field_type_manager = \Drupal::service('plugin.manager.field.field_type');

    $props = $this->getStoredTypes();

    $values = [];
    foreach ($props as $field_name => $keys) {
      foreach ($keys as $key => $prop_type) {
        $field_definition = $this->field_definitions[$field_name];
        $configuration = [
          'field_definition' => $field_definition,
          'name' => $field_name,
          'parent' => NULL,
        ];
        $instance = $field_type_manager->createInstance($field_definition->getType(), $configuration);
        $field_class = get_class($instance);

        $prop_value = new StoragePropertyValue($field_definition->getTargetEntityTypeId(),
            $field_class::$id, $prop_type->getKey(), $prop_type->getTerm()->getTermId(), NULL);
        $values[$field_name][0][$key]['value'] = $prop_value;
      }
    }
    return $values;
  }

  /**
   * A helper function to clone a values array.
   *
   * @param array $values
   *   An array of property values.
   */
  protected function cloneValues($values) {
    $copy = [];
    foreach ($values as $field_name => $deltas) {
      $copy[$field_name] = [];
      foreach ($deltas as $delta => $keys) {
        $copy[$field_name][$delta] = [];
        foreach ($keys as $key => $value) {
          $copy[$field_name][$delta][$key] = [];
          $copy[$field_name][$delta][$key]['value'] = clone $value['value'];
        }
      }
    }
    return $copy;
  }

  /**
   * Sets the values for a field to be empty.
   *
   * If the delta value doesn't exist in the values array then a new values
   * array is added.
   *
   * @param array $values
   *   An array of property values.
   * @param string $field_name
   *   The name of the field to addd an item to.
   * @param int $delta
   *   The index of the specific values item to reset.
   */
  protected function resetValuesItem(&$values, $field_name, $delta) {
    $is_new = FALSE;

    // Is the caller wanting to add a new element? If so, add one.
    $num_items = count($values[$field_name]);
    if ($delta > $num_items - 1) {
      $values[$field_name][$delta] = [];
      $is_new = TRUE;
    }

    // Reset the values to NULL. Use the first values element
    // to get the keys.
    foreach ($values[$field_name][0] as $key => $details) {
      if ($is_new) {
        $values[$field_name][$delta][$key] = [];
        $values[$field_name][$delta][$key]['value'] = clone $details['value'];
      }
      $values[$field_name][$delta][$key]['value']->setValue(NULL);
    }
  }

  /**
   * {@inheritDoc}
   */
  public function markPropertiesForCaching(string $field_name, array &$prop_types) {

    // We want to keep track of whether all the properties are cached (TRUE).
    $all_cached = TRUE;

    // For each of the property types, we want to check if they should be
    // saved in the Drupal field tables.
    foreach ($prop_types as $k => $prop_type) {
      $key = $prop_type->getKey();

      // Now actually perform the cache.
      $is_required = $this->isDrupalStoreByFieldNameKey($field_name, $key, $prop_type);
      // And set it on the property type.
      $prop_types[$k]->setCacheStatus($is_required);

      if (!$is_required) {
        $all_cached = FALSE;
      }
    }

    $this->cached_fields[$field_name] = $all_cached;
  }

  /**
   * {@inheritDoc}
   */
  public function isDrupalStoreByFieldNameKey(string $field_name, string $key, object|null $property_type = NULL): bool|null {

    if ($property_type === NULL) {
      $property_type = $this->getPropertyType($field_name, $key);
      if ($property_type === NULL) {
        return NULL;
      }
    }
    $storage_settings = $property_type->getStorageSettings();

    // Get the default from the configuration.
    $is_required = $this->default_is_required;

    // Any field that stores a base record id, a primary key, or a foreign key
    // link is required. This takes precedence and cannot be overridden.
    if (array_key_exists('action', $storage_settings) and (
      ($storage_settings['action'] == 'store_id') or
      ($storage_settings['action'] == 'store_pkey') or
      ($storage_settings['action'] == 'store_link')
    )) {
      $is_required = TRUE;
    }
    // For any other fields that have 'drupal_exclude' set, we want to ensure it
    // is excluded regardless of the default configuration. To force exclusion
    // of the field from entity storage, add `drupal_exclude: true` in the
    // `storage_settings` of the field definition in its
    // tripalfield_collection*_chado.yml file.
    elseif ((array_key_exists('drupal_exclude', $storage_settings)) and ($storage_settings['drupal_exclude'] === TRUE)) {
      $is_required = FALSE;
    }
    // Stay compatible with the original behavior and allow to force drupal
    // storage. To force inclusion of the field into Entity storage, add
    // `drupal_store: true` in the section `storage_settings` of the field
    // definition in its tripal field collection.
    elseif ((array_key_exists('drupal_store', $storage_settings)) and ($storage_settings['drupal_store'] === TRUE)) {
      $is_required = TRUE;
    }

    return $is_required;
  }

  /**
   * {@inheritDoc}
   *
   * @see \Drupal\tripal\TripalStorage\Interfaces\TripalStorageInterface::publishForm()
   */
  public function publishForm($form, FormStateInterface &$form_state) {
    return [];
  }

  /**
   * {@inheritDoc}
   *
   * @see \Drupal\tripal\TripalStorage\Interfaces\TripalStorageInterface::publishFormValidate()
   */
  public function publishFormValidate($form, FormStateInterface &$form_state) {

  }

  /**
   * {@inheritDoc}
   *
   * @see \Drupal\tripal\TripalStorage\Interfaces\TripalStorageInterface::publishFromSubmit()
   */
  public function publishFromSubmit($form, FormStateInterface &$form_state) {

  }

}
