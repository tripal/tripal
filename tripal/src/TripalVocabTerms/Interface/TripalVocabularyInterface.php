<?php

namespace Drupal\tripal\TripalVocabTerms\Interface;

use Drupal\tripal\TripalVocabTerms\Interface\TripalCollectionPluginInterface;
use Drupal\tripal\TripalVocabTerms\TripalTerm;

/**
 * Defines an interface for tripal vocabulary plugins.
 */
interface TripalVocabularyInterface extends TripalCollectionPluginInterface {

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

  /**
   * Returns the terms in this vocabulary whose names match the given name.
   * Matches can only be exact or a substring depending on the given flag. The
   * default is to only return exact matches.
   *
   * @param string $name
   *   The name.
   *
   * @param bool $exact
   *   True to only include exact matches else include all substring matches.
   *
   * @return array
   *   Array of matching Drupal\tripal\TripalVocabTerms\TripalTerm instances.
   */
  public function getTerms($name,$exact = True);

  /**
   * Returns the URL of this vocabulary.
   *
   * @return string
   *   The URL.
   */
  public function getURL();

  /**
   * Sets the URL of this vocabulary to the given URL.
   *
   * @param string $url
   *   The URL.
   */
  public function setURL($url);

  /**
   * Returns the description of this vocabulary.
   *
   * @return string
   *   The description.
   */
  public function getDescription();

  /**
   * Sets the description of this vocabulary to the given description.
   *
   * @param string $description
   *   The description.
   */
  public function setDescription($description);

}
