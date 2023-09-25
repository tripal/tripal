<?php

namespace Drupal\Tests\tripal_chado\Kernel\Plugin\ChadoStorage;

use Drupal\Tests\tripal_chado\Kernel\ChadoTestKernelBase;
use Drupal\Tests\tripal_chado\Traits\ChadoStorageTestTrait;

use Drupal\tripal\TripalStorage\StoragePropertyValue;
use Drupal\tripal\TripalStorage\StoragePropertyTypeBase;

use Drupal\Tests\tripal_chado\Functional\MockClass\FieldConfigMock;

/**
 * Tests that ChadoStorage can handle linker fields as we expect.
 * The array of fields/properties used for these tests are designed
 * to match those in the ChadoLinkerContactDefault field with values filled
 * based on the base table project which uses the linker table project_contact.
 *
 *@@@ Note: testotherphylotreefield and testotherquantificationfield are added
 * to ensure we meet the unique constraints on the phylotree and quantification
 * tables respectively.
 *
 *  Specific test cases:
 *   - [PROJECT] Create Values in Chado using ChadoStorage when they don't yet exist.
 *   - [PROJECT] Load values in Chado using ChadoStorage after we just inserted them.
 *   - [PROJECT] Update values in Chado using ChadoStorage after we just inserted them.
 *
 * @group Tripal
 * @group Tripal Chado
 * @group Tripal Chado Fields
 * @group Tripal Chado Fields Contact
 * @group Splunge
 * @group ChadoStorage
 */
class ChadoLinkerContactDefaultTest extends ChadoTestKernelBase {

  use ChadoStorageTestTrait;

  /**
   * Properties directly from the ChadoLinkerContactDefault field type:
   * @code
    $properties[] = new ChadoIntStoragePropertyType($entity_type_id, self::$id, 'record_pkey_id', $record_id_term, [
      'action' => 'store_id',
      'drupal_store' => TRUE,
      'chado_table' => $base_table,
      'chado_column' => $base_pkey_col,
    ]);

    // Define the linker table that links the base table to the object table.
    // Note that type_id and rank are not in all linker tables.
    $properties[] = new ChadoIntStoragePropertyType($entity_type_id, self::$id, 'linker_pkey_id', $record_id_term, [
      'action' => 'store_pkey',
      'drupal_store' => TRUE,
      'chado_table' => $linker_table,
      'chado_column' => $linker_pkey_col,
    ]);
    // Define the link between the base table and the linker table.
    $properties[] = new ChadoIntStoragePropertyType($entity_type_id, self::$id, 'subject_id', $linker_base_term, [
      'action' => 'store_link',
      'drupal_store' => TRUE,
      'chado_table' => $linker_table,
      'chado_column' => $linker_fk_col,
    ]);
    // Define the link between the linker table and the object table.
    $properties[] = new ChadoIntStoragePropertyType($entity_type_id, self::$id, 'object_id', $linker_object_term, [
      'action' => 'store_link',
      'drupal_store' => TRUE,
      'chado_table' => $linker_table,
      'chado_column' => $linker_obj_col,
    ]);

    $properties[] = new ChadoVarCharStoragePropertyType($entity_type_id, self::$id, 'value', $value_term, $value_len, [
      'action' => 'join',
      'drupal_store' => FALSE,
      'path' => $base_table . '.' . $base_pkey_col . '>' . $linker_table . '.' . $base_pkey_col
        . ';' . $linker_table . '.' . $object_pkey_col . '>' . $object_table . '.' . $object_pkey_col,
      'chado_column' => self::$value_column,
      'as' => 'value',
    ]);
    // The type for the displayed value
    $properties[] = new ChadoVarCharStoragePropertyType($entity_type_id, self::$id, 'value_type', $value_type_term, $value_type_len, [
      'action' => 'join',
      'drupal_store' => FALSE,
      'path' => $base_table . '.' . $base_pkey_col . '>' . $linker_table . '.' . $base_pkey_col
        . ';' . $linker_table . '.' . $object_pkey_col . '>' . $object_table . '.' . $object_pkey_col
        . ';' . $object_table . '.' . $object_pkey_col . '>cvterm.cvterm_id',
      'chado_column' => 'name',
      'as' => 'value_type',
    ]);
   * @endcode
   *
   * These will be repeated in the testLinkerContactFieldProject
   * properties array below for testing.
   */
  protected $fields = [
    'testLinkerContactFieldProject' => [
      'field_name' => 'testLinkerContactFieldProject',
      'base_table' => 'project',
      'properties' => [
        'record_id' => [
          'propertyType class' => 'Drupal\tripal_chado\TripalStorage\ChadoIntStoragePropertyType',
          'action' => 'store_id',
          'drupal_store' => TRUE,
          'chado_table' => 'project',
          'chado_column' => 'project_id'
        ],
        'linker_pkey_id' => [
          'propertyType class' => 'Drupal\tripal_chado\TripalStorage\ChadoIntStoragePropertyType',
          'action' => 'store',
          'chado_table' => 'project_contact',
          'chado_column' => 'project_contact_id'
        ],
        'subject_id' => [
          'propertyType class' => 'Drupal\tripal_chado\TripalStorage\ChadoIntStoragePropertyType',
          'action' => 'store',
          'chado_table' => 'project_contact',
          'chado_column' => 'project_id'
        ],
        'object_id' => [
          'propertyType class' => 'Drupal\tripal_chado\TripalStorage\ChadoIntStoragePropertyType',
          'action' => 'store',
          'chado_table' => 'project_contact',
          'chado_column' => 'contact_id'
        ],
        'value' => [
          'propertyType class' => 'Drupal\tripal_chado\TripalStorage\ChadoVarCharStoragePropertyType',
          'action' => 'join',
          'path' => 'project.project_id>project_contact.project_id;project_contact.contact_id>contact.contact_id',
          'chado_column' => 'name',
          'as' => 'value',
        ],
        'value_type' => [
          'propertyType class' => 'Drupal\tripal_chado\TripalStorage\ChadoVarCharStoragePropertyType',
          'action' => 'join',
          'path' => 'project.project_id>project_contact.project_id;project_contact.contact_id>contact.contact_id;contact.type_id>cvterm.cvterm_id',
          'chado_column' => 'name',
          'as' => 'type_value',
        ],
      ],
    ],
  ];

  protected array $contact_id;

  /**
   * {@inheritdoc}
   */
  protected function setUp() :void {
    parent::setUp();
    $this->setUpChadoStorageTestEnviro();

    // Create basic contact records for use with these fields.
    // This field does not create a contact but rather just links to one.
    foreach (range(0, 1) as $id) {
      $query = $this->chado_connection->insert('1:contact');
      $query->fields([
        'name' => $this->randomString(25), //'Contact name for testing #' . $id,
      ]);
      $this->contact_id[$id] = $query->execute();
    }
  }

  /**
   * Testing ChadoStorage with the ChadoLinkerContactDefault field on a project content type.
   *
   * Test Cases:
   *   - Create link in Chado using ChadoStorage when they don't yet exist.
   *   - Load link in Chado using ChadoStorage after we just inserted them.
   *   - Update link in Chado using ChadoStorage after we just inserted them.
   */
  public function testProjectBaseTableFieldCRUD() {

print "CP1\n"; //@@@
    $project_id = [];
    $query = $this->chado_connection->insert('1:project');
    $query->fields([
      'name' => $this->randomString(25), //@@@'Project One',
      'description' => 'Test project',
    ]);
    $project_id[0] = $query->execute();
    $this->assertNotNull($project_id[0], 'Failed creating test project');
print "CP1a project_id=".$project_id[0]."\n"; //@@@

    // Test Case: Insert valid link when it does not yet exist in Chado.
    // ---------------------------------------------------------
    foreach (range(0, 1) as $id) {
print "CP2 id=$id\n"; //@@@
      $insert_values = [
        'testLinkerContactFieldProject' => [
          [
            'subject_id' => $project_id[0],
            'object_id' => $this->contact_id[$id],
          ],
        ],
      ];
      $this->chadoStorageTestInsertValues($insert_values);
print "CP3\n"; //@@@

      // Uncomment the following if you want to check that the arrays
      // for chado storage are being formed as we expect. This is very
      // useful for debugging.
      // @debug $this->debugChadoStorageTestTraitArrays();

      // Check that the link record was created as expected.
      $query = $this->chado_connection->select('1:project_contact', 'l')
          ->fields('l', ['project_id', 'contact_id']);
      $linker_records = $query->execute()->fetchAll();
print "CP4\n"; //@@@
      $this->assertCount($id + 1, $linker_records,
        'Only '.($id + 1).'project_contact record(s) should have been created.');
      $linker_record = $linker_records[$id];
      $this->assertEquals($project_id[0], $linker_record->project_id,
        "The project_id should be the one we set.");
      $this->assertEquals($contact_id[$id], $linker_record->contact_id,
        "The contact_id should be the one we set.");
    }

    // Test Case: Load values existing in Chado.
    // ---------------------------------------------------------
    // First we want to reset all the chado storage arrays to ensure we are
    // doing a clean test. The values will purposefully remain in Chado but the
    // Property Types, Property Values and Data Values will be built from scratch.
    $this->cleanChadoStorageValues();

@@@
    // For loading only the store id/pkey/link items should be populated.
    $load_values = [
      'testLinkerContactFieldProject' => [
        [
          'record_id' => $project_id[0],
        ],
      ],
      'testotherphylotreefield' => [
        [
          'other_record_id' => $phylotree_id,
        ]
      ],
    ];
    $retrieved_values = $this->chadoStorageTestLoadValues($load_values);

    // @debug Uncomment the following line if the asserts below fail.
    // @debug $this->debugChadoStorageTestTraitArrays();

    // Now test that the values have been loaded.
    // We want to test only our field
    // and retrieved values will be keyed by field name + delta.
    $retrieved = $retrieved_values['testLinkerContactFieldProject'][0];
    $this->assertEquals(
      $phylotree_id,
      $retrieved['record_id']['value']->getValue(),
      "The Phylotree ID did not match the one we retrieved from chado after insert."
    );
    $this->assertEquals(
      $this->analysis_id[0],
      $retrieved['analysis_id']['value']->getValue(),
      "The analysis_id did not match the one we retrieved from chado after insert."
    );
    $this->assertEquals(
      'Tripalus databasica Genome Assembly',
      $retrieved['analysis_name']['value']->getValue(),
      "The analysis name did not match the one we retrieved from chado after insert."
    );

    // Test Case: Update values in Chado using ChadoStorage.
    // ---------------------------------------------------------
    // When updating we need all the store id/pkey/link records
    // and all values of the other properties.
    // array_merge alone seems not to be sufficient

    $update_values = [
      'testLinkerContactFieldProject' => [
        [
          'record_id' => $phylotree_id,
          'analysis_id' => $this->analysis_id[1], // This is the change!
        ],
      ],
      'testotherphylotreefield' => [
        [
          'other_record_id' => $phylotree_id,
          'dbxref_id' => $dbxref_id,
        ]
      ],
    ];
    $this->chadoStorageTestUpdateValues($update_values);

    // Now we check chado to see if these values were changed...
    $query = $this->chado_connection->select('1:phylotree', 'p')
        ->fields('p', ['phylotree_id', 'dbxref_id', 'analysis_id']);
    $query->join('1:analysis', 'a', 'a.analysis_id = p.analysis_id');
    $query->addField('a', 'name', 'analysis_name');
    $linker_records = $query->execute()->fetchAll();
    $this->assertCount(1, $linker_records,
      'Only one phylotree record should be present as we should have updated the existing one.');

    $phylotree_dbrecord = $linker_records[0];
    $this->assertEquals($phylotree_id, $phylotree_dbrecord->phylotree_id,
      "The phylotree primary key should remain unchanged through update.");
    $this->assertEquals($this->analysis_id[1], $phylotree_dbrecord->analysis_id,
      "The analysis_id should be updated to the second one inserted.");
    $this->assertEquals('Tripal 4 Automated Testing', $phylotree_dbrecord->analysis_name,
      "An extra more readable check that the analysis is the one we expect.");
  }
}
