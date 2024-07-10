<?php

namespace Drupal\tripal_biodb\Lock;

use Drupal\Core\Lock\LockBackendInterface;

/**
 * Lock information interface.
 *
 * Defines methods to get lock informations such as lock owner, expiration and
 * such.
 *
 * @ingroup lock
 */
interface LockInfoInterface {

  /**
   * Returns the time in seconds when the lock started.
   *
   * @param string $name
   *   Name of the lock.
   *
   * @return float
   *   The lock starting time in seconds or 0 if not available.
   */
  public function getStartTime(string $name) :float;

  /**
   * Returns the lock expiration time in seconds.
   *
   * Note: this time may be extended by the lock owner.
   *
   * @param string $name
   *   Name of the lock.
   *
   * @return float
   *   The lock expiration time in seconds or 0 if not available.
   */
  public function getCurrentExpirationTime(string $name) :float;

  /**
   * Returns the lock time remaining in seconds.
   *
   * Note: this delay may be extended by the lock owner.
   *
   * @param string $name
   *   Name of the lock.
   *
   * @return float
   *   The lock remaining time in seconds or 0 if not available or expired.
   */
  public function getCurrentExpirationDelay(string $name) :float;

  /**
   * Returns the lock owner name.
   *
   * @param string $name
   *   Name of the lock.
   *
   * @return string
   *   The lock owner name or an empty string if not available.
   */
  public function getOwner(string $name) :string;

  /**
   * Returns the system process identifier using the lock.
   *
   * @param string $name
   *   Name of the lock.
   *
   * @return int
   *   The system process identifier using this lock or 0 if not available.
   */
  public function getOwnerPid(string $name) :int;

}
