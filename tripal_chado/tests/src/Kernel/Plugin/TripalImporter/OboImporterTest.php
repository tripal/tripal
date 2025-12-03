<?php

namespace Drupal\Tests\tripal_chado\Kernel\Plugin\TripalImporter;

use Drupal\Tests\user\Traits\UserCreationTrait;
use Drupal\Tests\tripal_chado\Kernel\ChadoTestKernelBase;
use Drupal\tripal\Services\TripalLogger;
use Symfony\Component\Yaml\Yaml;
use PHPUnit\Framework\Attributes\DataProvider;
use PHPUnit\Framework\Attributes\Group;
use PHPUnit\Framework\Attributes\RunTestsInSeparateProcesses;

/**
 * Tests for the OBO ontology importer.
 *
 * @group TripalImporter
 * @group ChadoImporter
 * @group OntologyImporter
 */
#[Group('tripal-importer')]
#[Group('chado-importer')]
#[group('importer-obo')]
#[Group('bio-cv')]
#[RunTestsInSeparateProcesses]
class OboImporterTest extends ChadoTestKernelBase {

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
  protected string $yaml_info_file = __DIR__ . '/OboImporter-TestInfo.yml';

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
   * Messages from the mocked tripal logger.
   *
   * @var array
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
      'allow tripal import',
    ];
    $this->user = $this->setUpCurrentUser($permissions);

    // Create a mocked logger to access error messages from the Tripal logger.
    $mock_logger = $this->getMockBuilder(TripalLogger::class)
      ->onlyMethods(['warning'])
      ->getMock();
    $mock_logger->method('warning')
      ->willReturnCallback(function ($message, $context, $options) {
        $this->mock_messages[] = str_replace(array_keys($context), $context, $message);
        return NULL;
      });
    $this->container->set('tripal.logger', $mock_logger);

    // The Drupal connection will be created in the parent.
    $this->drupal_connection = $this->container->get('database');

    // First retrieve info from the YAML file for each test.
    $yaml_data = Yaml::parse(file_get_contents($this->yaml_info_file));
    $this->system_under_test = $yaml_data['system-under-test'];
    $this->scenarios = $yaml_data['scenarios'];

    // Create the test Chado installation we will be using.
    $this->system_under_test['chado_version'] ??= '1.3';
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

    // Inserts records with the SQL to generate needed materialized views.
    $this->populateMviewSql();

    // Create a conflicting dbxref and cv term to test issue #2382 problem.
    $buddy_service = \Drupal::service('tripal_chado.chado_buddy');
    $cvterm_buddy = $buddy_service->createInstance('chado_cvterm_buddy', []);
    $term_values = [
      'db.name' => 'data',
      'cv.name' => 'EDAM',
      'dbxref.accession' => 'has_format',
      'cvterm.name' => 'has format',
      'cvterm.definition' => 'Tests a conflicting existing dbxref for a cvterm.',
    ];
    $cvterm_buddy->insertCvterm($term_values, []);
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
   * The mviews are populated when the importer postRun() is called.
   */
  protected function populateMviewSql() {
    $records = [
      0 => [
        'mview_id' => 1,
        'table_id' => 1,
        'name' => 'cv_root_mview',
        'query' => 'SELECT DISTINCT CVT.name, CVT.cvterm_id, CV.cv_id, CV.name FROM cvterm CVT
  LEFT JOIN cvterm_relationship CVTR ON CVT.cvterm_id = CVTR.subject_id
  INNER JOIN cvterm_relationship CVTR2 ON CVT.cvterm_id = CVTR2.object_id
  INNER JOIN cv CV on CV.cv_id = CVT.cv_id
  WHERE CVTR.subject_id is NULL and CVT.is_relationshiptype = 0 and CVT.is_obsolete = 0',
        'last_update' => 1234567890,
        'status' => 'test',
        'comment' => 'test',
      ],
      1 => [
        'mview_id' => 2,
        'table_id' => 2,
        'name' => 'db2cv_mview',
        'query' => 'SELECT DISTINCT CV.cv_id, CV.name as cvname, DB.db_id, DB.name as dbname, COUNT(CVT.cvterm_id) as num_terms FROM cv CV
  INNER JOIN cvterm CVT on CVT.cv_id = CV.cv_id
  INNER JOIN dbxref DBX on DBX.dbxref_id = CVT.dbxref_id
  INNER JOIN db DB on DB.db_id = DBX.db_id
  WHERE CVT.is_relationshiptype = 0 and CVT.is_obsolete = 0
  GROUP BY CV.cv_id, CV.name, DB.db_id, DB.name ORDER BY DB.name',
        'last_update' => 1234567890,
        'status' => 'test',
        'comment' => 'test',
      ],
    ];

    foreach ($records as $record) {
      $query = $this->drupal_connection->insert('tripal_mviews');
      $query->fields($record);
      $mview_id = $query->execute();
      $this->assertEquals($record['mview_id'], $mview_id, 'Prepared MView record not added.');
    }
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
  public function testOboImporter(int $current_scenario_key, string $current_scenario_label) {

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
    /** @var \Drupal\tripal_chado\Plugin\TripalImporter\OBOImporter $obo_importer */
    $obo_importer = $importer_manager->createInstance('chado_obo_loader');
    $this->assertIsObject($obo_importer, 'OBO Importer instance not created');

    // Create the import job.
    $obo_importer->createImportJob([
      'obo_id' => $obo_id,
      'schema_name' => $this->chado_connection->getSchemaName(),
    ]);

    // Test that expected counts before import are one less.
    foreach ($current_scenario['expect'] as $expect) {
      $expected_count = $expect['precount'] ?? 0;
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

    // Test that expected database records have been created.
    foreach ($current_scenario['expect'] as $expect) {
      $expected_count = $expect['count'] ?? 1;
      $query = $this->chado_connection->select('1:' . $expect['table'], 'T');
      $query->condition('T.' . $expect['column'], $expect['value'], '=');
      $query->fields('T', [$expect['column']]);
      $count = $query->countQuery()->execute()->fetchField();
      $this->assertEquals($expected_count, $count, 'Did not create a ' . $expect['table'] . '.' . $expect['column'] . ' record with the expected value');
    }

    $db_query = $this->chado_connection->select('1:db2cv_mview', 't');
    $db_query->fields('t', ['cv_id', 'cvname', 'db_id', 'dbname', 'num_terms']);
    $db_initial_count = $db_query->countQuery()->execute()->fetchField();
    $cv_query = $this->chado_connection->select('1:cv_root_mview', 't');
    $cv_query->fields('t', ['name', 'cvterm_id', 'cv_id', 'name']);
    $cv_initial_count = $cv_query->countQuery()->execute()->fetchField();

    // Postrun of the obo importer populates materialized views.
    $obo_importer->postRun();

    // Test that the materialized views have been populated.
    $db_num_added = $db_query->countQuery()->execute()->fetchField() - $db_initial_count;
    $this->assertEquals($current_scenario['expect_db2cv_count'], $db_num_added, 'Expected number of records in db2cv_mview not found');

    $cv_num_added = $cv_query->countQuery()->execute()->fetchField() - $cv_initial_count;
    $this->assertEquals($current_scenario['expect_cv_root_count'], $cv_num_added, 'Expected number of records in cv_root_mview not found');

  }

}
