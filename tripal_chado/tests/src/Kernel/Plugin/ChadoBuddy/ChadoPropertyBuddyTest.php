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
    $this->connection = $this->getTestSchema(ChadoTestKernelBase::INIT_DUMMY);
  }

  /**
   * Tests the getProperty(), insertProperty(), updateProperty(), deleteProperty() methods.
   */
  public function testPropertyMethods() {

    $type = \Drupal::service('tripal_chado.chado_buddy');
    $instance = $type->createInstance('chado_property_buddy', []);

    // TEST: if there is no record then it should return false when we try to get it.
    $chado_buddy_records = $instance->getProperty(['base_table' => 'feature', 'fkey' => 'feature_id', 'fkey_id' => 1]);
    $this->assertFalse($chado_buddy_records, 'We did not retrieve FALSE for a property that does not exist');

    // TEST: We should be able to insert a property record if it doesn't exist.
    $chado_buddy_records = $instance->insertProperty(['base_table' => 'feature', 'fkey_id' => 1,
                                                      'type_id' => 1, 'value' => 'prop001']);
    $this->assertIsObject($chado_buddy_records, 'We did not insert a new property "prop001"');
    $values = $chado_buddy_records->getValues();
    $base_table = $chado_buddy_records->getBaseTable();
    $schema_name = $chado_buddy_records->getSchemaName();
    $this->assertIsArray($values, 'We did not retrieve an array of values for the new property "prop001"');
    $this->assertEquals(6, count($values), 'The values array is of unexpected size for the new property "prop001"');
    $pkey_id = $chado_buddy_records->getValue('pkey_id');
    $this->assertTrue(is_numeric($pkey_id), 'We did not retrieve an integer pkey_id for the new property "prop001"');
    $this->assertEquals('feature', $base_table, 'The base table is incorrect for the existing new property "prop001"');
    $this->assertTrue(str_contains($schema_name, '_test_chado_'), 'The schema is incorrect for the new property "prop001"');

    // TEST: We should be able to update an existing property record.
    $chado_buddy_records = $instance->updateProperty(['type_id' => 2, 'value' => 'prop002'],
                                                     ['base_table' => 'feature', 'fkey_id' => 1]);
    $this->assertIsObject($chado_buddy_records, 'We did not update an existing property "prop001"');
    $values = $chado_buddy_records->getValues();
    $this->assertIsArray($values, 'We did not retrieve an array of values for the updated property "prop001"');
    $this->assertEquals('prop002', $values['value'], 'The property value was not updated for property "prop001"');
    $this->assertEquals(2, $values['type_id'], 'The property type was not updated for property "prop001"');

  }

}
