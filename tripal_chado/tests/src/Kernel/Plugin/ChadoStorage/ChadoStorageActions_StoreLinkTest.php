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
class ChadoStorageActions_StoreLinkTest extends ChadoTestKernelBase {

  use ChadoStorageTestTrait;

  // We will populate this variable at the start of each test
  // with fields specific to that test.
  protected $fields = [];

  protected $yaml_file = __DIR__ . "/ChadoStorageActions-FieldDefinitions.yml";

  protected int $project_id;

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
   * Test the store_link action.
   *
   * Chado Table: projectprop
   *     Columns: projectprop_id*, project_id*, type_id*, value, rank*
   *
   * Specifically, ensure that a property with the store action
   *  -
   */
  public function testStoreLinkAction() {

    // Set the fields for this test and then re-populate the storage arrays.
    $this->setFieldsFromYaml($this->yaml_file, 'testStoreLinkAction');
    $this->cleanChadoStorageValues();


    $types_used = [
      'right_linker'  => $this->getCvtermId('schema', 'comment'),
      'left_linker'  => $this->getCvtermId('schema', 'description'),
    ];
    $total_num_records = 4;

    // Test Case: Insert valid values when they do not yet exist in Chado.
    // ---------------------------------------------------------
    $insert_values = [
      'project' => [
        [
          'record_id' => NULL,
          'name_store' => uniqid(),
        ]
      ],
      'right_linker' => [
        [
          'record_pkey' => NULL,
          'fkey' => NULL,
          'type' => $types_used['right_linker'],
          'rank' => 0
        ],
        [
          'record_pkey' => NULL,
          'fkey' => NULL,
          'type' => $types_used['right_linker'],
          'rank' => 1
        ],
      ],
      'left_linker' => [
        [
          'record_pkey' => NULL,
          'fkey' => NULL,
          'type' => $types_used['left_linker'],
          'rank' => 0
        ],
        [
          'record_pkey' => NULL,
          'fkey' => NULL,
          'type' => $types_used['left_linker'],
          'rank' => 3
        ],
      ],
    ];
    $this->chadoStorageTestInsertValues($insert_values);

    $query = $this->chado_connection->select('1:projectprop', 'prop')
      ->fields('prop', ['projectprop_id', 'project_id', 'type_id', 'rank'])
      ->execute();
    $inserted_records = $query->fetchAll();
    $this->assertIsArray($inserted_records,
      "We should have been able to select from the records from the projectprop table.");
    $this->assertCount($total_num_records, $inserted_records,
      "We did not get the number of records in the projectprop table that we excepted after insert.");

    // Ensure there are to records for each field.
    foreach (['right_linker', 'left_linker'] as $field_name) {
      $query = $this->chado_connection->select('1:projectprop', 'prop')
        ->fields('prop', ['projectprop_id'])
        ->condition('prop.type_id', $types_used[$field_name], '=')
        ->orderBy('rank')
        ->execute();
      $varname = $field_name . '_pkeys';
      $$varname = $query->fetchCol();
      $this->assertIsArray($$varname,
        "We should have been able to select from the records from the projectprop table.");
      $this->assertCount(2, $$varname,
        "We did not get the number of records in the projectprop table for $field_name that we excepted after insert.");
    }

    // Test Case: Load values existing in Chado.
    // ---------------------------------------------------------
    // First we want to reset all the chado storage arrays to ensure we are
    // doing a clean test. The values will purposefully remain in Chado but the
    // Property Types, Property Values and Data Values will be built from scratch.
    $this->cleanChadoStorageValues();

    // For loading only the store id/pkey/link items should be populated.
    $project_id = 1;
    $load_values = [
      'project' => [
        [
          'record_id' => $project_id,
        ]
      ],
      'right_linker' => [
        [
          'record_pkey' => $right_linker_pkeys[0],
        ],
        [
          'record_pkey' => $right_linker_pkeys[1],
        ],
      ],
      'left_linker' => [
        [
          'record_pkey' => $left_linker_pkeys[0],
        ],
        [
          'record_pkey' => $left_linker_pkeys[1],
        ],
      ],
    ];
    $retrieved_values = $this->chadoStorageTestLoadValues($load_values);

    // Lets put together an expected array here based on the load and insert values.
    $expected_values = $insert_values;
    $expected_values['project'][0]['record_id'] = $project_id;
    $expected_values['right_linker'][0]['record_pkey'] = $right_linker_pkeys[0];
    $expected_values['right_linker'][0]['fkey'] = $project_id;
    $expected_values['right_linker'][1]['record_pkey'] = $right_linker_pkeys[1];
    $expected_values['right_linker'][1]['fkey'] = $project_id;
    $expected_values['left_linker'][0]['record_pkey'] = $left_linker_pkeys[0];
    $expected_values['left_linker'][0]['fkey'] = $project_id;
    $expected_values['left_linker'][1]['record_pkey'] = $left_linker_pkeys[1];
    $expected_values['left_linker'][1]['fkey'] = $project_id;

    // Check that the store values in our fields have been loaded as they were inserted.
    foreach ($expected_values as $field_name => $delta_records) {
      if ($field_name == 'project') { continue; }
      foreach ($delta_records as $delta => $expected_values) {
        foreach(['fkey', 'type', 'rank'] as $property) {
          $retrieved = $retrieved_values[$field_name][$delta][$property]['value']->getValue();
          $expected = $expected_values[$property];

          $this->assertEquals($expected, $retrieved,
            "The value we retrieved for $field_name.$delta.$property did not match the one set with a store attribute during insert.");
        }
      }
    }

    // Test Case: Update values in Chado using ChadoStorage.
    // ---------------------------------------------------------
    // When updating we need all the store id/pkey/link records
    // and all values of the other properties.
    $update_values = $insert_values;
    $update_values['project'][0]['record_id'] = $project_id;
    $update_values['right_linker'][0]['record_pkey'] = $right_linker_pkeys[0];
    $update_values['right_linker'][1]['record_pkey'] = $right_linker_pkeys[1];
    $update_values['left_linker'][0]['record_pkey'] = $left_linker_pkeys[0];
    $update_values['left_linker'][1]['record_pkey'] = $left_linker_pkeys[1];

    // Let's test this without any changes.
    $this->chadoStorageTestUpdateValues($update_values);

    $query = $this->chado_connection->select('1:projectprop', 'prop')
      ->fields('prop', ['projectprop_id', 'project_id', 'type_id', 'rank'])
      ->execute();
    $records = $query->fetchAll();
    $this->assertIsArray($records,
      "We should have been able to select from the records from the projectprop table.");
    $this->assertCount($total_num_records, $records,
      "We did not get the number of records in the projectprop table that we excepted after update.");

  }
}
