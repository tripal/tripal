<?php

namespace Drupal\tripal\Entity;

use Drupal\Core\Entity\ContentEntityInterface;
use Drupal\Core\Entity\EntityChangedInterface;
use Drupal\user\EntityOwnerInterface;

/**
 * Provides an interface for defining Controlled Vocabulary Term entities.
 *
 * @ingroup tripal
 */
interface TripalTermInterface extends ContentEntityInterface, EntityChangedInterface {

  // Add get/set methods for your configuration properties here.

  /**
   * Gets the Controlled Vocabulary Term name.
   *
   * @return string
   *   Name of the Controlled Vocabulary Term.
   */
  public function getName();

  /**
   * Sets the Controlled Vocabulary Term name.
   *
   * @param string $name
   *   The Controlled Vocabulary Term name.
   *
   * @return \Drupal\tripal\Entity\TripalTermInterface
   *   The called Controlled Vocabulary Term entity.
   */
  public function setName($name);

  /**
   * Gets the Controlled Vocabulary Term creation timestamp.
   *
   * @return int
   *   Creation timestamp of the Controlled Vocabulary Term.
   */
  public function getCreatedTime();

  /**
   * Sets the Controlled Vocabulary Term creation timestamp.
   *
   * @param int $timestamp
   *   The Controlled Vocabulary Term creation timestamp.
   *
   * @return \Drupal\tripal\Entity\TripalTermInterface
   *   The called Controlled Vocabulary Term entity.
   */
  public function setCreatedTime($timestamp);

}
