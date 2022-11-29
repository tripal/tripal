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
use GFF3Importer;

/**
 * Tests for the GFF3Importer class
 *
 * @group Tripal
 * @group Tripal Chado
 * @group Tripal Chado ChadoStorage
 */
class GFF3ImporterTest extends ChadoTestBrowserBase {

  /**
   * Confirm basic GFF importer functionality.
   *
   * @group gff
   */
  public function testGFFImporterSimpleTest() {
    $public = \Drupal::database();

    // Installs up the chado with the test chado data
    $chado = $this->getTestSchema(ChadoTestBrowserBase::PREPARE_TEST_CHADO);

    // Keep track of the schema name in case we need it
    $schema_name = $chado->getSchemaName();

    // Import landmarks from fixture
    $chado->executeSqlFile(__DIR__ . '/../../../fixtures/gff3_loader/landmarks.sql');

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


    // Verify that gene is now in the cvterm table (which gets imported from SO obo)
    $result_gene_cvterm = $chado->query("SELECT * FROM {1:cvterm} WHERE name = 'gene' LIMIT 1;");
    $cvterm_object = null;
    $cvterm_object = $result_gene_cvterm->fetchObject();
    $this->assertNotEquals($cvterm_object, null);
    

    // Perform the GFF3 test by creating an instance of the GFF3 loader
    $importer_manager = \Drupal::service('tripal.importer');
    $gff3_importer = $importer_manager->createInstance('chado_gff3_loader');
    $run_args = [
      'files' => [
        0 => [
          'file_path' => __DIR__ . '/../../../fixtures/gff3_loader/small_gene.gff'
        ]
      ],
      'schema_name' => $schema_name,
      'analysis_id' => $analysis_id,
      'organism_id' => $organism_id,
      'use_transaction' => 1,
      'add_only' => 0,
      'update' => 1,
      'create_organism' => 0,
      'create_target' => 0,
      // regexps for mRNA and protein.
      're_mrna' => NULL,
      're_protein' => NULL,
      // optional
      'target_organism_id' => NULL,
      'target_type' => NULL,
      'start_line' => NULL,
      'line_number' => NULL, // Previous error without this
      'landmark_type' => NULL,
      'alt_id_attr' => NULL,
      'skip_protein' => NULL,
    ];

    $file_details = [
      'file_local' => __DIR__ . '/../../../fixtures/gff3_loader/small_gene.gff',
    ];
    
    $gff3_importer->create($run_args, $file_details); 
    $gff3_importer->prepareFiles();
    $gff3_importer->run([
      'chado' => $chado,
      'schema_name' => $schema_name
    ]);
    $gff3_importer->postRun();
    
    // $gff3_importer = new GFF3Importer();
    // $gff_file = __DIR__ . '/../data/small_gene.gff';
    // $fasta = ['file_local' => __DIR__ . '/../data/short_scaffold.fasta'];
    // $run_args = [
    //   'analysis_id' => $analysis->analysis_id,
    //   'organism_id' => $organism->organism_id,
    //   'use_transaction' => 1,
    //   'add_only' => 0,
    //   'update' => 1,
    //   'create_organism' => 0,
    //   'create_target' => 0,
    //   // regexps for mRNA and protein.
    //   're_mrna' => NULL,
    //   're_protein' => NULL,
    //   // optional
    //   'target_organism_id' => NULL,
    //   'target_type' => NULL,
    //   'start_line' => NULL,
    //   'line_number' => NULL, // Previous error without this
    //   'landmark_type' => NULL,
    //   'alt_id_attr' => NULL,
    //   'skip_protein' => NULL,
    // ];
    // $this->loadLandmarks($analysis, $organism, $fasta);

    // $arguments = $gff3_importer->getArguments();
    // $arguments['run_args'] = $run_args;
    // $arguments['files'][0]['file_path'] = $gff_file;
    // $gff3_importer->setArguments($arguments); // run arguments from above

    // // NEW T4 CODE
    // $gff3_importer->run();

    // // This protein is an explicit protein / polypeptide imported from the GFF
    // // file. 
    // $name = 'test_protein_001.1';

    // $result = $connection->query('SELECT uniquename FROM chado.feature 
		// 	WHERE uniquename=:un',
		// 	[':un' => $name])->fetchObject();
    // $this->assertEquals($name, $result->uniquename);
  }

}