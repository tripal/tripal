<?php

namespace Drupal\Tests\tripal_chado\Kernel\Plugin\ChadoBuddy;

use Drupal\Tests\tripal_chado\Kernel\ChadoTestKernelBase;
use Drupal\tripal_chado\ChadoBuddy\Exceptions\ChadoBuddyException;
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
    $this->connection = $this->getTestSchema(ChadoTestKernelBase::PREPARE_TEST_CHADO);
  }

  /**
   * Tests the xxxDb() methods.
   */
  public function testDbMethods() {

    $type = \Drupal::service('tripal_chado.chado_buddy');
    $instance = $type->createInstance('chado_dbxref_buddy', []);

    // TEST: if there is no record then it should return an empty array when we try to get it.
    $chado_buddy_records = $instance->getDb(['db.name' => 'nowaydoesthisexist']);
    $this->assertIsArray($chado_buddy_records, 'We did not retrieve an array for a DB that does not exist');
    $this->assertEquals(0, count($chado_buddy_records), 'We did not retrieve an empty array for a DB that does not exist');

    // TEST: We should be able to insert a DB record if it doesn't exist.
    $chado_buddy_record = $instance->insertDb(['db.name' => 'newDb001', 'db.description' => 'desc001']);
    $this->assertIsObject($chado_buddy_record, 'We did not insert a new DB "newDb001"');
    $values = $chado_buddy_record->getValues();
    $this->assertIsArray($values, 'We did not retrieve an array of values for the new DB "newDb001"');
    $this->assertEquals(5, count($values), 'The values array is of unexpected size for the new DB "newDb001"');
    $db_id = $chado_buddy_record->getValue('db.db_id');
    $this->assertTrue(is_numeric($db_id), 'We did not retrieve an integer db.id for the new DB "newDb001"');

    // TEST: We should be able to update an existing DB record.
    $chado_buddy_record = $instance->updateDb(['db.name' => 'newDb002', 'db.description' => 'desc002', 'db.urlprefix' => 'https://tripal.org/{db}/{accession}'],
                                               ['db.name' => 'newDb001']);
    $this->assertIsObject($chado_buddy_record, 'We did not update an existing DB "newDb001"');
    $values = $chado_buddy_record->getValues();
    $this->assertIsArray($values, 'We did not retrieve an array of values for the updated DB "newDb001"');
    $this->assertEquals('newDb002', $values['db.name'], 'The DB name was not updated for DB "newDb001"');
    $this->assertEquals('desc002', $values['db.description'], 'The DB description was not updated for DB "newDb001"');
    $this->assertEquals('https://tripal.org/{db}/{accession}', $values['db.urlprefix'],
      'The urlprefix was not added by updateDb');

    // TEST: Upsert should insert a record that doesn't exist.
    $chado_buddy_record = $instance->upsertDb(['db.name' => 'newDb003', 'db.description' => 'desc003']);
    $this->assertIsObject($chado_buddy_record, 'We did not upsert a new DB "newDb003"');
    $values = $chado_buddy_record->getValues();
    $this->assertIsArray($values, 'We did not retrieve an array of values for the new DB "newDb003"');
    $this->assertEquals(5, count($values), 'The values array is of unexpected size for the new DB "newDb003"');
    $db_id = $chado_buddy_record->getValue('db.db_id');
    $this->assertTrue(is_numeric($db_id), 'We did not retrieve an integer db_id for the new DB "newDb003"');

    // TEST: Upsert should update a record that does exist.
    // Conditions should not include description, url, or urlprefix
    $chado_buddy_record = $instance->upsertDb(['db.name' => 'newDb003', 'db.description' => 'desc004',
                                               'db.urlprefix' => 'pre004', 'db.url' => 'url004']);
    $this->assertIsObject($chado_buddy_record, 'We did not upsert an existing DB "newDb003"');
    $values = $chado_buddy_record->getValues();
    $this->assertIsArray($values, 'We did not retrieve an array of values for the upserted DB "newDb003"');
    $this->assertEquals(5, count($values), 'The values array is of unexpected size for the upserted DB "newDb003"');
    $db_id = $chado_buddy_record->getValue('db.db_id');
    $this->assertTrue(is_numeric($db_id), 'We did not retrieve an integer db.db_id for the upserted DB "newDb003"');
    $this->assertEquals('desc004', $values['db.description'], 'The DB description was not updated for the upserted DB "newDb003"');
    $this->assertEquals('pre004', $values['db.urlprefix'], 'The DB urlprefix was not updated for the upserted DB "newDb003"');
    $this->assertEquals('url004', $values['db.url'], 'The DB url was not updated for the upserted DB "newDb003"');

    // TEST: We should not be able to insert a DB record if it does exist.
    $exception_caught = FALSE;
    $exception_message = '';
    try {
      $chado_buddy_records = $instance->insertDb(['db.name' => 'newDb003', 'db.description' => 'should fail']);
    } catch (ChadoBuddyException $e) {
      $exception_caught = TRUE;
      $exception_message = $e->getMessage();
    }
    $this->assertTrue($exception_caught, 'We should get an exception when inserting a DB record that already exists.');
    $this->assertStringContainsString('already exists', $exception_message, "We did not get the exception message we expected when inserting a DB record that already exists.");

    // TEST: we should be able to get the two records created above. Will also catch if upsert did an insert instead of update.
    foreach (['newDb002', 'newDb003'] as $db_name) {
      $chado_buddy_records = $instance->getDb(['db.name' => $db_name]);
      $this->assertEquals(1, count($chado_buddy_records), "We did not retrieve the existing DB \"$db_name\"");
      $values = $chado_buddy_records[0]->getValues();
      $base_table = $chado_buddy_records[0]->getBaseTable();
      $schema_name = $chado_buddy_records[0]->getSchemaName();
      $this->assertIsArray($values, "We did not retrieve an array of values for the existing DB \"$db_name\"");
      $this->assertEquals(5, count($values), "The values array is of unexpected size for the existing DB \"$db_name\"");
      $this->assertEquals('db', $base_table, 'The base table is incorrect for the existing DB \"$db_name\"');
      $this->assertTrue(str_contains($schema_name, '_test_chado_'), 'The schema is incorrect for the existing DB \"$db_name\"');
    }

    // TEST: query should be case sensitive
    $chado_buddy_records = $instance->getDb(['db.name' => 'NEWdb003'], []);
    $this->assertEquals(0, count($chado_buddy_records), "We received case insensitive results for getDb when we should not have");

    // TEST: case insensitive override should work
    $chado_buddy_records = $instance->getDb(['db.name' => 'NEWdb003'], ['case_insensitive' => 'db.name']);
    $this->assertEquals(1, count($chado_buddy_records), "We did not receive case insensitive results for getDb when we should have");
  }

  /**
   * Tests the xxxDbxref() methods.
   */
  public function testDbxrefMethods() {

    $type = \Drupal::service('tripal_chado.chado_buddy');
    $instance = $type->createInstance('chado_dbxref_buddy', []);

    // TEST: if there is no record then it should return an empty array when we try to get it.
    $chado_buddy_records = $instance->getDbxref(['dbxref.accession' => 'nowaydoesthisexist']);
    $this->assertIsArray($chado_buddy_records, 'We did not retrieve an array for a Dbxref that does not exist');
    $this->assertEquals(0, count($chado_buddy_records), 'We did not retrieve an empty array for a Dbxref that does not exist');

    // TEST: We should be able to insert a Dbxref record if it doesn't exist.
    $chado_buddy_record = $instance->insertDbxref(['dbxref.accession' => 'newDbxref001', 'db.name' => 'local']);
    $this->assertIsObject($chado_buddy_record, 'We did not insert a new Dbxref "newDbxref001"');
    $values = $chado_buddy_record->getValues();
    $this->assertIsArray($values, 'We did not retrieve an array of values for the new Dbxref "newDbxref001"');
    $this->assertEquals(10, count($values), 'The values array is of unexpected size for the new Dbxref "newDbxref001"');
    $dbxref_id = $chado_buddy_record->getValue('dbxref.dbxref_id');
    $this->assertTrue(is_numeric($dbxref_id), 'We did not retrieve an integer dbxref_id for the new Dbxref "newDbxref001"');
    $db_id = $chado_buddy_record->getValue('db.db_id');
    $this->assertTrue(is_numeric($db_id), 'We did not retrieve an integer db.db_id for the new Dbxref "newDbxref001"');

    // TEST: We should be able to update an existing Dbxref record without including db.db_id.
    $chado_buddy_record = $instance->updateDbxref(['dbxref.accession' => 'newDbxref002'], ['dbxref.accession' => 'newDbxref001']);
    $this->assertIsObject($chado_buddy_record, 'We did not update an existing Dbxref "newDbxref001"');
    $values = $chado_buddy_record->getValues();
    $this->assertIsArray($values, 'We did not retrieve an array of values for the updated DB "newDbxref001"');
    $this->assertEquals('newDbxref002', $values['dbxref.accession'], 'The Dbxref accession was not updated for Dbxref "newDbxref001"');

    // TEST: Upsert should insert a Dbxref record that doesn't exist.
    $chado_buddy_record = $instance->upsertDbxref(['dbxref.accession' => 'newDbxref003', 'db.name' => 'local']);
    $this->assertIsObject($chado_buddy_record, 'We did not upsert a new Dbxref "newDbxref003"');
    $values = $chado_buddy_record->getValues();
    $this->assertIsArray($values, 'We did not retrieve an array of values for the new Dbxref "newDbxref003"');
    $this->assertEquals(10, count($values), 'The values array is of unexpected size for the new Dbxref "newDbxref003"');
    $dbxref_id = $chado_buddy_record->getValue('dbxref.dbxref_id');
    $this->assertTrue(is_numeric($dbxref_id), 'We did not retrieve an integer dbxref_id for the new Dbxref "newDbxref003"');

    // TEST: Upsert should update a Dbxref record that does exist.
    // Conditions should not include description, but would include version.
    $chado_buddy_record = $instance->upsertDbxref(['dbxref.accession' => 'newDbxref003', 'dbxref.dbxref_id' => $dbxref_id,
                                                   'dbxref.description' => 'desc004']);
    $this->assertIsObject($chado_buddy_record, 'We did not upsert an existing Dbxref "newDbxref003"');
    $values = $chado_buddy_record->getValues();
    $this->assertIsArray($values, 'We did not retrieve an array of values for the upserted Dbxref "newDbxref003"');
    $this->assertEquals(10, count($values), 'The values array is of unexpected size for the upserted Dbxref "newDbxref003"');
    $dbxref_id = $chado_buddy_record->getValue('dbxref.dbxref_id');
    $this->assertTrue(is_numeric($dbxref_id), 'We did not retrieve an integer dbxref_id for the upserted Dbxref "newDbxref003"');
    $this->assertEquals('desc004', $values['dbxref.description'], 'The Dbxref description was not updated for the upserted Dbxref "newDbxref003"');

    // TEST: we should be able to get the two records created above.
    foreach (['newDbxref002', 'newDbxref003'] as $dbxref_accession) {
      $chado_buddy_records = $instance->getDbxref(['dbxref.accession' => $dbxref_accession]);
      $this->assertEquals(1, count($chado_buddy_records), "We did not retrieve the existing Dbxref \"$dbxref_accession\"");
      $values = $chado_buddy_records[0]->getValues();
      $schema_name = $chado_buddy_records[0]->getSchemaName();
      $base_table = $chado_buddy_records[0]->getBaseTable();
      $this->assertIsArray($values, "We did not retrieve an array of values for the existing Dbxref \"$dbxref_accession\"");
      $this->assertEquals(10, count($values), "The values array is of unexpected size for the existing Dbxref \"$dbxref_accession\"");
      $this->assertEquals('dbxref', $base_table, 'The base table is incorrect for the existing Dbxref \"$dbxref_accession\"');
      $this->assertTrue(str_contains($schema_name, '_test_chado_'), 'The schema is incorrect for the existing Dbxref \"$dbxref_accession\"');
    }

    // TEST: query should be case sensitive
    $chado_buddy_records = $instance->getDbxref(['db.name' => 'Local', 'dbxref.accession' => 'NEWdbXREF003'], []);
    $this->assertEquals(0, count($chado_buddy_records), "We received case insensitive results for getDbxref when we should not have");

    // TEST: case insensitive override should work
    $chado_buddy_records = $instance->getDbxref(['db.name' => 'Local', 'dbxref.accession' => 'NEWdbXREF003'],
                                                ['case_insensitive' => ['db.name', 'dbxref.accession']]);
    $this->assertEquals(1, count($chado_buddy_records), "We did not receive case insensitive results for getDbxref when we should have");

    // TEST: We should be able to get a URL from a dbxref that has a urlprefix.
    $db_buddy = $instance->insertDb(['db.name' => 'newDb004', 'db.description' => 'desc004', 'db.urlprefix' => 'https://tripal.org/{db}/{accession}']);
    $this->assertIsObject($db_buddy, 'We did not insert a DB with a urlprefix');
    $db_id = $db_buddy->getValue('db.db_id');
    $this->assertTrue(is_numeric($db_id), 'We did not retrieve an integer db.db_id for the new DB with urlprefix');
    $dbxref_buddy = $instance->insertDbxref(['dbxref.accession' => 'newDbxref004', 'db.db_id' => $db_id]);
    $this->assertIsObject($dbxref_buddy, 'We did not insert a Dbxref with a urlprefix');
    $url = $instance->getDbxrefUrl($dbxref_buddy);
    $this->assertIsString($url, 'We did not receive a string from getDbxrefUrl with urlprefix');
    $this->assertEquals('https://tripal.org/newDb004/newDbxref004', $url, "Incorrect url for a DB with urlprefix");

    // TEST: We should be able to get a URL from a dbxref that does not have a urlprefix.
    $db_buddy_record = $instance->updateDb(['db.urlprefix' => ''], ['db.name' => 'newDb004']);
    $this->assertIsObject($db_buddy_record, "We did not remove the urlprefix");
    $chado_buddy_records = $instance->getDbxref(['dbxref.accession' => 'newDbxref004']);
    $this->assertEquals(1, count($chado_buddy_records), "We did not retrieve the dbxref \"newDbxref004\"");
    $urlprefix = $chado_buddy_records[0]->getValue('db.urlprefix');
    $this->assertEquals('', $urlprefix, "Removed urlprefix is not an empty string");
    $url = $instance->getDbxrefUrl($chado_buddy_records[0]);
    $this->assertIsString($url, 'We did not receive a string from getDbxrefUrl without urlprefix');
    $this->assertEquals('cv/lookup/newDb004/newDbxref004', $url, "Incorrect url for a DB without urlprefix");

    // TEST: associate a dbxref with a base table.
    $base_table = 'project';
    $query = $this->connection->insert('1:' . $base_table)
      ->fields(['name' => 'proj005'])
      ->execute();
    $linking_table = $base_table . '_dbxref';
    $status = $instance->associateDbxref($base_table, 1, $chado_buddy_records[0], []);
    $this->assertIsBool($status, "We did not retrieve a boolean when associating a dbxref with the base table \"$base_table\"");
    $this->assertTrue($status, "We did not retrieve TRUE when associating a dbxref with the base table \"$base_table\"");
    $query = $this->connection->select('1:' . $linking_table, 'lt')
      ->fields('lt', ['dbxref_id'])
      ->execute();
    $results = $query->fetchAll();
    $this->assertIsArray($results, "We should have been able to select from the \"$linking_table\" table");
    $this->assertCount(1, $results, "There should only be a single \"$linking_table\" record inserted");
    $expected_dbxref_id = $chado_buddy_records[0]->getValue('dbxref.dbxref_id');
    $retrieved_dbxref_id = $results[0]->dbxref_id;
    $this->assertEquals($expected_dbxref_id, $retrieved_dbxref_id,
      "We did not get the dbxref_id from \"$linking_table\" that should have been set by associateDbxref");
  }
}
