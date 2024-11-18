<?php

namespace Drupal\Tests\tripal_chado\Kernel\Plugin\ChadoBuddy;

use Drupal\Tests\tripal_chado\Kernel\ChadoTestKernelBase;
use Drupal\Tests\tripal_chado\Kernel\Plugin\ChadoBuddy\ChadoTestBuddyBase;
use Drupal\tripal_chado\ChadoBuddy\Exceptions\ChadoBuddyException;
use Drupal\tripal_chado\Database\ChadoConnection;

/**
 * Tests the Chado Dbxref Buddy.
 *
 * @group ChadoBuddy
 */
class ChadoDbxrefBuddyTest extends ChadoTestBuddyBase {

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
    $test_records = [];
    $test_records['set'] = $instance->insertDb(['db.name' => 'newDb001', 'db.description' => 'desc001']);
    $test_records['get'] = $instance->getDb(['db.name' => 'newDb001', 'db.description' => 'desc001']);
    $values = $this->multiAssert('insertDb', $test_records, 'db', 'db.db_id', 'db "newDb001"', 5);
    $db_id = $values['get']['db.db_id'];
    $this->assertTrue(is_numeric($db_id), 'We did not retrieve an integer db.id for the new DB "newDb001"');

    // TEST: Updating a non-existent DB should return FALSE.
    $chado_buddy_records = $instance->updateDb(['db.name' => 'newDb002', 'db.description' => 'desc002', 'db.urlprefix' => 'https://tripal.org/{db}/{accession}'],
                                               ['db.name' => 'does-not-exist']);
    $this->assertFalse($chado_buddy_records, "We received a value other than FALSE for an update to a DB that does not exist");

    // TEST: We should be able to update an existing DB record.
    $test_records = [];
    $test_records['set'] = $instance->updateDb(['db.name' => 'newDb002', 'db.description' => 'desc002', 'db.urlprefix' => 'https://tripal.org/{db}/{accession}'],
                                               ['db.name' => 'newDb001']);
    $test_records['get'] = $instance->getDb(['db.name' => 'newDb002']);
    $values = $this->multiAssert('updateDb', $test_records, 'db', 'db.db_id', 'db "newDb002"', 5);
    $this->assertEquals('newDb002', $values['get']['db.name'], 'The DB name was not updated for DB "newDb001"');
    $this->assertEquals('desc002', $values['get']['db.description'], 'The DB description was not updated for DB "newDb001"');
    $this->assertEquals('https://tripal.org/{db}/{accession}', $values['get']['db.urlprefix'],
      'The urlprefix was not added by updateDb');

    // TEST: Upsert should insert a record that doesn't exist.
    $test_records = [];
    $test_records['set'] = $instance->upsertDb(['db.name' => 'newDb003', 'db.description' => 'desc003']);
    $test_records['get'] = $instance->getDb(['db.name' => 'newDb003', 'db.description' => 'desc003']);
    $values = $this->multiAssert('upsertDb', $test_records, 'db', 'db.db_id', 'db "newDb003"', 5);
    $db_id = $values['get']['db.db_id'];
    $this->assertTrue(is_numeric($db_id), 'We did not retrieve an integer db_id for the new DB "newDb003"');

    // TEST: Upsert should update a record that does exist.
    // Conditions should not include description, url, or urlprefix
    $test_records = [];
    $test_records['set'] = $instance->upsertDb(['db.name' => 'newDb003', 'db.description' => 'desc004',
                                                'db.urlprefix' => 'pre004', 'db.url' => 'url004']);
    $test_records['get'] = $instance->getDb(['db.name' => 'newDb003']);
    $values = $this->multiAssert('upsertDb', $test_records, 'db', 'db.db_id', 'db "newDb003"', 5);
    $db_id = $values['get']['db.db_id'];
    $this->assertTrue(is_numeric($db_id), 'We did not retrieve an integer db.db_id for the upserted DB "newDb003"');
    $this->assertEquals('desc004', $values['get']['db.description'], 'The DB description was not updated for the upserted DB "newDb003"');
    $this->assertEquals('pre004', $values['get']['db.urlprefix'], 'The DB urlprefix was not updated for the upserted DB "newDb003"');
    $this->assertEquals('url004', $values['get']['db.url'], 'The DB url was not updated for the upserted DB "newDb003"');

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
    $chado_buddy_records = $instance->getDb(['db.description' => 'should fail']);
    $this->assertEquals(0, count($chado_buddy_records), "A db was incorrectly inserted when it already exists");

    // TEST: we should be able to get the two records created above. Will also catch if upsert did an insert instead of update.
    foreach (['newDb002', 'newDb003'] as $db_name) {
      $test_records = [];
      $test_records['get'] = $instance->getDb(['db.name' => $db_name]);
      $this->multiAssert('getDb', $test_records, 'db', 'db.db_id', 'db "'.$db_name.'"', 5);
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
    $test_records = [];
    $test_records['set'] = $instance->insertDbxref(['dbxref.accession' => 'newDbxref001', 'db.name' => 'local']);
    $test_records['get'] = $instance->getDbxref(['dbxref.accession' => 'newDbxref001', 'db.name' => 'local']);
    $values = $this->multiAssert('insertDbxref', $test_records, 'dbxref', 'dbxref.dbxref_id', 'dbxref "newDbxref001"', 10);
    $dbxref_id = $values['get']['dbxref.dbxref_id'];
    $this->assertTrue(is_numeric($dbxref_id), 'We did not retrieve an integer dbxref_id for the new Dbxref "newDbxref001"');
    $db_id = $values['get']['db.db_id'];
    $this->assertTrue(is_numeric($db_id), 'We did not retrieve an integer db.db_id for the new Dbxref "newDbxref001"');

    // TEST: Updating a non-existent Dbxref should return FALSE.
    $chado_buddy_records = $instance->updateDbxref(['dbxref.accession' => 'newDbxref002'],
                                                   ['dbxref.accession' => 'does-not-exist']);
    $this->assertFalse($chado_buddy_records, "We received a value other than FALSE for an update to a Dbxref that does not exist");

    // TEST: We should be able to update an existing Dbxref record without including db.db_id.
    $test_records = [];
    $test_records['set'] = $instance->updateDbxref(['dbxref.accession' => 'newDbxref002'],
                                                   ['dbxref.accession' => 'newDbxref001']);
    $test_records['get'] = $instance->getDbxref(['dbxref.accession' => 'newDbxref002']);
    $values = $this->multiAssert('updateDbxref', $test_records, 'dbxref', 'dbxref.dbxref_id', 'dbxref "newDbxref001"', 10);
    $this->assertEquals('newDbxref002', $values['get']['dbxref.accession'], "The Dbxref accession was not updated for newDbxref001");

    // TEST: Upsert should insert a Dbxref record that doesn't exist.
    $test_records = [];
    $test_records['set'] = $instance->upsertDbxref(['dbxref.accession' => 'newDbxref003', 'db.name' => 'local']);
    $test_records['get'] = $instance->getDbxref(['dbxref.accession' => 'newDbxref003', 'db.name' => 'local']);
    $values = $this->multiAssert('upsertDbxref', $test_records, 'dbxref', 'dbxref.dbxref_id', 'dbxref "newDbxref003"', 10);
    $dbxref_id = $values['get']['dbxref.dbxref_id'];
    $this->assertTrue(is_numeric($dbxref_id), 'We did not retrieve an integer dbxref_id for the new Dbxref "newDbxref003"');

    // TEST: Upsert should update a Dbxref record that does exist.
    // Conditions should not include description, but would include version.
    $test_records = [];
    $test_records['set'] = $instance->upsertDbxref(['dbxref.accession' => 'newDbxref003', 'dbxref.dbxref_id' => $dbxref_id,
                                                    'dbxref.description' => 'desc004']);
    $test_records['get'] = $instance->getDbxref(['dbxref.accession' => 'newDbxref003', 'dbxref.dbxref_id' => $dbxref_id,
                                                 'dbxref.description' => 'desc004']);
    $values = $this->multiAssert('upsertDbxref', $test_records, 'dbxref', 'dbxref.dbxref_id', 'dbxref "desc004"', 10);
    $dbxref_id = $values['get']['dbxref.dbxref_id'];
    $this->assertTrue(is_numeric($dbxref_id), 'We did not retrieve an integer dbxref_id for the upserted Dbxref "newDbxref003"');
    $this->assertEquals('desc004', $values['get']['dbxref.description'], 'The Dbxref description was not updated for the upserted Dbxref "newDbxref003"');

    // TEST: we should be able to get the two records created above.
    foreach (['newDbxref002', 'newDbxref003'] as $dbxref_accession) {
      $test_records = [];
      $test_records['get'] = $instance->getDbxref(['dbxref.accession' => $dbxref_accession]);
      $values = $this->multiAssert('getDbxref', $test_records, 'dbxref', 'dbxref.dbxref_id', 'dbxref "'.$dbxref_accession.'"', 10);
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

    // TEST: We should be able to get a URL from a dbxref that does NOT have a urlprefix.
    $db_buddy = $instance->insertDb(['db.name' => 'newDb005', 'db.description' => 'desc005']);
    $this->assertIsObject($db_buddy, 'We did not insert a DB without a urlprefix');
    $db_id = $db_buddy->getValue('db.db_id');
    $this->assertTrue(is_numeric($db_id), 'We did not retrieve an integer db.db_id for the new DB without a urlprefix');
    $dbxref_buddy = $instance->insertDbxref(['dbxref.accession' => 'newDbxref005', 'db.db_id' => $db_id]);
    $this->assertIsObject($dbxref_buddy, 'We did not insert a Dbxref without a urlprefix');
    $url = $instance->getDbxrefUrl($dbxref_buddy);
    $this->assertIsString($url, 'We did not receive a string from getDbxrefUrl without urlprefix');
    $this->assertEquals('cv/lookup/newDb005/newDbxref005', $url, "Incorrect url for a DB without a urlprefix");

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
