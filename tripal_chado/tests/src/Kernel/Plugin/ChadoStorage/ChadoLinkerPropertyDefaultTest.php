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
 * Specific test cases
 *  Test the following for both single and multiple property fields:
 *   - Create Values in Chado using ChadoStorage when they don't yet exist.
 *   - [NOT IMPLEMENTED] Create Values in Chado using ChadoStorage when they violate unique constraint.
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
   * Data Provider:
   * 1) Single Property Field + Feature Base
   * 2) Two Property Fields + Feature Base
   */
  public function provideFieldExpectations() {
    $data = [];

    $data[] = [
      'field_names' => [ 'testpropertyfieldA', 'testotherfeaturefield' ],
      'expections' => [
        'total number of properties' => 9,
        'number of fields' => 2,
        'testpropertyfieldA' => [
          'number of properties' => 6,
          'number of values' => 3,
          'static values' => [
            [
              'type' => 'cvterm lookup',
              'idspace' => 'rdfs',
              'accession' => 'comment',
              'property_key' => 'A_type_id',
            ],
          ],
          'values' => [
            [
              'A_value' => 'Note 1',
              'A_rank' => 0,
            ],
            [
              'A_value' => 'Note 2',
              'A_rank' => 1,
            ],
            [
              'A_value' => 'Note 3',
              'A_rank' => 2,
            ]
          ]
        ],
        'testotherfeaturefield' => [
          'number of properties' => 3,
          'number of values' => 1,
          'static values' => [
            [
              'type' => 'cvterm lookup',
              'idspace' => 'SO',
              'accession' => '0000704',
              'property_key' => 'feature_type',
            ],
            [
              'type' => 'organism',
              'property_key' => 'feature_organism',
            ]
          ],
          'values' => [
            [
              'feature_uname' => 'testGene4PropTableTest',
            ]
          ],
        ],
      ],
    ];

    return $data;
  }

  /**
   * TEST CASE: Create Values in Chado using ChadoStorage when they don't yet exist.
   *
   * @dataProvider provideFieldExpectations
   */
  public function testInsertValues($field_names, $expectations) {

    // Prep: Lookup static values in expectations.
    // This couldn't be done in the data provider as there was no database yet.
    foreach ($field_names as $field_name) {
      foreach($expectations[$field_name]['static values'] as $args) {
        switch ($args['type']) {
          case 'organism':
            foreach(array_keys($expectations[$field_name]['values']) as $delta) {
              $expectations[$field_name]['values'][$delta][ $args['property_key'] ] = $this->organism_id;
            }
            break;
          case 'cvterm lookup':
            $cvterm_id = $this->getCvtermID($args['idspace'], $args['accession']);
            foreach(array_keys($expectations[$field_name]['values']) as $delta) {
              $expectations[$field_name]['values'][$delta][ $args['property_key'] ] = $cvterm_id;
            }
            break;
        }
      }
    }

    // 1. Create the property types based on our fields array.
    foreach ($field_names as $field_name) {
      $this->createPropertyTypes($field_name);
    }
    $this->assertCount($expectations['total number of properties'], $this->propertyTypes,
      "We did not have the expected number of property types created on our behalf.");

    // 2. Add the types to chado storage.
    $this->chadoStorage->addTypes($this->propertyTypes);
    $retrieved_types = $this->chadoStorage->getTypes();
    $this->assertIsArray($retrieved_types,
      "Unable to retrieve the PropertyTypes after adding " . implode(' + ', $field_names) . ".");
    $this->assertCount($expectations['total number of properties'], $retrieved_types,
      "Did not revieve the expected number of PropertyTypes after adding testpropertyfieldA + testotherfeaturefield.");

    // 3. Create the property values + format them for testing with *Values methods.
    foreach ($field_names as $field_name) {
      $this->createDataStoreValues($field_name, $expectations[$field_name]['number of values']);
    }
    $this->assertCount($expectations['number of fields'], $this->dataStoreValues,
      "There was a different number of fields in our dataStoreValues then we expected.");

    // 4. Set the values in the propertyValue objects.
    foreach ($field_names as $field_name) {
      foreach($expectations[$field_name]['values'] as $delta => $values) {
        foreach($values as $property_key => $val) {

          $this->dataStoreValues[$field_name][$delta][$property_key]['value']->setValue($val);

          $retrieved_val = $this->dataStoreValues[$field_name][$delta][$property_key]['value']->getValue();
          $this->assertEquals($val, $retrieved_val,
            "We were unable to retrieve the value for $property_key right after we set it.");
        }
      }
    }

    // 5. Use ChadoStorage insertValues to create the records.
    $success = $this->chadoStorage->insertValues($this->dataStoreValues);
    $this->assertTrue($success, 'We were not able to insert the data.');

    // 6. Check that the base feature record was created in the database as expected.
    $field_name = 'testotherfeaturefield';
    $query = $this->chado_connection->select('1:feature', 'f')
      ->fields('f', ['feature_id', 'type_id', 'organism_id', 'uniquename'])
      ->execute();
    $records = $query->fetchAll();
    $this->assertCount($expectations[$field_name]['number of values'], $records,
      "There should only be a single feature record created by our storage properties.");
    foreach ($records as $record) {
      $record_expect = $expectations[$field_name]['values'][$delta];
      $this->assertIsObject($record,
        "The returned feature record should be an object.");
      $this->assertEquals($record_expect['feature_type'], $record->type_id,
        "The feature record should have the type we set in our storage properties.");
      $this->assertEquals($record_expect['feature_organism'], $record->organism_id,
        "The feature record should have the organism we set in our storage properties.");
      $this->assertEquals($record_expect['feature_uname'], $record->uniquename,
          "The feature record should have the unique name we set in our storage properties.");
      $feature_id = $record->feature_id;
    }

    // 7. Check that the featureprop records were created in the database as expected.
    $field_name = 'testpropertyfieldA';
    $query = $this->chado_connection->select('1:featureprop', 'prop')
      ->fields('prop', ['feature_id', 'type_id', 'value', 'rank'])
      ->execute();
    $records = $query->fetchAll();
    $this->assertCount($expectations[$field_name]['number of values'], $records,
      "We did not get the expected number of featureprop record created by our storage properties for $field_name.");
    foreach ($records as $record) {

      $this->assertIsObject($record,
        "The returned featureprop record should be an object.");

      $delta = $record->rank;
      $this->assertIsNumeric($record->rank, "The rank should be numeric");

      $this->assertArrayHasKey($delta,  $expectations[$field_name]['values'],
        "There was not a matching expected values array for this delta.");
      $record_expect = $expectations[$field_name]['values'][$delta];

      $this->assertEquals($record_expect['A_type_id'], $record->type_id,
        "The featureprop record should have the type we set in our storage properties.");
      $this->assertEquals($feature_id, $record->feature_id,
        "The featureprop record should have same feature_id as the one we created.");
      $this->assertEquals($record_expect['A_value'], $record->value,
        "The value should match the one we set for that rank ($delta)");
    }

  }
}
