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
   * Allows child field items to add default types.
   *
   * This function should be called in the tripalTypes implementation of
   * any child class to ensure that default types needed for all Tripal fields
   * get added.
   *
   * @param string $entity_type_id
   *   The entity type id of this field's entity.
   *
   * @param string $field_type
   *   The name of the field to which default types are needed..
   *
   * @return array
   *   Array of \Drupal\tripal\TripalStorage\StoragePropertyTypeBase property types.
   */
  public static function defaultTripalTypes($entity_type_id, $field_type);

  /**
   * Allows child field items to add default values to the template..
   *
   * This function should be called in the tripalValuesTemplate implementation
   * of any child class to ensure that default types needed for all Tripal
   * fields are known.
   *
   * @param string $entity_type_id
   *   The entity type id of this field's entity.
   *
   * @param string $field_type
   *   The name of the field to which default types are needed.
   *
   * @param string $entity_id
   *   The Id of the entity that the value belongs to.
   *
   * @return array
   *   Array of \Drupal\tripal\TripalStorage\StoragePropertyTypeBase property types.
   */
  public static function defaultTripalValuesTemplate($entity_type_id, $field_type, $entity_id);

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
   *
   * @param \Drupal\tripal\TripalField\Interfaces\TripalFieldItemInterface $field_item
   *   The field item for which properties should be saved.
   *
   * @param string $field_name
   *   The name of the field.
   *
   * @param array $properties
   *   Array of \Drupal\tripal\TripalStorage\\StoragePropertyValue objects.
   *
   * @param \Drupal\tripal\TripalStorage\TripalEntityBase $entity
   *   The entity.
   */
  public function tripalLoad($field_item, $field_name, $properties, $entity);

  /**
   * Saves the values to the given array of properties from the given entity.
   *
   * @param \Drupal\tripal\TripalField\Interfaces\TripalFieldItemInterface $field_item
   *   The field item for which properties should be saved.
   *
   * @param string $field_name
   *   The name of the field.
   *
   * @param array $properties
   *   Array of \Drupal\tripal\TripalStorage\\StoragePropertyValue objects.
   *
   * @param \Drupal\tripal\TripalStorage\TripalEntityBase $entity
   *   The entity.
   */
  public function tripalSave($field_item, $field_name, $properties, $entity);

  /**
   * Clears all field values from the given entity.
   *
   * This is to prevent Drupal from storing field values when they are
   * being stored in the Tripal field storage backend.
   *
   * @param \Drupal\tripal\TripalField\Interfaces\TripalFieldItemInterface $field_item
   *   The field item for which properties should be saved.
   *
   * @param string $field_name
   *   The name of the field.
   *
   * @param array $properties
   *   Array of \Drupal\tripal\TripalStorage\\StoragePropertyValue objects.
   *
   * @param \Drupal\tripal\TripalStorage\TripalEntityBase $entity
   *   The entity.
   */
  public function tripalClear($field_item, $field_name, $properties, $entity);
}
