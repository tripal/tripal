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
 * to match those in the ChadoContactDefault field with values filled
 * based on two base tables: study and arraydesign.
 *
 * Note that the arraydesign table is a bit unusal, since the relevant
 * column is 'manufacturer_id' which corresponds to 'contact_id' in the
 * contact table.
 *
 *  Specific test cases:
 *   - [STUDY] Create Values in Chado using ChadoStorage when they don't yet exist.
 *   - [STUDY] Load values in Chado using ChadoStorage after we just inserted them.
 *   - [STUDY] Update values in Chado using ChadoStorage after we just inserted them.
 *   - [ARRAYDESIGN] Create Values in Chado using ChadoStorage when they don't yet exist.
 *   - [ARRAYDESIGN] Load values in Chado using ChadoStorage after we just inserted them.
 *   - [ARRAYDESIGN] Update values in Chado using ChadoStorage after we just inserted them.
 *
 * @group Tripal
 * @group Tripal Chado
 * @group ChadoStorage
 * @group ChadoStorage Fields
 */
class ChadoContactDefaultTest extends ChadoTestKernelBase {

  use ChadoStorageTestTrait;

  // We will populate this variable at the start of each test
  // with fields specific to that test.
  protected $fields = [];

  protected $yaml_file = __DIR__ . "/ChadoContactDefault-FieldDefinitions.yml";

  protected array $contact_id;

  /**
   * {@inheritdoc}
   */
  protected function setUp() :void {
    parent::setUp();
    $this->setUpChadoStorageTestEnviro();

    $this->setFieldsFromYaml($this->yaml_file, "testContact");
    $this->cleanChadoStorageValues();

    // Create basic contact records for use with these fields.
    // This field does not create a contact but rather just links to one.
    foreach (range(0, 1) as $id) {
      $query = $this->chado_connection->insert('1:contact');
      $query->fields([
        'name' => 'Contact name for testing #' . $id,
      ]);
      $this->contact_id[$id] = $query->execute();
    }
  }



  /**
   * Testing ChadoStorage with the ChadoContactDefault field on a study content type.
   *
   * Test Cases:
   *   - Create Values in Chado using ChadoStorage when they don't yet exist.
   *   - Load values in Chado using ChadoStorage after we just inserted them.
   *   - Update values in Chado using ChadoStorage after we just inserted them.
   */
  public function testStudyBaseTableFieldCRUD() {

    // Test Case: Insert valid values when they do not yet exist in Chado.
    // ---------------------------------------------------------
    $insert_values = [
      'testContactFieldStudy' => [
        [
          // No value for record_id as we do not yet have a Study record.
          'contact_id' => $this->contact_id[0],
          'name' => 'ChadoContactDefaultTest study #1',
        ],
      ],
    ];
    $this->chadoStorageTestInsertValues($insert_values);

    // Uncomment the following if you want to check that the arrays
    // for chado storage are being formed as we expect. This is very
    // useful for debugging.
    // @debug $this->debugChadoStorageTestTraitArrays();

    // Check that the Study record was created as expected.
    $query = $this->chado_connection->select('1:study', 'base')
        ->fields('base', ['study_id', 'name', 'contact_id']);
    $query->join('1:contact', 'linked', 'base.contact_id = linked.contact_id');
    $query->addField('linked', 'name', 'linked_name');
    $base_records = $query->execute()->fetchAll();
    $this->assertCount(
      1,
      $base_records,
      'Only one Study record should have been created.');
    $base_dbrecord = $base_records[0];
    $this->assertEquals(
      $this->contact_id[0],
      $base_dbrecord->contact_id,
      "The contact_id should be the one we set.");
    $this->assertEquals(
      'Contact name for testing #0',
      $base_dbrecord->linked_name,
      "Failing the extra more readable check that the contact is the one we expect.");
    $base_id = $base_dbrecord->study_id;

    // Test Case: Load values existing in Chado.
    // ---------------------------------------------------------
    // First we want to reset all the chado storage arrays to ensure we are
    // doing a clean test. The values will purposefully remain in Chado but the
    // Property Types, Property Values and Data Values will be built from scratch.
    $this->cleanChadoStorageValues();

    // For loading only the store id/pkey/link items should be populated.
    $load_values = [
      'testContactFieldStudy' => [
        [
          'record_id' => $base_id,
        ],
      ],
    ];
    $retrieved_values = $this->chadoStorageTestLoadValues($load_values);

    // @debug Uncomment the following line if the asserts below fail.
    // @debug $this->debugChadoStorageTestTraitArrays();

    // Now test that the values have been loaded.
    // We want to test only our field
    // and retrieved values will be keyed by field name + delta.
    $retrieved = $retrieved_values['testContactFieldStudy'][0];
    $this->assertEquals(
      $base_id,
      $retrieved['record_id']['value']->getValue(),
      "The Study ID did not match the one we retrieved from chado after insert."
    );
    $this->assertEquals(
      $this->contact_id[0],
      $retrieved['contact_id']['value']->getValue(),
      "The contact_id did not match the one we retrieved from chado after insert."
    );
    $this->assertEquals(
      'Contact name for testing #0',
      $retrieved['contact_name']['value']->getValue(),
      "The contact name did not match the one we retrieved from chado after insert."
    );

    // Test Case: Update values in Chado using ChadoStorage.
    // ---------------------------------------------------------
    // When updating we need all the store id/pkey/link records
    // and all values of the other properties.
    // array_merge alone seems not to be sufficient

    $update_values = [
      'testContactFieldStudy' => [
        [
          'record_id' => $base_id,
          'name' => 'ChadoContactDefaultTest study #1',
          'contact_id' => $this->contact_id[1], // This is the change!
        ],
      ],
    ];
    $this->chadoStorageTestUpdateValues($update_values);

    // Now we check chado to see if these values were changed...
    $query = $this->chado_connection->select('1:study', 'base')
        ->fields('base', ['study_id', 'name', 'contact_id']);
    $query->join('1:contact', 'linked', 'base.contact_id = linked.contact_id');
    $query->addField('linked', 'name', 'linked_name');
    $base_records = $query->execute()->fetchAll();
    $this->assertCount(
      1,
      $base_records,
      'Only one study record should be present as we should have updated the existing one.');

    $base_dbrecord = $base_records[0];
    $this->assertEquals(
      $base_id,
      $base_dbrecord->study_id,
      "The study primary key should remain unchanged through update.");
    $this->assertEquals(
      $this->contact_id[1],
      $base_dbrecord->contact_id,
      "The contact_id should be updated to the second one inserted.");
    $this->assertEquals(
      'Contact name for testing #1',
      $base_dbrecord->linked_name,
      "Failing the extra more readable check that the updated contact is the one we expect.");
  }



  /**
   * Testing ChadoStorage with the ChadoContactDefault field on an arraydesign content type.
   *
   * Test Cases:
   *   - Create Values in Chado using ChadoStorage when they don't yet exist.
   *   - Load values in Chado using ChadoStorage after we just inserted them.
   *   - Update values in Chado using ChadoStorage after we just inserted them.
   */
  public function testArrayDesignBaseTableFieldCRUD() {

    // ArrayDesign requires a platformtype_id, however it has no impact on our field.
    // As such, we will just use the "null" CV term (cvterm_id = 1).
    $null_platformtype_id = 1;

    // Test Case: Insert valid values when they do not yet exist in Chado.
    // ---------------------------------------------------------
    $insert_values = [
      'testContactFieldArrayDesign' => [
        [
          // No value for record_id as we do not yet have an arraydesign record.
          'manufacturer_id' => $this->contact_id[0],
          'name' => 'ChadoContactDefaultTest arraydesign #1',
          'platformtype_id' => $null_platformtype_id,
        ],
      ],
    ];
    $this->chadoStorageTestInsertValues($insert_values);

    // Uncomment the following if you want to check that the arrays
    // for chado storage are being formed as we expect. This is very
    // useful for debugging.
    // @debug $this->debugChadoStorageTestTraitArrays();

    // Check that the arraydesign record was created as expected.
    $query = $this->chado_connection->select('1:arraydesign', 'base')
        ->fields('base', ['arraydesign_id', 'platformtype_id', 'manufacturer_id']);
    $query->join('1:contact', 'linked', 'base.manufacturer_id = linked.contact_id');
    $query->addField('linked', 'name', 'linked_name');
    $records = $query->execute()->fetchAll();
    $this->assertCount(
      1,
      $records,
      'Only one arraydesign record should have been created.');
    $base_dbrecord = $records[0];
    $this->assertEquals(
      $this->contact_id[0],
      $base_dbrecord->manufacturer_id,
      "The manufacturer_id should be the one we set.");
    $this->assertEquals(
      'Contact name for testing #0',
      $base_dbrecord->linked_name,
      "Failing the extra more readable check that the manufacturer is the one we expect.");
    $base_id = $base_dbrecord->arraydesign_id;

    // Test Case: Load values existing in Chado.
    // ---------------------------------------------------------
    // First we want to reset all the chado storage arrays to ensure we are
    // doing a clean test. The values will purposefully remain in Chado but the
    // Property Types, Property Values and Data Values will be built from scratch.
    $this->cleanChadoStorageValues();

    // For loading only the store id/pkey/link items should be populated.
    $load_values = [
      'testContactFieldArrayDesign' => [
        [
          'record_id' => $base_id,
        ],
      ],
    ];
    $retrieved_values = $this->chadoStorageTestLoadValues($load_values);

    // @debug Uncomment the following line if the asserts below fail.
    // @debug $this->debugChadoStorageTestTraitArrays();

    // Now test that the values have been loaded.
    // We want to test only our field
    // and retrieved values will be keyed by field name + delta.
    $retrieved = $retrieved_values['testContactFieldArrayDesign'][0];
    $this->assertEquals(
      $base_id,
      $retrieved['record_id']['value']->getValue(),
      "The arraydesign ID did not match the one we retrieved from chado after insert."
    );
    $this->assertEquals(
      $this->contact_id[0],
      $retrieved['manufacturer_id']['value']->getValue(),
      "The manufacturer_id did not match the one we retrieved from chado after insert."
    );
    $this->assertEquals(
      'Contact name for testing #0',
      $retrieved['contact_name']['value']->getValue(),
      "The contact name did not match the one we retrieved from chado after insert."
    );

    // Test Case: Update values in Chado using ChadoStorage.
    // ---------------------------------------------------------
    // When updating we need all the store id/pkey/link records
    // and all values of the other properties.
    // array_merge alone seems not to be sufficient

    $update_values = [
      'testContactFieldArrayDesign' => [
        [
          'record_id' => $base_id,
          'name' => 'ChadoContactDefaultTest arraydesign #1',
          'manufacturer_id' => $this->contact_id[1], // This is the change!
          'platformtype_id' => $null_platformtype_id,
        ],
      ],
    ];
    $this->chadoStorageTestUpdateValues($update_values);

    // Now we check chado to see if these values were changed...
    $query = $this->chado_connection->select('1:arraydesign', 'base')
        ->fields('base', ['arraydesign_id', 'platformtype_id', 'manufacturer_id']);
    $query->join('1:contact', 'linked', 'base.manufacturer_id = linked.contact_id');
    $query->addField('linked', 'name', 'linked_name');
    $records = $query->execute()->fetchAll();
    $this->assertCount(
      1,
      $records,
      'Only one arraydesign record should be present as we should have updated the existing one.');

    $base_dbrecord = $records[0];
    $this->assertEquals($base_id,
      $base_dbrecord->arraydesign_id,
      "The arraydesign primary key should remain unchanged through update.");
    $this->assertEquals(
      $this->contact_id[1],
      $base_dbrecord->manufacturer_id,
      "The manufacturer_id should be updated to the second one inserted.");
    $this->assertEquals(
      'Contact name for testing #1',
      $base_dbrecord->linked_name,
      "Failing the extra more readable check that the updated manufacturer is the one we expect.");

    // Test that a link can be generated to the referenced manufacturer (i.e. contact)
    $lookup_manager = \Drupal::service('tripal.tripal_entity.lookup');
    // CV Term for the contact field is 'Communication Contact'
    $item_settings = [
      'storage_plugin_id' => 'chado_storage',
      'termIdSpace' => 'NCIT',
      'termAccession' => 'C47954',
    ];
//    $id = 'manufacturer_id';
print "records="; var_dump($records); //@@@


//      $item_settings = $item->getDataDefinition()->getSettings();
//      $id = $item_settings['storage_plugin_settings']['linker_fkey_column'] ?? 'contact_id';
    $renderable_item = $lookup_manager->getRenderableItem(
      'test_xyzzy',
      $base_dbrecord->manufacturer_id,
      $item_settings
    );
print "renderable item="; var_dump($renderable_item); //@@@


  }
}
