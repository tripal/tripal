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

  // Add get/set methods for your configuration properties here.

  /**
   * Gets the Controlled Vocabulary creation timestamp.
   *
   * @return int
   *   Creation timestamp of the Controlled Vocabulary.
   */
  public function getCreatedTime();

  /**
   * Sets the Controlled Vocabulary creation timestamp.
   *
   * @param int $timestamp
   *   The Controlled Vocabulary creation timestamp.
   *
   * @return \Drupal\tripal\Entity\TripalVocabInterface
   *   The called Controlled Vocabulary entity.
   */
  public function setCreatedTime($timestamp);

  /**
   *
   */
  public function getVocabulary();
  /**
   *
   * @param unknown $vocabulary
   */
  public function setVocabulary($vocabulary);

}
