<?php

namespace Drupal\Tests\tripal_chado\Functional;

use Drupal\tripal_chado\TripalStorage\ChadoIntStoragePropertyType;
use Drupal\tripal_chado\TripalStorage\ChadoVarCharStoragePropertyType;
use Drupal\tripal_chado\TripalStorage\ChadoTextStoragePropertyType;
use Drupal\tripal\TripalStorage\StoragePropertyValue;
use Drupal\tripal\TripalVocabTerms\TripalTerm;
use Drupal\Tests\tripal_chado\Functional\MockClass\FieldConfigMock;

// FROM OLD CODE:
use Drupal\Core\Url;
use Drupal\Tests\BrowserTestBase;
use Drupal\Core\Database\Database;
use Drupal\tripal_chado\api\ChadoSchema;
use TaxonomyImporter;

/**
 * Tests for the TaxonomyImporter class
 *
 * @group TripalImporter
 * @group ChadoImporter
 * @group TreeGenerator
 */
class TreeGeneratorTest extends ChadoTestBrowserBase {

  /**
   * Confirm basic Tree Generator functionality.
   *
   * @group taxonomy
   */
  public function testTreeGeneratorSimpleTest() {
    // Public schema connection
    $public = \Drupal::database();

    // Installs up the chado with the test chado data
    $chado = $this->getTestSchema(ChadoTestBrowserBase::PREPARE_TEST_CHADO);

    // Keep track of the schema name in case we need it
    $schema_name = $chado->getSchemaName();

    // Test to ensure cvterms are found in the cvterms table
    $cvterms_count_query = $chado->query("SELECT count(*) as c1 FROM {1:cvterm}");
    $cvterms_count_object = $cvterms_count_query->fetchObject();
    $this->assertNotEquals($cvterms_count_object->c1, 0);

    // Create a Taxonomy Importer
    // Perform the Taxonomy Importer test by creating an instance of the Taxonomy loader
    $importer_manager = \Drupal::service('tripal.importer');
    $taxonomy_importer = $importer_manager->createInstance('chado_taxonomy_loader');
    $run_args = [
      'schema_name' => $schema_name,
      'tree_name' => 'Taxonomy Tree',
      'taxonomy_ids' => '3702',
      'use_transaction' => 1,
      'import_existing' => 1,
      'ncbi_api_key' => NULL,
      'root_taxon' => '',
    ];

    $file_details = [
      // 'file_local' => __DIR__ . '/../../../fixtures/gff3_loader/small_gene.gff',
    ];

    $taxonomy_importer->createImportJob($run_args, $file_details);
    $taxonomy_importer->prepareFiles();
    $taxonomy_importer->run();
    $taxonomy_importer->postRun();

    // Check if Arabidopsis thaliana
    $results = $chado->query("SELECT count(*) as c1 FROM {1:organism}
        where genus = 'Arabidopsis' AND species = 'thaliana';");
    $results_object = $results->fetchObject();
    $this->assertEquals(1, $results_object->c1,
        'No organism Arabidopsis thaliana found which should have been created');

    // Check if a phylotree named 'Taxonomy Tree'
    $results = $chado->query("SELECT count(*) as c1 FROM {1:phylotree}
        where name = 'Taxonomy Tree'
    ");
    $this->assertEquals(1, $results->c1,
        'A phylotree named Taxonomy Tree should have been created but wasnt. TaxonomyImporter test failed');

    // Check if phylonode organism was created
    $results = $chado->query("SELECT count(*) as c1 FROM {1:phylonode_organism}");
    $this->assertEquals(1, $results_object->c1,
        'A phylonode organism should have been created but wasnt.');

    // Check if more than 5 phylonodes were created
    $results = $chado->query("SELECT count(*) as c1 FROM {1:phylonode}");
    $this->assertGreaterThan(0, $results_object->c1,
        'Phylonodes count should be more than 0. TaxonomyImporter test failed.');

    // Check if there are phylonodeprops like
    $results = $chado->query("SELECT count(*) as c1 FROM {1:phylonodeprop}");
    $this->assertGreaterThan(0, $results_object->c1,
        'Phylonodeprop count should be more than 5. TaxonomyImporter test failed.');

    // Check more specifics in phylonodeprop - genus
    $results = $chado->query("SELECT count(*) as c1 FROM {1:phylonodeprop} WHERE value='genus'");
    $this->assertGreaterThan(0, $results_object->c1,
        'Phylonodeprop genus should exist. TaxonomyImporter test failed.');

    // Check more specifics in phylonodeprop - species
    $results = $chado->query("SELECT count(*) as c1 FROM {1:phylonodeprop} WHERE value='species'");
    $this->assertGreaterThan(0, $results_object->c1,
        'Phylonodeprop species should exist. TaxonomyImporter test failed.');

    // Check more specifics in phylonodeprop - family
    $results = $chado->query("SELECT count(*) as c1 FROM {1:phylonodeprop} WHERE value='family'");
    $this->assertGreaterThan(0, $results_object->c1,
        'Phylonodeprop family should exist. TaxonomyImporter test failed.');

    // Check more specifics in phylonodeprop - kingdom
    $results = $chado->query("SELECT count(*) as c1 FROM {1:phylonodeprop} WHERE value='kingdom'");
    $this->assertGreaterThan(0, $results_object->c1,
        'Phylonodeprop kingdom should exist. TaxonomyImporter test failed.');

    // Check more specifics in phylonodeprop - order
    $results = $chado->query("SELECT count(*) as c1 FROM {1:phylonodeprop} WHERE value='order'");
    $this->assertGreaterThan(0, $results_object->c1,
        'Phylonodeprop order should exist. TaxonomyImporter test failed.');

    // Check more specifics in phylonodeprop - superkingdom
    $results = $chado->query("SELECT count(*) as c1 FROM {1:phylonodeprop} WHERE value='superkingdom'");
    $this->assertGreaterThan(0, $results_object->c1,
        'Phylonodeprop superkingdom should exist. TaxonomyImporter test failed.');

    // Check more specifics in phylonodeprop - clade
    $results = $chado->query("SELECT count(*) as c1 FROM {1:phylonodeprop} WHERE value='clade'");
    $this->assertGreaterThan(0, $results_object->c1,
        'Phylonodeprop clade should exist. TaxonomyImporter test failed.');

    // Check more specifics in phylonodeprop - class
    $results = $chado->query("SELECT count(*) as c1 FROM {1:phylonodeprop} WHERE value='class'");
    $this->assertGreaterThan(0, $results_object->c1,
        'Phylonodeprop class should exist. TaxonomyImporter test failed.');
  }

}
?>
