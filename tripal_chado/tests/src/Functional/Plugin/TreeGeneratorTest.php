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

    // Populate some test organisms with just the required properties, the taxonomy
    // importer has its own tests.
    $results = $chado->query("INSERT INTO {1:organism} (genus, species) VALUES ('Arabidopsis', 'thaliana')");
    $results = $chado->query("INSERT INTO {1:organism} (genus, species) VALUES ('Arabidopsis', 'arenosa')");
    $lineageex = 'no rank:131567:cellular organisms;superkingdom:2759:Eukaryota;kingdom:33090:Viridiplantae;phylum:35493:Streptophyta;subphylum:131221:Streptophytina;clade:3193:Embryophyta;clade:58023:Tracheophyta;clade:78536:Euphyllophyta;clade:58024:Spermatophyta;class:3398:Magnoliopsida;clade:1437183:Mesangiospermae;clade:71240:eudicotyledons;clade:91827:Gunneridae;clade:1437201:Pentapetalae;clade:71275:rosids;clade:91836:malvids;order:3699:Brassicales;family:3700:Brassicaceae;tribe:980083:Camelineae;genus:3701:Arabidopsis';
    $sql = "INSERT INTO {1:organismprop} (organism_id, type_id, value) VALUES (
      (SELECT organism_id FROM {1:organism} WHERE genus=:genus AND species=:species),
      (SELECT cvterm_id FROM {1:cvterm} WHERE name='lineageex'),
      :lineage)";
    $results = $chado->query($sql, [':genus' => 'Arabidopsis', ':species' => 'thaliana', ':lineage' => $lineageex]);
    $results = $chado->query($sql, [':genus' => 'Arabidopsis', ':species' => 'arenosa', ':lineage' => $lineageex]);

    // Create a Tree Generator instance
    $importer_manager = \Drupal::service('tripal.importer');
    $tree_generator = $importer_manager->createInstance('chado_tree_generator');
    $run_args = [
      'schema_name' => $schema_name,
      'tree_name' => 'Test Taxonomy Tree',
      'use_transaction' => 1,
      'import_existing' => 1,
      'root_taxon' => 'Brassicales',
    ];

    $file_details = [
    ];

    $tree_generator->createImportJob($run_args, $file_details);
    $tree_generator->prepareFiles();
    $tree_generator->run();
    $tree_generator->postRun();

    // Check if a phylotree named 'Test Taxonomy Tree' was created
    $results = $chado->query("SELECT count(*) as c1 FROM {1:phylotree}
        where name = :name", [':name' => 'Test Taxonomy Tree']);
    $results_object = $results->fetchObject();
    $this->assertEquals(1, $results_object->c1,
        'A phylotree named Test Taxonomy Tree should have been created but was not.');

//@@@ to-do this is failing:
    // Check if phylonode organism was created
    $results = $chado->query("SELECT count(*) as c2 FROM {1:phylonode_organism}");
    $results_object = $results->fetchObject();
    $this->assertEquals(2, $results_object->c2,
        'Two phylonode organisms should have been created but were not.');

    // Check if more than 5 phylonodes were created
    $results = $chado->query("SELECT count(*) as c3 FROM {1:phylonode}");
    $results_object = $results->fetchObject();
    $this->assertGreaterThan(5, $results_object->c3,
        'Phylonodes count should be more than 5.');

    // Check if there are phylonodeprops like
    $results = $chado->query("SELECT count(*) as c4 FROM {1:phylonodeprop}");
    $results_object = $results->fetchObject();
    $this->assertGreaterThan(5, $results_object->c4,
        'Phylonodeprop count should be more than 5.');

    // Check more specifics in phylonodeprop - genus
    $results = $chado->query("SELECT count(*) as c5 FROM {1:phylonodeprop} WHERE value='genus'");
    $results_object = $results->fetchObject();
    $this->assertGreaterThan(0, $results_object->c5,
        'Phylonodeprop genus should exist.');

    // Check more specifics in phylonodeprop - species
    $results = $chado->query("SELECT count(*) as c6 FROM {1:phylonodeprop} WHERE value='species'");
    $results_object = $results->fetchObject();
    $this->assertGreaterThan(0, $results_object->c6,
        'Phylonodeprop species should exist.');

    // Check more specifics in phylonodeprop - family
    $results = $chado->query("SELECT count(*) as c7 FROM {1:phylonodeprop} WHERE value='family'");
    $results_object = $results->fetchObject();
    $this->assertGreaterThan(0, $results_object->c7,
        'Phylonodeprop family should exist.');

    // Check more specifics in phylonodeprop - kingdom
    // This should be excluded because we specified a root_taxon
    $results = $chado->query("SELECT count(*) as c8 FROM {1:phylonodeprop} WHERE value='kingdom'");
    $results_object = $results->fetchObject();
    $this->assertEqual(0, $results_object->c8,
        'Phylonodeprop kingdom should not exist.');

  }

}
?>
