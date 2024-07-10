<?php

namespace Drupal\tripal\Entity;

use Drupal\Core\Entity\ContentEntityInterface;
use Drupal\Core\Entity\EntityChangedInterface;
use Drupal\user\EntityOwnerInterface;

/**
 * Provides an interface for defining Tripal Content entities.
 *
 * @ingroup tripal
 */
interface TripalEntityInterface extends ContentEntityInterface, EntityChangedInterface, EntityOwnerInterface {

  // Add get/set methods for your configuration properties here.

  /**
   * Gets the Tripal Content type.
   *
   * @return string
   *   The Tripal Content type.
   */
  public function getType();

  /**
   * Gets the Tripal Content creation timestamp.
   *
   * @return int
   *   Creation timestamp of the Tripal Content.
   */
  public function getCreatedTime();

  /**
   * Sets the Tripal Content creation timestamp.
   *
   * @param int $timestamp
   *   The Tripal Content creation timestamp.
   *
   * @return \Drupal\tripal\Entity\TripalEntityInterface
   *   The called Tripal Content entity.
   */
  public function setCreatedTime($timestamp);

  /**
   * Returns the Tripal Content published status indicator.
   *
   * Unpublished Tripal Content are only visible to restricted users.
   *
   * @return bool
   *   TRUE if the Tripal Content is published.
   */
  public function isPublished();

  /**
   * Sets the published status of a Tripal Content.
   *
   * @param bool $published
   *   TRUE to set this Tripal Content to published, FALSE to set it to unpublished.
   *
   * @return \Drupal\tripal\Entity\TripalEntityInterface
   *   The called Tripal Content entity.
   */
  public function setPublished($published);

}
