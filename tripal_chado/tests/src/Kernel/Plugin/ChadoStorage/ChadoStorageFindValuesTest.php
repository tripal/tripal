<?php

namespace Drupal\Tests\tripal_chado\Kernel\Plugin\ChadoStorage;

use Drupal\Tests\tripal_chado\Kernel\ChadoTestKernelBase;
use Drupal\Tests\tripal_chado\Traits\ChadoStorageTestTrait;

use Drupal\tripal\TripalStorage\StoragePropertyValue;
use Drupal\tripal\TripalStorage\StoragePropertyTypeBase;
use Drupal\tripal_chado\TripalStorage\ChadoIntStoragePropertyType;
use Drupal\tripal_chado\TripalStorage\ChadoVarCharStoragePropertyType;

/**
 * Tests that ChadoStorage::findValues() works as expected.
 *
 * @group Tripal
 * @group Tripal Chado
 * @group ChadoStorage
 */
class ChadoStorageFindValuesTest extends ChadoTestKernelBase {

  use ChadoStorageTestTrait;

  // We will populate this variable at the start of each test
  // with fields specific to that test.
  protected $fields = [];

  protected $yaml_file = __DIR__ . "/ChadoStorageFindValuesTest-FieldDefinitions.yml";

    /**
   * {@inheritdoc}
   */
  protected function setUp() :void {
    parent::setUp();

    // We need to mock the logger to test the progress reporting.
    $container = \Drupal::getContainer();
    $mock_logger = $this->getMockBuilder(\Drupal\tripal\Services\TripalLogger::class)
      ->onlyMethods(['warning', 'error'])
      ->getMock();
    $mock_logger->method('warning')
      ->willReturnCallback(function($message, $context, $options) {
        print str_replace(array_keys($context), $context, $message);
        return NULL;
      });
    $mock_logger->method('error')
      ->willReturnCallback(function($message, $context, $options) {
        print str_replace(array_keys($context), $context, $message);
        return NULL;
      });
    $container->set('tripal.logger', $mock_logger);

    $this->setFieldsFromYaml($this->yaml_file, 'gene');
    $this->setUpChadoStorageTestEnviro();

    $success = $this->chado_connection->executeSqlFile(
      __DIR__ . '/../../../../fixtures/TripalusDatabasicaChr1Genes.sql',
      ['chado' => $this->testSchemaName]);
    $this->assertTrue($success, 'Imported gene reports for Tripalus databasica.');
  }

  /**
   * Test the findValue method used by publish.
   */
  public function testFindValues() {

    // Setup an empty values array based on $this->fields
    $values = [];
    foreach($this->fields as $field_name => $parts) {
      $values[$field_name] = [ 0 => []];
      foreach ($parts['properties'] as $propery_key => $storage_deets) {
        $values[$field_name][0][$propery_key] = NULL;
      }
    }
    $field_names = array_keys($this->fields);
    // Count total number of properties expected for the fields in the
    // values array we are testing.
    $expected_property_counts = [
      'total' => 0,
    ];
    foreach ($field_names as $field_name) {
      $this->assertArrayHasKey($field_name, $this->fields,
        "The \$fields array is malformed. '$field_name' must exist as a key in the top level of the array.");
      $this->assertArrayHasKey('properties', $this->fields[$field_name],
        "The \$fields array is malformed. '$field_name' array must have a 'properties' key which defines all the properties for this field.");

      $expected_property_counts['total'] += count($this->fields[$field_name]['properties']);
      $expected_property_counts[$field_name] = count($this->fields[$field_name]['properties']);
    }

    // Create the property types based on our fields array.
    $this->createPropertyTypes($field_names, $expected_property_counts);

    // Add the types to chado storage.
    $this->addPropertyTypes2ChadoStorage($field_names, $expected_property_counts);

    // Create the property values + format them for testing with *Values methods.
    $this->createDataStoreValues($field_names, $values);

    // Set the values in the propertyValue objects.
    $this->setExpectedValues($field_names, $values);

    // $this->debugChadoStorageTestTraitArrays();

    // Now finally call findValues()
    $found_list = $this->chadoStorage->findValues($this->dataStoreValues);
    $this->assertIsArray($found_list, 'We were not able to call the findValues method without error.');
    $this->assertCount(50, $found_list,
      "There were 50 genes in the SQL file we populated the database with for this test. We should have found all of them and none others BUT we forgot to restrict the find to genes!!!");
  }
}
