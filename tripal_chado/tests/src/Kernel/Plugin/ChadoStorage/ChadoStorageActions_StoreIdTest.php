<?php

namespace Drupal\Tests\tripal_chado\Kernel\Plugin\ChadoStorage;

use Drupal\Tests\tripal_chado\Kernel\ChadoTestKernelBase;
use Drupal\Tests\tripal_chado\Traits\ChadoStorageTestTrait;

use Drupal\tripal\TripalStorage\StoragePropertyValue;
use Drupal\tripal\TripalStorage\StoragePropertyTypeBase;
use Drupal\tripal_chado\TripalStorage\ChadoIntStoragePropertyType;
use Drupal\tripal_chado\TripalStorage\ChadoVarCharStoragePropertyType;

/**
 * Tests that specific ChadoStorage actions perform as expected.
 *
 * @group Tripal
 * @group Tripal Chado
 * @group ChadoStorage
 * @group ChadoStorage Actions
 */
class ChadoStorageActions_StoreIdTest extends ChadoTestKernelBase {

  use ChadoStorageTestTrait;

  // We will populate this variable at the start of each test
  // with fields specific to that test.
  protected $fields = [];

  protected $yaml_file = __DIR__ . "/ChadoStorageActions-FieldDefinitions.yml";

  /**
   * {@inheritdoc}
   */
  protected function setUp() :void {
    parent::setUp();

    // Ensure we see all logging in tests.
    \Drupal::state()->set('is_a_test_environment', TRUE);

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

    $this->setUpChadoStorageTestEnviro();
  }

  /**
   * Test the store_id action.
   *
   * Chado Table: db
   *     Columns: db_id*, name*
   *
   * Specifically, ensure that a property with the store_id action
   *  - and a NULL value can insert a new record
   *  - has the value set on load
   *  - does not get changed on update
   */
  public function testStoreIdAction() {

    $field_name = 'test_store_id';

    // Set the fields for this test and then re-populate the storage arrays.
    $this->setFieldsFromYaml($this->yaml_file, "testStoreIdAction");
    $this->cleanChadoStorageValues();

    // Test Case: Insert valid values when they do not yet exist in Chado.
    // ---------------------------------------------------------
    $insert_values = [
      $field_name => [
        [
          'random_name' => NULL,
          'name' => 'Something random that is only here because this field is required.',
        ],
      ],
    ];
    $this->chadoStorageTestInsertValues($insert_values);

    // Check that there is now a single db record.
    $query = $this->chado_connection->select('1:db', 'db')
      ->fields('db', ['db_id', 'name'])
      ->condition('db.name', $insert_values[$field_name][0]['name'], '=')
      ->execute();
    $records = $query->fetchAll();
    $this->assertIsArray($records,
      "We should have been able to select from the record from the db table.");
    $this->assertCount(1, $records,
      "There should only be a single db with this name");

    $db_id = $records[0]->db_id;

    // Test Case: Load values existing in Chado.
    // ---------------------------------------------------------
    // First we want to reset all the chado storage arrays to ensure we are
    // doing a clean test. The values will purposefully remain in Chado but the
    // Property Types, Property Values and Data Values will be built from scratch.
    $this->cleanChadoStorageValues();

    // For loading only the store id/pkey/link items should be populated.
    // Since the id is passed in we're just checking it was set to the value property.
    $load_values = [
      $field_name => [
        [
          'random_name' => $db_id,
          'name' => NULL,
        ],
      ],
    ];
    $retrieved_values = $this->chadoStorageTestLoadValues($load_values);

    // Check that the name in our fields have been loaded.
    $retrieved_id = $retrieved_values[$field_name][0]['random_name']['value']->getValue();
    $this->assertEquals($db_id, $retrieved_id,
      "The ID we retrieved for the $field_name field did not match the one set with a store_id attribute during insert.");

    // We also check the name to be thorough.
    $expected_name = $insert_values[$field_name][0]['name'];
    $retrieved_name = $retrieved_values[$field_name][0]['name']['value']->getValue();
    $this->assertEquals($expected_name, $retrieved_name,
      "The name we retrieved for the $field_name field did not match the one set with a store attribute during insert.");

    // Test Case: Update values in Chado using ChadoStorage.
    // ---------------------------------------------------------
    // When updating we need all the store id/pkey/link records
    // and all values of the other properties.
    $update_values = [
      $field_name => [
        [
          'random_name' => $db_id,
          'name' => 'Something random that is only here because this field is required.',
        ],
      ],
    ];

    // Let's test this without changing anything.
    $this->chadoStorageTestUpdateValues($update_values);

    // Check that there is still only a single db record with this name.
    $query = $this->chado_connection->select('1:db', 'db')
      ->fields('db', ['db_id', 'name'])
      ->condition('db.name', $update_values[$field_name][0]['name'], '=')
      ->execute();
    $records = $query->fetchAll();
    $this->assertIsArray($records,
      "We should have been able to select from the record from the db table.");
    $this->assertCount(1, $records,
      "There should only be a single db with this name");
  }

  /**
   * Test the store_id action.
   *
   * Chado Table: db
   *     Columns: db_id*, name*
   *
   * Specifically, ensure that a property with the store_id action
   *  MUST BE associated with the base table of the field
   *  BUT NEED NOT BE a typical base table.
   */
  public function testStoreIdActionNotBase() {

    $field_name = 'test_non_base_match';

    // Set the fields for this test and then re-populate the storage arrays.
    $this->setFieldsFromYaml($this->yaml_file, "testStoreIdActionNotBase");
    $this->cleanChadoStorageValues();

    // Test Case: Insert valid values when they do not yet exist in Chado.
    // ---------------------------------------------------------
    $insert_values = [
      $field_name => [
        [
          'random_name' => NULL,
          'name' => 'Something random that is only here because this field is required.',
        ],
      ],
    ];
    ob_start();
    $this->chadoStorageTestInsertValues($insert_values);
    $printed_output = ob_get_clean();

    $this->assertStringContainsString(
      'uses the store_id action type but is not associated with the base table of the field',
      $printed_output,
      'We did not get the error message we expected when using the store_id action for a non-base table.'
    );
  }

  /**
   * Test the store_id action.
   *
   * Chado Table: db
   *     Columns: db_id*, name*
   *
   * Specifically, ensure that a property with the store_id action
   *  DOES NOT try to set a table alias.
   */
  public function testStoreIdActionTableAlias() {

    $field_name = 'test_chado_alias';

    // Set the fields for this test and then re-populate the storage arrays.
    $this->setFieldsFromYaml($this->yaml_file, "testStoreIdActionTableAlias");
    $this->cleanChadoStorageValues();

    // Test Case: Insert valid values when they do not yet exist in Chado.
    // ---------------------------------------------------------
    $insert_values = [
      $field_name => [
        [
          'random_name' => NULL,
        ],
      ],
    ];
    $this->expectExceptionMessage('The base table cannot have an alias');
    $this->chadoStorageTestInsertValues($insert_values);
  }
}
