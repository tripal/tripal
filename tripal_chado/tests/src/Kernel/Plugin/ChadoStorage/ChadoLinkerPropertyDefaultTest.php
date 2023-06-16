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
 * Specific test cases to cover:
 *  - CRUD of chado records associated with a single property field.
 *  - CRUD of chado records associated with multiple property
 *    fields with different types on the same content type.
 *  - Ensure property field picks up records in Chado not added through field.
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
  }

  /**
   * CRUD of chado records associated with a single property field.
   *
   * Specifically,
   */
  public function testSingleProperty() {

    // Setup Specific to this particular Test
    // ------------------------------------------
    // 1. Insert/Select Dependant Chado Records.
    // (these should not be generated by my field).
    // -- Organism.
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
    $gene_organism_id = $query->execute();
    // -- CVterms.
    $gene_type_id = $this->getCvtermID('SO', '0000704');
    $propA_type_id = $this->getCvtermID('rdfs', 'comment');
    // $propB_type_id = $this->getCvtermID('TAXRANK', '0000010');

    // 2. Define the Values of Fields to use in testing.
    // (these should not be in the database yet).
    // -- testotherfeaturefield:feature_uname (base gene record).
    $gene_uname = 'testGene4PropTableTest';
    // -- testpropertyfieldA:A_value (Comment property).
    //    tests multiple properties of the same type.
    $propA_values = ['Note 1', 'Note 2', 'Note 3'];
    // -- testpropertyfieldB:B_value (Species Group property).
    //    tests a second property type + tests a single value.
    // $propB_values[] = ['postgresquelus'];

    // 3. Create the property types based on our fields array.
    $this->createPropertyTypes('testpropertyfieldA');
    $this->createPropertyTypes('testotherfeaturefield');
    $this->assertCount(9, $this->propertyTypes,
      "We did not have the expected number of property types created on our behalf.");

    // 4. Add the types to chado storage.
    $this->chadoStorage->addTypes($this->propertyTypes);
    $retrieved_types = $this->chadoStorage->getTypes();
    $this->assertIsArray($retrieved_types,
      "Unable to retrieve the PropertyTypes after adding testpropertyfieldA + testotherfeaturefield.");
    $this->assertCount(9, $retrieved_types,
      "Did not revieve the expected number of PropertyTypes after adding testpropertyfieldA + testotherfeaturefield.");

    // 5. Create the property values + format them for testing with *Values methods.
    $this->createDataStoreValues('testpropertyfieldA', 3);
    $this->createDataStoreValues('testotherfeaturefield');
    $this->assertCount(2, $this->dataStoreValues,
      "There was a different number of fields in our dataStoreValues then we expected.");

    // Test Case: Create Values in Chado using ChadoStorage
    // when they didn't already exist.
    // ------------------------------------------
    // 1. Set the values in the propertyValue objects.
    $this->dataStoreValues['testotherfeaturefield'][0]['feature_type']['value']->setValue($gene_type_id);
    $this->dataStoreValues['testotherfeaturefield'][0]['feature_organism']['value']->setValue($gene_organism_id);
    $this->dataStoreValues['testotherfeaturefield'][0]['feature_uname']['value']->setValue($gene_uname);
    foreach ($propA_values as $delta => $value) {
      $this->dataStoreValues['testpropertyfieldA'][$delta]['A_type_id']['value']->setValue($propA_type_id);
      $this->dataStoreValues['testpropertyfieldA'][$delta]['A_value']['value']->setValue($value);
      $this->dataStoreValues['testpropertyfieldA'][$delta]['A_rank']['value']->setValue($delta);
    }
    // 2. Use ChadoStorage insertValues to create the records.
    $success = $this->chadoStorage->insertValues($this->dataStoreValues);
    $this->assertTrue($success, 'We were not able to insert the data.');
  }
}
