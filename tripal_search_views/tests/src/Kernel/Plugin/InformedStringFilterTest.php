<?php

namespace Drupal\Tests\tripal_search_views\Kernel\Plugin;

use Drupal\Core\Form\FormState;
use Drupal\Tests\tripal_chado\Kernel\ChadoTestKernelBase;
use PHPUnit\Framework\Attributes\RunTestsInSeparateProcesses;
use Drupal\views\Views;
use Drupal\Tests\user\Traits\UserCreationTrait;
use Drupal\views\Tests\ViewResultAssertionTrait;
use Drupal\tripal\Entity\TripalEntityType;
use Drupal\tripal_chado\Database\ChadoConnection;

/**
 * Tests the InformedStringFilter plugin for Views.
 */
#[RunTestsInSeparateProcesses]
class InformedStringFilterTest extends ChadoTestKernelBase {

  use UserCreationTrait;
  use ViewResultAssertionTrait;

  /**
   * {@inheritdoc}
   */
  protected static $modules = [
    'system',
    'user',
    'path',
    'path_alias',
    'tripal',
    'tripal_chado',
    'views',
    'filter',
    'field',
    'tripal_search_views',
    'tripal_search_views_test_views',
  ];

  /**
   * The database connection to the test chado.
   *
   * @var Drupal\tripal_chado\Database\ChadoConnection
   */
  protected ChadoConnection $chado_connection;

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
    $this->installConfig([
      'tripal',
      'tripal_chado',
      'tripal_search_views',
      'tripal_search_views_test_views',
    ]);

    \Drupal::service('views.views_data')->clear();

    // Open connection to a test Chado.
    $this->chado_connection = $this->getTestSchema(ChadoTestKernelBase::PREPARE_TEST_CHADO);

    // Create some test organisms.
    $this->organisms = [
      1 => [
        'genus' => 'Tripalus',
        'species' => 'bogusii',
        'type_id' => NULL,
      ],
      2 => [
        'genus' => 'Tripalus',
        'species' => 'databasica',
        'type_id' => NULL,
      ],
      3 => [
        'genus' => 'Tripalus',
        'species' => 'fictus',
        'type_id' => NULL,
      ],
    ];

    foreach ($this->organisms as $organism) {
      $insert = $this->chado_connection->insert('1:organism');
      $insert->fields([
        'genus' => $organism['genus'],
        'species' => $organism['species'],
        'type_id' => $organism['type_id'] ?? NULL,
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

    $organism_bundle = TripalEntityType::load('organism');
    $organism_bundle->save();
  }

  /**
   * Tests that the exposed form for the dynamic filter is built as expected.
   */
  public function testBuildExposedForm() {
    // Create a view with informed string filter on the organism_species field.
    $view = Views::getView('test_search_view');
    $this->assertNotNull($view, 'The view is available.');
    $view->initHandlers();
    // Ensure the filter is present and is the expected type.
    $this->assertNotNull($view->filter, 'The view has no filters defined.');
    $this->assertArrayHasKey('organism_species_value', $view->filter, 'The filter for organism_species_value is not present in the view.');

    $form = [];
    $form_state = new FormState();
    $filter = $view->filter['organism_species_value'];

    // Build the exposed form.
    $filter->buildExposedForm($form, $form_state);

    // Check that the form has the expected select element and expected options.
    $this->assertArrayHasKey('organism_species_value', $form, 'The exposed form does not have the filter for organism_species_value.');
    $this->assertEquals('select', $form['organism_species_value']['#type'], 'The exposed filter is not a select element.');
    $this->assertEquals('', $form['organism_species_value']['#empty_value'], 'The exposed filter does not have the expected empty value.');
    $this->assertEquals('- Any -', $form['organism_species_value']['#empty_option'], 'The exposed filter does not have the expected empty option.');
    $expected_options = [
      'bogusii' => 'bogusii',
      'databasica' => 'databasica',
      'fictus' => 'fictus',
    ];
    $this->assertEquals($expected_options, $form['organism_species_value']['#options'], 'The exposed filter does not have the expected options.');
  }

}
