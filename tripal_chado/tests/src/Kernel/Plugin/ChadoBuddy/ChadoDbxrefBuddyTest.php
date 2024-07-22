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

  protected ChadoConnection $connection;

  protected static $modules = ['system', 'user', 'file', 'tripal', 'tripal_chado'];

  /**
   * {@inheritdoc}
   */
  protected function setUp(): void {
    parent::setUp();

    // Open connection to a test Chado
    $this->connection = $this->getTestSchema(ChadoTestKernelBase::INIT_DUMMY);
  }

  /**
   * Tests the getDb(), insertDb(), updateDb(), upsertDb() methods.
   * Focuses on those expected to work ;-)
   */
  public function testDbMethods() {

    $type = \Drupal::service('tripal_chado.chado_buddy');
    $instance = $type->createInstance('chado_dbxref_buddy', []);

    // TEST: if there is no record then it should return false when we try to get it.
    $chado_buddy_records = $instance->getDb(['name' => 'nowaydoesthisexist']);
    $this->assertFalse($chado_buddy_records, 'We did not retrieve FALSE for a DB that does not exist');

    // TEST: We should be able to retrieve an existing DB record. Dummy chado has 'test db'
    $chado_buddy_records = $instance->getDb(['name' => 'test db']);
    $this->assertIsObject($chado_buddy_records, 'We did not retrieve the existing DB "test db"');
    $values = $chado_buddy_records->getValues();
    $this->assertIsArray($values, 'We did not retrieve an array of values for the existing DB "test db"');
    $this->assertEquals(5, count($values), 'The values array is of unexpected size for the existing DB "test db"');

    // TEST: We should be able to insert a record if it doesn't exist.
    $chado_buddy_records = $instance->insertDb(['name' => 'newDb001', 'description' => 'desc001']);
    $this->assertIsObject($chado_buddy_records, 'We did not insert a new DB "newDb001"');
    $values = $chado_buddy_records->getValues();
    $this->assertIsArray($values, 'We did not retrieve an array of values for the new DB "newDb001"');
    $this->assertEquals(5, count($values), 'The values array is of unexpected size for the new DB "newDb001"');
    $db_id = $chado_buddy_records->getValue('db_id');
    $this->assertTrue(is_numeric($db_id), 'We did not retrieve an integer db_id for the new DB "newDb001"');

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

    $type = \Drupal::service('tripal_chado.chado_buddy');
    $instance = $type->createInstance('chado_dbxref_buddy', []);

    // TEST: if there is no record then it should return false when we try to get it.
    $chado_buddy_records = $instance->getDbxref(['accession' => 'nowaydoesthisexist']);
    $this->assertFalse($chado_buddy_records, 'We did not retrieve FALSE for a Dbxref that does not exist');

    // TEST: We should be able to retrieve an existing Dbxref record. Dummy chado has db_id=1, 'test_dbxref'
    $chado_buddy_records = $instance->getDbxref(['accession' => 'test_dbxref']);
    $this->assertIsObject($chado_buddy_records, 'We did not retrieve the existing Dbxref "test_dbxref"');
    $values = $chado_buddy_records->getValues();
    $this->assertIsArray($values, 'We did not retrieve an array of values for the existing Dbxref "test_dbxref"');
    $this->assertEquals(8, count($values), 'The values array is of unexpected size for the existing Dbxref "test_dbxref"');

    // TEST: We should be able to insert a record if it doesn't exist.

    // TEST: We should be able to update an existing record.

    // TEST: Upsert should insert a record that doesn't exist.

    // TEST: Upsert should update a record that does exist.

    // TEST: we should be able to get the two records created above.

    // TEST: We should be able to get a URL from a dbxref that has a urlprefix.

    // TEST: We should be able to get a URL from a dbxref that does not have a urlprefix.

  }
}
