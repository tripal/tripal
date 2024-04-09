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
   */
  public function testDbMethods() {

  }

  /**
   * Tests the getDbxref(), insertDbxref(), updateDbxref(), upsertDbxref() methods.
   */
  public function testDbxrefMethods() {

  }
}
