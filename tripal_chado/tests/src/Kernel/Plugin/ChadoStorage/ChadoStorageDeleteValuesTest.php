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
class ChadoStorageDeleteValuesTest extends ChadoTestKernelBase {

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
  public function testDeleteValues() {

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

    $field_names = array_keys($this->fields);
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

    // Now finally call findValues()
    $found_list = $this->chadoStorage->findValues($this->dataStoreValues);
    $this->assertIsArray($found_list, 'We were not able to call the findValues method without error.');
    $this->assertCount(50, $found_list,
      "There were 50 genes in the SQL file we populated the database with for this test. We should have found all of them and none others BUT we forgot to restrict the find to genes!!!");

    $found1 = $found_list[0];
    $found2 = $found_list[1];
    $name1 = $found1['gene_name'][0]['value']['value']->getValue();
    $record_id1 = $found1['gene_name'][0]['record_id']['value']->getValue();
    $type_id = $found1['gene_type'][0]['type_id']['value']->getValue();
    $contact_id = $found1['gene_contact'][0]['linker_id']['value']->getValue();

    $name2 = $found2['gene_name'][0]['value']['value']->getValue();
    $record_id2 = $found2['gene_name'][0]['record_id']['value']->getValue();

    // Before we delete a record, as a sanity check, let's confirm the
    // record exists in the database using straight SQL.
    $query1 = $this->chado_connection->select('1:feature','f');
    $query1->fields('f');
    $query1->condition('feature_id', $record_id1);
    $result1 = $query1->execute()->fetchObject();
    $this->assertEquals($result1->name, $name1,
        'Could not find the first record in the database returned by findValues().');

    // Make sure the contact link is gone, but not the contact.
    $query = $this->chado_connection->select('1:feature_contact','fc');
    $query->fields('fc');
    $result = $query->execute()->fetchObject();
    $this->assertIsObject($result, 'Missing the feature_contact record');

    $query2 = $this->chado_connection->select('1:feature','f');
    $query2->fields('f');
    $query2->condition('feature_id', $record_id2);
    $result2 = $query2->execute()->fetchObject();
    $this->assertEquals($result2->name, $name2,
        'Could not find the second record in the database returned by findValues().');

    // Now delete the first record record.
    $this->chadoStorage->deleteValues($found1);

    // Make sure the first record is gone but the second one is still there.
    $result1 = $query1->execute()->fetchObject();
    $result2 = $query2->execute()->fetchObject();
    $this->assertTrue($result1 == NULL, 'Failed to delete the record.');
    $this->assertEquals($result2->name, $name2,
        'Could not find the second record in the database after the first was deleted.');

    // Make sure we get 49 matches this time.
    $found_list = $this->chadoStorage->findValues($this->dataStoreValues);
    $this->assertCount(49, $found_list,
        "There were 49 genes in the SQL file we populated the database with for this test. We should have found all of them and none others BUT we forgot to restrict the find to genes!!!");

    // Make sure that the CVterm record is not deleted
    $query = $this->chado_connection->select('1:cvterm','cvt');
    $query->fields('cvt');
    $query->condition('cvterm_id', $type_id);
    $result = $query->execute()->fetchObject();
    $this->assertIsObject($result,
        'Deleted the cvterm record that should not have been deleted.');

    // Make sure the contact link is gone, but not the contact.
    $query = $this->chado_connection->select('1:feature_contact','fc');
    $query->fields('fc');
    $result = $query->execute()->fetchObject();
    $this->assertFalse($result, 'Did not delete the feature_contact record');

    // Make srue the contact record is still present.
    $query = $this->chado_connection->select('1:contact','c');
    $query->fields('c');
    $query->condition('contact_id', $contact_id);
    $result = $query->execute()->fetchObject();
    $this->assertIsObject($result, 'Should not have deleted the contact record');
  }
}
