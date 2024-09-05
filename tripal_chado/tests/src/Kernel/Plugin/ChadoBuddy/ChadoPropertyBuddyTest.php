<?php

namespace Drupal\Tests\tripal_chado\Kernel\Plugin\ChadoBuddy;

use Drupal\Tests\tripal_chado\Kernel\Plugin\ChadoBuddy\ChadoTestBuddyBase;
use Drupal\Tests\tripal_chado\Kernel\ChadoTestKernelBase;
use Drupal\tripal_chado\ChadoBuddy\Exceptions\ChadoBuddyException;
use Drupal\tripal_chado\Database\ChadoConnection;

/**
 * Tests the Chado Property Buddy.
 *
 * @group ChadoBuddy
 */
class ChadoPropertyBuddyTest extends ChadoTestBuddyBase {

  /**
   * {@inheritdoc}
   */
  protected function setUp(): void {
    parent::setUp();

    // Open connection to a test Chado
    $this->connection = $this->getTestSchema(ChadoTestKernelBase::PREPARE_TEST_CHADO);

    // Create some simple project records
    $this->connection->insert('1:project')
      ->fields(['name' => 'proj001'])
      ->execute();
      $this->connection->insert('1:project')
      ->fields(['name' => 'proj002'])
      ->execute();
  }

  /**
   * Tests the getProperty(), insertProperty(), updateProperty(), deleteProperty() methods.
   */
  public function testPropertyMethods() {

    $type = \Drupal::service('tripal_chado.chado_buddy');
    $instance = $type->createInstance('chado_property_buddy', []);

    // TEST: if there is no record then it should return an empty array when we try to get it.
    $chado_buddy_records = $instance->getProperty('project', 1, ['projectprop.type_id' => 1]);
    $this->assertIsArray($chado_buddy_records, 'We did not retrieve an array for a property that does not exist');
    $this->assertEquals(0, count($chado_buddy_records), 'We did not retrieve an empty array for a property that does not exist');

    // TEST: We should be able to insert a property record if it doesn't exist.
    $test_records = [];
    $test_records['set'] = $instance->insertProperty('project', 1, ['projectprop.type_id' => 1, 'projectprop.value' => 'prop001'], []);
    $test_records['get'] = $instance->getProperty('project', 1, ['projectprop.type_id' => 1]);
    $this->multiAssert('insertProperty', $test_records, 'project', 'property "prop001"', 28);

    // TEST: We should be able to update an existing property record.
    $test_records = [];
    $test_records['set'] = $instance->updateProperty('project', 1, ['projectprop.type_id' => 2, 'projectprop.value' => 'prop002', 'projectprop.rank' => 5],
                                                     ['projectprop.projectprop_id' => 1], []);
    $test_records['get'] = $instance->getProperty('project', 1, ['projectprop.type_id' => 2]);
    $values = $this->multiAssert('insertProperty', $test_records, 'project', 'property "prop002"', 28);
    $this->assertEquals(2, $values['get']['projectprop.type_id'], 'The property type was not updated for property "prop001"');
    $this->assertEquals(5, $values['get']['projectprop.rank'], 'The property rank was not updated for property "prop001"');

    // TEST: Updating a property that does not exist returns FALSE.
    $chado_buddy_record = $instance->updateProperty('project', 1, ['projectprop.type_id' => 2, 'projectprop.value' => 'prop000', 'projectprop.rank' => 5],
                                                    ['projectprop.projectprop_id' => 10], []);
    $this->assertFalse($chado_buddy_record, 'We did not get FALSE trying to update a property that does not exist');

    // TEST: Upsert should insert a Property record that doesn't exist.
    $test_records = [];
    $test_records['set'] = $instance->upsertProperty('project', 1, ['projectprop.type_id' => 1, 'projectprop.value' => 'prop003', 'projectprop.rank' => 5], []);
    $test_records['get'] = $instance->getProperty('project', 1, ['projectprop.type_id' => 1, 'projectprop.value' => 'prop003', 'projectprop.rank' => 5], []);
    $this->multiAssert('upsertProperty (new)', $test_records, 'project', 'property "prop003"', 28);

    // TEST: We should not be able to insert a property record if it does exist.
    $exception_caught = FALSE;
    $exception_message = '';
    try {
      $chado_buddy_record = $instance->insertProperty('project', 1, ['projectprop.type_id' => 1, 'projectprop.value' => 'prop001', 'projectprop.rank' => 5], []);
    } catch (ChadoBuddyException $e) {
      $exception_caught = TRUE;
      $exception_message = $e->getMessage();
    }
    $this->assertTrue($exception_caught, 'We should get an exception when inserting a property record that already exists.');
    $this->assertStringContainsString('already exists', $exception_message, "We did not get the exception message we expected when inserting a property record that already exists.");

    // TEST: Inserting a property without specifying a Cvterm should throw an exception.
    $exception_caught = FALSE;
    $exception_message = '';
    try {
      $chado_buddy_record = $instance->insertProperty('project', 1, ['projectprop.value' => 'propfail', 'projectprop.rank' => 5], []);
    } catch (ChadoBuddyException $e) {
      $exception_caught = TRUE;
      $exception_message = $e->getMessage();
    }
    $this->assertTrue($exception_caught, 'We should get an exception when inserting a property record that already exists.');
    $this->assertStringContainsString('neither cvterm.cvterm_id nor projectprop.type_id were specified', $exception_message, "We did not get the exception message we expected when inserting a property record that already exists.");

    // TEST: Upsert should update a Property record that does exist. Value is not in a unique constraint, so will be updated.
    $test_records = [];
    $test_records['set'] = $instance->upsertProperty('project', 1, ['projectprop.type_id' => 1, 'projectprop.value' => 'prop004', 'projectprop.rank' => 5], []);
    $test_records['get'] = $instance->getProperty('project', 1, ['projectprop.type_id' => 1, 'projectprop.rank' => 5], []);
    $values = $this->multiAssert('upsertProperty (existing)', $test_records, 'project', 'property "prop004"', 28);
    $cvterm_id = $values['get']['projectprop.projectprop_id'];
    $this->assertTrue(is_numeric($cvterm_id), 'We did not retrieve an integer cvterm_id for the upserted property "prop004"');

    // TEST: Update should throw an exception if it matches more than one record.
    $chado_buddy_records = $instance->getProperty('project', 1, ['projectprop.rank' => 5]);
    $this->assertEquals(2, count($chado_buddy_records), "Conditions for update do not match multiple records");
    $exception_caught = FALSE;
    $exception_message = '';
    $test_records = [];
    try {
      $test_records['set'] = $instance->updateProperty('project', 1, ['projectprop.value' => 'propfail'],
                                                                     ['projectprop.rank' => 5], []);
    } catch (ChadoBuddyException $e) {
      $exception_caught = TRUE;
      $exception_message = $e->getMessage();
    }
    $this->assertTrue($exception_caught, 'We should get an exception when updating a property record that matches multiple properties.');
    $this->assertStringContainsString('more than one record matched', $exception_message, "We did not get the exception message we expected when updating a property that matches multiple properties.");
    // Test that the update was not performed
    $chado_buddy_records = $instance->getProperty('project', 1, ['projectprop.value' => 'propfail']);
    $this->assertEquals(0, count($chado_buddy_records), "An update was incorrectly performed for conditions that match more than one record");

    // TEST: Upsert should throw an exception if it matches more than one record.
    $exception_caught = FALSE;
    $exception_message = '';
    try {
      $chado_buddy_record = $instance->upsertProperty('project', 1, ['projectprop.value' => 'propfail', 'projectprop.rank' => 5], []);
    } catch (ChadoBuddyException $e) {
      $exception_caught = TRUE;
      $exception_message = $e->getMessage();
    }
    $this->assertTrue($exception_caught, 'We should get an exception when upserting a property record that matches multiple properties.');
    $this->assertStringContainsString('more than one record matched', $exception_message, "We did not get the exception message we expected when upserting a property that matches multiple properties.");

    // TEST: we should not by default be able to delete more than one property
    // record at a time. The two existing ones both have rank=5.
    $exception_caught = FALSE;
    $exception_message = '';
    try {
      $num_deleted = $instance->deleteProperty('project', 1, ['projectprop.rank' => 5], []);
    } catch (ChadoBuddyException $e) {
      $exception_caught = TRUE;
      $exception_message = $e->getMessage();
    }
    $this->assertTrue($exception_caught, 'We should get an exception when deleting more than one property record.');
    $this->assertStringContainsString('max_delete is set to', $exception_message, "We did not get the exception message we expected when deleting more than one property.");

    // TEST: we should be able to delete a single property record
    $num_deleted = $instance->deleteProperty('project', 1, ['projectprop.value' => 'prop002'], []);
    $this->assertTrue(is_numeric($num_deleted), 'We did not retrieve an integer from deleteProperty');
    $this->assertEquals(1, $num_deleted, "We did not delete exactly one property record \"prop002\"");

    // TEST: We should be able to insert a property record even if the cvterm doesn't exist.
    // This tests a use case for an importer, both term and dbxref are automatically created.
    // This needs the opt-in flag 'create_cvterm'.
    $test_records = [];
    $test_records['set'] = $instance->insertProperty('project', 1, ['projectprop.value' => 'prop005', 'projectprop.rank' => 5,
                                                     'db.name' => 'local', 'cv.name' => 'local',
                                                     'cvterm.name' => 'name005', 'dbxref.accession' => 'acc005'],
                                                     ['create_cvterm' => TRUE]);
    $test_records['get'] = $instance->getProperty('project', 1, ['projectprop.value' => 'prop005', 'projectprop.rank' => 5]);
    $values = $this->multiAssert('insertProperty (create cvterm)', $test_records, 'project', 'property "prop005"', 28);
    $this->assertEquals('local', $values['get']['db.name'], 'The DB name is incorrect for the new property "prop005"');
    $this->assertEquals('local', $values['get']['cv.name'], 'The CV name is incorrect for the new property "prop005"');
    $this->assertEquals('acc005', $values['get']['dbxref.accession'], 'The dbxref accession is incorrect for the new property "prop005"');
    $this->assertEquals('name005', $values['get']['cvterm.name'], 'The dbxref accession is incorrect for the new property "prop005"');

    // TEST: we can pass cvterm.cvterm_id instead of projectprop.type_id for an upsert
    $test_records = [];
    $test_records['set'] = $instance->upsertProperty('project', 1, ['cvterm.cvterm_id' => 2, 'projectprop.value' => 'prop006'], []);
    $test_records['get'] = $instance->getProperty('project', 1, ['cvterm.cvterm_id' => 2, 'projectprop.value' => 'prop006']);
    $values = $this->multiAssert('upsertProperty (use cvterm.cvterm_id)', $test_records, 'project', 'property "prop006"', 28);
    $this->assertEquals(2, $values['get']['cvterm.cvterm_id'], 'The CV term id is incorrect for the property "prop006"');
    $this->assertEquals('prop006', $values['get']['projectprop.value'], 'The upserted value is incorrect for the property "prop006"');

    // TEST: we should be able to delete more than one property
    // record at a time if we set the max_delete option to unlimited.
    $chado_buddy_records = $instance->getProperty('project', 1, ['projectprop.rank' => 5]);
    $this->assertEquals(2, count($chado_buddy_records), "We did select multiple property records");
    $num_deleted = $instance->deleteProperty('project', 1, ['projectprop.rank' => 5], ['max_delete' => -1]);
    $this->assertTrue(is_numeric($num_deleted), 'We did not retrieve an integer from deleteProperty');
    $this->assertEquals(2, $num_deleted, "We did not delete exactly two property records");
    $chado_buddy_records = $instance->getProperty('project', 1, ['projectprop.rank' => 5]);
    $this->assertEquals(0, count($chado_buddy_records), "We did not delete multiple property records");

  }
}
