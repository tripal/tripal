<?php

namespace Drupal\tripal\Entity;

use Drupal\Core\Entity\ContentEntityInterface;
use Drupal\Core\Entity\EntityChangedInterface;
use Drupal\user\EntityOwnerInterface;

/**
 * Provides an interface for defining Controlled Vocabulary entities.
 *
 * @ingroup tripal
 */
interface TripalVocabInterface extends ContentEntityInterface, EntityChangedInterface {

  /**
   * Retrieves the internal unique identifier for this term.
   *
   * @return int
   *   The internal unique identifier.
   */
  public function getID();

  /**
   * Retrieves the human-readable label for this term.
   *
   * @return string
   *   The human-readable label for the term.
   */
  public function getLabel();

  /**
   * Retrieves the human-readable vocabulary name.
   *
   * @return string
   *   The human-readable vocabulary name.
   */
  public function getName();

  /**
   * Sets the human-readable vocabulary name.
   *
   * @param string $name
   *   The human-readable name of the Tripal vocabulary.
   *
   * @return \Drupal\tripal\Entity\TripalVocabInterface
   *   The current vocabulary is returned to allow chaining of commands.
   */
  public function setName($name);

  /**
   * Retrieves the namespace of the vocabulary.
   *
   * @return string
   *   The vocabulary namespace.
   */
  public function getNamespace();

  /**
   * Sets the vocabulary namespace.
   *
   * @param string $name
   *   The namespace of the Tripal vocabulary.
   *
   * @return \Drupal\tripal\Entity\TripalVocabInterface
   *   The current vocabulary is returned to allow chaining of commands.
   */
  public function setNamespace($name);

  /**
   * Retrieves the description of the vocabulary.
   *
   * @return string
   *   The description of the vocabulary.
   */
  public function getDescription();

  /**
   * Sets the description of the vocabulary.
   *
   * @param string $description
   *   The description of the Tripal vocabulary.
   *
   * @return \Drupal\tripal\Entity\TripalVocabInterface
   *   The current vocabulary is returned to allow chaining of commands.
   */
  public function setDescription($description);

  /**
   * Retrieves the URL reference for the vocabulary.
   *
   * @return string
   *   The vocabulary URL.
   */
  public function getURL();

  /**
   * Sets the vocabulary URL reference.
   *
   * @param string $url
   *   The URL referencing the ontology described by this Tripal Vocabulary.
   *
   * @return \Drupal\tripal\Entity\TripalVocabInterface
   *   The current vocabulary is returned to allow chaining of commands.
   */
  public function setURL($url);

  /**
   * Retrieves the time the vocabulary was created.
   *
   * @return string
   *   The timestamp indicating when the vocabulary was created.
   */
  public function getCreatedTime();

  /**
   * Sets the timestamp the vocabulary was created.
   *
   * @param string $timestamp
   *   The UNIX timestamp indicating when the vocabulary was created.
   *
   * @return \Drupal\tripal\Entity\TripalVocabInterface
   *   The current vocabulary is returned to allow chaining of commands.
   */
  public function setCreatedTime($timestamp);

  /**
   * Retrieves the time the vocabulary was last changed.
   *
   * @return string
   *   The timestamp indicating when the vocabulary was last changed.
   */
  public function getChangedTime();

  /**
   * Sets the timestamp the vocabulary was last changed.
   *
   * @param string $timestamp
   *   The UNIX timestamp indicating when the vocabulary was last changed.
   *
   * @return \Drupal\tripal\Entity\TripalVocabInterface
   *   The current vocabulary is returned to allow chaining of commands.
   */
  public function setChangedTime($timestamp);

  /**
   * Retrives the number of terms which reference this vocabulary.
   *
   * @return int
   *   The number of terms which reference this vocabulary.
   */
  public function getNumberofTerms();

  /**
   * Retrieves the full set of details for the Tripal Vocabulary.
   *
   * @return array
   *   An array of properties where the key is the property name and the value
   *   is the string value.
   */
  public function getDetails();

}
