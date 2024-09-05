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
abstract class ChadoTestBuddyBase extends ChadoTestKernelBase {
  protected $defaultTheme = 'stark';

  protected ChadoConnection $connection;

  protected static $modules = ['system', 'user', 'file', 'tripal', 'tripal_chado'];

  /**
   * Performs a set of basic assertions for a chado buddy function.
   *
   * @param string $test_type
   *   Description of the type of action being tested,
   *   e.g. 'getProperty', 'insertProperty', etc.
   * @param array $test_records
   *   Associative array of chado buddy records, keys will be 'set' and 'get'.
   * @param string $base_table
   *   The name of the chado base table that the property is attached to.
   * @param array $description
   *   Describes the buddy in assertions, e.g. 'db "local"', 'property "prop001"'
   * @param int $count
   *   The expected number of values.
   * @return array
   *   Each of the sets of record values from the supplied buddy records,
   *   i.e. the 'set' values and the 'get' values.
   */
  protected function multiAssert(string $test_type, array $test_records,
      string $base_table, string $description, int $count) {
    $values = [];
    foreach ($test_records as $mode => $chado_buddy_records) {
      // mode 'set' will be an object, while 'get' will be an array of objects
      if ($mode == 'get') {
        $this->assertIsArray($chado_buddy_records, "On $test_type+$mode, we do not have an array of chado buddy records for $description");
        $this->assertEquals(1, count($chado_buddy_records), "On $test_type+$mode, we do not have exactly one record for $description");
        $chado_buddy_record = $chado_buddy_records[0];
      }
      else {
        $chado_buddy_record = $chado_buddy_records;
      }
      $this->assertIsObject($chado_buddy_record, "On $test_type+$mode, we do not have $description");
      $record_base_table = $chado_buddy_record->getBaseTable();
      $record_schema_name = $chado_buddy_record->getSchemaName();
      $record_values = $chado_buddy_record->getValues();
      $values[$mode] = $record_values;
      $this->assertIsArray($record_values, "On $test_type+$mode, we did not retrieve an array of values for $description");
      $this->assertEquals($count, count($record_values), "On $test_type+$mode, the values array is of unexpected size for $description");
      $pkey_id = $chado_buddy_record->getValue($base_table . 'prop.' . $base_table . 'prop_id');
      $this->assertTrue(is_numeric($pkey_id), "On $test_type+$mode, we did not retrieve an integer pkey_id for $description");
      $this->assertEquals($base_table, $record_base_table, "On $test_type+$mode, the base table is incorrect for $description");
      $this->assertEquals($this->testSchemaName, $record_schema_name, "On $test_type+$mode, the schema is incorrect for $description");
    }
    return $values;
  }

}