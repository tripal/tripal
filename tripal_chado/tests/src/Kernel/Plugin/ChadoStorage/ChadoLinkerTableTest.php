<?php

namespace Drupal\Tests\tripal_chado\Kernel\Plugin\ChadoStorage;

use Drupal\Tests\tripal_chado\Kernel\ChadoTestKernelBase;
use Drupal\Tests\tripal_chado\Traits\ChadoStorageTestTrait;

use Drupal\tripal\TripalStorage\StoragePropertyValue;
use Drupal\tripal\TripalStorage\StoragePropertyTypeBase;

use Drupal\Tests\tripal_chado\Functional\MockClass\FieldConfigMock;

/**
 * Tests that ChadoStorage can handle fields for linker tables as we expect.
 * The array of fields/properties used for these tests are designed
 * to match those for a variety of linker tables as examples since the fields
 * are note yet developed. These are the specific cases tested:
 *   - synonymfield: feature > feature_synonym
 *   - analysisfield: feature > analysisfeature
 *   - contactfield: feature > feature_contact
 *   - relationshipfield: feature > feature_relationship
 *
 * Note: testotherfeaturefield is added to ensure we meet the unique constraint
 * on the base table and also to ensure we are testing multi-field functionality.
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
 *
 * @group Tripal
 * @group Tripal Chado
 * @group ChadoStorage
 */
class ChadoLinkerTableTest extends ChadoTestKernelBase {

  use ChadoStorageTestTrait;

  protected $fields = [
    'synonymfield' => [
      'field_name' => 'synonymfield',
      'base_table' => 'feature',
      'properties' => [
        'record_id' => [
          'propertyType class' => 'Drupal\tripal_chado\TripalStorage\ChadoIntStoragePropertyType',
          'action' => 'store_id',
          'drupal_store' => TRUE,
          'chado_table' => 'feature',
          'chado_column' => 'feature_id'
        ],
        'linker_id' => [
          'propertyType class' => 'Drupal\tripal_chado\TripalStorage\ChadoIntStoragePropertyType',
          'action' => 'store_pkey',
          'chado_table' => 'feature_synonym',
          'chado_column' => 'feature_synonym_id',
        ],
        'link' => [
          'propertyType class' => 'Drupal\tripal_chado\TripalStorage\ChadoIntStoragePropertyType',
          'action' => 'store_link',
          'left_table' => 'feature',
          'left_table_id' => 'feature_id',
          'right_table' => 'feature_synonym',
          'right_table_id' => 'feature_id'
        ],
        'right_id' => [
          'propertyType class' => 'Drupal\tripal_chado\TripalStorage\ChadoIntStoragePropertyType',
          'action' => 'store',
          'chado_table' => 'feature_synonym',
          'chado_column' => 'synonym_id'
        ],
        // Other columns in feature_synonym. These are set by the widget.
        'pub_id' => [
          'propertyType class' => 'Drupal\tripal_chado\TripalStorage\ChadoIntStoragePropertyType',
          'action' => 'store',
          'chado_table' => 'feature_synonym',
          'chado_column' => 'pub_id'
        ],
        'is_current' => [
          'propertyType class' => 'Drupal\tripal_chado\TripalStorage\ChadoBoolStoragePropertyType',
          'action' => 'store',
          'chado_table' => 'feature_synonym',
          'chado_column' => 'is_current'
        ],
        'is_internal' => [
          'propertyType class' => 'Drupal\tripal_chado\TripalStorage\ChadoBoolStoragePropertyType',
          'action' => 'store',
          'chado_table' => 'feature_synonym',
          'chado_column' => 'is_internal'
        ],
      ],
    ],
    'analysisfield' => [
      'field_name' => 'analysisfield',
      'base_table' => 'feature',
      'properties' => [
        'record_id' => [
          'propertyType class' => 'Drupal\tripal_chado\TripalStorage\ChadoIntStoragePropertyType',
          'action' => 'store_id',
          'drupal_store' => TRUE,
          'chado_table' => 'feature',
          'chado_column' => 'feature_id'
        ],
        'linker_id' => [
          'propertyType class' => 'Drupal\tripal_chado\TripalStorage\ChadoIntStoragePropertyType',
          'action' => 'store_pkey',
          'chado_table' => 'analysisfeature',
          'chado_column' => 'analysisfeature_id',
        ],
        'link' => [
          'propertyType class' => 'Drupal\tripal_chado\TripalStorage\ChadoIntStoragePropertyType',
          'action' => 'store_link',
          'left_table' => 'feature',
          'left_table_id' => 'feature_id',
          'right_table' => 'analysisfeature',
          'right_table_id' => 'feature_id'
        ],
        'right_id' => [
          'propertyType class' => 'Drupal\tripal_chado\TripalStorage\ChadoIntStoragePropertyType',
          'action' => 'store',
          'chado_table' => 'analysisfeature',
          'chado_column' => 'analysis_id'
        ],
      ],
    ],
    'contactfield' => [
      'field_name' => 'contactfield',
      'base_table' => 'feature',
      'properties' => [
        'record_id' => [
          'propertyType class' => 'Drupal\tripal_chado\TripalStorage\ChadoIntStoragePropertyType',
          'action' => 'store_id',
          'drupal_store' => TRUE,
          'chado_table' => 'feature',
          'chado_column' => 'feature_id'
        ],
        'linker_id' => [
          'propertyType class' => 'Drupal\tripal_chado\TripalStorage\ChadoIntStoragePropertyType',
          'action' => 'store_pkey',
          'chado_table' => 'feature_contact',
          'chado_column' => 'feature_contact_id',
        ],
        'link' => [
          'propertyType class' => 'Drupal\tripal_chado\TripalStorage\ChadoIntStoragePropertyType',
          'action' => 'store_link',
          'left_table' => 'feature',
          'left_table_id' => 'feature_id',
          'right_table' => 'feature_contact',
          'right_table_id' => 'feature_id'
        ],
        'right_id' => [
          'propertyType class' => 'Drupal\tripal_chado\TripalStorage\ChadoIntStoragePropertyType',
          'action' => 'store',
          'chado_table' => 'feature_contact',
          'chado_column' => 'contact_id'
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
  protected int $cvterm_id;
  protected array $right_id;

  /**
   * {@inheritdoc}
   */
  protected function setUp() :void {
    parent::setUp();

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

    $this->cvterm_id = $this->getCvtermID('rdfs', 'type');

    // Pub
    $query = $this->chado_connection->insert('1:pub');
    $query->fields([
      'uniquename' => 'test' . uniqid() . 'PUB',
      'type_id' => $this->cvterm_id,
    ]);
    $query->execute();
    // Don't need to save as it will be 1 since the table is empty.
    // We just need it to exist.

    // Synonym.
    $this->right_id['synonymfield'] = [];
    $query = $this->chado_connection->insert('1:synonym');
    $query->fields([
      'name' => 'test' . uniqid() . '-1',
      'synonym_sgml' => 'test-1',
      'type_id' => $this->cvterm_id,
    ]);
    $this->right_id['synonymfield'][0] = $query->execute();
    $query = $this->chado_connection->insert('1:synonym');
    $query->fields([
      'name' => 'test' . uniqid() . '-2',
      'synonym_sgml' => 'test-2',
      'type_id' => $this->cvterm_id,
    ]);
    $this->right_id['synonymfield'][1] = $query->execute();
    $query = $this->chado_connection->insert('1:synonym');
    $query->fields([
      'name' => 'test' . uniqid() . '-3',
      'synonym_sgml' => 'test-3',
      'type_id' => $this->cvterm_id,
    ]);
    $this->right_id['synonymfield'][2] = $query->execute();

    // Analysis.
    $this->right_id['analysisfield'] = [];
    $query = $this->chado_connection->insert('1:analysis');
    $query->fields([
      'program' => 'test' . uniqid() . '-1',
      'programversion' => 'test-1',
    ]);
    $this->right_id['analysisfield'][0] = $query->execute();
    $query = $this->chado_connection->insert('1:analysis');
    $query->fields([
      'program' => 'test' . uniqid() . '-2',
      'programversion' => 'test-2',
    ]);
    $this->right_id['analysisfield'][1] = $query->execute();
    $query = $this->chado_connection->insert('1:analysis');
    $query->fields([
      'program' => 'test' . uniqid() . '-3',
      'programversion' => 'test-3',
    ]);
    $this->right_id['analysisfield'][2] = $query->execute();

    // Contact.
    $this->right_id['contactfield'] = [];
    $query = $this->chado_connection->insert('1:contact');
    $query->fields([
      'name' => 'test' . uniqid() . '-1',
      'type_id' => $this->cvterm_id,
    ]);
    $this->right_id['contactfield'][0] = $query->execute();
    $query = $this->chado_connection->insert('1:contact');
    $query->fields([
      'name' => 'test' . uniqid() . '-2',
      'type_id' => $this->cvterm_id,
    ]);
    $this->right_id['contactfield'][1] = $query->execute();
    $query = $this->chado_connection->insert('1:contact');
    $query->fields([
      'name' => 'test' . uniqid() . '-3',
      'type_id' => $this->cvterm_id,
    ]);
    $this->right_id['contactfield'][2] = $query->execute();
  }

  /**
   * Data Provider: define each test case.
   */
  public function provideTestCases() {
    return [
      // synonymfield: feature > feature_synonym
      [
        'synonymfield',
        'feature_synonym',
        'synonym_id',
        [
          'pub_id' => 1,
          'is_current' => TRUE,
          'is_internal' => TRUE,
        ]
      ],
      // analysisfield: feature > analysisfeature
      [
        'analysisfield',
        'analysisfeature',
        'analysis_id',
        []
      ],
      // contactfield: feature > feature_contact
      [
        'contactfield',
        'feature_contact',
        'contact_id',
        []
      ],
      // relationshipfield: feature > feature_relationship
      /*
      [
        'relationshipfield',
        'feature_relationship',
        'synonym_id',
        [
          'pub_id' => 1,
          'is_current' => TRUE,
          'is_internal' => TRUE,
        ]
      ],
      */
    ];
  }

  /**
   * Testing ChadoStorage on linker fields.
   *
   * @dataProvider provideTestCases
   *
   * Test Cases:
   *   - Create Values in Chado using ChadoStorage when they don't yet exist.
   *   - Load values in Chado using ChadoStorage after we just inserted them.
   *   - Update values in Chado using ChadoStorage after we just inserted them.
   *   - [NOT IMPLEMENTED] Delete values in Chado using ChadoStorage.
   *   - [NOT IMPLEMENTED] Ensure property field picks up records in Chado not added through field.
   *
   * Parameters provided by provideTestCases().
   * @params $linker_field_name
   *   The specific field in $fields to be used for the current test case.
   * @param $linker_table_name
   *   The name of the chado linker table we are testing in the field specified by $linker_field_name
   * @param $right_table_id
   *   The primary key for the right table in the link.
   * @param $extra_values
   *   The values for the extra fields specific to each linker table
   *   where the key is the property key and the value is the value we should set it to.
   */
  public function testLinkerTableField($linker_field_name, $linker_table_name, $right_table_id, $extra_values) {

    // Test Case: Insert valid values when they do not yet exist in Chado.
    // ---------------------------------------------------------
    $insert_values = [
      $linker_field_name => [
        [
          'record_id' => NULL,
          'linker_id' => NULL,
          'link' => NULL,
          'right_id' => $this->right_id[$linker_field_name][0],
        ] + $extra_values,
        [
          'record_id' => NULL,
          'linker_id' => NULL,
          'link' => NULL,
          'right_id' => $this->right_id[$linker_field_name][1],
        ] + $extra_values,
      ],
      'testotherfeaturefield' => [
        [
          'feature_type' => $this->cvterm_id,
          'feature_organism' => $this->organism_id,
          'feature_uname' => 'testGene4' . $linker_field_name . 'Test',
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
    // in the linker table.
    $query = $this->chado_connection->select('1:' . $linker_table_name, 'linker')
        ->fields('linker')
        ->execute();
    $all_linker_records = $query->fetchAll();
    $this->assertCount(2, $all_linker_records,
      "There were more records then we were expecting in the $linker_table_name table: " . print_r($all_linker_records, TRUE));

    // Check that the linker table records were created in the database as expected.
    // We use the unique key to select this particular value in order to
    // ensure it is here and there is one one.
    foreach ($insert_values[$linker_field_name] as $delta => $expected) {
      $query = $this->chado_connection->select('1:' . $linker_table_name, 'linker')
        ->fields('linker')
        ->condition('feature_id', $feature_id, '=')
        ->condition($right_table_id, $expected['right_id'])
        ->execute();
      $records = $query->fetchAll();
      $this->assertCount(1, $records, "We expected to get exactly one record for:" . print_r($expected, TRUE));

      $varname = 'link' . $delta;
      $$varname = $records[0];
    }

    // Test Case: Load values existing in Chado.
    // ---------------------------------------------------------
    // First we want to reset all the chado storage arrays to ensure we are
    // doing a clean test. The values will purposefully remain in Chado but the
    // Property Types, Property Values and Data Values will be built from scratch.


    // Test Case: Update values in Chado using ChadoStorage.
    // ---------------------------------------------------------
    // When updating we need all the store id/pkey/link records
    // and all values of the other properties.

    // Test Case: Delete values in Chado using ChadoStorage.
    // ---------------------------------------------------------

    // NOT YET IMPLEMENTED IN CHADOSTORAGE.
  }
}
