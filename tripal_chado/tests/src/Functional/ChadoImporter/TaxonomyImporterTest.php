<?php

namespace Drupal\Tests\tripal_chado\Functional\ChadoImporter;

use Drupal\Tests\tripal_chado\Functional\ChadoTestBrowserBase;
use Drupal\tripal\Services\TripalLogger;
use PHPUnit\Framework\Attributes\Group;
use PHPUnit\Framework\Attributes\RunTestsInSeparateProcesses;

/**
 * Tests for the TaxonomyImporter class.
 *
 * @group TripalImporter
 * @group ChadoImporter
 * @group TaxonomyImporter
 */
#[Group('tripal-importer')]
#[Group('chado-importer')]
#[group('importer-taxonomy')]
#[Group('bio-phylogeny')]
#[RunTestsInSeparateProcesses]
class TaxonomyImporterTest extends ChadoTestBrowserBase {

  /**
   * @var string
   *   The most recent error message from the mocked tripal logger
   */
  protected string $mock_error = '';

  /**
   * Messages encountered when NCBI is down or broken.
   *
   * @var array
   *   A list of error messages encountered when the external
   *   web site is down or not functioning normally are tagged
   *   with 'skip', messages normally encountered are tagged
   *   with 'normal'.
   */
  protected array $message_actions = [
    '/Error contacting NCBI/' => 'skip',
    '/Invalid XML returned/' => 'skip',
    '/500 Internal Server Error/' => 'skip',
    '/Empty taxon returned/' => 'skip',
  ];

  /**
   *
   */
  protected function setUp() :void {
    parent::setUp();

    // Grab the container.
    $container = \Drupal::getContainer();

    // Create a mocked logger so we can access error messages from the Tripal logger.
    $mock_logger = $this->getMockBuilder(TripalLogger::class)
      ->onlyMethods(['error'])
      ->getMock();
    $mock_logger->method('error')
      ->willReturnCallback(function ($message, $context, $options) {
          $this->mock_error .= str_replace(array_keys($context), $context, $message);
          return NULL;
      });
    $container->set('tripal.logger', $mock_logger);
  }

  /**
   * Confirm basic Taxonomy importer functionality.
   */
  public function testTaxonomyImporterSimpleTest() {

    // Installs up the chado with the test chado data.
    $chado = $this->getTestSchema(ChadoTestBrowserBase::PREPARE_TEST_CHADO);

    // Keep track of the schema name in case we need it.
    $schema_name = $chado->getSchemaName();

    // Test to ensure cvterms are found in the cvterms table.
    $cvterms_count_query = $chado->query("SELECT count(*) as c1 FROM {1:cvterm}");
    $cvterms_count_object = $cvterms_count_query->fetchObject();
    $this->assertNotEquals($cvterms_count_object->c1, 0);

    // Create an organism without any properties and without an NCBI
    // taxon id, to be used for the import_existing test. NCBI taxid=38785.
    $results = $chado->query("INSERT INTO {1:organism} (genus, species) VALUES ('Arabidopsis', 'arenosa')");

    // Create a Taxonomy Importer
    // Perform the Taxonomy Importer test by creating an instance of the Taxonomy loader.
    $importer_manager = \Drupal::service('tripal.importer');
    $taxonomy_importer = $importer_manager->createInstance('chado_taxonomy_loader');
    $run_args = [
      'schema_name' => $schema_name,
      'taxonomy_ids' => '3702',
      'use_transaction' => 1,
      'import_existing' => 0,
      'ncbi_api_key' => NULL,
    ];

    $file_details = [];

    $this->mock_error = '';
    $taxonomy_importer->createImportJob($run_args, $file_details);
    $taxonomy_importer->prepareFiles();
    $taxonomy_importer->run();
    $taxonomy_importer->postRun();
    $this->checkForNetworkError();

    // Check if Arabidopsis thaliana retrieved by tax_id 3702 from NCBI and organism created.
    $results = $chado->query("SELECT count(*) as c1 FROM {1:organism}
        WHERE genus = 'Arabidopsis' AND species = 'thaliana';");
    $results_object = $results->fetchObject();
    $this->assertEquals(1, $results_object->c1,
        'No organism Arabidopsis thaliana found which should have been created');

    // Test import_existing, check if Arabidopsis arenosa
    // lineageex property was looked up from NCBI.
    $run_args['taxonomy_ids'] = '';
    $run_args['import_existing'] = 1;
    $this->mock_error = '';
    $taxonomy_importer->createImportJob($run_args, $file_details);
    $taxonomy_importer->prepareFiles();
    $taxonomy_importer->run();
    $taxonomy_importer->postRun();
    $this->checkForNetworkError();

    $results = $chado->query("SELECT count(*) as c2 FROM {1:organism} O
        LEFT JOIN {1:organismprop} P ON O.organism_id=P.organism_id
        LEFT JOIN {1:cvterm} T ON P.type_id=T.cvterm_id
        WHERE O.genus = 'Arabidopsis' AND O.species = 'arenosa' AND T.name='lineageex';");
    $results_object = $results->fetchObject();
    $this->assertEquals(1, $results_object->c2,
        'No lineageex was retrieved from NCBI for Arabidopsis arenosa');

    // Verify the correct NCBI Taxid value was retrieved.
    $results = $chado->query("SELECT X.accession FROM {1:organism} O
        LEFT JOIN {1:organism_dbxref} D ON O.organism_id=D.organism_id
        LEFT JOIN {1:dbxref} X ON D.dbxref_id=X.dbxref_id
        WHERE O.genus = 'Arabidopsis' AND O.species = 'arenosa';");
    $results_object = $results->fetchObject();
    $this->assertEquals(38785, $results_object->accession,
        'An incorrect NCBI Taxid value was retrieved from NCBI for Arabidopsis arenosa');
  }

  /**
   * Mark test skipped when NCBI is down.
   */
  protected function checkForNetworkError() {
    if ($this->mock_error) {
      // If we get a specific known message due to the NCBI external
      // database being down, then mark this test as skipped.
      $matched_any = FALSE;
      // For each expected external message...
      foreach ($this->message_actions as $pattern => $action) {
        // If it's in our list with an action of skip
        // then we indicate the test should be skipped.
        if (preg_match($pattern, $this->mock_error) && $action == 'skip') {
          $matched_any = TRUE;
        }
      }
      if ($matched_any) {
        $this->markTestSkipped("We received only known external error messages and chose to ignore them. Specifically:\n"
          . $this->mock_error);
      }
      else {
        // If it doesn't match any of our pattern then it is a new
        // unanticipated error and we want to immediately fail.
        $this->fail('Unanticipated error: ' . $this->mock_error);
      }
    }
  }

}
