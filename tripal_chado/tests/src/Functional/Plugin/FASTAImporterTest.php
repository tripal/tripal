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
use FASTAImporter;

/**
 * Tests for the FASTAImporter class
 *
 * @group TripalImporter
 * @group ChadoImporter
 * @group FASTAImporter
 */
class FASTAImporterTest extends ChadoTestBrowserBase
{

  /**
   * Confirm basic FASTA importer functionality.
   *
   * @group FASTA
   */
  public function testFASTAImporterSimpleTest()
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


    // Perform the FASTA test by creating an instance of the FASTA loader
    $importer_manager = \Drupal::service('tripal.importer');
    $fasta_importer = $importer_manager->createInstance('chado_fasta_loader');

    $run_args = [
      'files' => [
        0 => [
          'file_path' => __DIR__ . '/../../../fixtures/fasta_loader/Citrus_sinensis-orange1.1g015632m.g.fasta'
        ]
      ],
      'schema_name' => $schema_name,
      'analysis_id' => $analysis_id,
      'organism_id' => $organism_id,
      'seqtype' => 'polypeptide',
      'parent_type' => "mRNA",
      'rel_type' => "derives_from",
      'method' => '2',
      'match_type' => '1',
      're_name' => "",
      're_uname' => "",
      're_accession' => "",
      'db_id' => "",
      're_subject' => "",
      'match_type' => "1",
    ];

    $file_details = [
      'file_local' => __DIR__ . '/../../../fixtures/fasta_loader/Citrus_sinensis-orange1.1g015632m.g.fasta',
    ];

    $fasta_importer->createImportJob($run_args, $file_details);
    $fasta_importer->prepareFiles();
    $fasta_importer->run();
    $fasta_importer->postRun();

    $results = $chado->query("SELECT count(*) as c1 FROM {1:feature} WHERE name = :name", [
      ':name' => 'orange1.1g017341m'
    ]);

    $results_object = $results->fetchObject();
    $this->assertEquals($results_object->c1, 1, 'There should have been one feature named orange1.1g017341m');

    $results = $chado->query("SELECT count(*) as c1 FROM {1:feature} WHERE name = :name", [
      ':name' => 'orange1.1g022797m'
    ]);

    $results_object = $results->fetchObject();
    $this->assertEquals($results_object->c1, 1, 'There should have been one feature named orange1.1g017341m');

    $results = $chado->query("SELECT count(*) as c1 FROM {1:feature} WHERE name = :name", [
      ':name' => 'orange1.1g022799m'
    ]);

    $results_object = $results->fetchObject();
    $this->assertEquals($results_object->c1, 1, 'There should have been one feature named orange1.1g017341m');

    // Check the type_id
    $results = $chado->query("SELECT * FROM {1:feature} as f
      LEFT JOIN {1:cvterm} as cvterm ON (f.type_id = cvterm.cvterm_id)
      WHERE f.name = :name", [
      ':name' => 'orange1.1g022799m'
    ]);

    $results_object = $results->fetchObject();
    $this->assertEquals($results_object->name, 'polypeptide', 'CVTERM name should have been a polypeptide but returned a different name');
    $this->assertEquals($results_object->seqlen, 2325, 'Seqlen column should have returned 2325 but returned another value');

    // Get the feature_id
    $feature_id = $results_object->feature_id;
    $results = $chado->query('SELECT * FROM {1:analysisfeature} WHERE feature_id = :feature_id', [
      ':feature_id' => $feature_id
    ]);
    $results_object = $results->fetchObject();
    $this->assertEquals($results_object->feature_id, $feature_id, 'Did not find a feature_id that matched');

    // Test scaffold from Citrus Sinensis (trimmed version)
    $run_args = [
      'files' => [
        0 => [
          'file_path' => __DIR__ . '/../../../fixtures/fasta_loader/Citrus_sinensis-scaffold00001-trimmed.fasta'
        ]
      ],
      'schema_name' => $schema_name,
      'analysis_id' => $analysis_id,
      'organism_id' => $organism_id,
      'seqtype' => 'polypeptide',
      'parent_type' => "mRNA",
      'rel_type' => "derives_from",
      'method' => '2',
      'match_type' => '1',
      're_name' => "",
      're_uname' => "",
      're_accession' => "",
      'db_id' => "",
      're_subject' => "",
      'match_type' => "1",
    ];

    $file_details = [
      'file_local' => __DIR__ . '/../../../fixtures/fasta_loader/Citrus_sinensis-scaffold00001-trimmed.fasta',
    ];

    $fasta_importer->createImportJob($run_args, $file_details);
    $fasta_importer->prepareFiles();
    $fasta_importer->run();
    $fasta_importer->postRun();

    $results = $chado->query("SELECT count(*) as c1 FROM {1:feature} WHERE name = :name", [
      ':name' => 'scaffold00001'
    ]);

    $results_object = $results->fetchObject();
    $this->assertEquals($results_object->c1, 1, 'There should have been one feature named scaffold00001');


  }

}
?>
