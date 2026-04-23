<?php

namespace Drupal\Tests\tripal\Kernel\Services;

use Drupal\Tests\tripal_chado\Kernel\ChadoTestKernelBase;
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
   * @var \Drupal\Core\Database\Connection
   */
  protected $connection;

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
    $this->public = \Drupal::service('database');

    // Open connection to a test Chado.
    $this->connection = $this->getTestSchema(ChadoTestKernelBase::PREPARE_TEST_CHADO);

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
      $insert = $this->connection->insert('1:organism');
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
   * Tests the getUniqueFieldValues method.
   */
  public function testGetUniqueFieldValues() {
    // Get the service and call the method.
    $lookup = \Drupal::service('tripal.fieldvalue.lookup');
    $field_name = 'organism_species';
    $values = $lookup->getUniqueFieldValues($field_name, [], []);

    // Check that we got the expected values.
    $expected_vals = [
      [$field_name . '_value' => 'bogusii'],
      [$field_name . '_value' => 'databasica'],
      [$field_name . '_value' => 'fictus'],
    ];

    foreach ($expected_vals as $expected) {
      $this->assertTrue(in_array($expected, $values), "The expected value" . $expected[$field_name . '_value'] . " was not found in the results.");
    }
  }

}
