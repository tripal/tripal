<?php

namespace Drupal\tripal\TripalField;

use Drupal\Core\Field\FieldItemInterface;

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
   * @return array
   *   Array of \Drupal\tripal4\Base\StoragePropertyTypeBase property types.
   */
  public static function tripalTypes();

  /**
   * Returns an empty template array of all property values this field uses for loading and saving.
   *
   * @return array
   *   Array of \Drupal\tripal4\TripalStorage\StoragePropertyValue property value templates.
   */
  public function tripalValuesTemplate();

  /**
   * Loads the values from the given array of properties to the given entity.
   *
   * @param array $properties
   *   Array of \Drupal\tripal4\TripalStorage\StoragePropertyValue objects.
   *
   * @param \Drupal\tripal4\Base\TripalEntityBase $entity
   *   The entity.
   */
  public function tripalLoad($properties,$entity);

  /**
   * Saves the values to the given array of properties from the given entity.
   *
   * @param array $properties
   *   Array of \Drupal\tripal4\TripalStorage\StoragePropertyValue objects.
   *
   * @param \Drupal\tripal4\Base\TripalEntityBase $entity
   *   The entity.
   */
  public function tripalSave($properties,$entity);

  /**
   * Clears all field values from the given entity that is associated with this field.
   *
   * @param \Drupal\tripal4\Base\TripalEntityBase $entity
   *   The entity.
   */
  public function tripalClear($entity);
}
