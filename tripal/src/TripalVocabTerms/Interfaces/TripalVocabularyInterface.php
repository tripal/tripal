<?php

namespace Drupal\tripal\TripalVocabTerms\Interfaces;

use Drupal\tripal\TripalVocabTerms\Interfaces\TripalCollectionPluginInterface;
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
   *
   * @return bool
   *   True if the value was set or false otherwise.
   */
  public function setURL($url);


  /**
   * Returns the namespace of the vocabulary
   *
   * This should be identical to the name of the collection, and
   * therefore, there is no setter function.
   *
   * @return string $namespace
   *   The namespace of the vocabulary.
   */
  public function getNameSpace();


  /**
   * Sets the label for the vocabulary.
   *
   * This is the human readable proper name of the vocabulary.
   *
   * Note that the name of the collection serves as the namespace of the vocabulary.
   *
   * @param string $label
   *   The name of the vocabulary.
   *
   * @return bool
   *   True on success or false otherwise.
   *
   * @return bool
   *   True if the value was set or false otherwise.
   */
  public function setLabel($label);


  /**
   * Returns the label of the vocabulary.
   *
   * This is the human readable proper name of the vocabulary.
   *
   * Note that the name of the collection serves as the namespace of the vocabulary.
   *
   * @return string $label
   *   The name of the vocabulary.
   */
  public function getLabel();

}
