<?php

namespace Drupal\tripal\Entity;

use Drupal\Core\Entity\ContentEntityInterface;
use Drupal\Core\Entity\EntityChangedInterface;

/**
 * Provides an interface for defining Tripal Vocabulary IDSpace entities.
 *
 * @ingroup tripal
 */
interface TripalVocabSpaceInterface extends ContentEntityInterface, EntityChangedInterface {

  /**
   * Gets the Tripal Vocabulary IDSpace name.
   *
   * @return string
   *   Name of the Tripal Vocabulary IDSpace.
   */
  public function getIDSpace();

  /**
   * Sets the Tripal Vocabulary IDSpace name.
   *
   * @param string $idspace
   *   The Tripal Vocabulary IDSpace name.
   *
   * @return \Drupal\tripal\Entity\TripalVocabSpaceInterface
   *   The called Tripal Vocabulary IDSpace entity.
   */
  public function setIDSpace($idspace);

  /**
   * Gets the Tripal Vocabulary IDSpace URL prefix.
   *
   * @return string
   *   The URL Prefix.
   */
  public function getURLPrefix();

  /**
   * Sets the Tripal Vocabulary IDSpace URL prefix.
   *
   * @param string $urlprefix
   *   The URL Prefix.
   *
   * @return \Drupal\tripal\Entity\TripalVocabSpaceInterface
   *   The called Tripal Vocabulary IDSpace entity.
   */
  public function setURLPrefix($urlprefix);

  /**
   * Retrieves the unique identifier for the linked Tripal Vocabulary.
   *
   * @return int
   *   The unique identifier for the linked Tripal Vocabulary.
   */
  public function getVocabID();

  /**
   * Link an existing Tripal Vocabulary to this IDSpace.
   *
   * @param int $vocab_id
   *   The internal unique identifier for the TripalVocab to link.
   *
   * @return \Drupal\tripal\Entity\TripalVocabSpaceInterface
   *   The current term is returned to allow chaining of commands.
   */
  public function setVocabID($vocab_id);

  /**
   * The linked Tripal Vocabulary.
   *
   * @return \Drupal\tripal\Entity\TripalVocabInterface
   *   The linked Tripal Vocabulary.
   */
  public function getVocab();

  /**
   * Gets the Tripal Vocabulary IDSpace creation timestamp.
   *
   * @return int
   *   Creation timestamp of the Tripal Vocabulary IDSpace.
   */
  public function getCreatedTime();

  /**
   * Sets the Tripal Vocabulary IDSpace creation timestamp.
   *
   * @param int $timestamp
   *   The Tripal Vocabulary IDSpace creation timestamp.
   *
   * @return \Drupal\tripal\Entity\TripalVocabSpaceInterface
   *   The called Tripal Vocabulary IDSpace entity.
   */
  public function setCreatedTime($timestamp);

}
