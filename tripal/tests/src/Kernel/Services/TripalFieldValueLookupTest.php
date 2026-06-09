<?php

namespace Drupal\Tests\tripal\Kernel\Services;

use Drupal\Core\Database\Connection;
use Drupal\tripal_chado\Database\ChadoConnection;
use Drupal\Tests\tripal_chado\Kernel\ChadoTestKernelBase;
use PHPUnit\Framework\Attributes\DataProvider;
use PHPUnit\Framework\Attributes\RunTestsInSeparateProcesses;

/**
 * Tests the TripalFieldValueLookup service.
 */
#[RunTestsInSeparateProcesses]
class TripalFieldValueLookupTest extends ChadoTestKernelBase {

  /**
   * Modules to enable.
   *
   * @var array
   */
  protected static $modules = ['system', 'user', 'path', 'path_alias', 'tripal', 'tripal_chado', 'views', 'field'];

  /**
   * The database connection to the test chado.
   *
   * @var Drupal\tripal_chado\Database\ChadoConnection
   */
  protected ChadoConnection $chado_connection;

  /**
   * The drupal database connection.
   *
   * @var Drupal\Core\Database\Connection
   */
  protected Connection $drupal_connection;

  /**
   * An array of test organisms created.
   *
   * @var array
   */
  protected array $organisms;

  /**
   * {@inheritdoc}
   */
  protected function setUp(): void {
    parent::setUp();

    // Ensure we see all logging in tests.
    \Drupal::state()->set('is_a_test_environment', TRUE);

    // Ensure we install the schema/modules we need.
    $this->prepareEnvironment(['TripalTerm', 'TripalEntity']);
    // -- additionally we need tripal_chado config to access the yaml files.
    $this->installConfig('tripal_chado');

    // Get connection to drupal database in place.
    $this->drupal_connection = \Drupal::service('database');

    // Open connection to a test Chado.
    $this->chado_connection = $this->getTestSchema(ChadoTestKernelBase::PREPARE_TEST_CHADO);

    // Create some test organisms.
    $this->organisms = [
      1 => [
        'genus' => 'Tripalus',
        'species' => 'bogusii',
        'type_id' => NULL,
        'infraspecific_name' => 'fakus',
        'abbreviation' =>
        'T. bogusii subsp. fakus',
      ],
      2 => [
        'genus' => 'Tripalus',
        'species' => 'databasica',
        'type_id' => NULL,
        'infraspecific_name' => NULL,
        'abbreviation' => NULL,
      ],
      3 => [
        'genus' => 'Drupalus',
        'species' => 'fictus',
        'type_id' => NULL,
        'infraspecific_name' => NULL,
        'abbreviation' => 'Drupalus fictus',
      ],
    ];

    foreach ($this->organisms as $details) {
      $insert = $this->chado_connection->insert('1:organism');
      $insert->fields([
        'genus' => $details['genus'],
        'species' => $details['species'],
        'type_id' => $details['type_id'] ?? NULL,
        'infraspecific_name' => $details['infraspecific_name'] ?? NULL,
        'abbreviation' => $details['abbreviation'] ?? NULL,
        'common_name' => $details['common_name'] ?? NULL,
        'comment' => $details['comment'] ?? NULL,
      ]);
      $insert->execute();
    }

    // Create the terms for the field property storage types.
    $idsmanager = \Drupal::service('tripal.collection_plugin_manager.idspace');
    foreach (
      [
        'OBI',
        'local',
        'TAXRANK',
        'NCBITaxon',
        'SIO',
        'schema',
        'data',
        'NCIT',
        'operation',
        'OBCS', 'SWO',
        'IAO',
        'TPUB',
        'rdfs',
      ] as $termIdSpace) {
      $idsmanager->createCollection($termIdSpace, "chado_id_space");
    }
    $vmanager = \Drupal::service('tripal.collection_plugin_manager.vocabulary');
    foreach (
      [
        'obi',
        'local',
        'taxonomic_rank',
        'ncbitaxon',
        'SIO',
        'schema',
        'EDAM',
        'ncit',
        'OBCS',
        'swo',
        'IAO',
        'tripal_pub',
      ] as $termVocab) {
      $vmanager->createCollection($termVocab, "chado_vocabulary");
    }

    // Create terms for organism_dbxref since it seems to be missing.
    $term_details = [
      'vocab_name' => 'sbo',
      'id_space_name' => 'SBO',
      'term' => [
        'name' => 'reference annotation',
        'definition' => 'Additional information that supplements existing data, usually in a document, by providing a link to more detailed information, which is held externally, or elsewhere.',
        'accession' => '0000552',
      ],
    ];
    $this->createTripalTerm($term_details, 'chado_id_space', 'chado_vocabulary');

    // Create the content types + fields that we need.
    $this->createContentTypeFromConfig('general_chado', 'organism', TRUE);

    $publish_service = \Drupal::service('tripal.backend_publish');
    $chado_publish = $publish_service->createInstance('chado_storage', []);
    $publish_options = ['bundle' => 'organism', 'datastore' => 'chado_storage', 'schema_name' => $this->testSchemaName];
    $chado_publish->publish($publish_options);
  }

  /**
   * Provides data for testing the getUniqueFieldValues method.
   *
   * @return array
   *   An array of test cases, each containing:
   *   - scenario: A description of the test scenario.
   *   - field_name: The name of the field to look up.
   *   - filters: Optional filters restricting the values returned.
   *     Supported keys include:
   *     - remove_null (bool; default TRUE) ensures that NULL is not included
   *       in the result set.
   *     - remove_empty (bool; default TRUE) ensures that empty string
   *       is also removed from the result set.
   *     - bundles (array; default []) ensures only values for that
   *       field within the bundles specified are included. By default
   *       values from all bundles are included.
   *   - options: Additional options where supported keys include:
   *     - validate_field (bool; default TRUE) confirms that the field name
   *       passed in is valid, the field exists for the entity type.
   *     - refresh_cache (bool; default FALSE) allows you to indicate whether
   *       you want to use the cache (FALSE) or want to generate the values
   *       from a fresh query (TRUE).
   *   - expected_values: The expected values that is expected to be
   *     returned from the method.
   */
  public static function provideDataForGetUniqueFieldValues() {
    return [
      [
        'scenario' => 'no filters',
        'field_name' => 'organism_species',
        'real_field' => 'organism_species_value',
        'filters' => [],
        'options' => [],
        'expected_values' => [
          ['organism_species_value' => 'bogusii'],
          ['organism_species_value' => 'databasica'],
          ['organism_species_value' => 'fictus'],
        ],
      ],
      [
        'scenario' => 'with bundles filter',
        'field_name' => 'organism_species',
        'real_field' => 'organism_species_value',
        'filters' => ['bundles' => ['organism']],
        'options' => [],
        'expected_values' => [
          ['organism_species_value' => 'bogusii'],
          ['organism_species_value' => 'databasica'],
          ['organism_species_value' => 'fictus'],
        ],
      ],
      [
        'scenario' => 'with non-matching bundles filter',
        'field_name' => 'organism_species',
        'real_field' => 'organism_species_value',
        'filters' => ['bundles' => ['analysis']],
        'options' => [],
        'expected_values' => [],
      ],
      [
        'scenario' => 'with refresh cache option',
        'field_name' => 'organism_species',
        'real_field' => 'organism_species_value',
        'filters' => [],
        'options' => ['refresh_cache' => TRUE],
        'expected_values' => [
          ['organism_species_value' => 'bogusii'],
          ['organism_species_value' => 'databasica'],
          ['organism_species_value' => 'fictus'],
        ],
      ],
    ];
  }

  /**
   * Tests the getUniqueFieldValues method.
   *
   * @param string $scenario
   *   A description of the test scenario.
   * @param string $field_name
   *   The name of the field to look up.
   * @param string $real_field
   *   The property name of the field value to return.
   * @param array $filters
   *   Optional filters restricting the values returned.
   *   Supported keys include:
   *   - remove_null (bool; default TRUE) ensures that NULL is not included
   *     in the result set.
   *   - remove_empty (bool; default TRUE) ensures that empty string
   *     is also removed from the result set.
   *   - bundles (array; default []) ensures only values for that
   *     field within the bundles specified are included. By default
   *     values from all bundles are included.
   * @param array $options
   *   Additional options where supported keys include:
   *   - validate_field (bool; default TRUE) confirms that the field name
   *     passed in is valid, the field exists for the entity type.
   *   - refresh_cache (bool; default FALSE) allows you to indicate whether
   *     you want to use the cache (FALSE) or want to generate the values
   *     from a fresh query (TRUE).
   * @param array $expected_values
   *   The expected values that is expected to be returned from the method.
   *
   * @dataProvider provideDataForGetUniqueFieldValues
   */
  #[DataProvider('provideDataForGetUniqueFieldValues')]
  public function testGetUniqueFieldValues(string $scenario, string $field_name, string $real_field, array $filters, array $options, array $expected_values) {
    // Get the service and call the method.
    $lookup = \Drupal::service('tripal.fieldvalue.lookup');
    $bundles = $filters['bundles'] ?? [];

    // Set the entity type ID.
    $lookup->setEntityTypeId('tripal_entity');

    // Check that the entity type ID is set as expected.
    $entity_type = $lookup->getEntityTypeId();
    $this->assertEquals('tripal_entity', $entity_type, 'The service has the expected entity type ID set.');
    $values = $lookup->getUniqueFieldValues($field_name, $real_field, ['bundles' => $bundles], []);

    // Check that we got the expected values.
    foreach ($expected_values as $expected) {
      $this->assertTrue(in_array($expected, $values), "The expected value" . $expected[$field_name . '_value'] . " was not found in the results in the case of " . $scenario . ".");
    }
  }

  /**
   * Tests that an exception is thrown when an invalid field name is provided.
   */
  public function testInvalidFieldName() {
    $lookup = \Drupal::service('tripal.fieldvalue.lookup');
    $lookup->setEntityTypeId('tripal_entity');

    try {
      $lookup->getUniqueFieldValues('invalid_field_name', 'invalid_field_name_value', [], ['validate_field' => TRUE]);
    }
    catch (\InvalidArgumentException $e) {
      $this->assertStringContainsString("Invalid field name: invalid_field_name", $e->getMessage(), 'The expected exception message was not found.');
    }
  }

}
