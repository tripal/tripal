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

    // TEST: We should be able to insert a DB record if it doesn't exist.
    $chado_buddy_records = $instance->insertDb(['name' => 'newDb001', 'description' => 'desc001']);
    $this->assertIsObject($chado_buddy_records, 'We did not insert a new DB "newDb001"');
    $values = $chado_buddy_records->getValues();
    $this->assertIsArray($values, 'We did not retrieve an array of values for the new DB "newDb001"');
    $this->assertEquals(5, count($values), 'The values array is of unexpected size for the new DB "newDb001"');
    $db_id = $chado_buddy_records->getValue('db_id');
    $this->assertTrue(is_numeric($db_id), 'We did not retrieve an integer db_id for the new DB "newDb001"');

    // TEST: We should be able to update an existing DB record.
    $chado_buddy_records = $instance->updateDb(['name' => 'newDb002', 'description' => 'desc002', 'urlprefix' => 'https://tripal.org/{db}/{accession}'],
                                               ['name' => 'newDb001']);
    $this->assertIsObject($chado_buddy_records, 'We did not update an existing DB "newDb001"');
    $values = $chado_buddy_records->getValues();
    $this->assertIsArray($values, 'We did not retrieve an array of values for the updated DB "newDb001"');
    $this->assertEquals('newDb002', $values['name'], 'The DB name was not updated for DB "newDb001"');
    $this->assertEquals('desc002', $values['description'], 'The DB description was not updated for DB "newDb001"');
    $this->assertEquals('https://tripal.org/{db}/{accession}', $values['urlprefix'],
      'The urlprefix was not added by updateDb');

    // TEST: Upsert should insert a record that doesn't exist.
    $chado_buddy_records = $instance->upsertDb(['name' => 'newDb003', 'description' => 'desc003']);
    $this->assertIsObject($chado_buddy_records, 'We did not upsert a new DB "newDb003"');
    $values = $chado_buddy_records->getValues();
    $this->assertIsArray($values, 'We did not retrieve an array of values for the new DB "newDb003"');
    $this->assertEquals(5, count($values), 'The values array is of unexpected size for the new DB "newDb003"');
    $db_id = $chado_buddy_records->getValue('db_id');
    $this->assertTrue(is_numeric($db_id), 'We did not retrieve an integer db_id for the new DB "newDb003"');

    // TEST: Upsert should update a record that does exist.
    $chado_buddy_records = $instance->upsertDb(['name' => 'newDb003', 'description' => 'desc003']);
    $this->assertIsObject($chado_buddy_records, 'We did not upsert an existing DB "newDb003"');
    $values = $chado_buddy_records->getValues();
    $this->assertIsArray($values, 'We did not retrieve an array of values for the upserted DB "newDb003"');
    $this->assertEquals(5, count($values), 'The values array is of unexpected size for the upserted DB "newDb003"');
    $db_id = $chado_buddy_records->getValue('db_id');
    $this->assertTrue(is_numeric($db_id), 'We did not retrieve an integer db_id for the upserted DB "newDb003"');

    // TEST: we should be able to get the two records created above.
    foreach (['newDb002', 'newDb003'] as $db_name) {
      $chado_buddy_records = $instance->getDb(['name' => $db_name]);
      $this->assertIsObject($chado_buddy_records, "We did not retrieve the existing DB \"$db_name\"");
      $values = $chado_buddy_records->getValues();
      $this->assertIsArray($values, "We did not retrieve an array of values for the existing DB \"$db_name\"");
      $this->assertEquals(5, count($values), "The values array is of unexpected size for the existing DB \"$db_name\"");
    }

    // TEST: We should not be able to insert a DB record if it does exist.
    // Run last because this causes an exception.
    $this->expectException(\Drupal\tripal_chado\ChadoBuddy\Exceptions\ChadoBuddyException::class);
    $chado_buddy_records = $instance->insertDb(['name' => 'test db', 'description' => '']);
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

    // TEST: We should be able to insert a Dbxref record if it doesn't exist.
    $chado_buddy_records = $instance->insertDbxref(['accession' => 'newDbxref001', 'db_name' => 'test db']);
    $this->assertIsObject($chado_buddy_records, 'We did not insert a new Dbxref "newDbxref001"');
    $values = $chado_buddy_records->getValues();
    $this->assertIsArray($values, 'We did not retrieve an array of values for the new Dbxref "newDbxref001"');
    $this->assertEquals(8, count($values), 'The values array is of unexpected size for the new Dbxref "newDbxref001"');
    $dbxref_id = $chado_buddy_records->getValue('dbxref_id');
    $this->assertTrue(is_numeric($dbxref_id), 'We did not retrieve an integer dbxref_id for the new Dbxref "newDbxref001"');
    $db_id = $chado_buddy_records->getValue('db_id');
    $this->assertTrue(is_numeric($db_id), 'We did not retrieve an integer db_id for the new Dbxref "newDbxref001"');

    // TEST: We should be able to update an existing Dbxref record without including db_id.
    $chado_buddy_records = $instance->updateDbxref(['accession' => 'newDbxref002'], ['accession' => 'newDbxref001']);
    $this->assertIsObject($chado_buddy_records, 'We did not update an existing Dbxref "newDbxref001"');
    $values = $chado_buddy_records->getValues();
    $this->assertIsArray($values, 'We did not retrieve an array of values for the updated DB "newDbxref001"');
    $this->assertEquals('newDbxref002', $values['accession'], 'The Dbxref accession was not updated for Dbxref "newDbxref001"');

    // TEST: Upsert should insert a Dbxref record that doesn't exist.
    $chado_buddy_records = $instance->upsertDbxref(['accession' => 'newDbxref003', 'db_name' => 'test db']);
    $this->assertIsObject($chado_buddy_records, 'We did not upsert a new Dbxref "newDbxref003"');
    $values = $chado_buddy_records->getValues();
    $this->assertIsArray($values, 'We did not retrieve an array of values for the new Dbxref "newDbxref003"');
    $this->assertEquals(8, count($values), 'The values array is of unexpected size for the new Dbxref "newDbxref003"');
    $dbxref_id = $chado_buddy_records->getValue('dbxref_id');
    $this->assertTrue(is_numeric($dbxref_id), 'We did not retrieve an integer dbxref_id for the new Dbxref "newDbxref003"');

    // TEST: Upsert should update a Dbxref record that does exist.
    $chado_buddy_records = $instance->upsertDbxref(['accession' => 'newDbxref003', 'db_id' => $db_id]);
    $this->assertIsObject($chado_buddy_records, 'We did not upsert an existing Dbxref "newDbxref003"');
    $values = $chado_buddy_records->getValues();
    $this->assertIsArray($values, 'We did not retrieve an array of values for the upserted Dbxref "newDbxref003"');
    $this->assertEquals(8, count($values), 'The values array is of unexpected size for the upserted Dbxref "newDbxref003"');
    $dbxref_id = $chado_buddy_records->getValue('dbxref_id');
    $this->assertTrue(is_numeric($dbxref_id), 'We did not retrieve an integer dbxref_id for the upserted Dbxref "newDbxref003"');

    // TEST: we should be able to get the two records created above.
    foreach (['newDbxref002', 'newDbxref003'] as $dbxref_accession) {
      $chado_buddy_records = $instance->getDbxref(['accession' => $dbxref_accession]);
      $this->assertIsObject($chado_buddy_records, "We did not retrieve the existing Dbxref \"$dbxref_accession\"");
      $values = $chado_buddy_records->getValues();
      $this->assertIsArray($values, "We did not retrieve an array of values for the existing Dbxref \"$dbxref_accession\"");
      $this->assertEquals(8, count($values), "The values array is of unexpected size for the existing Dbxref \"$dbxref_accession\"");
    }

    // TEST: We should be able to get a URL from a dbxref that has a urlprefix.
    $db_buddy = $instance->insertDb(['name' => 'newDb004', 'description' => 'desc004', 'urlprefix' => 'https://tripal.org/{db}/{accession}']);
    $this->assertIsObject($db_buddy, 'We did not insert a DB with a urlprefix');
    $db_id = $db_buddy->getValue('db_id');
    $this->assertTrue(is_numeric($db_id), 'We did not retrieve an integer db_id for the new DB with urlprefix');
    $dbxref_buddy = $instance->insertDbxref(['accession' => 'newDbxref004', 'db_id' => $db_id]);
    $url = $instance->getDbxrefUrl($dbxref_buddy);
    $this->assertIsString($url, 'We did not receive a string from getDbxrefUrl with urlprefix');
    $this->assertEquals('https://tripal.org/newDb004/newDbxref004', $url, "Incorrect url for a DB with urlprefix");

    // TEST: We should be able to get a URL from a dbxref that does not have a urlprefix.
    $db_buddy_records = $instance->updateDb(['urlprefix' => ''], ['name' => 'newDb004']);
    $this->assertIsObject($db_buddy_records, "We did not remove the urlprefix");
    $chado_buddy_records = $instance->getDbxref(['accession' => 'newDbxref004']);
    $this->assertIsObject($chado_buddy_records, "We did not retrieve the dbxref \"newDbxref004\"");
    $urlprefix = $chado_buddy_records->getValue('urlprefix');
    $this->assertEquals('', $urlprefix, "Removed urlprefix is not an empty string");
    $url = $instance->getDbxrefUrl($chado_buddy_records);
    $this->assertIsString($url, 'We did not receive a string from getDbxrefUrl without urlprefix');
    $this->assertEquals('cv/lookup/newDb004/newDbxref004', $url, "Incorrect url for a DB without urlprefix");
  }
}
