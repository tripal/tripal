<?php

namespace Drupal\Tests\tripal_chado\Kernel\Plugin\ChadoStorage;

use Drupal\Tests\tripal_chado\Kernel\ChadoTestKernelBase;
use Drupal\Tests\tripal_chado\Traits\ChadoStorageTestTrait;

use Drupal\tripal\TripalStorage\StoragePropertyValue;
use Drupal\tripal\TripalStorage\StoragePropertyTypeBase;
use Drupal\tripal_chado\TripalStorage\ChadoIntStoragePropertyType;
use Drupal\tripal_chado\TripalStorage\ChadoVarCharStoragePropertyType;

/**
 * Tests that specific ChadoStorage actions perform as expected.
 *
 * @group Tripal
 * @group Tripal Chado
 * @group ChadoStorage
 * @group ChadoStorage Actions
 */
class ChadoStorageActions_ReadValueTest extends ChadoTestKernelBase {

  use ChadoStorageTestTrait;

  // We will populate this variable at the start of each test
  // with fields specific to that test.
  protected $fields = [];

  protected $yaml_file = __DIR__ . "/ChadoStorageActions-FieldDefinitions.yml";

  /**
   * {@inheritdoc}
   */
  protected function setUp() :void {
    parent::setUp();

    // Ensure we see all logging in tests.
    \Drupal::state()->set('is_a_test_environment', TRUE);

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
  }

  /**
   * Test the read_value action.
   *
   * Chado Table: project
   *     Columns: project_id*, name*, description
   *
   * Specifically,
   *  - Ensure that a property with the read_value action has the value set
   *  - Ensure that a property with the read_value action can't change the value
   *  - That two fields accessing the same chado column do not conflict
   *      A. both read_value action for the same column
   *      B. one read_value and one store for the same column
   */
  public function testReadValueAction() {

    // Set the fields for this test and then re-populate the storage arrays.
    $this->setFieldsFromYaml($this->yaml_file, 'testReadValueAction');
    $this->cleanChadoStorageValues();

    // Test Case: Insert valid values when they do not yet exist in Chado.
    // ---------------------------------------------------------
    $insert_values = [
      'test_read' => [
        [
          'record_id' => NULL,
          // A read value should not set the value for insert
          // We are doing so here to make sure it cannot modify the
          // values of the table!
          'name_read' => 'Project Name Set By Read',
        ],
      ],
      'other_field_store' => [
        [
          'record_id' => NULL,
          'name_store' => 'Correct Project Name',
        ],
      ],
      'other_field_read' => [
        [
          'record_id' => NULL,
          'name_read_again' => NULL,
        ],
      ],
    ];
    $this->chadoStorageTestInsertValues($insert_values);

    // Check that there is now a single project record.
    $query = $this->chado_connection->select('1:project', 'p')
      ->fields('p', ['project_id', 'name'])
      ->execute();
    $projects = $query->fetchAll();
    $this->assertIsArray($projects,
    "We should have been able to select from the project table.");
    $this->assertCount(1, $projects,
      "There should only be a single project inserted by these 3 fields");

    // Check that the single project record has the name set by the `store` action.
    $expected_name = $insert_values['other_field_store'][0]['name_store'];
    $retrieved_name = $projects[0]->name;
    $this->assertEquals($expected_name, $retrieved_name,
      "We did not get the name that should have been set by the other_field_store:name_store property.");

    $project_id = $projects[0]->project_id;

    // Test Case: Load values existing in Chado.
    // ---------------------------------------------------------
    // First we want to reset all the chado storage arrays to ensure we are
    // doing a clean test. The values will purposefully remain in Chado but the
    // Property Types, Property Values and Data Values will be built from scratch.
    $this->cleanChadoStorageValues();

    // For loading only the store id/pkey/link items should be populated.
    $load_values = [
      'test_read' => [
        [
          'record_id' => $project_id,
          'name_read' => NULL,
        ],
      ],
      'other_field_store' => [
        [
          'record_id' => $project_id,
          'name_store' => NULL,
        ],
      ],
      'other_field_read' => [
        [
          'record_id' => $project_id,
          'name_read_again' => NULL,
        ],
      ],
    ];
    $retrieved_values = $this->chadoStorageTestLoadValues($load_values);

    // Check that the name in our fields have been loaded.
    $expected_name = $insert_values['other_field_store'][0]['name_store'];
    $retrieved = [
      'test_read' => $retrieved_values['test_read'][0]['name_read']['value']->getValue(),
      'other_field_store' => $retrieved_values['other_field_store'][0]['name_store']['value']->getValue(),
      'other_field_read' => $retrieved_values['other_field_read'][0]['name_read_again']['value']->getValue(),
    ];
    foreach ($retrieved as $field_name => $retrieved_name) {
      $this->assertEquals($expected_name, $retrieved_name,
        "The name we retrieved for the $field_name field did not match the one set with a store attribute during insert.");
    }

    // Test Case: Update values in Chado using ChadoStorage.
    // ---------------------------------------------------------
    // When updating we need all the store id/pkey/link records
    // and all values of the other properties.
    $update_values = [
      'test_read' => [
        [
          'record_id' => $project_id,
          'name_read' => $expected_name,
        ],
      ],
      'other_field_store' => [
        [
          'record_id' => $project_id,
          'name_store' => $expected_name,
        ],
      ],
      'other_field_read' => [
        [
          'record_id' => $project_id,
          'name_read_again' => $expected_name,
        ],
      ],
    ];

    // We then change the name for the store value.
    // Since the other are read they shouldn't be connected to a widget
    // and thus will remain the old value.
    $update_values['other_field_store'][0]['name_store'] = 'Updated Project Name';
    $this->chadoStorageTestUpdateValues($update_values);

    // Check that there is still only a single project record.
    $query = $this->chado_connection->select('1:project', 'p')
      ->fields('p', ['project_id', 'name'])
      ->execute();
    $projects = $query->fetchAll();
    $this->assertIsArray($projects,
    "We should have been able to select from the project table.");
    $this->assertCount(1, $projects,
      "There should only be a single project affected by these 3 fields");

    // Check that the single project record has the name set by the `store` action.
    $expected_name = $update_values['other_field_store'][0]['name_store'];
    $retrieved_name = $projects[0]->name;
    $this->assertEquals($expected_name, $retrieved_name,
      "The name was not updated to match the other_field_store:name_store property.");
  }


  /**
   * Test the read_value action works with table alias.
   *
   * Chado Table: project
   *     Columns: project_id*, name*, description
   *
   * Specifically, ensure that a property with the read_value action
   *  - Can be used with a table alias.
   * Focusing on load since this action type does not impact insert/update.
   */
  public function testReadValueActionTableAlias() {

    // Set the fields for this test and then re-populate the storage arrays.
    $this->setFieldsFromYaml($this->yaml_file, 'testReadValueActionTableAlias');
    $this->cleanChadoStorageValues();

    // Create to project records for testing the load with later.
    // Test Case: Insert valid values when they do not yet exist in Chado.
    $test_values = [
      'test_alias' => [
        'name' => 'Project name for the aliased record',
      ],
      'test_noalias' => [
        'name' => 'Base Project Name',
      ],
    ];
    foreach ($test_values as $field => $values) {
      $project_id = $this->chado_connection->insert('1:project')
        ->fields($values)
        ->execute();
      $this->assertIsNumeric($project_id,
        "We should have been able to insert test data for $field into the project table with the values: " . print_r($values, TRUE));
      $test_values[$field]['project_id'] = $project_id;
    }

    // For loading only the store id/pkey/link items should be populated.
    $load_values = [
      'test_alias' => [
        [
          'record_id' => $test_values['test_alias']['project_id'],
        ],
      ],
      'test_noalias' => [
        [
          'record_id' => $test_values['test_noalias']['project_id'],
        ],
      ],
    ];
    $retrieved_values = $this->chadoStorageTestLoadValues($load_values);

    // Check that the name in our fields have been loaded.
    foreach ($test_values as $field => $values) {
      $ret_name = $retrieved_values[$field][0]['name_read']['value']->getValue();
      $this->assertEquals($test_values[$field]['name'], $ret_name,
        "The name retrieved should match the one we inserted into chado for $field.");

      $ret_id = $retrieved_values[$field][0]['record_id']['value']->getValue();
      $this->assertEquals($test_values[$field]['project_id'], $ret_id,
        "The project_id retrieved should match the one we inserted into chado for $field.");
    }
  }

  /**
   * Test read_value through a join.
   *
   * Base Table: Stock
   *     Columns: stock_id
   * Chado Table: cvterm
   *     Columns: name
   *
   * Specically, testing that we can read the cvterm name for a stock record
   * through the stock > stock_cvterm > cvterm join path.
   *
   * Again focusing on load since this action type does not impact insert/update.
   */
  public function testReadValueActionJoin() {

    // Set the fields for this test and then re-populate the storage arrays.
    $this->setFieldsFromYaml($this->yaml_file, 'testReadValueActionJoin');
    $this->cleanChadoStorageValues();

    // Create the organism record needed for the stock.
    $organism_id = $this->chado_connection->insert('1:organism')
      ->fields([
        'genus' => 'Tripalus',
        'species' => 'databasica',
      ])
      ->execute();
    $this->assertIsNumeric($organism_id,
      'We should have been able to insert a organism for use with testing.');
    // Create the stock base record.
    $stock_id = $this->chado_connection->insert('1:stock')
      ->fields([
        'type_id' => $this->getCvtermId('rdfs', 'comment'),
        'organism_id' => $organism_id,
        'uniquename' => uniqid(),
      ])
      ->execute();
    $this->assertIsNumeric($stock_id,
      'We should have been able to insert a stock for use with testing.');
    // Create the pub record needed for the stock_cvterm.
    $pub_id = $this->chado_connection->insert('1:pub')
      ->fields([
        'uniquename' => uniqid(),
        'type_id' => $this->getCvtermId('TPUB', '0000172'),
      ])
      ->execute();
    $this->assertIsNumeric($pub_id,
      'We should have been able to insert a pub for use with testing.');
    // Now create 3 connections to cvterms for testing purposes.
    $test_values = [
      [
        'stock_id' => $stock_id,
        'cvterm_id' => $this->getCvtermId('SO', '0001778'),
        'pub_id' => $pub_id,
      ],
    ];
    foreach ($test_values as $delta => $values) {
      $pkey = $this->chado_connection->insert('1:stock_cvterm')
        ->fields($values)
        ->execute();
      $this->assertIsNumeric($pkey,
        "We should have been able to insert test data for $delta into the stock_cvterm table with the values: " . print_r($values, TRUE));
      $test_values[$delta]['stock_cvterm_id'] = $pkey;
    }

    // For loading only the store id/pkey/link items should be populated.
    $load_values = [
      'test_join' => [
        [
          'record_id' => $test_values[0]['stock_id'],
        ],
      ],
    ];
    $retrieved_values = $this->chadoStorageTestLoadValues($load_values);

    // Check that the name in our fields have been loaded.
    $expected = [
      'germline_variant',
    ];
    foreach ($test_values as $delta => $values) {
      $ret_term = $retrieved_values['test_join'][$delta]['cvterm_read']['value']->getValue();
      $this->assertEquals($expected[$delta], $ret_term,
        "The cvterm name retrieved should match the one we inserted into chado for $delta.");

      $ret_id = $retrieved_values['test_join'][$delta]['record_id']['value']->getValue();
      $this->assertEquals($test_values[$delta]['stock_id'], $ret_id,
        "The stock_id retrieved should match the one we inserted into chado for $delta.");
    }
  }

  /**
   * Test read_value through a join where two fields access the same tables.
   *
   * Base Table: arraydesign
   *     Columns: arraydesign_id*, manufacturer_id, platformtype_id*, substratetype_id*, name
   * Chado Table: dbxref
   *     Columns: accession
   *
   * Specically, testing that we can read the database reference for an arraydesign
   * record through the arraydesign > cvterm > dbxref path for two separate
   * fields without their being data swap between them.
   *
   * Again focusing on load since this action type does not impact insert/update.
   */
  public function testReadValueActionJoinDouble() {

    // Set the fields for this test and then re-populate the storage arrays.
    $this->setFieldsFromYaml($this->yaml_file, 'testReadValueActionJoinDouble');
    $this->cleanChadoStorageValues();

    // Create the organism record needed for the stock.
    $contact_id = $this->chado_connection->insert('1:contact')
      ->fields([
        'name' => uniqid(),
      ])
      ->execute();
    $this->assertIsNumeric($contact_id,
      'We should have been able to insert a contact for use with testing.');
    // Create the arraydesign record needed for testing the load.
    $platform_accession = 'comment';
    $substrate_accession = 'type';
    $arraydesign_expected = [
        'manufacturer_id' => $contact_id,
        'platformtype_id' => $this->getCvtermId('rdfs', $platform_accession),
        'substratetype_id' => $this->getCvtermId('rdfs', $substrate_accession),
        'name' => uniqid(),
    ];
    $arraydesign_id = $this->chado_connection->insert('1:arraydesign')
      ->fields($arraydesign_expected)
      ->execute();
    $this->assertIsNumeric($arraydesign_id,
      'We should have been able to insert a arraydesign record for use with testing.');

    // For loading only the store id/pkey/link items should be populated.
    $load_values = [
      'test_join1' => [
        [
          'record_id' => $arraydesign_id,
          'type_id' => $arraydesign_expected['platformtype_id'],
          'accession_read' => NULL,
        ],
      ],
      'test_join2' => [
        [
          'record_id' => $arraydesign_id,
          'type_id' => $arraydesign_expected['substratetype_id'],
          'accession_read' => NULL,
        ],
      ],
    ];
    $retrieved_values = $this->chadoStorageTestLoadValues($load_values);

    // Check that the accession in our fields have been loaded.
    $delta = 0;
    $ret = $retrieved_values['test_join1'][$delta]['accession_read']['value']->getValue();
    $this->assertEquals($platform_accession, $ret,
      "The dbxref accession retrieved for test_join1 should match the one we inserted into chado for platformtype_id.");
    $ret = $retrieved_values['test_join1'][$delta]['record_id']['value']->getValue();
    $this->assertEquals($arraydesign_id, $ret,
      "The arraydesign_id retrieved should match the one we inserted into chado.");
    $ret = $retrieved_values['test_join1'][$delta]['type_id']['value']->getValue();
    $this->assertEquals($arraydesign_expected['platformtype_id'], $ret,
      "The type_id retrieved should match the one we inserted into chado for platformtype_id.");

    $ret = $retrieved_values['test_join2'][$delta]['accession_read']['value']->getValue();
    $this->assertEquals($substrate_accession, $ret,
      "The dbxref accession retrieved for test_join2 should match the one we inserted into chado for substratetype_id.");
    $ret = $retrieved_values['test_join2'][$delta]['record_id']['value']->getValue();
    $this->assertEquals($arraydesign_id, $ret,
      "The arraydesign_id retrieved should match the one we inserted into chado.");
    $ret = $retrieved_values['test_join2'][$delta]['type_id']['value']->getValue();
    $this->assertEquals($arraydesign_expected['substratetype_id'], $ret,
      "The type_id retrieved should match the one we inserted into chado for substratetype_id.");
  }


  /**
   * Test read_value through a join where two fields access the same tables.
   *
   * Base Table: arraydesign
   *     Columns: arraydesign_id*, manufacturer_id, platformtype_id*, substratetype_id*, name
   * Chado Table: dbxref
   *     Columns: accession
   *
   * Specically, testing that we can read the database reference for an arraydesign
   * record through the arraydesign > cvterm > dbxref path for two separate
   * fields without their being data swap between them.
   *
   * Again focusing on load since this action type does not impact insert/update.
   */
  public function testReadValueActionJoinLoop() {

    // Set the fields for this test and then re-populate the storage arrays.
    $this->setFieldsFromYaml($this->yaml_file, 'testReadValueActionJoinLoop');
    $this->cleanChadoStorageValues();

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
    $organism_id = $query->execute();

    // Add Feature, Src Feature & Featureloc
    // Create the feature and featureloc record for use with the feature table.
    $gene_type_id = $this->getCvtermID('SO', '0000704');
    $query = $this->chado_connection->insert('1:feature');
    $query->fields([
      'name' => 'Gene1',
      'uniquename' => 'Gene1',
      'organism_id' => $organism_id,
      'type_id' => $gene_type_id,
    ]);
    $feature_id = $query->execute();

    $chr_type_id = $this->getCvtermID('SO', '0000340');
    $query = $this->chado_connection->insert('1:feature');
    $query->fields([
      'name' => 'Chr1',
      'uniquename' => 'Chr1',
      'organism_id' => $organism_id,
      'type_id' => $chr_type_id,
    ]);
    $src_feature_id = $query->execute();

    $query = $this->chado_connection->insert('1:featureloc');
    $query->fields([
      'feature_id' => $feature_id,
      'srcfeature_id' => $src_feature_id,
      'fmin' => 100,
      'fmax' => 200,
      'strand' => 1,
      'phase' => 0
    ]);
    $featureloc_id = $query->execute();


    // For loading only the store id/pkey/link items should be populated.
    $load_values = [
      'featurelocfield' => [
        [
          'record_id' => $feature_id,
          'featureloc_id' => $featureloc_id,
          'fkey' => $feature_id,
        ],
      ],
    ];
    $retrieved_values = $this->chadoStorageTestLoadValues($load_values);

    // Check that the name in our fields have been loaded.
    $ret_name = $retrieved_values['featurelocfield'][0]['uniquename']['value']->getValue();
    $this->assertEquals('Chr1', $ret_name,
        "The uniquename retrieved should match the one we inserted into chado.");
    $fmax = $retrieved_values['featurelocfield'][0]['fmax']['value']->getValue();
    $this->assertEquals(200, $fmax,
        "The fmax retrieved should match the one we inserted into chado.");
    $fmin = $retrieved_values['featurelocfield'][0]['fmin']['value']->getValue();
    $this->assertEquals(100, $fmin,
        "The fmin retrieved should match the one we inserted into chado.");
    $phase = $retrieved_values['featurelocfield'][0]['phase']['value']->getValue();
    $this->assertEquals(0, $phase,
        "The phase retrieved should match the one we inserted into chado.");
    $strand = $retrieved_values['featurelocfield'][0]['strand']['value']->getValue();
    $this->assertEquals(1, $strand,
        "The strand retrieved should match the one we inserted into chado.");

  }

  /**
   * Test read_value works when there is no store on the same column.
   *
   * Chado Table: project
   *     Columns: project_id*, name*, description
   *
   * Specifically testing that we can read the project name for an existing
   * project record when there is no store property for the project name.
   *
   * Again focusing on load since this action type does not impact insert/update.
   */
  public function testReadValueActionNoStore() {

    // Set the fields for this test and then re-populate the storage arrays.
    $this->setFieldsFromYaml($this->yaml_file, 'testReadValueActionNoStore');
    $this->cleanChadoStorageValues();

    $project_name = uniqid();
    $project_description = 'This is a random comment with a unique ending ' . uniqid();
    $project_id = $this->chado_connection->insert('1:project')
      ->fields([
        'name' => $project_name,
        'description' => $project_description,
      ])
      ->execute();
    $this->assertIsNumeric($project_id,
      'We should have been able to insert a project for use with testing.');

    // For loading only the store id/pkey/link items should be populated.
    $load_values = [
      'test_read' => [
        [
          'record_id' => $project_id,
        ],
      ],
    ];
    $retrieved_values = $this->chadoStorageTestLoadValues($load_values);

    // Check that the name in our fields have been loaded.
    $ret_name = $retrieved_values['test_read'][0]['name_read']['value']->getValue();
    $this->assertEquals($project_name, $ret_name,
      "The name retrieved should match the one we inserted into chado.");

    $ret_descrip = $retrieved_values['test_read'][0]['description_read']['value']->getValue();
    $this->assertEquals($project_description, $ret_descrip,
      "The description retrieved should match the one we inserted into chado.");

    $ret_id = $retrieved_values['test_read'][0]['record_id']['value']->getValue();
    $this->assertEquals($project_id, $ret_id,
      "The project_id retrieved should match the one we inserted into chado.");
  }
}
