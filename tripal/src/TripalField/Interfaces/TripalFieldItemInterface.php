<?php

namespace Drupal\tripal\TripalField\Interfaces;

use Drupal\Core\Field\FieldItemInterface;
use Drupal\Core\Field\FieldStorageDefinitionInterface;

interface TripalFieldItemInterface extends FieldItemInterface {

  /**
   * Returns the tripal storage plugin id for this field.
   *
   * @return string
   *   The tripal storage plugin id.
   */
  public function tripalStorageId();

  /**
   * Returns the property types required by this field.
   *
   * @param FieldStorageDefinitionInterface $field_definition
   *   The entity type id of this field's entity.
   *
   * @return array
   *   Array of \Drupal\tripal\TripalStorage\StoragePropertyTypeBase property types.
   */
  public static function tripalTypes(FieldStorageDefinitionInterface $field_definition);

  /**
   * Returns an empty template array of all property values this field uses for loading and saving.
   *
   * @return array
   *   Array of \Drupal\tripal\TripalStorage\StoragePropertyValue property value templates.
   */
  public function tripalValuesTemplate();

  /**
   * Loads the values from the given array of properties to the given entity.
   *
   * @param array $properties
   *   Array of \Drupal\tripal\TripalStorage\\StoragePropertyValue objects.
   *
   * @param \Drupal\tripal\TripalStorage\TripalEntityBase $entity
   *   The entity.
   */
  public function tripalLoad($properties, $entity);

  /**
   * Saves the values to the given array of properties from the given entity.
   *
   * @param array $properties
   *   Array of \Drupal\tripal\TripalStorage\\StoragePropertyValue objects.
   *
   * @param \Drupal\tripal\TripalStorage\TripalEntityBase $entity
   *   The entity.
   */
  public function tripalSave($properties, $entity);

  /**
   * Clears all field values from the given entity that is associated with this field.
   *
   * @param \Drupal\tripal\TripalStorage\TripalEntityBase $entity
   *   The entity.
   */
  public function tripalClear($entity);
}
