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
class ChadoStorageActions_ReadValueTest extends ChadoTestKernelBase {

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
      ->onlyMethods(['warning'])
      ->getMock();
    $mock_logger->method('warning')
      ->willReturnCallback(function($message, $context, $options) {
        print str_replace(array_keys($context), $context, $message);
        return NULL;
      });
    $container->set('tripal.logger', $mock_logger);

    $this->setUpChadoStorageTestEnviro();
  }

  /**
   * Test the read_value action.
   *
   * Chado Table: project
   *     Columns: project_id*, name*, description
   *
   * Specifically,
   *  - Ensure that a property with the read_value action has the value set
   *  - Ensure that a property with the read_value action can't change the value
   *  - That two fields accessing the same chado column do not conflict
   *      A. both read_value action for the same column
   *      B. one read_value and one store for the same column
   */
  public function testReadValueAction() {

    // Set the fields for this test and then re-populate the storage arrays.
    $this->setFieldsFromYaml($this->yaml_file, 'testReadValueAction');
    $this->cleanChadoStorageValues();

    // Test Case: Insert valid values when they do not yet exist in Chado.
    // ---------------------------------------------------------
    $insert_values = [
      'test_read' => [
        [
          'record_id' => NULL,
          // A read value should not set the value for insert
          // We are doing so here to make sure it cannot modify the
          // values of the table!
          'name_read' => 'Project Name Set By Read',
        ],
      ],
      'other_field_store' => [
        [
          'record_id' => NULL,
          'name_store' => 'Correct Project Name',
        ],
      ],
      'other_field_read' => [
        [
          'record_id' => NULL,
          'name_read_again' => NULL,
        ],
      ],
    ];
    $this->chadoStorageTestInsertValues($insert_values);

    // Check that there is now a single project record.
    $query = $this->chado_connection->select('1:project', 'p')
      ->fields('p', ['project_id', 'name'])
      ->execute();
    $projects = $query->fetchAll();
    $this->assertIsArray($projects,
    "We should have been able to select from the project table.");
    $this->assertCount(1, $projects,
      "There should only be a single project inserted by these 3 fields");

    // Check that the single project record has the name set by the `store` action.
    $expected_name = $insert_values['other_field_store'][0]['name_store'];
    $retrieved_name = $projects[0]->name;
    $this->assertEquals($expected_name, $retrieved_name,
      "We did not get the name that should have been set by the other_field_store:name_store property.");

    $project_id = $projects[0]->project_id;

    // Test Case: Load values existing in Chado.
    // ---------------------------------------------------------
    // First we want to reset all the chado storage arrays to ensure we are
    // doing a clean test. The values will purposefully remain in Chado but the
    // Property Types, Property Values and Data Values will be built from scratch.
    $this->cleanChadoStorageValues();

    // For loading only the store id/pkey/link items should be populated.
    $load_values = [
      'test_read' => [
        [
          'record_id' => $project_id,
          'name_read' => NULL,
        ],
      ],
      'other_field_store' => [
        [
          'record_id' => $project_id,
          'name_store' => NULL,
        ],
      ],
      'other_field_read' => [
        [
          'record_id' => $project_id,
          'name_read_again' => NULL,
        ],
      ],
    ];
    $retrieved_values = $this->chadoStorageTestLoadValues($load_values);

    // Check that the name in our fields have been loaded.
    $expected_name = $insert_values['other_field_store'][0]['name_store'];
    $retrieved = [
      'test_read' => $retrieved_values['test_read'][0]['name_read']['value']->getValue(),
      'other_field_store' => $retrieved_values['other_field_store'][0]['name_store']['value']->getValue(),
      'other_field_read' => $retrieved_values['other_field_read'][0]['name_read_again']['value']->getValue(),
    ];
    foreach ($retrieved as $field_name => $retrieved_name) {
      $this->assertEquals($expected_name, $retrieved_name,
        "The name we retrieved for the $field_name field did not match the one set with a store attribute during insert.");
    }

    // Test Case: Update values in Chado using ChadoStorage.
    // ---------------------------------------------------------
    // When updating we need all the store id/pkey/link records
    // and all values of the other properties.
    $update_values = [
      'test_read' => [
        [
          'record_id' => $project_id,
          'name_read' => $expected_name,
        ],
      ],
      'other_field_store' => [
        [
          'record_id' => $project_id,
          'name_store' => $expected_name,
        ],
      ],
      'other_field_read' => [
        [
          'record_id' => $project_id,
          'name_read_again' => $expected_name,
        ],
      ],
    ];

    // We then change the name for the store value.
    // Since the other are read they shouldn't be connected to a widget
    // and thus will remain the old value.
    $update_values['other_field_store'][0]['name_store'] = 'Updated Project Name';
    $this->chadoStorageTestUpdateValues($update_values);

    // Check that there is still only a single project record.
    $query = $this->chado_connection->select('1:project', 'p')
      ->fields('p', ['project_id', 'name'])
      ->execute();
    $projects = $query->fetchAll();
    $this->assertIsArray($projects,
    "We should have been able to select from the project table.");
    $this->assertCount(1, $projects,
      "There should only be a single project affected by these 3 fields");

    // Check that the single project record has the name set by the `store` action.
    $expected_name = $update_values['other_field_store'][0]['name_store'];
    $retrieved_name = $projects[0]->name;
    $this->assertEquals($expected_name, $retrieved_name,
      "The name was not updated to match the other_field_store:name_store property.");
  }


  /**
   * Test the read_value action works with table alias.
   *
   * Chado Table: project
   *     Columns: project_id*, name*, description
   *
   * Specifically, ensure that a property with the read_value action
   *  - Can be used with a table alias. This is testing load only.
   */
  public function testReadValueActionTableAlias() {

    // Set the fields for this test and then re-populate the storage arrays.
    $this->setFieldsFromYaml($this->yaml_file, 'testReadValueActionTableAlias');
    $this->cleanChadoStorageValues();

    // Create to project records for testing the load with later.
    // Test Case: Insert valid values when they do not yet exist in Chado.
    $test_values = [
      'test_alias' => [
        'name' => 'Project name for the aliased record',
      ],
      'test_noalias' => [
        'name' => 'Base Project Name',
      ],
    ];
    foreach ($test_values as $field => $values) {
      $project_id = $this->chado_connection->insert('1:project')
        ->fields($values)
        ->execute();
      $this->assertIsNumeric($project_id,
        "We should have been able to insert test data for $field into the project table with the values: " . print_r($values, TRUE));
      $test_values[$field]['project_id'] = $project_id;
    }

    // For loading only the store id/pkey/link items should be populated.
    $load_values = [
      'test_alias' => [
        [
          'record_id' => $test_values['test_alias']['project_id'],
        ],
      ],
      'test_noalias' => [
        [
          'record_id' => $test_values['test_noalias']['project_id'],
        ],
      ],
    ];
    $retrieved_values = $this->chadoStorageTestLoadValues($load_values);

    // Check that the name in our fields have been loaded.
    foreach ($test_values as $field => $values) {
      $ret_name = $retrieved_values[$field][0]['name_read']['value']->getValue();
      $this->assertEquals($test_values[$field]['name'], $ret_name,
        "The name retrieved should match the one we inserted into chado for $field.");

      $ret_id = $retrieved_values[$field][0]['record_id']['value']->getValue();
      $this->assertEquals($test_values[$field]['project_id'], $ret_id,
        "The project_id retrieved should match the one we inserted into chado for $field.");
    }
  }
}
