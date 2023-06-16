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
        'A_base_id' => [
          'propertyType class' => 'Drupal\tripal_chado\TripalStorage\ChadoIntStoragePropertyType',
          'action' => 'store_id',
          'drupal_store' => TRUE,
          'chado_table' => 'feature',
          'chado_column' => 'feature_id'
        ],
        // Generate `JOIN {featureprop} ON feature.feature_id = featureprop.feature_id`
        // Will also store the feature.feature_id so no need for drupal_store => TRUE.
        'A_first_hop' => [
          'propertyType class' => 'Drupal\tripal_chado\TripalStorage\ChadoIntStoragePropertyType',
          'action' => 'store_link',
          'left_table' => 'feature',
          'left_table_id' => 'feature_id',
          'right_table' => 'featureprop',
          'right_table_id' => 'feature_id'
        ],
        // Store the primary key for the prop table.
        'A_prop_id' => [
          'propertyType class' => 'Drupal\tripal_chado\TripalStorage\ChadoIntStoragePropertyType',
          'action' => 'store_pkey',
          'chado_table' => 'featureprop',
          'chado_column' => 'featureprop_id',
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
          'chado_column' => 'value'
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
        // Keeps track of the feature record our hypothetical field cares about.
        'B_base_id' => [
          'propertyType class' => 'Drupal\tripal_chado\TripalStorage\ChadoIntStoragePropertyType',
          'action' => 'store_id',
          'drupal_store' => TRUE,
          'chado_table' => 'feature',
          'chado_column' => 'feature_id'
        ],
        // Generate `JOIN {featureprop} ON feature.feature_id = featureprop.feature_id`
        // Will also store the feature.feature_id so no need for drupal_store => TRUE.
        'B_first_hop' => [
          'propertyType class' => 'Drupal\tripal_chado\TripalStorage\ChadoIntStoragePropertyType',
          'action' => 'store_link',
          'left_table' => 'feature',
          'left_table_id' => 'feature_id',
          'right_table' => 'featureprop',
          'right_table_id' => 'feature_id'
        ],
        // Store the primary key for the prop table.
        'B_prop_id' => [
          'propertyType class' => 'Drupal\tripal_chado\TripalStorage\ChadoIntStoragePropertyType',
          'action' => 'store_pkey',
          'chado_table' => 'featureprop',
          'chado_column' => 'featureprop_id',
        ],
        // Now we are going to store all the core columns of the featureprop table to
        // ensure we can meet the unique and not null requirements of the table.
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
          'chado_column' => 'value'
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
    $propB_type_id = $this->getCvtermID('TAXRANK', '0000010');

    // 2. Define the Values of Fields to use in testing.
    // (these should not be in the database yet).
    // -- testotherfeaturefield:feature_uname (base gene record).
    $gene_uname = 'testGene4PropTableTest';
    // -- testpropertyfieldA:A_value (Comment property).
    //    tests multiple properties of the same type.
    $propA_values[] = ['Note 1', 'Note 2', 'Note 3'];
    // -- testpropertyfieldB:B_value (Species Group property).
    //    tests a second property type + tests a single value.
    $propB_values[] = ['postgresquelus'];

    // 3. Create the property types based on our fields array.
    $this->createPropertyTypes('testpropertyfieldA');
    $this->createPropertyTypes('testotherfeaturefield');
    $this->assertCount(3, $this->fieldConfig_mock,
      "We did not have the expected number of fieldConfig mock objects based on our fields array.");
    $this->assertCount(9, $this->propertyTypes,
      "We did not have the expected number of property types created on our behalf.");
  }
}
