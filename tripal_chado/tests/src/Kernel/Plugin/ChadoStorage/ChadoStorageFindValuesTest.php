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
    // Set the values for the gene_type field to indicate we only want genes.
    $values['gene_type'][0]['type_id'] = $this->getCvtermId('SO', '0000704');
    $values['gene_type'][0]['term_name'] = 'gene';
    $values['gene_type'][0]['id_space'] = 'SO';
    $values['gene_type'][0]['accession'] = '0000704';
    // And indicate the type of property.
    //$values['field_multi_value_chado_property'][0]['type_id'] = 3151;

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

    // Now we want to check a specific record
    // and make sure all the properties are set as we expect.
    $found = $found_list[0];
    $fields_working = ['gene_name', 'gene_uniquename', 'gene_type', 'gene_organism', 'gene_is_obsolete', 'gene_is_analysis', 'gene_sequence', 'gene_length', 'gene_sequence_md5_checksum'];
    foreach($fields_working as $field_name) {
      $this->assertArrayHasKey($field_name, $found,
        "The field was not in the found values array but it definitely should be.");
      foreach ($found[$field_name] as $delta => $found_values) {
        // Get the expected property types for this field.
        $expected_property_keys = array_keys($this->fields[$field_name]['properties']);
        foreach ($expected_property_keys as $property_key) {
          $this->assertArrayHasKey($property_key, $found_values,
            "This property should have existed in [$field_name][$delta] but it does not.");
          $this->assertNotNull($found_values[$property_key]['value']->getValue(),
            "The value should have been set for [$field_name][$delta][$property_key but it was NULL.");
        }
      }
    }

    // NOTE: Fields not fully working are:
    // gene_synonym, gene_contact and field_multi_value_chado_property

    /** Debugging information for the found list *
    foreach ($found as $k1 => $lvl2) {
      print "   $k1 =>\n";
      foreach ($lvl2 as $k2 => $lvl3) {
        print "       $k2 =>\n";
        foreach ($lvl3 as $k3 => $lvl4) {
          print "           $k3 => " . $lvl4['value']->getValue() . "\n";
        }
      }
    }
    */
  }
}
