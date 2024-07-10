<?php

namespace Drupal\tripal_biodb\Lock;

use Drupal\Core\Lock\PersistentDatabaseLockBackend;

/**
 * Defines the persistent database lock backend.
 *
 * This backend is global for this Drupal installation. See tests for usage
 * examples.
 *
 * In case of remaining locks that should be removed manually:
 * @code
 * // Try that first and check after.
 * $locker = new Drupal\tripal_biodb\Lock\PersistentDatabaseSharedLockBackend(\Drupal::database());
 * $locker->cleanUnusedSharedLocks();
 *
 * // If there still a problem and you need to remove ALL shared locks,
 * // including those which may still be in use (WARNING!), try that:
 * // First remove persistant shared locks in semaphore table.
 * $locker = new Drupal\Core\Lock\DatabaseLockBackend(\Drupal::database());
 * $locker->acquire(mt_rand()); // We need to lock something first to unlock all.
 * $locker->releaseAll(Drupal\tripal_biodb\Lock\PersistentDatabaseSharedLockBackend::LOCK_ID);
 * $locker->releaseAll(); // Release the random lock used at first.
 *
 * // Finally, remove all states recorded on shared locks.
 * $state = \Drupal::state();
 * $state->set('tripal_shared_lock_shared', []);
 * $state->set('tripal_shared_lock_exclusive', []);
 *
 * @endcode
 *
 * @ingroup lock
 */
class PersistentDatabaseSharedLockBackend extends PersistentDatabaseLockBackend implements SharedLockBackendInterface {

  /**
   * The Drupal State API state key used to store shared lock details.
   */
  const STATE_KEY_SHARED = 'tripal_shared_lock_shared';

  /**
   * The Drupal State API state key used to store exclusive lock details.
   */
  const STATE_KEY_EXCLUSIVE = 'tripal_shared_lock_exclusive';

  /**
   * Lock identifier used by all persistent shared locks.
   */
  const LOCK_ID = '_persistent_shared';

  /**
   * Default timeout for both exclusive and shared locks.
   *
   * 43200 seconds = 2 hours
   */
  const DEFAULT_LOCK_TIMEOUT = 43200.;

  /**
   * The state modification locker.
   *
   * @var \Drupal\Core\Lock\LockBackendInterface
   */
  protected $modLocker;

  /**
   * The state storage service.
   *
   * @var \Drupal\Core\State\StateInterface
   */
  protected $state;

  /**
   * Shared locks managed by this instance.
   *
   * @var array
   */
  protected $shared_locks = [];

  /**
   * Constructs a new PersistentDatabaseSharedLockBackend.
   *
   * @param \Drupal\Core\Database\Connection $database
   *   The database connection.
   * @param \Drupal\Core\Lock\LockBackendInterface $locker
   *   (optional) A lock backend used to lock modifications on shared locks.
   *   Default: if NULL, Drupal lock service is used.
   *   will be used.
   * @param \Drupal\Core\State\StateInterface $state
   *   Drupal state service.
   */
  public function __construct(
    \Drupal\Core\Database\Connection $database,
    ?\Drupal\Core\Lock\LockBackendInterface $locker = NULL,
    ?\Drupal\Core\State\StateInterface $state = NULL
  ) {
    parent::__construct($database);
    $this->lockId = static::LOCK_ID;
    // Get a locker for state modification operations.
    if (!isset($locker)) {
      $locker = \Drupal::getContainer()->get('lock');
    }
    $this->modLocker = $locker;
    // Get a state storage service.
    if (!isset($state)) {
      $state = \Drupal::state();
    }
    $this->state = $state;
  }

  /**
   * {@inheritdoc}
   */
  public function acquire(
    $name,
    $timeout = self::DEFAULT_LOCK_TIMEOUT,
    string $owner = '',
    ?int $pid = NULL
  ) {
    // Make sure it is not shared.
    $shared_locks = $this->state->get(static::STATE_KEY_SHARED, []);
    if (!empty($shared_locks[$name]) && !$this->lockMayBeAvailable($name)) {
      return FALSE;
    }
    // Set an owner if none.
    if (empty($owner)) {
      $owner = 'r' . mt_rand(10000000, 99999999);
    }

    $acquisition = parent::acquire($name, $timeout);
    // If lock acquired, acquire a state modification lock.
    if ($acquisition
        && ($this->modLocker->acquire(static::STATE_KEY_EXCLUSIVE))
    ) {
      $pid = $pid ?? getmypid() ?: 0;
      // Get exclusive lock status.
      $exclusive_locks = $this->state->get(static::STATE_KEY_EXCLUSIVE, []);
      // Store owner and PID.
      $exclusive_locks[$name] = [
        'owner' => $owner,
        'pid' => $pid,
        'start' => microtime(TRUE),
      ];
      // Save lock details.
      $this->state->set(static::STATE_KEY_EXCLUSIVE, $exclusive_locks);
      // Release modification lock.
      $this->modLocker->release(static::STATE_KEY_EXCLUSIVE);
    }
    else {
      trigger_error(
        'Unable to lock state "'
        . static::STATE_KEY_EXCLUSIVE
        . '" for modifications.',
        E_USER_WARNING
      );
    }
    return $acquisition;
  }

  /**
   * Acquires a shared lock.
   *
   * Shared locks relies on a regular lock as defined in the
   * \Drupal\Core\Lock\PersistentDatabaseLockBackend class plus a list of lock
   * share owners stored by the Drupal State API key static::STATE_KEY_SHARED. In order
   * to avoid race conditions on the state key value modification, a lock
   * backend provided to the constructor is used to manage modifications.
   *
   * @param string $name
   *   Shared lock name. Limit of name's length is 255 characters.
   * @param float $timeout
   *   (optional) Shared lock lifetime in seconds. Defaults to
   *   self::DEFAULT_LOCK_TIMEOUT.
   * @param string $owner
   *   Identifier of operation willing to own a share on the lock.
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
    float $timeout = self::DEFAULT_LOCK_TIMEOUT,
    string $owner = '',
    ?int $pid = NULL
  ) {
    // Acquire a state modification lock.
    if (!$this->modLocker->acquire(static::STATE_KEY_SHARED)) {
      // Unable to modify safely lock sharing status.
      trigger_error(
        'Unable to acquire a shared lock: unable to lock state "'
        . static::STATE_KEY_SHARED
        . '" for modifications.',
        E_USER_WARNING
      );
      return FALSE;
    }

    // Make sure we got an owner name.
    if (empty($owner)) {
      $owner = 'r' . mt_rand(10000000, 99999999);
    }

    // Init shared lock values.
    $name = $this->normalizeName($name);
    $timeout = max($timeout, 0.001);
    $expire = microtime(TRUE) + $timeout;
    $pid = $pid ?? getmypid() ?: 0;

    // Get shared lock status.
    $shared_locks = $this->state->get(static::STATE_KEY_SHARED, []);

    // Check if there already is a lock for that name.
    if ($this->lockMayBeAvailable($name)) {
      // Shared lock does not exist, create one.
      $success = $this->acquire($name, $timeout, $this->getLockId(), 0);
    }
    elseif (!array_key_exists($name, $shared_locks)) {
      // Lock already in use for exclusive use.
      $success = FALSE;
    }
    else {
      // Shared lock already acquired. Update timeout if needed.
      // Get current expiration.
      $success = $current_expire = $this->database->select(static::TABLE_NAME, 's')
        ->condition('s.name', $name)
        ->condition('s.value', $this->getLockId())
        ->fields('s', ['expire'])
        ->execute()
        ->fetch()
        ->expire
      ;
      // Keep the largest one.
      if ($expire > $current_expire) {
        $success = (bool) $this->database->update(static::TABLE_NAME)
          ->fields(['expire' => $expire])
          ->condition('name', $name)
          ->condition('value', $this->getLockId())
          ->execute();
      }
      if (!$success) {
        // The lock was broken, try to acquire it again.
        $new_timeout = max(
          max($current_expire, $expire) - microtime(TRUE),
          0.001
        );
        $success = $this->acquire($name, $new_timeout, $this->getLockId(), 0);
      }
    }
    // Make sure we got a shared lock.
    if ($success) {
      // Keep track this lock is also used by this instance.
      $this->locks[$name] = TRUE;

      // Add or update a share for this owner.
      $shared_locks[$name] = $shared_locks[$name] ?? [];
      $shared_locks[$name][$owner] = [
        'expire' => $expire,
        'pid' => $pid,
        'start' => $shared_locks[$name][$owner]['start'] ?? microtime(TRUE),
      ];
      // Save new share.
      $this->state->set(static::STATE_KEY_SHARED, $shared_locks);
      // Keep track of managed shares.
      $this->shared_locks[$name][$owner] = TRUE;
    }

    // Release the state modification lock.
    $this->modLocker->release(static::STATE_KEY_SHARED);
    
    return $success ? $owner : FALSE;
  }

  /**
   * {@inheritdoc}
   */
  public function release($name) {
    // Only release own locks.
    if (!array_key_exists($name, $this->locks)) {
      return;
    }

    // Should not release shared locks here.
    $shared_locks = $this->state->get(static::STATE_KEY_SHARED, []);
    if (!empty($shared_locks[$name])) {
      return;
    }
      
    // Lock status for modifications.
    if ($this->modLocker->acquire(static::STATE_KEY_EXCLUSIVE)) {
      // Get exclusive lock status.
      $exclusive_locks = $this->state->get(static::STATE_KEY_EXCLUSIVE, []);
      unset($exclusive_locks[$name]);
      // Save lock details.
      $this->state->set(static::STATE_KEY_EXCLUSIVE, $exclusive_locks);
      // Release modification lock.
      $this->modLocker->release(static::STATE_KEY_EXCLUSIVE);
    }
    parent::release($name);
  }

  /**
   * {@inheritdoc}
   */
  public function releaseShared($name, $owner) {
    // Only release own locks.
    if (empty($this->shared_locks[$name][$owner])) {
      return;
    }
    // Acquire a state modification lock.
    if ($this->modLocker->acquire(static::STATE_KEY_SHARED)) {
      // Get shared lock status.
      $shared_locks = $this->state->get(static::STATE_KEY_SHARED, []);

      // Remove the share for this owner.
      unset($shared_locks[$name][$owner]);

      // Check if the shared lock is still in use.
      if (empty($shared_locks[$name])) {
        // If not, remove it.
        $this->release($name);
      }
      // Save changes.
      $this->state->set(static::STATE_KEY_SHARED, $shared_locks);
      // Release the state modification lock.
      $this->modLocker->release(static::STATE_KEY_SHARED);
    }
    unset($this->shared_locks[$name][$owner]);
    if (empty($this->shared_locks[$name])) {
      unset($this->shared_locks[$name]);
      unset($this->locks[$name]);
    }
  }

  /**
   * {@inheritdoc}
   */
  public function lockMayBeAvailable($name) {
    $available = parent::lockMayBeAvailable($name);

    // Try cleaning shared locks.
    $this->cleanUnusedSharedLocks();
    
    if (!$available) {
      // Retry.
      $available = parent::lockMayBeAvailable($name);
    }

    return $available;
  }

  /**
   * {@inheritdoc}
   */
  public function releaseAll($lock_id = NULL) {
    // Only attempts to release locks that were acquired by this instance.
    foreach ($this->shared_locks as $name => $shares) {
      foreach ($shares as $owner => $status) {
        $this->releaseShared($name, $owner);
      }
    }
    foreach ($this->locks as $name => $status) {
      $this->release($name);
    }
  }

  /**
   * Removes expired or unused shared locks.
   */
  public function cleanUnusedSharedLocks() {
    // Acquire a state modification lock.
    if ($this->modLocker->acquire(static::STATE_KEY_SHARED)) {
      // Get shared lock status.
      $shared_locks = $this->state->get(static::STATE_KEY_SHARED, []);
      $new_shared_locks = $shared_locks;
      $now = microtime(TRUE);

      foreach ($shared_locks as $name => $owners) {
        // Is main shared lock still there?
        if (parent::lockMayBeAvailable($name)) {
          // Main shared lock broken, remove all shares.
          unset($new_shared_locks[$name]);
          // Cleanup this instance.
          unset($this->shared_locks[$name]);
          unset($this->locks[$name]);
        }
        else {
          // Check each share.
          foreach ($owners as $owner => $details) {
            $expired = FALSE;
            // Check PID.
            if (!empty($details['pid'])
                && (0 < $details['pid'])
                && !file_exists("/proc/" . $details['pid'])
            ) {
              // Process not existing, lock expired.
              $expired = TRUE;
            }
            elseif (!empty($details['expire'])
                && ($details['expire'] < $now)
            ) {
              // Share expired.
              $expired = TRUE;
            }
            
            // Remove lock share.
            if ($expired) {
              unset($new_shared_locks[$name][$owner]);
            }
          }
          // Check if shared lock is still in use.
          if (empty($new_shared_locks[$name])) {
            // Not used anymore, remove main lock.
            unset($new_shared_locks[$name]);
            // Assign this lock to current lock manager so it can be removed.
            $this->locks[$name] = TRUE;
            $this->release($name);
            // Cleanup this instance.
            unset($this->shared_locks[$name]);
          }
        }
      }

      // Save changes.
      $this->state->set(static::STATE_KEY_SHARED, $new_shared_locks);

      // Now cleanup main shared locks.
      if ($this->modLocker->acquire(static::STATE_KEY_EXCLUSIVE)) {
        // Get lock status.
        $exclusive_locks = $this->state->get(static::STATE_KEY_EXCLUSIVE, []);
        $new_exclusive_locks = $exclusive_locks;
        $now = microtime(TRUE);

        foreach ($exclusive_locks as $name => $details) {
          // Is main shared lock still there?
          if (parent::lockMayBeAvailable($name)) {
            // Main shared lock broken, remove lock details.
            unset($new_exclusive_locks[$name]);
            // Cleanup this instance.
            unset($this->locks[$name]);
          }
          elseif ((static::LOCK_ID == $details['owner'])
              && empty($new_shared_locks[$name])
          ) {
            // No more shared lock using this lock.
            unset($new_exclusive_locks[$name]);
            // Cleanup this instance.
            unset($this->locks[$name]);
          }
          else {
            // Check PID.
            if (!empty($details['pid'])
                && (0 < $details['pid'])
                && !file_exists("/proc/" . $details['pid'])
            ) {
              // Process not existing.
              unset($new_exclusive_locks[$name]);
              // Assign this lock to current lock manager so it can be removed.
              $this->locks[$name] = TRUE;
              $this->release($name);
            }
          }
        }

        // Save changes.
        $this->state->set(static::STATE_KEY_EXCLUSIVE, $new_exclusive_locks);

        // Cleanup left overs in semaphore table.
        // Note: we don't test ensureTableExists() as it returns value is only
        // TRUE if the table is created and it would be FALSE if the table
        // exists already or if an error occurred while creating the table.
        $this->ensureTableExists();
        $names = $this->database->select(static::TABLE_NAME, 's')
          ->condition('s.value', $this->getLockId())
          ->fields('s', ['name'])
          ->execute()
          ->fetchCol()
        ;
        foreach ($names as $name) {
          if (empty($new_exclusive_locks[$name])) {
            $this->locks[$name] = TRUE;
            $this->release($name);
          }
        }
        
        // Release state modification lock.
        $this->modLocker->release(static::STATE_KEY_EXCLUSIVE);
      }

      // Release state modification lock.
      $this->modLocker->release(static::STATE_KEY_SHARED);
    }
  }

  /**
   * {@inheritdoc}
   */
  public function getOwner(string $name) :string {
    $exclusive_locks = $this->state->get(static::STATE_KEY_EXCLUSIVE, []);
    if (!empty($exclusive_locks[$name]['owner'])
        && (static::LOCK_ID != $exclusive_locks[$name]['owner'])
    ) {
      return $exclusive_locks[$name]['owner'];
    }
    else {
      return '';
    }
  }

  /**
   * {@inheritdoc}
   */
  public function getOwners(string $name) :array {
    $owner_list = [];
    $shared_locks = $this->state->get(static::STATE_KEY_SHARED, []);

    foreach ($shared_locks as $name => $owners) {
      foreach ($owners as $owner => $details) {
        $owner_list[] = $owner;
      }
    }
    return $owner_list;
  }

  /**
   * {@inheritdoc}
   */
  public function getStartTime(string $name, ?string $owner = NULL) :float {
    $start_time = 0;
    if (!empty($owner)) {
      // Shared lock.
      $shared_locks = $this->state->get(static::STATE_KEY_SHARED, []);
      if (!empty($shared_locks[$name][$owner]['start'])) {
        $start_time = $shared_locks[$name][$owner]['start'];
      }
    }
    else {
      // Exclusive lock.
      $exclusive_locks = $this->state->get(static::STATE_KEY_EXCLUSIVE, []);
      if (!empty($exclusive_locks[$name]['start'])) {
        $start_time = $exclusive_locks[$name]['start'];
      }
    }
    return $start_time;
  }

  /**
   * {@inheritdoc}
   */
  public function getCurrentExpirationTime(string $name) :float {
    // Get current expiration.
    $current_expire = $this->database->select(static::TABLE_NAME, 's')
      ->condition('s.name', $name)
      ->condition('s.value', $this->getLockId())
      ->fields('s', ['expire'])
      ->execute()
      ->fetch()
      ->expire
    ;
    return $current_expire ?? 0;
  }

  /**
   * {@inheritdoc}
   */
  public function getCurrentExpirationDelay(string $name) :float {
    $current_expire = $this->getCurrentExpirationTime($name);
    return $current_expire ? max($current_expire - microtime(TRUE), 0) : 0;
  }

  /**
   * {@inheritdoc}
   */
  public function getOwnerPid(string $name, ?string $owner = NULL) :int {
    $pid = 0;
    if (!empty($owner)) {
      // Shared lock.
      $shared_locks = $this->state->get(static::STATE_KEY_SHARED, []);
      if (!empty($shared_locks[$name][$owner]['pid'])) {
        $pid = $shared_locks[$name][$owner]['pid'];
      }
    }
    else {
      // Exclusive lock.
      $exclusive_locks = $this->state->get(static::STATE_KEY_EXCLUSIVE, []);
      if (!empty($exclusive_locks[$name]['pid'])) {
        $pid = $exclusive_locks[$name]['pid'];
      }
    }
    return $pid;
  }

}
