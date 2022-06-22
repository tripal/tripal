<?php

namespace Drupal\tripal\TripalStorage\Interfaces;

use Drupal\Component\Plugin\PluginInspectionInterface;

/**
 * Defines an interface for tripal storage plugins.
 */
interface TripalStoragePluginInterface extends PluginInspectionInterface {

  /**
   * Adds the given array of new property types to this tripal storage plugin.
   *
   * @param array $schemas
   *   Array of \Drupal\tripal\Base\StoragePropertyTypeBase objects.
   */
  public function addTypes($types);

  /**
   * Removes the given array of property types from this tripal storage plugin.
   *
   * @param array $schemas
   *   Array of \Drupal\tripal\Base\StoragePropertyTypeBase objects.
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
   * Inserts the given array of new property values to this tripal storage
   * plugin.
   *
   * @param array $records
   *   Array of \Drupal\tripal\TripalStorage\StoragePropertyValue objects.
   */
  public function insertValues($values);

  /**
   * Updates the given array of property values that already exist to this
   * tripal storage plugin.
   *
   * @param array $records
   *   Array of \Drupal\tripal\TripalStorage\StoragePropertyValue objects.
   */
  public function updateValues($values);

  /**
   * Loads the values of the given array of property values from this tripal
   * storage plugin.
   *
   * @param array $records
   *   Array of \Drupal\tripal\TripalStorage\StoragePropertyValue objects.
   */
  public function loadValues($values);

  /**
   * Deletes the given array of property values from this tripal storage plugin.
   *
   * @param array $records
   *   Array of \Drupal\tripal\TripalStorage\StoragePropertyValue objects.
   */
  public function deleteValues($values);
}
