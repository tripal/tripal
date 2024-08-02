<?php

namespace Drupal\Tests\tripal_chado\Kernel\Plugin\ChadoBuddy;

use Drupal\Tests\tripal_chado\Kernel\ChadoTestKernelBase;
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

    // Create a simple project record
    $query = $this->connection->insert('1:project')
      ->fields(['name' => 'proj001'])
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
    $this->assertFalse($chado_buddy_records, 'We did not retrieve FALSE for a property that does not exist');

    // TEST: We should be able to insert a property record if it doesn't exist.
    $chado_buddy_records = $instance->insertProperty('project', 1, ['projectprop.type_id' => 1, 'projectprop.value' => 'prop001'], []);
    $this->assertIsObject($chado_buddy_records, 'We did not insert a new property "prop001"');
    $values = $chado_buddy_records->getValues();
    $base_table = $chado_buddy_records->getBaseTable();
    $schema_name = $chado_buddy_records->getSchemaName();
    $this->assertIsArray($values, 'We did not retrieve an array of values for the new property "prop001"');
    $this->assertEquals(28, count($values), 'The values array is of unexpected size for the new property "prop001"');
    $pkey_id = $chado_buddy_records->getValue('projectprop.projectprop_id');
    $this->assertTrue(is_numeric($pkey_id), 'We did not retrieve an integer pkey_id for the new property "prop001"');
    $this->assertEquals('project', $base_table, 'The base table is incorrect for the new property "prop001"');
    $this->assertTrue(str_contains($schema_name, '_test_chado_'), 'The schema is incorrect for the new property "prop001"');

    // TEST: We should be able to update an existing property record.
    $chado_buddy_records = $instance->updateProperty('project', 1, ['projectprop.type_id' => 2, 'projectprop.value' => 'prop002'],
                                                     ['projectprop.projectprop_id' => 1], []);
    $this->assertIsObject($chado_buddy_records, 'We did not update an existing property "prop001"');
    $values = $chado_buddy_records->getValues();
    $this->assertIsArray($values, 'We did not retrieve an array of values for the updated property "prop001"');
    $this->assertEquals('prop002', $values['projectprop.value'], 'The property value was not updated for property "prop001"');
    $this->assertEquals(2, $values['projectprop.type_id'], 'The property type was not updated for property "prop001"');

    // TEST: Upsert should insert a Property record that doesn't exist.
    $chado_buddy_records = $instance->upsertProperty('project', 1, ['projectprop.type_id' => 1, 'projectprop.value' => 'prop003', 'projectprop.rank' => 5], []);
    $this->assertIsObject($chado_buddy_records, 'We did not upsert a new property "prop003"');
    $values = $chado_buddy_records->getValues();
    $this->assertIsArray($values, 'We did not retrieve an array of values for the new property "prop003"');
    $this->assertEquals(28, count($values), 'The values array is of unexpected size for the new property "prop003"');
    $cvterm_id = $chado_buddy_records->getValue('projectprop.type_id');
    $this->assertTrue(is_numeric($cvterm_id), 'We did not retrieve an integer projectprop.type_id for the new property "prop003"');

    // TEST: Upsert should update a Property record that does exist. Value is not in a unique constraint, so will be updated.
    $chado_buddy_records = $instance->upsertProperty('project', 1, ['projectprop.type_id' => 1, 'projectprop.value' => 'prop004', 'projectprop.rank' => 5], []);
    $this->assertIsObject($chado_buddy_records, 'We did not upsert an existing Property "prop003"');
    $values = $chado_buddy_records->getValues();
    $this->assertIsArray($values, 'We did not retrieve an array of values for the upserted property "prop003"');
    $this->assertEquals(28, count($values), 'The values array is of unexpected size for the upserted property "prop003"');
    $cvterm_id = $chado_buddy_records->getValue('projectprop.projectprop_id');
    $this->assertTrue(is_numeric($cvterm_id), 'We did not retrieve an integer pkey_id for the upserted property "prop003"');
    $value = $chado_buddy_records->getValue('projectprop.value');
    $this->assertEquals('prop004', $value, 'The value was not updated for the upserted property "prop003"');

    // TEST: we should be able to get the two records created above.
    foreach (['prop002', 'prop004'] as $property_value) {
      $chado_buddy_records = $instance->getProperty('project', 1, ['projectprop.value' => $property_value], []);
      $this->assertIsObject($chado_buddy_records, "We did not retrieve the existing property \"$property_value\"");
      $values = $chado_buddy_records->getValues();
      $this->assertIsArray($values, "We did not retrieve an array of values for the existing property \"$property_value\"");
      $this->assertEquals(28, count($values), "The values array is of unexpected size for the existing property \"$property_value\"");
    }

    // TEST: we should be able to delete a record
    $num_deleted = $instance->deleteProperty('project', 1, ['projectprop.value' => 'prop002'], []);
    $this->assertTrue(is_numeric($num_deleted), 'We did not retrieve an integer from deleteProperty');
    $this->assertEquals(1, $num_deleted, "We did not delete exactly one property record \"prop002\"");

    // TEST: We should be able to insert a property record even if the cvterm doesn't exist.
    // This tests a use case for an importer, both term and dbxref are automatically created.
    // This needs the opt-in flag 'create_cvterm'
    $chado_buddy_records = $instance->insertProperty('project', 1, ['projectprop.value' => 'prop005',
                                                     'db.name' => 'local', 'cv.name' => 'local',
                                                     'cvterm.name' => 'name005', 'dbxref.accession' => 'acc005'],
                                                     ['create_cvterm' => TRUE]);
    $this->assertIsObject($chado_buddy_records, 'We did not insert a new property+cvterm "prop005"');
    $values = $chado_buddy_records->getValues();
    $base_table = $chado_buddy_records->getBaseTable();
    $schema_name = $chado_buddy_records->getSchemaName();
    $this->assertIsArray($values, 'We did not retrieve an array of values for the new property "prop005"');
    $this->assertEquals(28, count($values), 'The values array is of unexpected size for the new property "prop005"');
    $pkey_id = $chado_buddy_records->getValue('projectprop.projectprop_id');
    $this->assertTrue(is_numeric($pkey_id), 'We did not retrieve an integer pkey_id for the new property "prop005"');
    $this->assertEquals('project', $base_table, 'The base table is incorrect for the new property "prop005"');
    $this->assertTrue(str_contains($schema_name, '_test_chado_'), 'The schema is incorrect for the new property "prop005"');
    $this->assertEquals('local', $values['db.name'], 'The DB name is incorrect for the new property "prop005"');
    $this->assertEquals('local', $values['cv.name'], 'The CV name is incorrect for the new property "prop005"');
    $this->assertEquals('acc005', $values['dbxref.accession'], 'The dbxref accession is incorrect for the new property "prop005"');
    $this->assertEquals('name005', $values['cvterm.name'], 'The dbxref accession is incorrect for the new property "prop005"');


  }
}
