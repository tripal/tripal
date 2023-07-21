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
 * to match those in the ChadoLinkerPropertyDefault field with values filled
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
 *   - Create Values in Chado using ChadoStorage when they don't yet exist.
 *   - [NOT IMPLEMENTED] Load values in Chado using ChadoStorage when they don't yet exist.
 *   - [NOT IMPLEMENTED] Load values in Chado using ChadoStorage after we just inserted them.
 *   - [NOT IMPLEMENTED] Update values in Chado using ChadoStorage after we just inserted them.
 *   - [NOT IMPLEMENTED] Update values in Chado using ChadoStorage when they don't actually exist.
 *   - [NOT IMPLEMENTED] Delete values in Chado using ChadoStorage.
 *   - [NOT IMPLEMENTED] Ensure property field picks up records in Chado not added through field.
 *
 * @group Tripal
 * @group Tripal Chado
 * @group ChadoStorage
 */
class ChadoLinkerPropertyDefaultTest extends ChadoTestKernelBase {

  use ChadoStorageTestTrait;

  protected $fields = [
    'testpropertyfieldA' => [
      'field_name' => 'testpropertyfieldA',
      'base_table' => 'feature',
      'properties' => [
        // Keeps track of the feature record our hypothetical field cares about.
        'A_record_id' => [
          'propertyType class' => 'Drupal\tripal_chado\TripalStorage\ChadoIntStoragePropertyType',
          'action' => 'store_id',
          'drupal_store' => TRUE,
          'chado_table' => 'feature',
          'chado_column' => 'feature_id'
        ],
        // Store the primary key for the prop table.
        'A_prop_id' => [
          'propertyType class' => 'Drupal\tripal_chado\TripalStorage\ChadoIntStoragePropertyType',
          'action' => 'store_pkey',
          'chado_table' => 'featureprop',
          'chado_column' => 'featureprop_id',
        ],
        // Generate `JOIN {featureprop} ON feature.feature_id = featureprop.feature_id`
        // Will also store the feature.feature_id so no need for drupal_store => TRUE.
        'A_linker_id' => [
          'propertyType class' => 'Drupal\tripal_chado\TripalStorage\ChadoIntStoragePropertyType',
          'action' => 'store_link',
          'chado_table' => 'featureprop',
          'chado_column' => 'feature_id'
        ],
        // Now we are going to store all the core columns of the featureprop table to
        // ensure we can meet the unique and not null requirements of the table.
        'A_type_id' => [
          'propertyType class' => 'Drupal\tripal_chado\TripalStorage\ChadoIntStoragePropertyType',
          'action' => 'store',
          'chado_table' => 'featureprop',
          'chado_column' => 'type_id'
        ],
        'A_value' => [
          'propertyType class' => 'Drupal\tripal_chado\TripalStorage\ChadoTextStoragePropertyType',
          'action' => 'store',
          'chado_table' => 'featureprop',
          'chado_column' => 'value',
          'delete_if_empty' => TRUE,
          'empty_value' => ''
        ],
        'A_rank' => [
          'propertyType class' => 'Drupal\tripal_chado\TripalStorage\ChadoIntStoragePropertyType',
          'action' => 'store',
          'chado_table' => 'featureprop',
          'chado_column' => 'rank'
        ],
      ],
    ],
    'testpropertyfieldB' => [
      'field_name' => 'testpropertyfieldB',
      'base_table' => 'feature',
      'properties' => [
        'B_record_id' => [
          'propertyType class' => 'Drupal\tripal_chado\TripalStorage\ChadoIntStoragePropertyType',
          'action' => 'store_id',
          'drupal_store' => TRUE,
          'chado_table' => 'feature',
          'chado_column' => 'feature_id'
        ],
        'B_prop_id' => [
          'propertyType class' => 'Drupal\tripal_chado\TripalStorage\ChadoIntStoragePropertyType',
          'action' => 'store_pkey',
          'chado_table' => 'featureprop',
          'chado_column' => 'featureprop_id',
        ],
        'B_linker_id' => [
          'propertyType class' => 'Drupal\tripal_chado\TripalStorage\ChadoIntStoragePropertyType',
          'action' => 'store_link',
          'chado_table' => 'featureprop',
          'chado_column' => 'feature_id'
        ],
        'B_type_id' => [
          'propertyType class' => 'Drupal\tripal_chado\TripalStorage\ChadoIntStoragePropertyType',
          'action' => 'store',
          'chado_table' => 'featureprop',
          'chado_column' => 'type_id'
        ],
        'B_value' => [
          'propertyType class' => 'Drupal\tripal_chado\TripalStorage\ChadoTextStoragePropertyType',
          'action' => 'store',
          'chado_table' => 'featureprop',
          'chado_column' => 'value',
          'delete_if_empty' => TRUE,
          'empty_value' => ''
        ],
        'B_rank' => [
          'propertyType class' => 'Drupal\tripal_chado\TripalStorage\ChadoIntStoragePropertyType',
          'action' => 'store',
          'chado_table' => 'featureprop',
          'chado_column' => 'rank'
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

  protected int $organism_id;

  /**
   * {@inheritdoc}
   */
  protected function setUp() :void {
    parent::setUp();
    $this->setUpChadoStorageTestEnviro();

    // Create the organism record for use with the feature table.
    $infra_type_id = $this->getCvtermID('TAXRANK', '0000010');
    $query = $this->chado_connection->insert('1:organism');
    $query->fields([
      'genus' => 'Tripalus',
      'species' => 'databasica',
      'common_name' => 'Tripal',
      'abbreviation' => 'T. databasica',
      'infraspecific_name' => 'postgresql',
      'type_id' => $infra_type_id,
      'comment' => 'This is fake organism specifically for testing purposes.'
    ]);
    $this->organism_id = $query->execute();
  }

  /**
   * TEST CASE: Create Values in Chado using ChadoStorage when
   * the values don't yet exist and no unique constraint is violated.
   */
  public function testInsertValuesWhenTheyAreValid() {

    $rdfs_comment_cvtermID = $this->getCvtermID('rdfs', 'comment');
    $gene_cvtermID = $this->getCvtermID('SO', '0000704');
    $subspecies_cvtermID = $this->getCvtermID('SO', '0000704');

    // Test Case: We can use ChadoStorage to insert values where
    //   we have a single featureprop-based field with 3 values.
    // ---------------------------------------------------------
    $values = [
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
    $this->chadoStorageTestInsertValues($values);

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
    $record_expect = $values[$field_name][0];
    $this->assertIsObject($record,
      "The returned feature record should be an object.");
    $this->assertEquals($record_expect['feature_type'], $record->type_id,
      "The feature record should have the type we set in our storage properties.");
    $this->assertEquals($record_expect['feature_organism'], $record->organism_id,
      "The feature record should have the organism we set in our storage properties.");
    $this->assertEquals($record_expect['feature_uname'], $record->uniquename,
        "The feature record should have the unique name we set in our storage properties.");
    $feature_id = $record->feature_id;

    // Check that the featureprop records were created in the database as expected.
    // We use the unique key to select this particular value in order to
    // ensure it is here and there is one one.
    foreach ($values['testpropertyfieldA'] as $delta => $expected) {
      $query = $this->chado_connection->select('1:featureprop', 'prop')
        ->fields('prop', ['feature_id', 'type_id', 'value', 'rank'])
        ->condition('feature_id', $feature_id, '=')
        ->condition('type_id', $expected['A_type_id'])
        ->condition('rank', $expected['A_rank'])
        ->execute();
      $records = $query->fetchAll();
      $this->assertCount(1, $records, "We expected to get exactly one record for:" . print_r($expected, TRUE));
      $this->assertEquals($expected['A_value'], $records[0]->value, "We did not get the value we expected using the unique key." . print_r($expected, TRUE));
    }

    // Also check that there are only the expected number of records
    // in the featureprop table.
    $query = $this->chado_connection->select('1:featureprop', 'prop')
        ->fields('prop', ['feature_id', 'type_id', 'value', 'rank'])
        ->execute();
    $all_featureprop_records = $query->fetchAll();
    $this->assertCount(3, $all_featureprop_records,
      "There were more records then we were expecting in the featureprop table: " . print_r($all_featureprop_records, TRUE));

    // Test Case: We can use ChadoStorage to insert values where
    //   we have a two featureprop-based fields:
    //   where testpropertyfieldA has 3 values
    //   and testpropertyfieldB has 1 value.
    // ---------------------------------------------------------
    // First clean up after the last test.
    $this->cleanChadoStorageValues();

    // Now add our second featureprop field to the values array.
    /**
     * This fails as expected based on Issue #1398.
    $values['testpropertyfieldB'][0] = [
      'B_type_id' => $subspecies_cvtermID,
      'B_value' => 'postgresquelus',
      'B_rank' => 0,
    ];
    // We also add the feature_id for the gene to tell
    // chadostorage it's already been inserted.
    $values['testotherfeaturefield'][0]['record_id'] = $feature_id;
    $this->chadoStorageTestInsertValues($values);
    */
  }
}
