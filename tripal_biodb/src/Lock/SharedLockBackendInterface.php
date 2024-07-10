<?php

namespace Drupal\tripal_biodb\Lock;

use Drupal\Core\Lock\LockBackendInterface;

/**
 * Shared lock backend interface.
 *
 * A shared lock allows an element to be locked for one or more operations that
 * can access the locked element concurrently without problems. However, it
 * prevents an operation to acquire an exclusive lock on that element.
 *
 * The lock meccanism is quite similar to the exclusive one except that more
 * than one operation at a time can aquire a same shared lock. Operations that
 * try to acquire the lock in exclusive mode are denied. Finaly, the lock will
 * only be released once all operations that acquired it released it.
 *
 * This type of lock is useful when an element needs to be accessed in reading
 * mode by several operations (that wont alter it) but no other operation should
 * be able to alter that element while it is in use by thoses operations.
 *
 * This interface extends the existing LockBackendInterface interface with 2 new
 * methods that are dedicated to acquire and release a shared lock.
 *
 * @ingroup lock
 */
interface SharedLockBackendInterface extends LockBackendInterface, LockInfoInterface {

  /**
   * Acquires a lock.
   *
   * @param string $name
   *   Lock name. Limit of name's length is 255 characters.
   * @param float $timeout
   *   (optional) Lock lifetime in seconds. Defaults to 30.0.
   * @param string $owner
   *   (optional) Name of the (exclusive) lock owner.
   * @param ?int $pid
   *   (optional) An operating system process ID owning the lock. It may be
   *   used to release the lock if the given PID becomes unused. A value of 0
   *   will prevent the share to be released based on a PID.
   *   Default: if NULL, the current process ID (getmypid()) will be used.
   *
   * @return bool
   *   TRUE if acquired and FALSE otherwise.
   */
  public function acquire(
    $name,
    $timeout = 30.0,
    string $owner = '',
    ?int $pid = NULL
  );

  /**
   * Acquires a shared lock.
   *
   * @param string $name
   *   Shared lock name. Limit of name's length is 255 characters.
   * @param float $timeout
   *   (optional) Shared lock lifetime in seconds. Defaults to 30.0.
   * @param string $owner
   *   Name of the owner willing a share on the lock (used as identifier).
   * @param ?int $pid
   *   (optional) An operating system process ID owning the share. It may be
   *   used to release the lock if the given PID becomes unused. A value of 0
   *   will prevent the share to be released based on a PID.
   *   Default: if NULL, the current process ID (getmypid()) will be used.
   *
   * @return mixed
   *   The owner name if the shared lock has been acquired and FALSE otherwise.
   */
  public function acquireShared(
    string $name,
    float $timeout = 30.0,
    string $owner = '',
    ?int $pid = NULL
  );

  /**
   * Releases the given shared lock.
   *
   * @param string $name
   *   Lock name.
   * @param string $owner
   *   Name of the owner of a share on the lock.
   */
  public function releaseShared(string $name, string $owner);

  /**
   * Returns the time in seconds when the lock started.
   *
   * If no owner is specified, the lock is assumed to be exclusive and 0 will be
   * returned in case of a shared lock. If an owner is specified, the starting
   * time of the lock share of the specified owner is returned.
   *
   * @param string $name
   *   Name of the lock.
   * @param ?string $owner
   *   Name of the owner of the share of a shared lock.
   *
   * @return float
   *   The lock starting time in seconds or 0 if not available.
   */
  public function getStartTime(string $name, ?string $owner = NULL) :float;

  /**
   * Returns the list of shared lock owners.
   *
   * @param string $name
   *   Name of the shared lock.
   *
   * @return array
   *   An array of shared lock owner names.
   */
  public function getOwners(string $name) :array;

  /**
   * Returns the system process identifier using the shared or exclusive lock.
   *
   * If no owner is specified, the lock is assumed to be exclusive and 0 will be
   * returned in case of a sheared lock. If an owner is specified, the PID of
   * the owner is returned.
   *
   * @param string $name
   *   Name of the lock.
   * @param ?string $owner
   *   Name of the owner of the share of a shared lock.
   *
   * @return int
   *   The system process identifier using this lock or 0 if not available.
   */
  public function getOwnerPid(string $name, ?string $owner = NULL) :int;
}
