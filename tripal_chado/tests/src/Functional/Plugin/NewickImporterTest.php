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
use NewickImporter;

/**
 * Tests for the NewickImporter class
 *
 * @group Tripal
 * @group Tripal Chado
 * @group Tripal Chado ChadoStorage
 */
class NewickImporterTest extends ChadoTestBrowserBase
{

  /**
   * Confirm basic GFF importer functionality.
   *
   * @group gff
   */
  public function testNewickImporterSimpleTest()
  {
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

    // Insert organism
    $organism_id = $chado->insert('1:organism')
      ->fields([
        'genus' => 'Citrus',
        'species' => 'sinensis',
        'common_name' => 'Sweet Orange',
      ])
      ->execute();

    // Insert Analysis
    $analysis_id = $chado->insert('1:analysis')
      ->fields([
        'name' => 'Test Analysis',
        'description' => 'Test Analysis',
        'program' => 'PROGRAM',
        'programversion' => '1.0',
      ])
      ->execute();
      

    // Perform the Newick test by creating an instance of the Newick loader
    $importer_manager = \Drupal::service('tripal.importer');
    $newick_importer = $importer_manager->createInstance('chado_newick_tree_loader');

    $run_args = [
      'files' => [
        0 => [
          'file_path' => __DIR__ . '/../../../fixtures/newick_loader/mrna_mini.fasta.tree'
        ]
      ],
      'analysis_id' => $analysis_id,
      'schema_name' => $schema_name,
      'tree_name' => 'Tree 2',
      'leaf_type' => 'polypeptide (SO:0000104)',
      'dbxref' => '',
      'description' => 'No description',
      'name_re' => '',
      'match' => 0,
      'load_later' => 0 
    ];

    $file_details = [
      'file_local' => __DIR__ . '/../../../fixtures/fasta_loader/mrna_mini.fasta.tree',
    ];

    $newick_importer->create($run_args, $file_details);
    $newick_importer->prepareFiles();
    $newick_importer->run();
    $newick_importer->postRun();
  }

}