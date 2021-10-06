<?php

namespace Drupal\tripal4\Plugin;

use Drupal\tripal4\Plugin\CollectionPluginInterface;
use Drupal\tripal4\Term

/**
 * Defines an interface for tripal vocabulary plugins.
 */
interface VocabularyInterface extends CollectionPluginInterface {

  /**
   * Returns list of id space collection names that is contained in this vocabulary.
   * 
   * @return array
   *   An array of id space collection name strings.
   */
  public function getIdSpaceNames();

  /**
   * Adds the id space with the given collection name to this vocabulary. The
   * given collection name must be a valid id space collection.
   *
   * @param string $idSpace
   *   The id space collection name.
   *
   * @return bool
   *   True on success or false otherwise.
   */
  public function addIdSpace($idSpace);

  /**
   * Removes the id space from this vocabulary with the given collection name.
   *
   * @param string $idSpace
   *   The id space collection name.
   *
   * @return bool
   *   True on success or false otherwise.
   */
  public function removeIdSpace($idspace);

}
