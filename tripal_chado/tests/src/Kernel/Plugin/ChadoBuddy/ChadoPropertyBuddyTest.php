<?php

namespace Drupal\Tests\tripal_chado\Kernel\Plugin\ChadoBuddy;

use Drupal\Tests\tripal_chado\Kernel\ChadoTestKernelBase;
use Drupal\tripal_chado\ChadoBuddy\Exceptions\ChadoBuddyException;
use Drupal\tripal_chado\Database\ChadoConnection;

/**
 * Tests the Chado Property Buddy.
 *
 * @group ChadoBuddy
 */
class ChadoPropertyBuddyTest extends ChadoTestKernelBase {
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

    // Create some simple project records
    $query = $this->connection->insert('1:project')
      ->fields(['name' => 'proj001'])
      ->execute();
      $query = $this->connection->insert('1:project')
      ->fields(['name' => 'proj002'])
      ->execute();
  }

  /**
   * Tests the getProperty(), insertProperty(), updateProperty(), deleteProperty() methods.
   */
  public function testPropertyMethods() {

    $type = \Drupal::service('tripal_chado.chado_buddy');
    $instance = $type->createInstance('chado_property_buddy', []);

    // TEST: if there is no record then it should return false when we try to get it.
    $chado_buddy_records = $instance->getProperty('project', 1, ['projectprop.type_id' => 1]);
    $this->assertIsArray($chado_buddy_records, 'We did not retrieve an array for a property that does not exist');
    $this->assertEquals(0, count($chado_buddy_records), 'We did not retrieve an empty array for a property that does not exist');

    // TEST: We should be able to insert a property record if it doesn't exist.
    $chado_buddy_record = $instance->insertProperty('project', 1, ['projectprop.type_id' => 1, 'projectprop.value' => 'prop001'], []);
    $this->assertIsObject($chado_buddy_record, 'We did not insert a new property "prop001"');
    $values = $chado_buddy_record->getValues();
    $base_table = $chado_buddy_record->getBaseTable();
    $schema_name = $chado_buddy_record->getSchemaName();
    $this->assertIsArray($values, 'We did not retrieve an array of values for the new property "prop001"');
    $this->assertEquals(28, count($values), 'The values array is of unexpected size for the new property "prop001"');
    $pkey_id = $chado_buddy_record->getValue('projectprop.projectprop_id');
    $this->assertTrue(is_numeric($pkey_id), 'We did not retrieve an integer pkey_id for the new property "prop001"');
    $this->assertEquals('project', $base_table, 'The base table is incorrect for the new property "prop001"');
    $this->assertTrue(str_contains($schema_name, '_test_chado_'), 'The schema is incorrect for the new property "prop001"');

    // TEST: We should be able to update an existing property record.
    $chado_buddy_record = $instance->updateProperty('project', 1, ['projectprop.type_id' => 2, 'projectprop.value' => 'prop002', 'projectprop.rank' => 5],
                                                    ['projectprop.projectprop_id' => 1], []);
    $this->assertIsObject($chado_buddy_record, 'We did not update an existing property "prop001"');
    $values = $chado_buddy_record->getValues();
    $this->assertIsArray($values, 'We did not retrieve an array of values for the updated property "prop001"');
    $this->assertEquals('prop002', $values['projectprop.value'], 'The property value was not updated for property "prop001"');
    $this->assertEquals(2, $values['projectprop.type_id'], 'The property type was not updated for property "prop001"');
    $this->assertEquals(5, $values['projectprop.rank'], 'The property rank was not updated for property "prop001"');

    // TEST: Updating a property that does not exist returns FALSE.
    $chado_buddy_record = $instance->updateProperty('project', 1, ['projectprop.type_id' => 2, 'projectprop.value' => 'prop000', 'projectprop.rank' => 5],
                                                    ['projectprop.projectprop_id' => 10], []);
    $this->assertFalse($chado_buddy_record, 'We dot not get FALSE trying to update a property that does not exist');

    // TEST: Upsert should insert a Property record that doesn't exist.
    $chado_buddy_record = $instance->upsertProperty('project', 1, ['projectprop.type_id' => 1, 'projectprop.value' => 'prop003', 'projectprop.rank' => 5], []);
    $this->assertIsObject($chado_buddy_record, 'We did not upsert a new property "prop003"');
    $values = $chado_buddy_record->getValues();
    $this->assertIsArray($values, 'We did not retrieve an array of values for the new property "prop003"');
    $this->assertEquals(28, count($values), 'The values array is of unexpected size for the new property "prop003"');
    $cvterm_id = $chado_buddy_record->getValue('projectprop.type_id');
    $this->assertTrue(is_numeric($cvterm_id), 'We did not retrieve an integer projectprop.type_id for the new property "prop003"');

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
    $chado_buddy_record = $instance->upsertProperty('project', 1, ['projectprop.type_id' => 1, 'projectprop.value' => 'prop004', 'projectprop.rank' => 5], []);
    $this->assertIsObject($chado_buddy_record, 'We did not upsert an existing Property "prop003"');
    $values = $chado_buddy_record->getValues();
    $this->assertIsArray($values, 'We did not retrieve an array of values for the upserted property "prop003"');
    $this->assertEquals(28, count($values), 'The values array is of unexpected size for the upserted property "prop003"');
    $cvterm_id = $chado_buddy_record->getValue('projectprop.projectprop_id');
    $this->assertTrue(is_numeric($cvterm_id), 'We did not retrieve an integer pkey_id for the upserted property "prop003"');
    $value = $chado_buddy_record->getValue('projectprop.value');
    $this->assertEquals('prop004', $value, 'The value was not updated for the upserted property "prop003"');

    // TEST: Update should throw an exception if it matches more than one record.
    $exception_caught = FALSE;
    $exception_message = '';
    try {
      $chado_buddy_record = $instance->updateProperty('project', 1, ['projectprop.value' => 'propfail'],
                                                                    ['projectprop.rank' => 5], []);
    } catch (ChadoBuddyException $e) {
      $exception_caught = TRUE;
      $exception_message = $e->getMessage();
    }
    $this->assertTrue($exception_caught, 'We should get an exception when updating a property record that matches multiple properties.');
    $this->assertStringContainsString('more than one record matched', $exception_message, "We did not get the exception message we expected when updating a property that matches multiple properties.");

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

    // TEST: we should be able to get the two records created above.
    foreach (['prop002', 'prop004'] as $property_value) {
      $chado_buddy_records = $instance->getProperty('project', 1, ['projectprop.value' => $property_value], []);
      $this->assertEquals(1, count($chado_buddy_records), "We did not retrieve the existing property \"$property_value\"");
      $values = $chado_buddy_records[0]->getValues();
      $this->assertIsArray($values, "We did not retrieve an array of values for the existing property \"$property_value\"");
      $this->assertEquals(28, count($values), "The values array is of unexpected size for the existing property \"$property_value\"");
    }

    // TEST: we should not by default be able to delete more than one property
    // record at a time. The two existing ones both have rank=5.
    $exception_caught = FALSE;
    $exception_message = '';
    try {
      $num_deleted = $instance->deleteProperty('project', 1, ['projectprop.rank' => 5], []);
      print "CP1 num_deleted=$num_deleted\n"; //@@@
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
    $chado_buddy_record = $instance->insertProperty('project', 1, ['projectprop.value' => 'prop005', 'projectprop.rank' => 5,
                                                    'db.name' => 'local', 'cv.name' => 'local',
                                                    'cvterm.name' => 'name005', 'dbxref.accession' => 'acc005'],
                                                    ['create_cvterm' => TRUE]);
    $this->assertIsObject($chado_buddy_record, 'We did not insert a new property+cvterm "prop005"');
    $values = $chado_buddy_record->getValues();
    $base_table = $chado_buddy_record->getBaseTable();
    $schema_name = $chado_buddy_record->getSchemaName();
    $this->assertIsArray($values, 'We did not retrieve an array of values for the new property "prop005"');
    $this->assertEquals(28, count($values), 'The values array is of unexpected size for the new property "prop005"');
    $pkey_id = $chado_buddy_record->getValue('projectprop.projectprop_id');
    $this->assertTrue(is_numeric($pkey_id), 'We did not retrieve an integer pkey_id for the new property "prop005"');
    $this->assertEquals('project', $base_table, 'The base table is incorrect for the new property "prop005"');
    $this->assertTrue(str_contains($schema_name, '_test_chado_'), 'The schema is incorrect for the new property "prop005"');
    $this->assertEquals('local', $values['db.name'], 'The DB name is incorrect for the new property "prop005"');
    $this->assertEquals('local', $values['cv.name'], 'The CV name is incorrect for the new property "prop005"');
    $this->assertEquals('acc005', $values['dbxref.accession'], 'The dbxref accession is incorrect for the new property "prop005"');
    $this->assertEquals('name005', $values['cvterm.name'], 'The dbxref accession is incorrect for the new property "prop005"');

    // TEST: we can pass cvterm.cvterm_id instead of projectprop.type_id for an upsert
    $chado_buddy_record = $instance->upsertProperty('project', 1, ['cvterm.cvterm_id' => 2, 'projectprop.value' => 'prop006'], []);
    $this->assertIsObject($chado_buddy_record, 'We did not upsert a new property "prop006"');
    $projectprop_value = $chado_buddy_record->getValue('projectprop.value');
    $this->assertEquals('prop006', $projectprop_value, 'We did not update the value using cvterm_id instead of type_id');

    // TEST: we should be able to delete more than one property
    // record at a time if we set the max_delete option to unlimited.
    $num_deleted = $instance->deleteProperty('project', 1, ['projectprop.rank' => 5], ['max_delete' => -1]);
    $this->assertTrue(is_numeric($num_deleted), 'We did not retrieve an integer from deleteProperty');
    $this->assertEquals(2, $num_deleted, "We did not delete exactly two property records");

  }
}
