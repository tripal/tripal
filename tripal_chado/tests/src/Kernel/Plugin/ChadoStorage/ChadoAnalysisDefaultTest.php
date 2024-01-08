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
 * based on two base tables: phylotree, quantification.
 *
 * Note: quantification is not a typically created content type but we
 * can test it in this manner anyway as the tests are in the kernel environment
 * and do not interact with content types and fields attached to them but rather
 * focuses on the property types/values directly. This also allows us to test
 * phylotree directly even though at the time of writing this test, there is no
 * dbxref_id field attached to phylotree.
 *
 * Note: testotherphylotreefield and testotherquantificationfield are added
 * to ensure we meet the unique constraints on the phylotree and quantification
 * tables respectively.
 *
 *  Specific test cases:
 *   - [PHYLOTREE] Create Values in Chado using ChadoStorage when they don't yet exist.
 *   - [PHYLOTREE] Load values in Chado using ChadoStorage after we just inserted them.
 *   - [PHYLOTREE] Update values in Chado using ChadoStorage after we just inserted them.
 *   - [QUANTIFICATION] Create Values in Chado using ChadoStorage when they don't yet exist.
 *   - [QUANTIFICATION] Load values in Chado using ChadoStorage after we just inserted them.
 *   - [QUANTIFICATION] Update values in Chado using ChadoStorage after we just inserted them.
 *
 * @group Tripal
 * @group Tripal Chado
 * @group ChadoStorage
 * @group ChadoStorage Fields
 */
class ChadoAnalysisDefaultTest extends ChadoTestKernelBase {

  use ChadoStorageTestTrait;

  // We will populate this variable at the start of each test
  // with fields specific to that test.
  protected $fields = [];

  protected $yaml_file = __DIR__ . "/ChadoAnalysisDefault-FieldDefinitions.yml";

  protected array $analysis_id;

  /**
   * {@inheritdoc}
   */
  protected function setUp() :void {
    parent::setUp();
    $this->setUpChadoStorageTestEnviro();

    $this->setFieldsFromYaml($this->yaml_file, "testAnalysis");
    $this->cleanChadoStorageValues();

    // Create the analysis record for use with these fields.
    // This field does not create an analysis but rather just links to one.
    $query = $this->chado_connection->insert('1:analysis');
    $query->fields([
      'name' => 'Tripalus databasica Genome Assembly',
      'program' => 'Best Assembly Software Yet',
      'programversion' => '108',
      'sourcename' => 'Sweat and Tears of Tripal Core Developers',
    ]);
    $this->analysis_id[0] = $query->execute();

    // Create the analysis record for use with these fields.
    // This field does not create an analysis but rather just links to one.
    $query = $this->chado_connection->insert('1:analysis');
    $query->fields([
      'name' => 'Tripal 4 Automated Testing',
      'program' => 'PHPUnit',
      'programversion' => '9.6',
      'sourcename' => 'Current Tripal 4 Code',
    ]);
    $this->analysis_id[1] = $query->execute();
  }

  /**
   * Testing ChadoStorage with the ChadoAnalysisDefault field on a phylotree content type.
   *
   * Test Cases:
   *   - Create Values in Chado using ChadoStorage when they don't yet exist.
   *   - Load values in Chado using ChadoStorage after we just inserted them.
   *   - Update values in Chado using ChadoStorage after we just inserted them.
   */
  public function testPhylotreeBaseTableFieldCRUD() {

    // Phylotree requires a dbxref_id however it has no impact on our field.
    // As such, let's just grab the one associated with the taxrank genus cvterm
    // since it's easy to lookup.
    $genus_cvtermID = $this->getCvtermID('TAXRANK', '0000005');
    $query = $this->chado_connection->select('1:cvterm', 'cvt');
    $query->fields('cvt', ['dbxref_id']);
    $query->condition('cvterm_id', $genus_cvtermID, '=');
    $dbxref_id = $query->execute()->fetchField();

    // Test Case: Insert valid values when they do not yet exist in Chado.
    // ---------------------------------------------------------
    $insert_values = [
      'testAnalysisFieldPhylotree' => [
        [
          // No value for record_id as we do not yet have a phylotree record.
          'analysis_id' => $this->analysis_id[0],
          // No value for analysis_name as it will be loaded.
          // I do not think join properties are populated on save (thinking)
        ],
      ],
      'testotherphylotreefield' => [
        [
          'dbxref_id' => $dbxref_id,
        ]
      ],
    ];
    $this->chadoStorageTestInsertValues($insert_values);

    // Uncomment the following if you want to check that the arrays
    // for chado storage are being formed as we expect. This is very
    // useful for debugging.
    // @debug $this->debugChadoStorageTestTraitArrays();

    // Check that the phylotree record was created as expected.
    $query = $this->chado_connection->select('1:phylotree', 'p')
        ->fields('p', ['phylotree_id', 'dbxref_id', 'analysis_id']);
    $query->join('1:analysis', 'a', 'a.analysis_id = p.analysis_id');
    $query->addField('a', 'name', 'analysis_name');
    $phylotree_records = $query->execute()->fetchAll();
    $this->assertCount(1, $phylotree_records,
      'Only one phylotree record should have been created.');
    $phylotree_dbrecord = $phylotree_records[0];
    $this->assertEquals($this->analysis_id[0], $phylotree_dbrecord->analysis_id,
      "The analysis_id should be the one we set.");
    $this->assertEquals('Tripalus databasica Genome Assembly', $phylotree_dbrecord->analysis_name,
      "An extra more readable check that the analysis is the one we expect.");
    $phylotree_id = $phylotree_dbrecord->phylotree_id;

    // Test Case: Load values existing in Chado.
    // ---------------------------------------------------------
    // First we want to reset all the chado storage arrays to ensure we are
    // doing a clean test. The values will purposefully remain in Chado but the
    // Property Types, Property Values and Data Values will be built from scratch.
    $this->cleanChadoStorageValues();

    // For loading only the store id/pkey/link items should be populated.
    $load_values = [
      'testAnalysisFieldPhylotree' => [
        [
          'record_id' => $phylotree_id,
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
    $retrieved = $retrieved_values['testAnalysisFieldPhylotree'][0];
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
      'testAnalysisFieldPhylotree' => [
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
    $phylotree_records = $query->execute()->fetchAll();
    $this->assertCount(1, $phylotree_records,
      'Only one phylotree record should be present as we should have updated the existing one.');

    $phylotree_dbrecord = $phylotree_records[0];
    $this->assertEquals($phylotree_id, $phylotree_dbrecord->phylotree_id,
      "The phylotree primary key should remain unchanged through update.");
    $this->assertEquals($this->analysis_id[1], $phylotree_dbrecord->analysis_id,
      "The analysis_id should be updated to the second one inserted.");
    $this->assertEquals('Tripal 4 Automated Testing', $phylotree_dbrecord->analysis_name,
      "An extra more readable check that the analysis is the one we expect.");
  }

  /**
   * Testing ChadoStorage with the ChadoAnalysisDefault field on a quantification content type.
   *
   * Test Cases:
   *   - Create Values in Chado using ChadoStorage when they don't yet exist.
   *   - Load values in Chado using ChadoStorage after we just inserted them.
   *   - Update values in Chado using ChadoStorage after we just inserted them.
   */
  public function testQuantificationBaseTableFieldCRUD() {


    // Quantification requires a acquisition_id however it has no impact on our field.
    // As such, we will just create one with dummy details to meet the constraint...
    // However, these are a network of tables...
    // acquisition requires assay; assay requires an arraydesign + contact;
    // arraydesign requires a cvterm + contact.
    // -- cvterm
    $genus_cvtermID = $this->getCvtermID('TAXRANK', '0000005');
    // -- contact
    $query = $this->chado_connection->insert('1:contact');
    $query->fields([
      'name' => 'One Sad Developer wishing these tables were not so interconnected',
    ]);
    $contact_id = $query->execute();
    // -- arraydesign
    $query = $this->chado_connection->insert('1:arraydesign');
    $query->fields([
      'manufacturer_id' => $contact_id,
      'platformtype_id' => $genus_cvtermID,
      'name' => 'A Fake Array Design Chip',
    ]);
    $arraydesign_id = $query->execute();
    // -- assay
    $query = $this->chado_connection->insert('1:assay');
    $query->fields([
      'arraydesign_id' => $arraydesign_id,
      'operator_id' => $contact_id
    ]);
    $assay_id = $query->execute();
    // -- Finally acquisition.
    $query = $this->chado_connection->insert('1:acquisition');
    $query->fields([
      'assay_id' => $assay_id,
      'name' => 'Fake Acquisition'
    ]);
    $acquisition_id = $query->execute();

    // Test Case: Insert valid values when they do not yet exist in Chado.
    // ---------------------------------------------------------
    $insert_values = [
      'testAnalysisFieldQuantification' => [
        [
          // No value for record_id as we do not yet have a phylotree record.
          'analysis_id' => $this->analysis_id[0],
          // No value for analysis_name as it will be loaded.
          // I do not think join properties are populated on save (thinking)
        ],
      ],
      'testotherquantificationfield' => [
        [
          'acquisition_id' => $acquisition_id,
        ]
      ],
    ];
    $this->chadoStorageTestInsertValues($insert_values);

    // Uncomment the following if you want to check that the arrays
    // for chado storage are being formed as we expect. This is very
    // useful for debugging.
    // @debug $this->debugChadoStorageTestTraitArrays();

    // Check that the quantification record was created as expected.
    $query = $this->chado_connection->select('1:quantification', 'q')
        ->fields('q', ['quantification_id', 'acquisition_id', 'analysis_id']);
    $query->join('1:analysis', 'a', 'a.analysis_id = q.analysis_id');
    $query->addField('a', 'name', 'analysis_name');
    $records = $query->execute()->fetchAll();
    $this->assertCount(1, $records,
      'Only one quantification record should have been created.');
    $quantification_dbrecord = $records[0];
    $this->assertEquals($this->analysis_id[0], $quantification_dbrecord->analysis_id,
      "The analysis_id should be the one we set.");
    $this->assertEquals('Tripalus databasica Genome Assembly', $quantification_dbrecord->analysis_name,
      "An extra more readable check that the analysis is the one we expect.");
    $quantification_id = $quantification_dbrecord->quantification_id;

    // Test Case: Load values existing in Chado.
    // ---------------------------------------------------------
    // First we want to reset all the chado storage arrays to ensure we are
    // doing a clean test. The values will purposefully remain in Chado but the
    // Property Types, Property Values and Data Values will be built from scratch.
    $this->cleanChadoStorageValues();

    // For loading only the store id/pkey/link items should be populated.
    $load_values = [
      'testAnalysisFieldQuantification' => [
        [
          'record_id' => $quantification_id,
        ],
      ],
      'testotherquantificationfield' => [
        [
          'other_record_id' => $quantification_id,
        ]
      ],
    ];
    $retrieved_values = $this->chadoStorageTestLoadValues($load_values);

    // @debug Uncomment the following line if the asserts below fail.
    // @debug $this->debugChadoStorageTestTraitArrays();

    // Now test that the values have been loaded.
    // We want to test only our field
    // and retrieved values will be keyed by field name + delta.
    $retrieved = $retrieved_values['testAnalysisFieldQuantification'][0];
    $this->assertEquals(
      $quantification_id,
      $retrieved['record_id']['value']->getValue(),
      "The Quantification ID did not match the one we retrieved from chado after insert."
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
      'testAnalysisFieldQuantification' => [
        [
          'record_id' => $quantification_id,
          'analysis_id' => $this->analysis_id[1], // This is the change!
        ],
      ],
      'testotherquantificationfield' => [
        [
          'other_record_id' => $quantification_id,
          'acquisition_id' => $acquisition_id,
        ]
      ],
    ];
    $this->chadoStorageTestUpdateValues($update_values);

    // Now we check chado to see if these values were changed...
    $query = $this->chado_connection->select('1:quantification', 'q')
        ->fields('q', ['quantification_id', 'acquisition_id', 'analysis_id']);
    $query->join('1:analysis', 'a', 'a.analysis_id = q.analysis_id');
    $query->addField('a', 'name', 'analysis_name');
    $records = $query->execute()->fetchAll();
    $this->assertCount(1, $records,
      'Only one phylotree record should be present as we should have updated the existing one.');

    $dbrecord = $records[0];
    $this->assertEquals($quantification_id, $dbrecord->quantification_id,
      "The quantification primary key should remain unchanged through update.");
    $this->assertEquals($this->analysis_id[1], $dbrecord->analysis_id,
      "The analysis_id should be updated to the second one inserted.");
    $this->assertEquals('Tripal 4 Automated Testing', $dbrecord->analysis_name,
      "An extra more readable check that the analysis is the one we expect.");
  }
}
