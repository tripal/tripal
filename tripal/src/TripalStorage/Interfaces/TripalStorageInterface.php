<?php

namespace Drupal\tripal\TripalStorage\Interfaces;

use Drupal\Component\Plugin\PluginInspectionInterface;

/**
 * Defines an interface for tripal storage plugins.
 */
interface TripalStorageInterface extends PluginInspectionInterface {

  /**
   * Adds the given array of new property types to this tripal storage plugin.
   *
   * @param array $types
   *   Array of \Drupal\tripal\TripalStorage\StoragePropertyTypeBase objects.
   */
  public function addTypes($types);

  /**
   * Removes the given array of property types from this tripal storage plugin.
   *
   * @param array $types
   *   Array of \Drupal\tripal\TripalStorage\StoragePropertyTypeBase objects.
   */
  public function removeTypes($types);

  /**
   * Returns a list of all property types added to this storage plugin type.
   * WARING! This could be a very expensive call!
   *
   * @return array
   *   Array of all \Drupal\tripal\Base\StoragePropertyTypeBase objects that
   *   have been added to this storage plugin type.
   */
  public function getTypes();

  /**
   * Inserts values in the field data store.
   *
   * The record Ids of the inserted records will be set in the property
   * value objects.
   *
   * @param array $values
   *   Associative array 5-levels deep.
   *   The 1st level is the field name (e.g. obi__organism).
   *   The 2nd level is the delta value (e.g. 0).
   *   The 3rd level is a field key name (i.e. record_id + value).
   *   The 4th level must contain the following three keys/value pairs
   *   - "value": a \Drupal\tripal\TripalStorage\StoragePropertyValue object
   *   - "type": a\Drupal\tripal\TripalStorage\StoragePropertyType object
   *   - "definition": a \Drupal\Field\Entity\FieldConfig object
   *   When the function returns, any values retreived from the data store
   *   will be set in the StoragePropertyValue object.
   *
   * @return bool
   *   True if successful. False otherwise.
   */
  public function insertValues(&$values) : bool;

  /**
   * Updates values in the field data store.
   *
   * @param array $values
   *   Associative array 5-levels deep.
   *   The 1st level is the field name (e.g. obi__organism).
   *   The 2nd level is the delta value (e.g. 0).
   *   The 3rd level is a field key name (i.e. record_id + value).
   *   The 4th level must contain the following three keys/value pairs
   *   - "value": a \Drupal\tripal\TripalStorage\StoragePropertyValue object
   *   - "type": a\Drupal\tripal\TripalStorage\StoragePropertyType object
   *   - "definition": a \Drupal\Field\Entity\FieldConfig object
   *   When the function returns, any values retreived from the data store
   *   will be set in the StoragePropertyValue object.
   *
   * @return bool
   *   True if successful. False otherwise.
   */
  public function updateValues($values) : bool;

  /**
   * Loads the values of the field data store.
   *
   * @param array $values
   *   Associative array 5-levels deep.
   *   The 1st level is the field name (e.g. obi__organism).
   *   The 2nd level is the delta value (e.g. 0).
   *   The 3rd level is a field key name (i.e. record_id + value).
   *   The 4th level must contain the following three keys/value pairs
   *   - "value": a \Drupal\tripal\TripalStorage\StoragePropertyValue object
   *   - "type": a\Drupal\tripal\TripalStorage\StoragePropertyType object
   *   - "definition": a \Drupal\Field\Entity\FieldConfig object
   *
   * @return bool
   *   True if successful. False otherwise.
   */
  public function loadValues(&$values) : bool;

  /**
   * Deletes the given array of property values from this tripal storage plugin.
   *
   * @param array $values
   *   Array of \Drupal\tripal\TripalStorage\StoragePropertyValue objects.
   *
   * @return bool
   *   True if successful. False otherwise.
   */
  public function deleteValues($values) : bool;

  /**
   * Finds and returns all property values stored in this storage plugin
   * implementation that matches the given match argument.
   *
   * @param mixed $match
   *   The value that is matched.
   *
   * @return array
   *   Array of all \Drupal\tripal\TripalStorage\StoragePropertyValue objects
   *   that match.
   */
  public function findValues($match);
}
