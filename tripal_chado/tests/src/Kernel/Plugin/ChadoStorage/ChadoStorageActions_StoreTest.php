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
 * NOTE: The store action is actually tested fairly well as part of the testing
 * for other actions. Specifically, pretty much every action test includes a store
 * property and the value of that property is checked alongside the other properties
 * as a way to confirm that the full record loaded properly. This was especially
 * necessary for the store_* actions since they are set before loading so checking
 * the store property is needed to ensure the record was actually loaded based
 * on the pre-set keys.
 *
 * Anyway, as such, this class only focuses on testing edge cases like alias use
 * and the delete_if_empty setting.
 *
 * @group Tripal
 * @group Tripal Chado
 * @group ChadoStorage
 * @group ChadoStorage Actions
 */
class ChadoStorageActions_StoreTest extends ChadoTestKernelBase {

  use ChadoStorageTestTrait;

  // We will populate this variable at the start of each test
  // with fields specific to that test.
  protected $fields = [];

  protected $yaml_file = __DIR__ . "/ChadoStorageActions-FieldDefinitions.yml";

  protected int $feature_id;

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

    // Create the organism record needed for testing.
    $organism_id = $this->chado_connection->insert('1:organism')
      ->fields([
        'genus' => 'Tripalus',
        'species' => 'databasica',
      ])
      ->execute();
    $this->assertIsNumeric($organism_id,
      'We should have been able to insert a organism for use with testing.');
    // Create the feature record needed for testing.
    $this->feature_id = $this->chado_connection->insert('1:feature')
      ->fields([
        'organism_id' => $organism_id,
        'type_id' => $this->getCvtermId('rdfs','type'),
        'uniquename' => uniqid(),
      ])
      ->execute();
    $this->assertIsNumeric($this->feature_id,
      'We should have been able to insert a feature for use with testing.');

  }

  /**
   * Test the store action when an alias is set and it's on a non-base table.
   *
   * Chado Table: featureprop
   *     Columns: featureprop_id*, feature_id*, type_id*, value, rank*
   *
   * Specifically, ensure that a property with the store action
   *  - can be inserted when an alias is used on a non-base table
   *  - can be loaded when an alias is used on a non-base table
   *  - can be updated when an alias is used on a non-base table
   */
  public function testStoreActionAlias() {

    // Set the fields for this test and then re-populate the storage arrays.
    $this->setFieldsFromYaml($this->yaml_file, 'testStoreActionAlias');
    $this->cleanChadoStorageValues();

    $types_used = [
      'test_store_alias'  => $this->getCvtermId('schema', 'comment'),
      'test_store_other_alias'  => $this->getCvtermId('schema', 'description'),
    ];

    // Test Case: Insert valid values when they do not yet exist in Chado.
    // ---------------------------------------------------------
    $insert_values = [
      'test_store_alias' => [
        [
          'primary_key' => NULL,
          'fkey' => $this->feature_id,
          'type' => $types_used['test_store_alias'],
          'rank' => 0
        ],
        [
          'primary_key' => NULL,
          'fkey' => $this->feature_id,
          'type' => $types_used['test_store_alias'],
          'rank' => 1
        ],
      ],
      'test_store_other_alias' => [
        [
          'primary_key' => NULL,
          'fkey' => $this->feature_id,
          'type' => $types_used['test_store_other_alias'],
          'rank' => 0
        ],
        [
          'primary_key' => NULL,
          'fkey' => $this->feature_id,
          'type' => $types_used['test_store_other_alias'],
          'rank' => 3
        ],
      ],
    ];
    $this->chadoStorageTestInsertValues($insert_values);

    $query = $this->chado_connection->select('1:featureprop', 'prop')
      ->fields('prop', ['featureprop_id', 'feature_id', 'type_id', 'rank'])
      ->execute();
    $inserted_records = $query->fetchAll();
    $this->assertIsArray($inserted_records,
      "We should have been able to select from the records from the featureprop table.");
    $this->assertCount(4, $inserted_records,
      "We did not get the number of records in the featureprop table that we excepted after insert.");

    // Ensure there are to records for each field.
    foreach (['test_store_alias', 'test_store_other_alias'] as $field_name) {
      $query = $this->chado_connection->select('1:featureprop', 'prop')
        ->fields('prop', ['featureprop_id'])
        ->condition('prop.type_id', $types_used[$field_name], '=')
        ->orderBy('rank')
        ->execute();
      $varname = $field_name . '_pkeys';
      $$varname = $query->fetchCol();
      $this->assertIsArray($$varname,
        "We should have been able to select from the records from the featureprop table.");
      $this->assertCount(2, $$varname,
        "We did not get the number of records in the featureprop table for $field_name that we excepted after insert.");
    }

    // Test Case: Load values existing in Chado.
    // ---------------------------------------------------------
    // First we want to reset all the chado storage arrays to ensure we are
    // doing a clean test. The values will purposefully remain in Chado but the
    // Property Types, Property Values and Data Values will be built from scratch.
    $this->cleanChadoStorageValues();

    // For loading only the store id/pkey/link items should be populated.
    $load_values = [
      'test_store_alias' => [
        [
          'primary_key' => $test_store_alias_pkeys[0],
        ],
        [
          'primary_key' => $test_store_alias_pkeys[1],
        ],
      ],
      'test_store_other_alias' => [
        [
          'primary_key' => $test_store_other_alias_pkeys[0],
        ],
        [
          'primary_key' => $test_store_other_alias_pkeys[1],
        ],
      ],
    ];
    $retrieved_values = $this->chadoStorageTestLoadValues($load_values);

    // Check that the store values in our fields have been loaded as they were inserted.
    foreach ($insert_values as $field_name => $delta_records) {
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
    $update_values['test_store_alias'][0]['primary_key'] = $test_store_alias_pkeys[0];
    $update_values['test_store_alias'][1]['primary_key'] = $test_store_alias_pkeys[1];
    $update_values['test_store_other_alias'][0]['primary_key'] = $test_store_other_alias_pkeys[0];
    $update_values['test_store_other_alias'][1]['primary_key'] = $test_store_other_alias_pkeys[1];

    // Let's test this with a change.
    $update_values['test_store_other_alias'][1]['rank'] = 1;
    $this->chadoStorageTestUpdateValues($update_values);

    $query = $this->chado_connection->select('1:featureprop', 'prop')
      ->fields('prop', ['featureprop_id', 'feature_id', 'type_id', 'rank'])
      ->execute();
    $records = $query->fetchAll();
    $this->assertIsArray($records,
      "We should have been able to select from the records from the featureprop table.");
    $this->assertCount(4, $records,
      "We did not get the number of records in the featureprop table that we excepted after update.");

    // Finally check that the change happened in the database.
    /** This is not an easy check since ChadoStorage deletes all these records
     *  and recreates them... hense the primary keys are incremented and do not
     * remain constant.
    print_r($records);
    $query = $this->chado_connection->select('1:featureprop', 'prop')
      ->fields('prop', ['rank'])
      ->condition('prop.featureprop_id', $test_store_other_alias_pkeys[1], '=')
      ->execute();
    $ret_rank = $query->fetchField();
    $this->assertEquals(1, $ret_rank,
      "The rank does not seem to have been updated as we expected.");
     */

  }

  /**
   * Test the store action delete_if_empty on insert and update.
   *
   * Chado Table: featureprop
   *     Columns: featureprop_id*, feature_id*, type_id*, value*, rank*
   *
   * Specifically, ensure that a property with the store action
   *  - is deleted if it's an empty string and delete_if_empty is true
   *  - is kept and inserted as an empty string if delete_if_empty is false
   */
  public function testStoreActionDeleteIfEmpty() {

    // Set the fields for this test and then re-populate the storage arrays.
    $this->setFieldsFromYaml($this->yaml_file, 'testStoreActionDeleteIfEmpty');
    $this->cleanChadoStorageValues();

    $types_used = [
      'test_store_alias'  => $this->getCvtermId('schema', 'comment'),
      'test_store_other_alias'  => $this->getCvtermId('schema', 'description'),
    ];

    // Test Case: Insert valid values when they do not yet exist in Chado.
    // ---------------------------------------------------------
    $insert_values = [
      'test_store_alias' => [
        [
          'primary_key' => NULL,
          'fkey' => $this->feature_id,
          'type' => $types_used['test_store_alias'],
          'value' => '', // Should NOT be inserted since delete_if_empty: TRUE
          'rank' => 0
        ],
        [
          'primary_key' => NULL,
          'fkey' => $this->feature_id,
          'type' => $types_used['test_store_alias'],
          'value' => 'pippin',
          'rank' => 1
        ],
        [
          'primary_key' => NULL,
          'fkey' => $this->feature_id,
          'type' => $types_used['test_store_alias'],
          'value' => 'samwise',
          'rank' => 2
        ],
      ],
      'test_store_other_alias' => [
        [
          'primary_key' => NULL,
          'fkey' => $this->feature_id,
          'type' => $types_used['test_store_other_alias'],
          'value' => 'merry',
          'rank' => 0
        ],
        [
          'primary_key' => NULL,
          'fkey' => $this->feature_id,
          'type' => $types_used['test_store_other_alias'],
          'value' => '', // SHOULD be inserted as delete_if_empty: FALSE
          'rank' => 3
        ],
      ],
    ];
    $this->chadoStorageTestInsertValues($insert_values);

    $query = $this->chado_connection->select('1:featureprop', 'prop')
      ->fields('prop', ['featureprop_id', 'feature_id', 'type_id', 'rank'])
      ->execute();
    $inserted_records = $query->fetchAll();
    $this->assertIsArray($inserted_records,
      "We should have been able to select from the records from the featureprop table.");
    $this->assertCount(4, $inserted_records,
      "We did not get the number of records in the featureprop table that we excepted after insert.");

    // Ensure there are to records for each field.
    foreach (['test_store_alias' => 2, 'test_store_other_alias' => 2] as $field_name => $expected_count) {
      $query = $this->chado_connection->select('1:featureprop', 'prop')
        ->fields('prop', ['featureprop_id'])
        ->condition('prop.type_id', $types_used[$field_name], '=')
        ->orderBy('rank')
        ->execute();
      $varname = $field_name . '_pkeys';
      $$varname = $query->fetchCol();
      $this->assertIsArray($$varname,
        "We should have been able to select from the records from the featureprop table.");
      $this->assertCount($expected_count, $$varname,
        "We did not get the number of records in the featureprop table for $field_name that we excepted after insert.");
    }

    // Test Case: Update values in Chado using ChadoStorage.
    // ---------------------------------------------------------
    // When updating we need all the store id/pkey/link records
    // and all values of the other properties.
    $update_values = $insert_values;
    $update_values['test_store_alias'][1]['primary_key'] = $test_store_alias_pkeys[0];
    $update_values['test_store_alias'][2]['primary_key'] = $test_store_alias_pkeys[1];
    $update_values['test_store_other_alias'][0]['primary_key'] = $test_store_other_alias_pkeys[0];
    $update_values['test_store_other_alias'][1]['primary_key'] = $test_store_other_alias_pkeys[1];

    // Now this one should be removed too.
    $update_values['test_store_alias'][2]['value'] = '';
    // Add another empty value which should be kept.
    $update_values['test_store_other_alias'][2] = $insert_values['test_store_other_alias'][1];
    $update_values['test_store_other_alias'][2]['rank'] = 5;
    $this->chadoStorageTestUpdateValues($update_values);

    $query = $this->chado_connection->select('1:featureprop', 'prop')
      ->fields('prop', ['featureprop_id', 'feature_id', 'type_id', 'rank'])
      ->execute();
    $records = $query->fetchAll();
    $this->assertIsArray($records,
      "We should have been able to select from the records from the featureprop table.");
    $this->assertCount(4, $records,
      "We did not get the number of records in the featureprop table that we excepted after update.");

    // Ensure there are to records for each field.
    foreach (['test_store_alias' => 1, 'test_store_other_alias' => 3] as $field_name => $expected_count) {
      $query = $this->chado_connection->select('1:featureprop', 'prop')
        ->fields('prop', ['featureprop_id'])
        ->condition('prop.type_id', $types_used[$field_name], '=')
        ->orderBy('rank')
        ->execute();
      $varname = $field_name . '_pkeys';
      $$varname = $query->fetchCol();
      $this->assertIsArray($$varname,
        "We should have been able to select from the records from the featureprop table.");
      $this->assertCount($expected_count, $$varname,
        "We did not get the number of records in the featureprop table for $field_name that we excepted after insert.");
    }
  }
}
