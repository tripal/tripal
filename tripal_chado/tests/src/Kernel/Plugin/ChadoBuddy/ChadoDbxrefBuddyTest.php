<?php

namespace Drupal\Tests\tripal_chado\Kernel\Plugin\ChadoBuddy;

use Drupal\Tests\tripal_chado\Kernel\ChadoTestKernelBase;
use Drupal\tripal_chado\Database\ChadoConnection;

/**
 * Tests the Chado Dbxref Buddy.
 *
 * @group ChadoBuddy
 */
class ChadoDbxrefBuddyTest extends ChadoTestKernelBase {
  protected $defaultTheme = 'stark';

  protected static $modules = ['system', 'user', 'file', 'tripal', 'tripal_chado'];

  protected ChadoConnection $connection;

  /**
   * Tests the getDb(), insertDb(), updateDb(), upsertDb() methods.
   * Focuses on those expected to work ;-)
   */
  public function testDbMethods() {

    // TEST: if there is no record then it should return false when we try to get it.

    // TEST: We should be able to insert a record if it doesn't exist.

    // TEST: We should be able to update an existing record.

    // TEST: Upsert should insert a record that doesn't exist.

    // TEST: Upsert should update a record that does exist.

    // TEST: we should be able to get the two records created above.

  }

  /**
   * Tests the getDbxref(), insertDbxref(), updateDbxref(), upsertDbxref() methods.
   * Focuses on those expected to work ;-)
   */
  public function testDbxrefMethods() {

    // TEST: if there is no record then it should return false when we try to get it.

    // TEST: We should be able to insert a record if it doesn't exist.

    // TEST: We should be able to update an existing record.

    // TEST: Upsert should insert a record that doesn't exist.

    // TEST: Upsert should update a record that does exist.

    // TEST: we should be able to get the two records created above.

  }
}
