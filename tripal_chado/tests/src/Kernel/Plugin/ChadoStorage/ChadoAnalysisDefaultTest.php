<?php

namespace Drupal\Tests\tripal_chado\Kernel\Plugin\ChadoStorage;

use Drupal\Tests\tripal_chado\Kernel\ChadoTestKernelBase;
use Drupal\Tests\tripal_chado\Traits\ChadoStorageTestTrait;

use Drupal\tripal\TripalStorage\StoragePropertyValue;
use Drupal\tripal\TripalStorage\StoragePropertyTypeBase;

use Drupal\Tests\tripal_chado\Functional\MockClass\FieldConfigMock;

/**
 * Tests that ChadoStorage can handle property fields as we expect.
 * The array of fields/properties used for these tests are designed
 * to match those in the ChadoAnalysisDefault field with values filled
 * based on a gene content type.
 *
 * Note: testotherfeaturefield is added to ensure we meet the unique constraint
 * on the base feature table and also to ensure we are testing multi-field functionality.
 *
 * Note: We do not need to test invalid conditions for createValues() and
 * updateValues() as these are only called after the entity has validated
 * the system using validateValues(). Instead we test all invalid conditions
 * are caught by validateValues().
 *
 * Specific test cases
 *  Test the following for both single and multiple property fields:
 *   - [SINGLE FIELD ONLY] Create Values in Chado using ChadoStorage when they don't yet exist.
 *   - [SINGLE FIELD ONLY] Load values in Chado using ChadoStorage after we just inserted them.
 *   - [SINGLE FIELD ONLY] Update values in Chado using ChadoStorage after we just inserted them.
 *   - [NOT IMPLEMENTED] Delete values in Chado using ChadoStorage.
 *   - [NOT IMPLEMENTED] Ensure property field picks up records in Chado not added through field.
 *
 * @group Tripal
 * @group Tripal Chado
 * @group Tripal Chado Fields
 * @group ChadoStorage
 */
class ChadoAnalysisDefaultTest extends ChadoTestKernelBase {

  use ChadoStorageTestTrait;

  protected $fields = [
    'testanalysisfieldA' => [
      'field_name' => 'testanalysisfieldA',
      'base_table' => 'phylotree',
      'properties' => [
        // Keeps track of the phylotree record our hypothetical field cares about.
        'A_record_id' => [
          'propertyType class' => 'Drupal\tripal_chado\TripalStorage\ChadoIntStoragePropertyType',
          'action' => 'store_id',
          'drupal_store' => TRUE,
          'chado_table' => 'phylotree',
          'chado_column' => 'phylotree_id'
        ],
        'A_phylotree_analysis' => [
          'propertyType class' => 'Drupal\tripal_chado\TripalStorage\ChadoIntStoragePropertyType',
          'action' => 'store',
          'chado_table' => 'phylotree',
          'chado_column' => 'analysis_id'
        ],
      ],
    ],
    'testanalysisfieldB' => [
      'field_name' => 'testanalysisfieldB',
      'base_table' => 'feature',
      'properties' => [
        'B_record_id' => [
          'propertyType class' => 'Drupal\tripal_chado\TripalStorage\ChadoIntStoragePropertyType',
          'action' => 'store_id',
          'drupal_store' => TRUE,
          'chado_table' => 'feature',
          'chado_column' => 'feature_id'
        ],
        'B_phylotree_analysis' => [
          'propertyType class' => 'Drupal\tripal_chado\TripalStorage\ChadoIntStoragePropertyType',
          'action' => 'store',
          'chado_table' => 'phylotree',
          'chado_column' => 'analysis_id'
        ],
      ],
    ],
    'testotherfeaturefield' => [
      'field_name' => 'testotherfeaturefield',
      'base_table' => 'feature',
      'properties' => [
        'record_id' => [
          'propertyType class' => 'Drupal\tripal_chado\TripalStorage\ChadoIntStoragePropertyType',
          'action' => 'store_id',
          'chado_table' => 'feature',
          'chado_column' => 'feature_id'
        ],
        'feature_type' => [
          'propertyType class' => 'Drupal\tripal_chado\TripalStorage\ChadoIntStoragePropertyType',
          'action' => 'store',
          'chado_table' => 'feature',
          'chado_column' => 'type_id'
        ],
        'feature_organism' => [
          'propertyType class' => 'Drupal\tripal_chado\TripalStorage\ChadoIntStoragePropertyType',
          'action' => 'store',
          'chado_table' => 'feature',
          'chado_column' => 'organism_id'
        ],
        'feature_uname' => [
          'propertyType class' => 'Drupal\tripal_chado\TripalStorage\ChadoVarCharStoragePropertyType',
          'action' => 'store',
          'chado_table' => 'feature',
          'chado_column' => 'uniquename'
        ],
      ],
    ],
  ];

  protected int $analysis_id;

  /**
   * {@inheritdoc}
   */
  protected function setUp() :void {
    parent::setUp();
    $this->setUpChadoStorageTestEnviro();

    // Create the analysis record for use with the phylotree table.
    $query = $this->chado_connection->insert('1:analysis');
    $query->fields([
      'name' => 'BogusA',
      'program' => 'bogusA',
      'programversion' => 'versionA',
    ]);
    $this->analysis_id = $query->execute();
  }

  /**
   * Testing ChadoStorage on single analysis field with multiple values.
   *
   * Test Cases:
   *   - Create Values in Chado using ChadoStorage when they don't yet exist.
   *   - Load values in Chado using ChadoStorage after we just inserted them.
   *   - Update values in Chado using ChadoStorage after we just inserted them.
   *   - [NOT IMPLEMENTED] Delete values in Chado using ChadoStorage.
   *   - [NOT IMPLEMENTED] Ensure analysis field picks up records in Chado not added through field.
   */
  public function testInsertValuesForSingleField() {

    $rdfs_comment_cvtermID = $this->getCvtermID('rdfs', 'comment');
    $gene_cvtermID = $this->getCvtermID('SO', '0000704');
    $subspecies_cvtermID = $this->getCvtermID('SO', '0000704');

    // Test Case: Insert valid values when they do not yet exist in Chado.
    // ---------------------------------------------------------
    $insert_values = [
      'testpropertyfieldA' => [
        [
          'A_type_id' => $rdfs_comment_cvtermID,
          'A_value' => 'Note 1',
          'A_rank' => 0,
        ],
        [
          'A_type_id' => $rdfs_comment_cvtermID,
          'A_value' => 'Note 2',
          'A_rank' => 1,
        ],
        [
          'A_type_id' => $rdfs_comment_cvtermID,
          'A_value' => 'Note 3',
          'A_rank' => 2,
        ]
      ],
      'testotherfeaturefield' => [
        [
          'feature_type' => $gene_cvtermID,
          'feature_organism' => $this->organism_id,
          'feature_uname' => 'testGene4PropTableTest',
        ]
      ],
    ];
    $this->chadoStorageTestInsertValues($insert_values);

    // @debug $this->debugChadoStorageTestTraitArrays();

    // Check that the base feature record was created in the database as expected.
    // Note: makes some assumptions based on knowing the data provider for
    // better readability of the tests.
    $field_name = 'testotherfeaturefield';
    $query = $this->chado_connection->select('1:feature', 'f')
      ->fields('f', ['feature_id', 'type_id', 'organism_id', 'uniquename'])
      ->execute();
    $records = $query->fetchAll();
    $this->assertCount(1, $records,
      "There should only be a single feature record created by our storage properties.");
    $record = $records[0];
    $record_expect = $insert_values[$field_name][0];
    $this->assertIsObject($record,
      "The returned feature record should be an object.");
    $this->assertEquals($record_expect['feature_type'], $record->type_id,
      "The feature record should have the type we set in our storage properties.");
    $this->assertEquals($record_expect['feature_organism'], $record->organism_id,
      "The feature record should have the organism we set in our storage properties.");
    $this->assertEquals($record_expect['feature_uname'], $record->uniquename,
        "The feature record should have the unique name we set in our storage properties.");
    $feature_id = $record->feature_id;

    // Also check that there are only the expected number of records
    // in the featureprop table.
    $query = $this->chado_connection->select('1:featureprop', 'prop')
        ->fields('prop', ['feature_id', 'type_id', 'value', 'rank'])
        ->execute();
    $all_featureprop_records = $query->fetchAll();
    $this->assertCount(3, $all_featureprop_records,
      "There were more records then we were expecting in the featureprop table: " . print_r($all_featureprop_records, TRUE));

    // Check that the featureprop records were created in the database as expected.
    // We use the unique key to select this particular value in order to
    // ensure it is here and there is one one.
    foreach ($insert_values['testpropertyfieldA'] as $delta => $expected) {
      $query = $this->chado_connection->select('1:featureprop', 'prop')
        ->fields('prop', ['featureprop_id', 'feature_id', 'type_id', 'value', 'rank'])
        ->condition('feature_id', $feature_id, '=')
        ->condition('type_id', $expected['A_type_id'])
        ->condition('rank', $expected['A_rank'])
        ->execute();
      $records = $query->fetchAll();
      $this->assertCount(1, $records, "We expected to get exactly one record for:" . print_r($expected, TRUE));
      $this->assertEquals($expected['A_value'], $records[0]->value, "We did not get the value we expected using the unique key." . print_r($expected, TRUE));

      $varname = 'prop' . $delta;
      $$varname = $records[0];
    }

    // Test Case: Load values existing in Chado.
    // ---------------------------------------------------------
    // First we want to reset all the chado storage arrays to ensure we are
    // doing a clean test. The values will purposefully remain in Chado but the
    // Property Types, Property Values and Data Values will  be built from scratch.
    $this->cleanChadoStorageValues();

    // For loading only the store id/pkey/link items should be populated.
    $load_values = [
      'testpropertyfieldA' => [
        [
          'A_record_id' => $feature_id,
          'A_prop_id' => $prop0->featureprop_id,
          'A_linker_id' => $feature_id,
        ],
        [
          'A_record_id' => $feature_id,
          'A_prop_id' => $prop1->featureprop_id,
          'A_linker_id' => $feature_id,
        ],
        [
          'A_record_id' => $feature_id,
          'A_prop_id' => $prop2->featureprop_id,
          'A_linker_id' => $feature_id,
        ]
      ],
      'testotherfeaturefield' => [
        [
          'record_id' => $feature_id,
        ]
      ],
    ];
    $retrieved_values = $this->chadoStorageTestLoadValues($load_values);

    // Now test that the additional values have been loaded.
    // @debug $this->debugChadoStorageTestTraitArrays();
    foreach([0,1,2] as $delta) {
      $retrieved = $retrieved_values['testpropertyfieldA'][$delta];
      $varname = 'prop' . $delta;
      $expected = $$varname;
      $this->assertEquals(
        $expected->type_id,
        $retrieved['A_type_id']['value']->getValue(),
        "The type for delta $delta did not match the one we retrieved from chado after insert."
      );
      $this->assertEquals(
        $expected->value,
        $retrieved['A_value']['value']->getValue(),
        "The value for delta $delta did not match the one we retrieved from chado after insert."
      );
      $this->assertEquals(
        $expected->rank,
        $retrieved['A_rank']['value']->getValue(),
        "The rank for delta $delta did not match the one we retrieved from chado after insert."
      );
    }

    // Test Case: Update values in Chado using ChadoStorage.
    // ---------------------------------------------------------
    // When updating we need all the store id/pkey/link records
    // and all values of the other properties.
    // array_merge alone seems not to be sufficient
    $update_values = $insert_values;
    foreach ($load_values as $field_name => $tmp) {
      foreach ($tmp as $delta => $id_values) {
        $update_values[$field_name][$delta] += $id_values;
      }
    }

    // We then change a few non key related values...
    $update_values['testpropertyfieldA'][1]['A_value'] = 'Changed Note to be more informative.';
    $update_values['testpropertyfieldA'][2]['A_value'] = 'Something completely different. Not even a note at all.';
    $this->chadoStorageTestUpdateValues($update_values);

    // Now we check chado to see if these values were changed...
    // Still the expected number of records in the featureprop table?
    $query = $this->chado_connection->select('1:featureprop', 'prop')
        ->fields('prop', ['feature_id', 'type_id', 'value', 'rank'])
        ->execute();
    $all_featureprop_records = $query->fetchAll();
    $this->assertCount(3, $all_featureprop_records,
      "There were more records then we were expecting in the featureprop table: " . print_r($all_featureprop_records, TRUE));

    // Check that the featureprop records were created in the database as expected.
    // We use the unique key to select this particular value in order to
    // ensure it is here and there is one one.
    foreach ($update_values['testpropertyfieldA'] as $delta => $expected) {
      $query = $this->chado_connection->select('1:featureprop', 'prop')
        ->fields('prop', ['featureprop_id', 'feature_id', 'type_id', 'value', 'rank'])
        ->condition('feature_id', $feature_id, '=')
        ->condition('type_id', $expected['A_type_id'])
        ->condition('rank', $expected['A_rank'])
        ->execute();
      $records = $query->fetchAll();
      $this->assertCount(1, $records, "We expected to get exactly one record for:" . print_r($expected, TRUE));
      $this->assertEquals($expected['A_value'], $records[0]->value, "We did not get the value we expected using the unique key." . print_r($expected, TRUE));
    }

    // Test Case: Delete values in Chado using ChadoStorage.
    // ---------------------------------------------------------

    // NOT YET IMPLEMENTED IN CHADOSTORAGE.

  }
}
