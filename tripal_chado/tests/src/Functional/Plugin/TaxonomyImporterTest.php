<?php

namespace Drupal\Tests\tripal_chado\Functional;


/**
 * Tests for the TaxonomyImporter class
 *
 * @group TripalImporter
 * @group ChadoImporter
 * @group TaxonomyImporter
 */
class TaxonomyImporterTest extends ChadoTestBrowserBase {

  /**
   * Confirm basic Taxonomy importer functionality.
   *
   * @group taxonomy
   */
  public function testTaxonomyImporterSimpleTest() {

    // Installs up the chado with the test chado data
    $chado = $this->getTestSchema(ChadoTestBrowserBase::PREPARE_TEST_CHADO);

    // Keep track of the schema name in case we need it
    $schema_name = $chado->getSchemaName();

    // Test to ensure cvterms are found in the cvterms table
    $cvterms_count_query = $chado->query("SELECT count(*) as c1 FROM {1:cvterm}");
    $cvterms_count_object = $cvterms_count_query->fetchObject();
    $this->assertNotEquals($cvterms_count_object->c1, 0);

    // Create an organism without any properties and without an NCBI
    // taxon id, to be used for the import_existing test. NCBI taxid=38785
    $results = $chado->query("INSERT INTO {1:organism} (genus, species) VALUES ('Arabidopsis', 'arenosa')");

    // Create a Taxonomy Importer
    // Perform the Taxonomy Importer test by creating an instance of the Taxonomy loader
    $importer_manager = \Drupal::service('tripal.importer');
    $taxonomy_importer = $importer_manager->createInstance('chado_taxonomy_loader');
    $run_args = [
      'schema_name' => $schema_name,
      'taxonomy_ids' => '3702',
      'use_transaction' => 1,
      'import_existing' => 1,
      'ncbi_api_key' => NULL,
    ];

    $file_details = [
    ];

    $taxonomy_importer->createImportJob($run_args, $file_details);
    $taxonomy_importer->prepareFiles();
    $taxonomy_importer->run();
    $taxonomy_importer->postRun();

    // Check if Arabidopsis thaliana retrieved by tax_id 3702 from NCBI and organism created
    $results = $chado->query("SELECT count(*) as c1 FROM {1:organism}
        WHERE genus = 'Arabidopsis' AND species = 'thaliana';");
    $results_object = $results->fetchObject();
    $this->assertEquals(1, $results_object->c1,
        'No organism Arabidopsis thaliana found which should have been created');

    // Test import_existing, check if Arabidopsis arenosa
    // lineageex property was looked up from NCBI
    $results = $chado->query("SELECT count(*) as c2 FROM {1:organism} O
        LEFT JOIN {1:organismprop} P ON O.organism_id=P.organism_id
        LEFT JOIN {1:cvterm} T ON P.type_id=T.cvterm_id
        WHERE O.genus = 'Arabidopsis' AND O.species = 'arenosa' AND T.name='lineageex';");
    $results_object = $results->fetchObject();
    $this->assertEquals(1, $results_object->c2,
        'No lineageex was retrieved from NCBI for Arabidopsis arenosa');

    // Verify the correct NCBI Taxid value was retrieved
    $results = $chado->query("SELECT X.accession FROM {1:organism} O
        LEFT JOIN {1:organism_dbxref} D ON O.organism_id=D.organism_id
        LEFT JOIN {1:dbxref} X ON D.dbxref_id=X.dbxref_id
        WHERE O.genus = 'Arabidopsis' AND O.species = 'arenosa';");
    $results_object = $results->fetchObject();
    $this->assertEquals(38785, $results_object->accession,
        'An incorrect NCBI Taxid value was retrieved from NCBI for Arabidopsis arenosa');
  }

}
