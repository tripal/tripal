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
 * based on two base tables: study, and arraydesign.
 *
 * Note that the arraydesign table is a bit unusal, since the relevant
 * column is 'manufacturer_id' which corresponds to 'contact_id' in the
 * contact table.
 * # Note: quantification is not a typically created content type but we
 * # can test it in this manner anyway as the tests are in the kernel environment
 * # and do not interact with content types and fields attached to them but rather
 * # focuses on the property types/values directly. This also allows us to test
 * # phylotree directly even though at the time of writing this test, there is no
 * # dbxref_id field attached to phylotree.
 *
 * Note: testotherstudyfield and testotherarraydesignfield are added
 * to ensure we meet the unique constraints on the study and arraydesign
 * tables respectively.
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
 * @group ContactTest
 */
class ChadoContactDefaultTest extends ChadoTestKernelBase {

  use ChadoStorageTestTrait;

  /**
   * Properties directly from the ChadoContactDefault field type:
   * @code
    $properties[] = new ChadoIntStoragePropertyType($entity_type_id, self::$id, 'record_id', $record_id_term, [
      'action' => 'store_id',
      'drupal_store' => TRUE,
      'chado_table' => $base_table,
      'chado_column' => $base_pkey_col,
    ]);
    $properties[] = new ChadoIntStoragePropertyType($entity_type_id, self::$id, 'contact_id', $contact_id_term, [
      'action' => 'store',
      'chado_table' => $base_table,
      'chado_column' => $base_fkey_col,
    ]);
    $properties[] =  new ChadoVarCharStoragePropertyType($entity_type_id, self::$id, 'contact_name', $contact_name_term, $contact_name_length, [
      'action' => 'join',
      'path' => $base_table . '.' . $base_fkey_col . '>contact.contact_id',
      'chado_column' => 'name',
      'as' => 'contact_name',
    ]);
   * @endcode
   *
   * These will be repeated in the testContactFieldStudy and
   * testContactFieldArrayDesign properties array below for testing.
   */
  protected $fields = [
    'testContactFieldStudy' => [
      'field_name' => 'testContactFieldStudy',
      'base_table' => 'study',
      'properties' => [
        'record_id' => [
          'propertyType class' => 'Drupal\tripal_chado\TripalStorage\ChadoIntStoragePropertyType',
          'action' => 'store_id',
          'drupal_store' => TRUE,
          'chado_table' => 'study',
          'chado_column' => 'study_id'
        ],
        'contact_id' => [
          'propertyType class' => 'Drupal\tripal_chado\TripalStorage\ChadoIntStoragePropertyType',
          'action' => 'store',
          'chado_table' => 'study',
          'chado_column' => 'contact_id'
        ],
        'contact_name' => [
          'propertyType class' => 'Drupal\tripal_chado\TripalStorage\ChadoVarCharStoragePropertyType',
          'action' => 'join',
          'path' => 'study.contact_id>contact.contact_id',
          'chado_column' => 'name',
          'as' => 'contact_name',
        ],
      ],
    ],
    // Just adds in any properties needed to meet the unique constraints on the
    // study table.
    'testotherstudyfield' => [
      'field_name' => 'testotherstudyfield',
      'base_table' => 'study',
      'properties' => [
        'other_record_id' => [
          'propertyType class' => 'Drupal\tripal_chado\TripalStorage\ChadoIntStoragePropertyType',
          'action' => 'store_id',
          'drupal_store' => TRUE,
          'chado_table' => 'study',
          'chado_column' => 'study_id'
        ],
        // Name is not null
        'name' => [
          'propertyType class' => 'Drupal\tripal_chado\TripalStorage\ChadoVarCharStoragePropertyType',
          'action' => 'store',
          'chado_table' => 'study',
          'chado_column' => 'name'
        ],
      ],
    ],
    'testContactFieldArrayDesign' => [
      'field_name' => 'testContactFieldArrayDesign',
      'base_table' => 'arraydesign',
      'properties' => [
        'record_id' => [
          'propertyType class' => 'Drupal\tripal_chado\TripalStorage\ChadoIntStoragePropertyType',
          'action' => 'store_id',
          'drupal_store' => TRUE,
          'chado_table' => 'arraydesign',
          'chado_column' => 'arraydesign_id'
        ],
        'contact_id' => [
          'propertyType class' => 'Drupal\tripal_chado\TripalStorage\ChadoIntStoragePropertyType',
          'action' => 'store',
          'chado_table' => 'arraydesign',
          'chado_column' => 'manufacturer_id'
        ],
        'contact_name' => [
          'propertyType class' => 'Drupal\tripal_chado\TripalStorage\ChadoVarCharStoragePropertyType',
          'action' => 'join',
          'path' => 'arraydesign.manufacturer_id>contact.contact_id',
          'chado_column' => 'name',
          'as' => 'contact_name',
        ],
      ],
    ],
    // Just adds in any properties needed to meet the unique constraints on the
    // arraydesign table.
    'testotherarraydesignfield' => [
      'field_name' => 'testotherarraydesignfield',
      'base_table' => 'arraydesign',
      'properties' => [
        'other_record_id' => [
          'propertyType class' => 'Drupal\tripal_chado\TripalStorage\ChadoIntStoragePropertyType',
          'action' => 'store_id',
          'drupal_store' => TRUE,
          'chado_table' => 'arraydesign',
          'chado_column' => 'arraydesign_id'
        ],
        // platformtype_id corresponds to a cvterm.cvterm_id
        'platformtype_id' => [
          'propertyType class' => 'Drupal\tripal_chado\TripalStorage\ChadoIntStoragePropertyType',
          'action' => 'store',
          'chado_table' => 'arraydesign',
          'chado_column' => 'platformtype_id'
        ],
        // Name is not null
        'name' => [
          'propertyType class' => 'Drupal\tripal_chado\TripalStorage\ChadoVarCharStoragePropertyType',
          'action' => 'store',
          'chado_table' => 'arraydesign',
          'chado_column' => 'name'
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
print "CP1 id=\"$id\"\n"; //@@@
      $query = $this->chado_connection->insert('1:contact');
      $query->fields([
        'name' => 'Contact name for testing #' . $id,
      ]);
      $this->contact_id[$id] = $query->execute();
print "CP2 contact_id[$id] = \"".$this->contact_id[$id]."\"\n"; //@@@
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
          // No value for study_name as it will be loaded.
          // I do not think join properties are populated on save (thinking)
        ],
      ],
    ];
    $this->chadoStorageTestInsertValues($insert_values);

    // Uncomment the following if you want to check that the arrays
    // for chado storage are being formed as we expect. This is very
    // useful for debugging.
    // @debug 
print "CP31\n"; //@@@
$this->debugChadoStorageTestTraitArrays();

    // Check that the Study record was created as expected.
    $query = $this->chado_connection->select('1:study', 'base')
        ->fields('base', ['study_id', 'contact_id']);
    $query->join('1:contact', 'linked', 'base.study_id = linked.study_id');
    $query->addField('linked', 'name', 'linked_name');
    $base_records = $query->execute()->fetchAll();
    $this->assertCount(1, $base_records,
      'Only one Study record should have been created.');
    $base_dbrecord = $base_records[0];
    $this->assertEquals($this->contact_id[0], $base_dbrecord->contact_id,
      "The contact_id should be the one we set.");
    $this->assertEquals('Contact name for testing #0', $base_dbrecord->linked_name,
      "An extra more readable check that the contact is the one we expect.");
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
          'record_id' => $study_id,
        ],
      ],
      'testotherstudyfield' => [
        [
          'other_record_id' => $study_id,
        ]
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
      $study_id,
      $retrieved['record_id']['value']->getValue(),
      "The Study ID did not match the one we retrieved from chado after insert."
    );
    $this->assertEquals(
      $this->contact_id[0],
      $retrieved['study_id']['value']->getValue(),
      "The study_id did not match the one we retrieved from chado after insert."
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
          'record_id' => $study_id,
          'study_id' => $this->contact_id[1], // This is the change!
        ],
      ],
      'testotherstudyfield' => [
        [
          'other_record_id' => $study_id,
        ]
      ],
    ];
    $this->chadoStorageTestUpdateValues($update_values);

    // Now we check chado to see if these values were changed...
    $query = $this->chado_connection->select('1:study', 'base')
        ->fields('base', ['study_id', 'contact_id']);
    $query->join('1:contact', 'linked', 'base.study_id = linked.study_id');
    $query->addField('linked', 'name', 'linked_name');
    $base_records = $query->execute()->fetchAll();
    $this->assertCount(1, $base_records,
      'Only one study record should be present as we should have updated the existing one.');

    $base_dbrecord = $base_records[0];
    $this->assertEquals($study_id, $base_dbrecord->study_id,
      "The study primary key should remain unchanged through update.");
    $this->assertEquals($this->contact_id[1], $base_dbrecord->study_id,
      "The study_id should be updated to the second one inserted.");
    $this->assertEquals('Contact name for testing #1', $base_dbrecord->analysis_name,
      "An extra more readable check that the contact is the one we expect.");
  }

  /**
   * Testing ChadoStorage with the ChadoAnalysisDefault field on a quantification content type.
   *
   * Test Cases:
   *   - Create Values in Chado using ChadoStorage when they don't yet exist.
   *   - Load values in Chado using ChadoStorage after we just inserted them.
   *   - Update values in Chado using ChadoStorage after we just inserted them.
   */
  public function testArrayDesignBaseTableFieldCRUD() {


    // ArrayDesign requires a platformtype_id, however it has no impact on our field.
    // As such, we will just use the "null" CV term (cvterm_id = 1).
    $platformtype_id = 1;

    // Test Case: Insert valid values when they do not yet exist in Chado.
    // ---------------------------------------------------------
    $insert_values = [
      'testContactFieldArrayDesign' => [
        [
          // No value for record_id as we do not yet have an arraydesign record.
          'contact_id' => $this->contact_id[0],
          // No value for arraydesign_name as it will be loaded.
          // I do not think join properties are populated on save (thinking)
        ],
      ],
      'testotherarraydesignfield' => [
        [
          'platformtype_id' => $platformtype_id,
        ]
      ],
    ];
print "CPAD1\n"; //@@@
    $this->chadoStorageTestInsertValues($insert_values);
print "CPAD2\n"; //@@@

    // Uncomment the following if you want to check that the arrays
    // for chado storage are being formed as we expect. This is very
    // useful for debugging.
    // @debug 
$this->debugChadoStorageTestTraitArrays();

    // Check that the arraydesign record was created as expected.
    $query = $this->chado_connection->select('1:arraydesign', 'base')
        ->fields('base', ['arraydesign_id', 'platformtype_id', 'contact_id']);
    $query->join('1:contact', 'linked', 'base.contact_id = linked.contact_id');
    $query->addField('linked', 'name', 'linked_name');
    $records = $query->execute()->fetchAll();
    $this->assertCount(1, $records,
      'Only one arraydesign record should have been created.');
    $arraydesign_dbrecord = $records[0];
    $this->assertEquals($this->contact_id[0], $arraydesign_dbrecord->contact_id,
      "The contact_id should be the one we set.");
    $this->assertEquals('Tripalus databasica Genome Assembly', $arraydesign_dbrecord->analysis_name,
      "An extra more readable check that the analysis is the one we expect.");
    $arraydesign_id = $arraydesign_dbrecord->arraydesign_id;

    // Test Case: Load values existing in Chado.
    // ---------------------------------------------------------
    // First we want to reset all the chado storage arrays to ensure we are
    // doing a clean test. The values will purposefully remain in Chado but the
    // Property Types, Property Values and Data Values will be built from scratch.
    $this->cleanChadoStorageValues();
print "CPAD4\n"; //@@@

    // For loading only the store id/pkey/link items should be populated.
    $load_values = [
      'testContactFieldArrayDesign' => [
        [
          'record_id' => $arraydesign_id,
        ],
      ],
      'testotherarraydesignfield' => [
        [
          'other_record_id' => $arraydesign_id,
#          'name' => 'ArbitraryName',
#          'platformtype_id' => 1,
        ]
      ],
    ];
    $retrieved_values = $this->chadoStorageTestLoadValues($load_values);
print "CPAD5\n"; //@@@

    // @debug Uncomment the following line if the asserts below fail.
    // @debug $this->debugChadoStorageTestTraitArrays();

    // Now test that the values have been loaded.
    // We want to test only our field
    // and retrieved values will be keyed by field name + delta.
    $retrieved = $retrieved_values['testContactFieldArrayDesign'][0];
    $this->assertEquals(
      $arraydesign_id,
      $retrieved['record_id']['value']->getValue(),
      "The arraydesign ID did not match the one we retrieved from chado after insert."
    );
    $this->assertEquals(
      $this->contact_id[0],
      $retrieved['study_id']['value']->getValue(),
      "The study_id did not match the one we retrieved from chado after insert."
    );
    $this->assertEquals(
      'Tripalus databasica Genome Assembly',
      $retrieved['analysis_name']['value']->getValue(),
      "The analysis name did not match the one we retrieved from chado after insert."
    );

print "CPAD7\n"; //@@@

    // Test Case: Update values in Chado using ChadoStorage.
    // ---------------------------------------------------------
    // When updating we need all the store id/pkey/link records
    // and all values of the other properties.
    // array_merge alone seems not to be sufficient

    $update_values = [
      'testContactFieldArrayDesign' => [
        [
          'record_id' => $arraydesign_id,
          'study_id' => $this->contact_id[1], // This is the change!
        ],
      ],
      'testotherarraydesignfield' => [
        [
          'other_record_id' => $arraydesign_id,
#          'platformtype_id' => $platformtype_id,
        ]
      ],
    ];
    $this->chadoStorageTestUpdateValues($update_values);
print "CPAD9\n"; //@@@

    // Now we check chado to see if these values were changed...
    $query = $this->chado_connection->select('1:arraydesign', 'q')
        ->fields('q', ['arraydesign_id', 'platformtype_id', 'study_id']);
    $query->join('1:contact', 'linked', 'base.study_id = linked.study_id');
    $query->addField('linked', 'name', 'linked_name');
    $records = $query->execute()->fetchAll();
    $this->assertCount(1, $records,
      'Only one phylotree record should be present as we should have updated the existing one.');

    $dbrecord = $records[0];
    $this->assertEquals($arraydesign_id, $dbrecord->arraydesign_id,
      "The arraydesign primary key should remain unchanged through update.");
    $this->assertEquals($this->contact_id[1], $dbrecord->study_id,
      "The study_id should be updated to the second one inserted.");
    $this->assertEquals('Tripal 4 Automated Testing', $dbrecord->analysis_name,
      "An extra more readable check that the analysis is the one we expect.");
  }
}
