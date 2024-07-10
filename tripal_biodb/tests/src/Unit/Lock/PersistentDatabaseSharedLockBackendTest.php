<?php

namespace Drupal\Tests\tripal_biodb\Unit\Lock;

use Drupal\Tests\tripal\Kernel\TripalTestKernelBase;
use Drupal\Core\Lock\DatabaseLockBackend;
use Drupal\Core\Lock\PersistentDatabaseLockBackend;
use Drupal\tripal_biodb\Lock\PersistentDatabaseSharedLockBackend;

/**
 * Tests for PersistentDatabaseSharedLockBackend.
 *
 * Note: we use PersistentDatabaseLockBackend instead of DatabaseLockBackend in
 * these tests when we need another type of lock backend because
 * DatabaseLockBackend registers shutdown function releaseAll() which will raise
 * an error when an unexpected error occurs during tests, silencing the real
 * problem. It's because at shutdown, the semaphore table in the test schema
 * is already removed when releaseAll() is called leading to the error:
 * ```
 * PHPUnit\Framework\Exception: PDOException: SQLSTATE[42P01]:
 * Undefined table: 7 ERROR:  relation "test########semaphore" does not exist
 * LINE 1: DELETE FROM "test########semaphore"
 *                   ^ in core/lib/Drupal/Core/Database/StatementWrapper.php:116
 * ```
 *
 * @coversDefaultClass \Drupal\tripal_biodb\Lock\PersistentDatabaseSharedLockBackend
 *
 * @ingroup Tripal BioDb
 *
 * @group Tripal
 * @group Tripal BioDb
 * @group Tripal BioDb Lock
 */
class PersistentDatabaseSharedLockBackendTest extends TripalTestKernelBase {

  /**
   * Database persitent shared lock backend to test.
   *
   * @var \Drupal\tripal_biodb\Lock\PersistentDatabaseSharedLockBackend
   */
  protected $sharedLocker;

  /**
   * Internal locker used by sheared locker.
   *
   * @var \Drupal\Core\Lock\LockBackendInterface
   */
  protected $internalLocker;

  /**
   * Drupal state.
   *
   * @var \Drupal\Core\State\StateInterface
   */
  protected $state;

  /**
   * Tests setup method.
   */
  protected function setUp(): void {
    parent::setUp();
    $this->internalLocker = new \Drupal\Core\Lock\DatabaseLockBackend(
      $this->container->get('database')
    );
    $this->state = $this->container->get('state');
    $this->sharedLocker = new PersistentDatabaseSharedLockBackend(
      $this->container->get('database'),
      $this->internalLocker,
      $this->state
    );
  }

  /**
   * Tests exclusive lock.
   *
   * @covers ::acquire
   */
  public function testExclusiveLockBasics() {
    $lock_name = 'test_lock_a';

    // Create a secondary locker.
    $other_locker = new PersistentDatabaseSharedLockBackend($this->container->get('database'));

    // Acquire a first lock.
    $success = $this->sharedLocker->acquire($lock_name);
    $this->assertTrue($success, 'Could acquire first lock.');

    // Acquire a second lock.
    $success = $this->sharedLocker->acquire('test_lock_b');
    $this->assertTrue($success, 'Could acquire second lock.');

    // Release second lock.
    $success = $other_locker->release('test_lock_b');

    // Make sure exclusive.
    $is_free = $this->sharedLocker->lockMayBeAvailable($lock_name);
    $this->assertFalse($is_free, 'First lock is locked.');

    // Try to steal lock.
    $this->expectException(\PHPUnit\Framework\Error\Warning::class);
    $success = $other_locker->acquire($lock_name);
    $this->assertFalse($success, 'First lock is protected while in use.');

    // Try to force lock sharing.
    $success = $other_locker->acquireShared($lock_name);
    $this->assertFalse($success, 'First lock is exclusive and not shared.');

    // Own lock duration can be extended.
    $success = $this->sharedLocker->acquire($lock_name);
    $this->assertTrue($success, 'Could extend first lock duration.');

    // Make sure additional info on exclusive lock are stored.
    // PID.
    $pid = $this->sharedLocker->getOwnerPid($lock_name);
    $this->assertNotEmpty($pid, 'Got owner PID.');
    $this->assertIsInt($pid, 'Got a valid owner PID.');
    $this->assertEquals(getmypid(), $pid, 'Default PID is current process.');
    // Owner.
    $owner = $this->sharedLocker->getOwner($lock_name);
    $this->assertNotEmpty($owner, 'Got owner name.');
    // Start time.
    $start = $this->sharedLocker->getStartTime($lock_name);
    $this->assertNotEmpty($start, 'Got start time.');
    $this->assertLessThan(microtime(TRUE), $start, 'Lock started before now.');
    // End time.
    $end = $this->sharedLocker->getCurrentExpirationTime($lock_name);
    $this->assertNotEmpty($end, 'Got end time.');
    $this->assertGreaterThan($start, $end, 'Lock ends later.');
    // End delay.
    $delay = $this->sharedLocker->getCurrentExpirationDelay($lock_name);
    $this->assertNotEmpty($delay, 'Got lock life delay.');
    $this->assertIsFloat($delay, 'Got a valid lock life delay.');
    // Multiple owners.
    $end = $this->sharedLocker->getOwners($lock_name);
    $this->assertEmpty($end, 'No multiple owners (not shared).');

    // Release lock.
    $this->sharedLocker->release($lock_name);
    $is_free = $this->sharedLocker->lockMayBeAvailable($lock_name);
    $this->assertTrue($is_free, 'First lock has been released.');

    // Try to re-use released lock.
    $success = $other_locker->acquire($lock_name);
    $this->assertTrue($success, 'Released first lock can be reused by someone else.');

    // Try to reacquire first lock while in use by other.
    $success = $this->sharedLocker->acquire($lock_name);
    $this->assertFalse($success, 'Given up first lock cannot be re-acquired.');

    // Release lock.
    $success = $other_locker->release($lock_name);
  }

  /**
   * Tests shared lock.
   *
   * @covers ::acquire
   */
  public function testSharedLockBasics() {
    $lock_name = 'test_lock_a';

    // Create a secondary locker.
    $other_locker = new PersistentDatabaseSharedLockBackend($this->container->get('database'));

    // Make sure we start in a clean environment.
    $this->sharedLocker->cleanUnusedSharedLocks();

    // Acquire a first lock.
    $owner = $this->sharedLocker->acquireShared($lock_name, 180);
    $this->assertNotEmpty($owner, 'Could acquire first shared lock.');

    // Make sure lock is marked as being in use.
    $is_free = $this->sharedLocker->lockMayBeAvailable($lock_name);
    $this->assertFalse($is_free, 'First shared lock is locked.');

    // Try to get an exclusive lock.
    $success = $other_locker->acquire($lock_name);
    $this->assertFalse($success, 'First lock is shared and could not become exclusive.');

    // Try to get another share.
    $other_owner = $other_locker->acquireShared($lock_name, 180, 'other');
    $this->assertEquals('other', $other_owner, 'First shared lock can be shared.');

    // Make sure additional info on shared lock are stored.
    // PID.
    $pid = $this->sharedLocker->getOwnerPid($lock_name);
    // Should return nothing since we must specify an owner.
    $this->assertEmpty($pid, 'No exclusive owner PID.');
    // Now, should get current PID.
    $pid = $this->sharedLocker->getOwnerPid($lock_name, $owner);
    $this->assertNotEmpty($pid, 'Got owner PID.');
    $this->assertIsInt($pid, 'Got a valid owner PID.');
    $this->assertEquals(getmypid(), $pid, 'Default PID is current process.');
    // Owner.
    // No single owner as it is shared.
    $single_owner = $this->sharedLocker->getOwner($lock_name);
    $this->assertEmpty($single_owner, 'No single owner name.');
    // Multiple owners.
    $owners = $this->sharedLocker->getOwners($lock_name);
    $this->assertEqualsCanonicalizing([$owner, 'other'], $owners, 'Got all owners.');
    // Start time.
    $start = $this->sharedLocker->getStartTime($lock_name, $owner);
    $this->assertNotEmpty($start, 'Got start time.');
    $this->assertLessThan(microtime(TRUE), $start, 'Lock started before now.');
    // End time.
    $end = $this->sharedLocker->getCurrentExpirationTime($lock_name, $owner);
    $this->assertNotEmpty($end, 'Got end time.');
    $this->assertGreaterThan($start, $end, 'Lock ends later.');
    // End delay.
    $delay = $this->sharedLocker->getCurrentExpirationDelay($lock_name, $owner);
    $this->assertNotEmpty($delay, 'Got lock life delay.');
    $this->assertIsFloat($delay, 'Got a valid lock life delay.');

    // Release lock.
    // Release first share.
    $this->sharedLocker->releaseShared($lock_name, $owner);
    $is_free = $this->sharedLocker->lockMayBeAvailable($lock_name);
    $this->assertFalse($is_free, 'First shared lock not released yet - Other still own a share.');
    // Try to release the share of the other.
    $this->sharedLocker->releaseShared($lock_name, 'other');
    $is_free = $this->sharedLocker->lockMayBeAvailable($lock_name);
    $this->assertFalse($is_free, 'First shared lock still not released yet - Not allowed to release other\'s lock.');
    // Release other share.
    $other_locker->releaseShared($lock_name, 'other');
    $is_free = $this->sharedLocker->lockMayBeAvailable($lock_name);
    $this->assertTrue($is_free, 'First shared lock has been released.');

    // Leave a clean environment.
    $this->sharedLocker->cleanUnusedSharedLocks();
  }

  /**
   * Tests exclusive acquire denies a shared lock.
   *
   * @covers ::acquire
   */
  public function testNotAcquireWhenShared() {
    $lock_name = 'test_lock_share';

    // Acquire a first lock.
    $owner = $this->sharedLocker->acquireShared($lock_name, 180);
    $this->assertNotEmpty($owner, 'Could acquire a shared lock.');

    // Should not be able to "exclusive" lock a shared lock.
    $success = $this->sharedLocker->acquire($lock_name);
    $this->assertFalse($success, 'Shared lock can not be locked for exclusive use.');

    // Try to lock using parent class.
    $other_locker = new PersistentDatabaseLockBackend($this->container->get('database'));
    $success = $other_locker->acquire($lock_name);
    $this->assertFalse($success, 'Shared lock can not be locked for exclusive use by parent class.');

    // Clear.
    $this->sharedLocker->releaseShared($lock_name, $owner);
  }

  /**
   * Tests acquire will continue even if it could not use state API to store
   * infos.
   *
   * @covers ::acquire
   */
  public function testAcquireWithoutState() {
    $lock_name = 'test_lock_share';

    // No details before.
    // PID.
    $pid = $this->sharedLocker->getOwnerPid($lock_name);
    $this->assertEmpty($pid, 'No owner PID before.');
    // Owner.
    $owner = $this->sharedLocker->getOwner($lock_name);
    $this->assertEmpty($owner, 'No owner name before.');

    // Lock state.
    $other_locker = new PersistentDatabaseLockBackend($this->container->get('database'));
    $success = $other_locker->acquire(PersistentDatabaseSharedLockBackend::STATE_KEY_EXCLUSIVE, 180);
    $this->assertTrue($success, 'Could lock states.');

    // Acquire a lock.
    $this->expectException(\PHPUnit\Framework\Error\Warning::class);
    $success = $this->sharedLocker->acquire($lock_name, 180);
    $this->assertTrue($success, 'Could acquire a lock.');

    // Make sure state lock was not released.
    $this->assertFalse($other_locker->lockMayBeAvailable(PersistentDatabaseSharedLockBackend::STATE_KEY_EXCLUSIVE), 'States still locked.');
    $this->assertFalse($this->sharedLocker->lockMayBeAvailable(PersistentDatabaseSharedLockBackend::STATE_KEY_EXCLUSIVE), 'States still locked even from current shared lock.');
    $this->assertFalse($this->internalLocker->lockMayBeAvailable(PersistentDatabaseSharedLockBackend::STATE_KEY_EXCLUSIVE), 'States not available even from current internal lock.');

    // No details.
    // PID.
    $pid = $this->sharedLocker->getOwnerPid($lock_name);
    $this->assertEmpty($pid, 'No owner PID.');
    // Owner.
    $owner = $this->sharedLocker->getOwner($lock_name);
    $this->assertEmpty($owner, 'No owner name.');

    // Release lock.
    $other_locker->release(PersistentDatabaseSharedLockBackend::STATE_KEY_EXCLUSIVE);
    $this->sharedLocker->release($lock_name);
  }

  /**
   * Tests acquireShared will stop if it could not use state API.
   *
   * @covers ::acquireShared
   */
  public function testAcquireSharedWithoutState() {
    $lock_name = 'test_lock_share';
    // Lock state.
    $other_locker = new PersistentDatabaseLockBackend($this->container->get('database'));
    $other_locker->acquire(PersistentDatabaseSharedLockBackend::STATE_KEY_SHARED, 180);

    // Acquire a lock.
    $this->expectException(\PHPUnit\Framework\Error\Warning::class);
    $owner = $this->sharedLocker->acquireShared($lock_name);
    $this->assertFalse($owner, 'Could not acquire a shared lock.');

    // Release lock.
    $other_locker->release(PersistentDatabaseSharedLockBackend::STATE_KEY_EXCLUSIVE);
  }

  /**
   * Tests acquireShared can allocate a main shared lock.
   *
   * @covers ::acquireShared
   */
  public function testAcquireSharedNoMainLock() {
    $lock_name = 'test_lock_share';

    // Make sure main lock is free.
    $is_free = $this->sharedLocker->lockMayBeAvailable($lock_name);
    $this->assertTrue($is_free, 'No main shared lock.');

    // Acquire a lock.
    $owner = $this->sharedLocker->acquireShared($lock_name);
    $this->assertNotEmpty($owner, 'Could acquire a shared lock.');

    // Release lock.
    $this->sharedLocker->releaseShared($lock_name, $owner);
  }

  /**
   * Tests acquireShared can not use an existing exclusive lock as main lock.
   *
   * @covers ::acquireShared
   * @covers ::lockMayBeAvailable
   */
  public function testAcquireSharedExistingExclusiveLock() {
    $lock_name = 'test_lock_share';

    // Make sure main lock is free.
    $is_free = $this->sharedLocker->lockMayBeAvailable($lock_name);
    $this->assertTrue($is_free, 'No lock for "' . $lock_name . '".');

    // Create a secondary locker.
    $other_locker = new PersistentDatabaseSharedLockBackend($this->container->get('database'));
    $success = $other_locker->acquire($lock_name, 180);
    $this->assertTrue($success, 'Exclusive lock acquired.');

    // Make sure main lock is not free.
    $is_free = $this->sharedLocker->lockMayBeAvailable($lock_name);
    $this->assertFalse($is_free, 'Main shared lock in use.');

    // Acquire a lock.
    $owner = $this->sharedLocker->acquireShared($lock_name);
    $this->assertFalse($owner, 'Could not acquire a shared lock.');

    // Release lock.
    $other_locker->release($lock_name);
  }

  /**
   * Tests acquireShared can use an existing main shared lock.
   *
   * @covers ::acquireShared
   * @covers ::lockMayBeAvailable
   */
  public function testAcquireSharedExistingMainLock() {
    $lock_name = 'test_lock_share';

    // Make sure main lock is free.
    $is_free = $this->sharedLocker->lockMayBeAvailable($lock_name);
    $this->assertTrue($is_free, 'No main shared lock.');

    // Create a secondary locker.
    $other_locker = new PersistentDatabaseSharedLockBackend($this->container->get('database'));
    $other_owner = $other_locker->acquireShared($lock_name);

    // Make sure main lock is not free.
    $is_free = $this->sharedLocker->lockMayBeAvailable($lock_name);
    $this->assertFalse($is_free, 'Main shared lock in use.');

    // Acquire a lock.
    $owner = $this->sharedLocker->acquireShared($lock_name);
    $this->assertNotEmpty($owner, 'Could acquire a shared lock.');

    // Release lock.
    $this->sharedLocker->releaseShared($lock_name, $owner);
    $other_locker->releaseShared($lock_name, $other_owner);
  }

  /**
   * Tests acquireShared can extend a timeout.
   *
   * @covers ::acquireShared
   */
  public function testAcquireSharedExtendTimeout() {
    $lock_name = 'test_lock_share';

    // Make sure main lock is free.
    $is_free = $this->sharedLocker->lockMayBeAvailable($lock_name);
    $this->assertTrue($is_free, 'No main shared lock.');

    // Create a secondary locker and share a lock.
    $other_locker = new PersistentDatabaseSharedLockBackend($this->container->get('database'));
    $other_owner = $other_locker->acquireShared($lock_name, 600);
    $this->assertNotEmpty($other_owner, 'Could acquire a shared lock.');
    // Get timeout.
    $first_timeout = $other_locker->getCurrentExpirationTime($lock_name);
    $this->assertNotEmpty($first_timeout, 'Got timeout.');

    // Acquire a share.
    $owner = $this->sharedLocker->acquireShared($lock_name, 30);
    $this->assertNotEmpty($owner, 'Could acquire another share.');
    // No change, timeout not reduced.
    $new_timeout = $this->sharedLocker->getCurrentExpirationTime($lock_name);
    $this->assertEquals($first_timeout, $new_timeout, 'Timeout unchanged.');

    // Re-acquire a lock.
    $owner = $this->sharedLocker->acquireShared($lock_name, 6000, $owner);
    $this->assertNotEmpty($owner, 'Could renew share.');
    // Timeout extended.
    $new_timeout = $this->sharedLocker->getCurrentExpirationTime($lock_name);
    $this->assertGreaterThan($first_timeout, $new_timeout, 'Timeout extended.');
    $new_timeout2 = $other_locker->getCurrentExpirationTime($lock_name);
    $this->assertEquals($new_timeout, $new_timeout2, 'Same timeout for both shares.');

    // Release lock.
    $this->sharedLocker->releaseShared($lock_name, $owner);
    $other_locker->releaseShared($lock_name, $other_owner);
  }

  /**
   * Tests acquireShared can extend a timeout.
   *
   * @covers ::acquireShared
   * @covers ::getOwnerPid
   * @covers ::getStartTime
   */
  public function testAcquireSharedDetails() {
    $lock_name = 'test_lock_share';

    // Acquire a shared lock.
    $owner = $this->sharedLocker->acquireShared($lock_name, 180);
    $this->assertNotEmpty($owner, 'Could acquire a shared lock.');

    // Start time.
    $start_time = $this->sharedLocker->getStartTime($lock_name, $owner);
    $this->assertGreaterThan(0, $start_time, 'Got a start time.');
    // PID.
    $pid = $this->sharedLocker->getOwnerPid($lock_name, $owner);
    $this->assertGreaterThan(0, $pid, 'Got a PID.');

    // Check that main share lock has no PID.
    $pid = $this->sharedLocker->getOwnerPid(
      $lock_name,
      PersistentDatabaseSharedLockBackend::LOCK_ID
    );
    $this->assertEquals(0, $pid, 'No PID for main share lock.');

    // Release lock.
    $this->sharedLocker->releaseShared($lock_name, $owner);
  }

  /**
   * Tests no release other locks.
   *
   * @covers ::release
   * @covers ::lockMayBeAvailable
   */
  public function testNoReleaseOther() {
    $lock_name = 'test_lock_share';

    // Create a secondary locker and get a lock.
    $other_locker = new PersistentDatabaseSharedLockBackend($this->container->get('database'));
    $success = $other_locker->acquire($lock_name, 180);
    $this->assertNotEmpty($success, 'Could acquire a lock.');

    // Try to release other lock.
    $this->sharedLocker->release($lock_name);
    $is_free =
      $this->sharedLocker->lockMayBeAvailable($lock_name)
      || $other_locker->lockMayBeAvailable($lock_name)
    ;
    $this->assertFalse($is_free, 'Other lock not released.');

    $other_locker->release($lock_name);

    $is_free = $this->sharedLocker->lockMayBeAvailable($lock_name);
    $this->assertTrue($is_free, 'Other lock released.');
  }

  /**
   * Tests no exclusive release on shared locks.
   *
   * @covers ::release
   * @covers ::lockMayBeAvailable
   */
  public function testNoReleaseOnShared() {
    $lock_name = 'test_lock_share';

    // Acquire a shared lock.
    $owner = $this->sharedLocker->acquireShared($lock_name, 180);
    $this->assertNotEmpty($owner, 'Could acquire shared lock.');

    // Try to release main lock or as an exclusive lock.
    $this->sharedLocker->release($lock_name);
    $is_free = $this->sharedLocker->lockMayBeAvailable($lock_name);
    $this->assertFalse($is_free, 'Shared lock not released.');

    // Release share for real.
    $this->sharedLocker->releaseShared($lock_name, $owner);
    $is_free = $this->sharedLocker->lockMayBeAvailable($lock_name);
    $this->assertTrue($is_free, 'Shared lock released.');
  }

  /**
   * Tests no releaseShared on exclusive locks.
   *
   * @covers ::releaseShared
   * @covers ::lockMayBeAvailable
   */
  public function testNoReleaseSharedOnExclusive() {
    $lock_name = 'test_lock_share';

    // Acquire a lock.
    $success = $this->sharedLocker->acquire($lock_name, 180);
    $this->assertTrue($success, 'Could acquire a lock.');
    // Get owner.
    $owner = $this->sharedLocker->getOwner($lock_name);
    $this->assertNotEmpty($owner, 'Got owner name.');

    // Try to release lock as a shared lock.
    $this->sharedLocker->releaseShared($lock_name, $owner);
    $is_free = $this->sharedLocker->lockMayBeAvailable($lock_name);
    $this->assertFalse($is_free, 'Lock not released by releaseShare().');

    // Release lock for real.
    $this->sharedLocker->release($lock_name);
    $is_free = $this->sharedLocker->lockMayBeAvailable($lock_name);
    $this->assertTrue($is_free, 'Lock released.');
  }

  /**
   * Tests releaseAll only release all owned locks.
   *
   * @covers ::releaseAll
   * @covers ::lockMayBeAvailable
   * @covers ::cleanUnusedSharedLocks
   */
  public function testReleaseAllOwn() {
    $lock_prefix = 'test_lock_share_';

    // Create a secondary locker.
    $other_locker = new PersistentDatabaseSharedLockBackend($this->container->get('database'));

    // Make sure we start in a clean environment.
    $this->sharedLocker->cleanUnusedSharedLocks();

    // Acquire locks.
    $success = $this->sharedLocker->acquire($lock_prefix . '1', 180);
    $this->assertTrue($success, 'Could acquire lock 1.');
    $owner = $this->sharedLocker->acquireShared($lock_prefix . '2', 180);
    $this->assertNotEmpty($owner, 'Could acquire shared lock 2.');
    $success = $other_locker->acquire($lock_prefix . '3', 180);
    $this->assertTrue($success, 'Could acquire lock 3.');
    $other_owner = $other_locker->acquireShared($lock_prefix . '4', 180);
    $this->assertNotEmpty($other_owner, 'Could acquire shared lock 4.');

    // Release all locks of current instance.
    $this->sharedLocker->releaseAll();
    $is_free = $this->sharedLocker->lockMayBeAvailable($lock_prefix . '1');
    $this->assertTrue($is_free, 'Lock 1 released.');
    $is_free = $this->sharedLocker->lockMayBeAvailable($lock_prefix . '2');
    $this->assertTrue($is_free, 'Lock 2 released.');
    $is_free = $this->sharedLocker->lockMayBeAvailable($lock_prefix . '3');
    $this->assertFalse($is_free, 'Lock 3 not released.');
    $is_free = $this->sharedLocker->lockMayBeAvailable($lock_prefix . '4');
    $this->assertFalse($is_free, 'Lock 4 not released.');

    // Force lock release through PersistentDatabaseSharedLockBackend.
    $parent_locker = new PersistentDatabaseLockBackend($this->container->get('database'));
    // We must acquire at least one lock with this locker in order to use
    // releaseAll().
    $parent_locker->acquire($lock_prefix . '5');
    $parent_locker->releaseAll(PersistentDatabaseSharedLockBackend::LOCK_ID);
    $parent_locker->release($lock_prefix . '5');
    // Check locks are now released.
    $is_free = $parent_locker->lockMayBeAvailable($lock_prefix . '3');
    $this->assertTrue($is_free, 'Lock 3 released.');
    $is_free = $parent_locker->lockMayBeAvailable($lock_prefix . '4');
    $this->assertTrue($is_free, 'Lock 4 released.');

    // Check states are not clean this way.
    $state = \Drupal::state();
    $exclusive_locks = $state->get(
      PersistentDatabaseSharedLockBackend::STATE_KEY_EXCLUSIVE,
      []
    );
    $shared_locks = $state->get(
      PersistentDatabaseSharedLockBackend::STATE_KEY_SHARED,
      []
    );
    $this->assertArrayHasKey($lock_prefix . '3', $exclusive_locks, 'Lock 3 not clean.');
    $this->assertArrayHasKey($lock_prefix . '4', $shared_locks, 'Lock 4 not clean.');

    // Cleanup.
    $this->sharedLocker->cleanUnusedSharedLocks();
    $exclusive_locks = $state->get(
      PersistentDatabaseSharedLockBackend::STATE_KEY_EXCLUSIVE,
      []
    );
    $shared_locks = $state->get(
      PersistentDatabaseSharedLockBackend::STATE_KEY_SHARED,
      []
    );
    $this->assertArrayNotHasKey($lock_prefix . '3', $exclusive_locks, 'Lock 3 clean.');
    $this->assertArrayNotHasKey($lock_prefix . '4', $shared_locks, 'Lock 4 clean.');
  }

  /**
   * Tests expiration delay.
   *
   * @covers ::getCurrentExpirationDelay
   */
  public function testGetCurrentExpirationDelay() {
    $lock_name = 'test_lock_share';
    $timeout = 600;

    // Acquire a lock.
    $success = $this->sharedLocker->acquire($lock_name, $timeout);
    $this->assertTrue($success, 'Could acquire a lock.');

    // Get Expiration (should not be 10 sec less than above timeout).
    $delay = $this->sharedLocker->getCurrentExpirationDelay($lock_name);
    $this->assertGreaterThan($timeout - 10, $delay, 'Expiration close to the initial timeout.');
    $this->assertLessThan($timeout, $delay, 'Expiration not greater than the initial timeout.');

    // Release lock.
    $this->sharedLocker->release($lock_name);

    // Acquire a shared lock.
    $owner = $this->sharedLocker->acquireShared($lock_name, $timeout);
    $this->assertNotEmpty($owner, 'Could acquire a shared lock.');

    // Get Expiration (should not be 10 sec less than above timeout).
    $delay = $this->sharedLocker->getCurrentExpirationDelay($lock_name);
    $this->assertGreaterThan($timeout - 10, $delay, 'Expiration close to the initial timeout (shared).');
    $this->assertLessThan($timeout, $delay, 'Expiration not greater than the initial timeout (shared).');

    // Release shared lock.
    $this->sharedLocker->releaseShared($lock_name, $owner);
  }

}
