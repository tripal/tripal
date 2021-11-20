<?php

namespace Drupal\tripal4\Plugin;

use Drupal\Component\Plugin\PluginInspectionInterface;

/**
 * Defines an interface for tripal storage plugins.
 */
interface CollectionPluginInterface extends PluginInspectionInterface {

  /**
   * Adds the given list of new property types to this tripal storage plugin.
   *
   * @param list $schemas
   *   List of \Drupal\tripal4\Base\StoragePropertyTypeBase objects.
   */
  public function addTypes($types);

  /**
   * Removes the given list of property types from this tripal storage plugin.
   *
   * @param list $schemas
   *   List of \Drupal\tripal4\Base\StoragePropertyTypeBase objects.
   */
  public function removeTypes($types);

  /**
   * Adds the given list of new property values to this tripal storage plugin.
   *
   * @param list $records
   *   List of \Drupal\tripal4\TripalStorage\StoragePropertyValue objects.
   */
  public function addValues($values);

  /**
   * Saves the given list of property values that already exist to this tripal
   * storage plugin.
   *
   * @param list $records
   *   List of \Drupal\tripal4\TripalStorage\StoragePropertyValue objects.
   */
  public function saveValues($values);

  /**
   * Loads the values of the given list of property values from this tripal
   * storage plugin.
   *
   * @param list $records
   *   List of \Drupal\tripal4\TripalStorage\StoragePropertyValue objects.
   */
  public function loadValues($values);

  /**
   * Removes the given list of property values from this tripal storage plugin.
   *
   * @param list $records
   *   List of \Drupal\tripal4\TripalStorage\StoragePropertyValue objects.
   */
  public function removeValues($values);
}
