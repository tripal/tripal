<?php

namespace Drupal\tripal_chado\Plugin\TripalStorage;

use Drupal\Core\Plugin\PluginBase;
use Drupal\tripal\TripalStorage\Interfaces\TripalStorageInterface;
use Symfony\Component\Validator\ConstraintViolation;



/**
 * Chado implementation of the TripalStorageInterface.
 *
 * @TripalStorage(
 *   id = "tripal_default_storage",
 *   label = @Translation("Tripal Default Storage"),
 *   description = @Translation("A storage backend for Tripal that simply sits above the default SqlContentEntityStorage. Use this only when no other appropriate backend is available."),
 * )
 */
class TripalDefaultStorage extends PluginBase implements TripalStorageInterface {

  /**
   * An associative array that contains all of the property types that
   * have been added to this object. It is indexed by entityType ->
   * fieldName -> key and the value is the
   * Drupal\tripal\TripalStoreage\StoragePropertyValue object.
   *
   * @var array
   */
  protected $property_types = [];

  /**
   * An associative array that holds the data for mapping an
   * entityTypes to Chado tables.  It is indexed by entityType and the
   * value is the object containing the mapping information.
   *
   * @var array
   */
  protected $type_mapping = [];

  /**
   * An associative array that holds the data for mapping a
   * fieldType key to a Chado table column for a given entity.  It is indexed
   * by entityType -> entityID and the value is the object containing the
   * mapping information.
   *
   * @var array
   */
  protected $id_mapping = [];

  /**
   * {@inheritDoc}
   */
  public function addTypes($types) {
    $logger = \Drupal::service('tripal.logger');

    // Index the types by their entity type, field type and key.
    foreach ($types as $index => $type) {
      if (!is_object($type) OR !is_subclass_of($type, 'Drupal\tripal\TripalStorage\StoragePropertyTypeBase')) {
        $logger->error('Type provided must be an object extending StoragePropertyTypeBase. Instead index @index was this: @type',
            ['@index' => $index, '@type' => print_r($type, TRUE)]);
        return FALSE;
      }

      $field_name = $type->getFieldType();
      $entity_type = $type->getEntityType();
      $key = $type->getKey();

      if (!array_key_exists($entity_type, $this->property_types)) {
        $this->property_types[$entity_type] = [];
      }
      if (!array_key_exists($field_name, $this->property_types[$entity_type])) {
        $this->property_types[$entity_type][$field_name] = [];
      }
      if (array_key_exists($key, $this->property_types[$entity_type])) {
        $logger->error('Cannot add a property type, "@prop", as it already exists',
            ['@prop' => $entity_type . '.' . $field_name . '.' . $key]);
        return FALSE;
      }
      $this->property_types[$entity_type][$field_name][$key] = $type;
    }
  }
  /**
   * {@inheritDoc}
   */

  public function updateValues($values): bool {
  }
  /**
   * {@inheritDoc}
   */

  public function deleteValues($values): bool {
    $logger = \Drupal::service('tripal.logger');
    $logger->warning('The TripalDefaultStorage::deleteValues() function is currently not implemented');
  }
  /**
   * {@inheritDoc}
   */

  public function findValues($match) {
    $logger = \Drupal::service('tripal.logger');
    $logger->warning('The TripalDefaultStorage::findValues() function is currently not implemented');

  }
  /**
   * {@inheritDoc}
   */

  public function insertValues($values): bool {
    $logger = \Drupal::service('tripal.logger');
    $logger->warning('The TripalDefaultStorage::insertValues() function is currently not implemented');
  }
  /**
   * {@inheritDoc}
   */

  public function getTypes() {
    $types = [];
    foreach ($this->property_types as $field_types) {
      foreach ($field_types as $keys) {
        foreach ($keys as $type) {
          $types[] = $type;
        }
      }
    }
    return $types;
  }
  /**
   * {@inheritDoc}
   */

  public function loadValues($values): bool {
    $logger = \Drupal::service('tripal.logger');
    $logger->warning('The TripalDefaultStorage::loadValues() function is currently not implemented');
  }
  /**
   * {@inheritDoc}
   */

  public function removeTypes($types) {
    foreach ($types as $type) {
      $entity_type = $type->getEntityType();
      $field_type = $type->getFieldType();
      $key = $type->getKey();
      if (array_key_exists($entity_type, $this->property_types)) {
        if (array_key_exists($field_type, $this->property_types[$entity_type])) {
          if (array_key_exists($key, $this->property_types[$entity_type])) {
            unset($this->property_types[$entity_type][$field_type][$key]);
          }
        }
      }
    }
  }
  /**
   * {@inheritDoc}
   */

  public function validateValues($values) {
    // Nothin to do here. We'll let the SqlContentEntityStorage handle
    // validations.
    $violations = [];
    return $violations;
  }

}