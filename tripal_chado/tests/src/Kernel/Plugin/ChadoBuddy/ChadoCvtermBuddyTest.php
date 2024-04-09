<?php

namespace Drupal\Tests\tripal_chado\Kernel\Plugin\ChadoBuddy;

use Drupal\Tests\tripal_chado\Kernel\ChadoTestKernelBase;
use Drupal\tripal_chado\Database\ChadoConnection;

/**
 * Tests the Chado Cvterm Buddy.
 *
 * @group ChadoBuddy
 */
class ChadoCvtermBuddyTest extends ChadoTestKernelBase {
  protected $defaultTheme = 'stark';

  protected static $modules = ['system', 'user', 'file', 'tripal', 'tripal_chado'];

  protected ChadoConnection $connection;

  /**
   * Tests the getCv(), insertCv(), updateCv(), upsertCv() methods.
   * Focuses on those expected to work ;-)
   */
  public function testCvMethods() {

    $this->markTestIncomplete(
          'This test has not been implemented yet.'
    );

    // TEST: if there is no record then it should return false when we try to get it.

    // TEST: We should be able to insert a record if it doesn't exist.

    // TEST: We should be able to update an existing record.

    // TEST: Upsert should insert a record that doesn't exist.

    // TEST: Upsert should update a record that does exist.

    // TEST: we should be able to get the two records created above.

  }

  /**
   * Tests the getCvterm(), insertCvterm(), updateCvterm(), upsertCvterm() methods.
   * Focuses on those expected to work ;-)
   */
  public function testCvtermMethods() {

    $this->markTestIncomplete(
          'This test has not been implemented yet.'
    );

    // TEST: if there is no record then it should return false when we try to get it.

    // TEST: We should be able to insert a record if it doesn't exist.

    // TEST: We should be able to update an existing record.

    // TEST: Upsert should insert a record that doesn't exist.

    // TEST: Upsert should update a record that does exist.

    // TEST: we should be able to get the two records created above.

  }
}
