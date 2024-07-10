<?php

namespace Drupal\tripal\TripalVocabTerms\Interfaces;

use Drupal\Component\Plugin\PluginInspectionInterface;

/**
 * Defines an interface for tripal collection plugins.
 */
interface TripalCollectionPluginInterface extends PluginInspectionInterface {

  /**
   * Creates the records needed for this collection.
   *
   * This must only be called once on this new collection instance that has
   * just been created by its collection plugin manager.
   */
  public function createRecord();

  /**
   * Destroys this collection.
   *
   * This must only be called once when on this existing collection that is
   * being removed from its collection plugin manager.
   */
  public function destroy();

  /**
   * Tests if this collection is valid or not.
   *
   * @return bool
   *   True if this collection is valid or false otherwise.
   */
  public function isValid();


  /**
   * Indicates if the underlying data store has a record for this collection.
   *
   * This function will be called by the collection plugin manager to ensure
   * that the record for this collection exists and if not allow it to create
   * it by calling the create() method or prevent addition of duplicate
   * entries.
   *
   * @return bool
   *  True if a record exists in the data store, False if not.
   */
  public function recordExists();

}
