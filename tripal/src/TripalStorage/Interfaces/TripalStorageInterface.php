<?php

namespace Drupal\tripal4\Plugin;

use Drupal\Component\Plugin\PluginInspectionInterface;

/**
 * Defines an interface for tripal storage plugins.
 */
interface CollectionPluginInterface extends PluginInspectionInterface {

  /**
   * Adds the given array of new property types to this tripal storage plugin.
   *
   * @param array $schemas
   *   Array of \Drupal\tripal4\Base\StoragePropertyTypeBase objects.
   */
  public function addTypes($types);

  /**
   * Removes the given array of property types from this tripal storage plugin.
   *
   * @param array $schemas
   *   Array of \Drupal\tripal4\Base\StoragePropertyTypeBase objects.
   */
  public function removeTypes($types);

  /**
   * Inserts the given array of new property values to this tripal storage
   * plugin.
   *
   * @param array $records
   *   Array of \Drupal\tripal4\TripalStorage\StoragePropertyValue objects.
   */
  public function insertValues($values);

  /**
   * Updates the given array of property values that already exist to this
   * tripal storage plugin.
   *
   * @param array $records
   *   Array of \Drupal\tripal4\TripalStorage\StoragePropertyValue objects.
   */
  public function updateValues($values);

  /**
   * Loads the values of the given array of property values from this tripal
   * storage plugin.
   *
   * @param array $records
   *   Array of \Drupal\tripal4\TripalStorage\StoragePropertyValue objects.
   */
  public function loadValues($values);

  /**
   * Deletes the given array of property values from this tripal storage plugin.
   *
   * @param array $records
   *   Array of \Drupal\tripal4\TripalStorage\StoragePropertyValue objects.
   */
  public function deleteValues($values);
}
