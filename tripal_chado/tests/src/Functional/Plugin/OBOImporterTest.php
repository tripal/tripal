<?php

namespace Drupal\Tests\tripal_chado\Functional;

use Drupal\Tests\user\Traits\UserCreationTrait;
use Drupal\Tests\tripal_chado\Kernel\ChadoTestKernelBase;
use Symfony\Component\Yaml\Yaml;
use PHPUnit\Framework\Attributes\DataProvider;
use PHPUnit\Framework\Attributes\Group;

/**
 * Tests for the ChadoCVTerm classes
 *
 * @group TripalImporter
 * @group ChadoImporter
 * @group OntologyImporter
 */
#[Group('TripalImporter')]
#[Group('ChadoImporter')]
#[Group('OntologyImporter')]
class OBOImporterTest extends ChadoTestKernelBase {

  use UserCreationTrait;

  /**
   * {@inheritdoc}
   */
  protected static $modules = ['user', 'tripal', 'tripal_chado', 'tripal_biodb'];

  /**
   * The YAML file indicating the scenarios to test.
   *
   * @var string
   */
  protected string $yaml_info_file = __DIR__ . '/OBOImporter-TestInfo.yml';

  /**
   * The test drupal connection. It is also set in the container.
   *
   * @var object
   */
  protected object $drupal_connection;

  /**
   * The test chado connection. It is also set in the container.
   *
   * @var ChadoConnection
   */
  protected object $chado_connection;

  /**
   * The messages from the mocked tripal logger.
   *
   * @var string $mock_messages
   */
  protected array $mock_messages = [];

  /**
   * A user with permission to run an importer.
   *
   * @var \Drupal\user\UserInterface
   */
  protected $user;

  /**
   * Describes the environment to setup for this test.
   *
   * @var array
   *   An array with the following keys:
   *   - chado_version: the version of chado to test under.
   */
  protected array $system_under_test;

  /**
   * Describes the scenarios to test.
   *
   * This will be used in combination with the data provider. It can't be
   * accessed directly in the dataProvider due to the way that PHPUnit is
   * setup.
   *
   * @var array
   *  A list of scenarios where each one has the following keys:
   *  - label: A human-readable label for the scenario to be used in assert
   *    messages.
   *  - description: A description of the scenario and what you are wanting to
   *    test. This will not be used in the test but is rather there to help
   *    people reading the YAML file and to make it easier to maintain.
   *  - tripal_cv_obo: The values to insert into this table that define
   *    the name of the ontology and location of the obo file.
   *  - expect: An array of table.column expected values to confirm that
   *    they have been imported.
   */
  protected array $scenarios;

  /**
   * {@inheritdoc}
   */
  protected function setUp(): void {
    parent::setUp();

    // The Tripal importers need a user.
    $permissions = [
      'administer tripal',
      'allow tripal import',
    ];
    $this->user = $this->setUpCurrentUser($permissions);

    // Create a mocked logger so we can access error messages from the Tripal logger.
    $mock_logger = $this->getMockBuilder(\Drupal\tripal\Services\TripalLogger::class)
      ->onlyMethods(['warning'])
      ->getMock();
    $mock_logger->method('warning')
      ->willReturnCallback(function($message, $context, $options) {
          $this->mock_messages[] = str_replace(array_keys($context), $context, $message);
          return NULL;
        });
    $this->container->set('tripal.logger', $mock_logger);

    // The Drupal connection will be created in the parent. This is used
    // when checking Drupal tables.
    $this->drupal_connection = $this->container->get('database');

    // First retrieve info from the YAML file for this particular test.
    $yaml_data = Yaml::parse(file_get_contents($this->yaml_info_file));
    $this->system_under_test = $yaml_data['system-under-test'];
    $this->scenarios = $yaml_data['scenarios'];

    // Create the test Chado installation we will be using.
    if (!array_key_exists('chado_version', $this->system_under_test)) {
      $this->system_under_test['chado_version'] = '1.3';
    }
    $this->chado_connection = $this->getTestSchema(
      ChadoTestKernelBase::PREPARE_TEST_CHADO,
      $this->system_under_test['chado_version']
    );

    // Install tables needed in the public schema.
    $this->installSchema('tripal', [
      'tripal_id_space_collection',
      'tripal_import',
      'tripal_vocabulary_collection',
    ]);
    $this->installSchema('tripal_chado', [
      'tripal_custom_tables',
      'tripal_cv_obo',
      'tripal_mviews',
    ]);
//    $this->populateMviewSql(); //@todo
  }

  /**
   * Data Provider: works with the YAML to provide scenarios for testing.
   *
   * @return array
   *   List of scenarios to test where each one matches a key and label in the
   *   associated YAML scenarios.
   */
  public static function provideScenarios() {
    $scenarios = [];

    $scenarios[] = [
      0,
      'TO subset vocabulary',
    ];

    $scenarios[] = [
      1,
      'EDAM subset vocabulary',
    ];

    return $scenarios;
  }

  /**
   * Retrieves the current scenario based on the data provider.
   *
   * NOTE: Also ensures the type_ids match what is currently in the database.
   *
   * @param int $current_scenario_key
   *   The key of the scenario in the YAML.
   * @param string $current_scenario_label
   *   The label of the scenario in the YAML.
   *
   * @return array
   *   The scenario to be tested as defined in the YAML.
   */
  public function retrieveCurrentScenario(int $current_scenario_key, string $current_scenario_label) {

    // Retrieve the correct scenario.
    $current_scenario = $this->scenarios[$current_scenario_key];
    $this->assertEquals($current_scenario_label, $current_scenario['label'], 'We may not have retrieved the expected scenario');

    return $current_scenario;
  }

  /**
   * Sets up the two materialized views needed for the importer.
   *
   * Rather than load the whole test SQL, we can just define these
   * two records to minimize resources needed for the test.
   * This is extracted from
   * tripal_chado/tests/fixtures/fill_public_test_prepare.sql.
   */
  protected function populateMviewSql() {
//@todo this does not work
    $sql = "INSERT INTO [tripal_mviews] VALUES (6, 10, 'db2cv_mview', '
      SELECT DISTINCT CV.cv_id, CV.name as cvname, DB.db_id, DB.name as dbname,
        COUNT(CVT.cvterm_id) as num_terms
      FROM cv CV
        INNER JOIN cvterm CVT on CVT.cv_id = CV.cv_id
        INNER JOIN dbxref DBX on DBX.dbxref_id = CVT.dbxref_id
        INNER JOIN db DB on DB.db_id = DBX.db_id
      WHERE CVT.is_relationshiptype = 0 and CVT.is_obsolete = 0
      GROUP BY CV.cv_id, CV.name, DB.db_id, DB.name
      ORDER BY DB.name
    ', 1667003601, 'Populated with 41 rows', 'A table for quick lookup of the vocabularies and the databases they are associated with.')";
    $this->drupal_connection->query($sql)->execute();
    $sql = "INSERT INTO [tripal_mviews] VALUES (5, 9, 'cv_root_mview', '
      SELECT DISTINCT CVT.name, CVT.cvterm_id, CV.cv_id, CV.name
      FROM cvterm CVT
        LEFT JOIN cvterm_relationship CVTR ON CVT.cvterm_id = CVTR.subject_id
        INNER JOIN cvterm_relationship CVTR2 ON CVT.cvterm_id = CVTR2.object_id
      INNER JOIN cv CV on CV.cv_id = CVT.cv_id
      WHERE CVTR.subject_id is NULL and
        CVT.is_relationshiptype = 0 and CVT.is_obsolete = 0
    ', 1667003601, 'Populated with 9 rows', 'A list of the root terms for all controlled vocabularies. This is needed for viewing CV trees')";
    $this->drupal_connection->query($sql)->execute();
  }

  /**
   * Tests the OBO ontology importer.
   *
   * @param int $current_scenario_key
   *   The key of the scenario in the YAML.
   * @param string $current_scenario_label
   *   The label of the scenario in the YAML.
   *
   * @dataProvider provideScenarios
   */
  #[DataProvider('provideScenarios')]
  public function testOBOImporter(int $current_scenario_key, string $current_scenario_label) {

    $current_scenario = $this->retrieveCurrentScenario($current_scenario_key, $current_scenario_label);

    $this->mock_messages = [];

    // Insert a record into the tripal_cv_obo table describing
    // the vocabulary to load.
    $record = $current_scenario['tripal_cv_obo'];
    $insert = $this->drupal_connection->insert('tripal_cv_obo');
    $insert->fields($record);
    $obo_id = (int) $insert->execute();
    $this->assertIsInt($obo_id, 'The ID of the inserted ontology ' . $record['name'] . ' is not an integer');
    $this->assertGreaterThan(0, $obo_id, 'The ID of the inserted ontology ' . $record['name'] . ' is not a positive integer');

    // Create an instance of the OBO importer.
    $importer_manager = \Drupal::service('tripal.importer');
    $this->assertIsObject($importer_manager, 'Importer manager not created');
    /* @var \Drupal\tripal_chado\Plugin\TripalImporter\OBOImporter $obo_importer */
    $obo_importer = $importer_manager->createInstance('chado_obo_loader');
    $this->assertIsObject($obo_importer, 'OBO Importer instance not created');

    // Create the import job.
    $obo_importer->createImportJob([
      'obo_id' => $obo_id,
      'schema_name' => $this->chado_connection->getSchemaName()
    ]);

    // Test that expected counts before import are one less.
    foreach ($current_scenario['expect'] as $expect) {
      $expected_count = ($expect['count'] ?? 1) - 1;
      $query = $this->chado_connection->select('1:' . $expect['table'], 'T');
      $query->condition('T.' . $expect['column'], $expect['value'], '=');
      $query->fields('T', [$expect['column']]);
      $count = $query->countQuery()->execute()->fetchField();
      $this->assertEquals($expected_count, $count, 'YAML error, count is wrong before import for query ' . $expect['table'] . '.' . $expect['column'] . ' = ' . $expect['value']);
    }

    // Run the importer.
    $obo_importer->run();

    // Test that any expected warning was generated.
    if (array_key_exists('expect_message', $current_scenario)) {
      $this->assertNotEmpty($this->mock_messages, 'No messages were generated but one was expected');
      $this->assertStringContainsString($current_scenario['expect_message'], $this->mock_messages[0], 'Message does not contain the expected text');
    }
    else {
      $this->assertCount(0, $this->mock_messages, 'A message was generated but none was expected: ' . implode('; ', $this->mock_messages));
    }

    // Postrun populates some materialized views.
    // @todo Doesn't run currently because the mviews don't exist
    // in the test environment.
    // $obo_importer->postRun();

    // Test that expected database records have been created.
    foreach ($current_scenario['expect'] as $expect) {
      $expected_count = $expect['count'] ?? 1;
      $query = $this->chado_connection->select('1:' . $expect['table'], 'T');
      $query->condition('T.' . $expect['column'], $expect['value'], '=');
      $query->fields('T', [$expect['column']]);
      $count = $query->countQuery()->execute()->fetchField();
      $this->assertEquals($expected_count, $count, 'Did not create a ' . $expect['table'] . '.' . $expect['column'] . ' record with the expected value');
    }
  }

}
