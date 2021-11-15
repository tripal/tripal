<?php

namespace Drupal\tripal4\Plugin;

use Drupal\Component\Plugin\PluginInspectionInterface;

/**
 * Defines an interface for tripal storage plugins.
 */
interface CollectionPluginInterface extends PluginInspectionInterface {

  /**
   * Adds the given list of new schemas to this tripal storage plugin.
   *
   * @param list $schemas
   *   List of \Drupal\tripal4\TripalStorage\SchemaBase objects.
   */
  public function addSchemas($schemas);

  /**
   * Removes the given list of schemas from this tripal storage plugin.
   *
   * @param list $schemas
   *   List of \Drupal\tripal4\TripalStorage\SchemaBase objects.
   */
  public function removeSchemas($schemas);

  /**
   * Adds the given list of new records to this tripal storage plugin.
   *
   * @param list $records
   *   List of \Drupal\tripal4\TripalStorage\Record objects.
   */
  public function addRecords($records);

  /**
   * Saves the given list of records that already exist to this tripal storage plugin.
   *
   * @param list $records
   *   List of \Drupal\tripal4\TripalStorage\Record objects.
   */
  public function saveRecords($records);

  /**
   * Removes the given list of records from this tripal storage plugin.
   *
   * @param list $records
   *   List of \Drupal\tripal4\TripalStorage\Record objects.
   */
  public function removeRecords($records);
}
