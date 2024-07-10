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
 * @group TripalImporter
 * @group ChadoImporter
 * @group GFF3Importer
 */
class GFF3ImporterTest extends ChadoTestBrowserBase
{

  /**
   * Confirm basic GFF importer functionality.
   *
   * @group gff
   */
  public function testGFFImporterSimpleTest()
  {
    // GFF3 Specifications document: http://gmod.org/wiki/GFF3

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


    // Verify that gene is now in the cvterm table (which gets imported from SO obo)
    $result_gene_cvterm = $chado->query("SELECT * FROM {1:cvterm}
      WHERE name = 'gene' LIMIT 1;");
    $cvterm_object = null;
    $cvterm_object = $result_gene_cvterm->fetchObject();
    $this->assertNotEquals($cvterm_object, null);

    // Manually insert landmarks into features table
    $chado->query("INSERT INTO {1:feature} (dbxref_id, organism_id, name, uniquename, residues, seqlen, md5checksum, type_id, is_analysis, is_obsolete, timeaccessioned, timelastmodified) VALUES (NULL, 1, 'scaffold00001', 'scaffold00001', '', 0, 'd41d8cd98f00b204e9800998ecf8427e', 474, false, false, '2022-11-26 05:39:59.809424', '2022-11-26 05:39:59.809424');");
    $chado->query("INSERT INTO {1:feature} (dbxref_id, organism_id, name, uniquename, residues, seqlen, md5checksum, type_id, is_analysis, is_obsolete, timeaccessioned, timelastmodified) VALUES (NULL, 1, 'scaffold1', 'scaffold1', 'CAACAAGAAGTAAGCATAGGTTAATTATCATCCACGCATATTAATCAAGAATCGATGCTCGATTAATGTTTTTGAATTGACAAACAAAAGTTTTGTAAAAAGGACTTGTTGGTGGTGGTGGGGTGGTGGTGATGGTGTGGTGGGTAGGTCGCTGGTCGTCGCCGGCGTGGTGGAAGTCTCGCTGGCCGGTGTCTCGGCGGTCTGGTGGCGGCTGGTGGCGGTAGTTGTGAGTTTTTTCTTTCTTTTTTTGTTTTTTTTTTTTACTTTTTACTTTTTTTTCGTCTTGAACAAATTAAAAATAGAGTTTGTTTGTATTTGGTTATTATTTATTGATAAGGGTATATTCGTCCTGTTTGGTCTTGATGTAATAAAATTAAATTAATTTACGGGCTTCAACTAATAAACTCCTTCATGTTGGTTTGAACTAATAAAAAAAGGGGAAATTTGCTAGACACCCCTAATTTTGGACTTATATGGGTAGAAGTCCTAGTTGCTAGATGAATATAGGCCTAGGTCCATCCACATAAAAAAATAATATAAATTAAATAATAAAAATAATATATAGACATAAGTACCCTTATTGAATAAACATATTTTAGGGGATTCAGTTATATACGTAAAGTTGGGAAATCAAATCCCACTAATCACGATTGAAGGCAGAGTATCGTGTAAGACGTTTGGAAAACATATCTTAGTCGATTCCAGTGGAATATGAGATCA', 720, '83578d8afdaec399c682aa6c0ddd29c9', 474, false, false, '2022-11-28 21:44:51.006276', '2022-11-28 21:44:51.006276');");
    $chado->query("INSERT INTO {1:feature} (dbxref_id, organism_id, name, uniquename, residues, seqlen, md5checksum, type_id, is_analysis, is_obsolete, timeaccessioned, timelastmodified) VALUES (NULL, 1, 'Contig10036', 'Contig10036', '', 0, 'd41d8cd98f00b204e9800998ecf8427e', 474, false, false, '2022-11-26 05:39:55.810798', '2022-11-26 05:39:55.810798')");
    $chado->query("INSERT INTO {1:feature} (dbxref_id, organism_id, name, uniquename, residues, seqlen, md5checksum, type_id, is_analysis, is_obsolete, timeaccessioned, timelastmodified) VALUES (NULL, 1, 'Contig1', 'Contig1', '', 0, 'd41d8cd98f00b204e9800998ecf8427e', 474, false, false, '2022-11-26 05:39:57.335594', '2022-11-26 05:39:57.335594');");
    $chado->query("INSERT INTO {1:feature} (dbxref_id, organism_id, name, uniquename, residues, seqlen, md5checksum, type_id, is_analysis, is_obsolete, timeaccessioned, timelastmodified) VALUES (NULL, 1, 'Contig0', 'Contig0', '', 0, 'd41d8cd98f00b204e9800998ecf8427e', 474, false, false, '2022-11-26 05:39:59.809424', '2022-11-26 05:39:59.809424');");

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
      'landmark_type' => 'supercontig',
      'alt_id_attr' => NULL,
      'skip_protein' => NULL,
    ];

    $file_details = [
      'file_local' => __DIR__ . '/../../../fixtures/gff3_loader/small_gene.gff',
    ];

    $gff3_importer->createImportJob($run_args, $file_details);
    $gff3_importer->prepareFiles();
    $gff3_importer->run();
    $gff3_importer->postRun();

    // This check determines if scaffold1 was added to the features table (this was done manually above)
    $results = $chado->query("SELECT * FROM {1:feature} WHERE uniquename='scaffold1';");
    $results_object = $results->fetchObject();
    $scaffold_feature_id = $results_object->feature_id;
    $this->assertEquals($results_object->uniquename, 'scaffold1');
    unset($results);
    unset($results_object);

    // This checks to ensure the test_gene_001 (gene) feature was inserted into the feature table
    $results = $chado->query("SELECT * FROM {1:feature}
      WHERE uniquename='test_gene_001';");
    $results_object = $results->fetchObject();
    $gene_feature_id = $results_object->feature_id;
    $this->assertEquals($results_object->uniquename, 'test_gene_001');
    unset($results);
    unset($results_object);

    // This checks to see whether the test_mrna_001.1 (mrna) feature got inserted into the feature table
    $results = $chado->query("SELECT * FROM {1:feature}
      WHERE uniquename='test_mrna_001.1';");
    $results_object = $results->fetchObject();
    $mrna_feature_id = $results_object->feature_id;
    $this->assertEquals($results_object->uniquename, 'test_mrna_001.1');
    unset($results);
    unset($results_object);

    // This checks to see whether the test_protein_001.1 (polypeptide) feature got inserted into the feature table
    $results = $chado->query("SELECT * FROM {1:feature}
      WHERE uniquename='test_protein_001.1';");
    $results_object = $results->fetchObject();
    $polypeptide_feature_id = $results_object->feature_id;
    $this->assertEquals($results_object->uniquename, 'test_protein_001.1');
    unset($results);
    unset($results_object);

    // Do checks on the featureprop table as well. Ensures the bio type value got added
    $results = $chado->query("SELECT * FROM {1:featureprop}
      WHERE feature_id = :feature_id AND value LIKE :value;", [
      ':feature_id' => $gene_feature_id,
      ':value' => 'protein_coding'
    ]);
    $results_object = $results->fetchObject();
    $this->assertEquals($results_object->value, 'protein_coding');
    unset($results);
    unset($results_object);


    // Ensures the GAP value got added
    $results = $chado->query("SELECT * FROM {1:featureprop}
      WHERE feature_id = :feature_id AND value LIKE :value;", [
      ':feature_id' => $gene_feature_id,
      ':value' => 'test_gap_1'
    ]);
    $results_object = $results->fetchObject();
    $this->assertEquals($results_object->value, 'test_gap_1');
    unset($results);
    unset($results_object);

    // Ensures the NOTE value got added
    $results = $chado->query("SELECT * FROM {1:featureprop}
      WHERE feature_id = :feature_id AND value LIKE :value;", [
      ':feature_id' => $gene_feature_id,
      ':value' => 'test_gene_001_note'
    ]);
    $results_object = $results->fetchObject();
    $this->assertEquals($results_object->value, 'test_gene_001_note');
    unset($results);
    unset($results_object);

    // TODO: To complete
    /**
     * Run the GFF loader on gff_duplicate_ids.gff for testing.
     *
     * This tests whether the GFF loader detects duplicate IDs which makes a
     * GFF file invalid since IDs should be unique. The GFF loader should throw
     * and exception which this test checks for
     */
    $gff3_importer = $importer_manager->createInstance('chado_gff3_loader');
    $run_args = [
      'files' => [
        0 => [
          'file_path' => __DIR__ . '/../../../fixtures/gff3_loader/gff_duplicate_ids.gff'
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
      'landmark_type' => 'supercontig',
      'alt_id_attr' => NULL,
      'skip_protein' => NULL,
    ];

    $file_details = [
      'file_local' => __DIR__ . '/../../../fixtures/gff3_loader/gff_duplicate_ids.gff',
    ];


    $has_exception = false;
    try {
      $gff3_importer->createImportJob($run_args, $file_details);
      $gff3_importer->prepareFiles();
      $gff3_importer->run();
      $gff3_importer->postRun();
    }
    catch (\Exception $ex) {
      $message = $ex->getMessage();
      $has_exception = true;
    }
    $this->assertEquals($has_exception, true, "This should have detected an exception since there are
      duplicated IDs in this GFF file but no exception was returned.");

    /**
     * Run the GFF loader on gff_tag_unescaped_character.gff for testing.
     *
     * This tests whether the GFF loader adds IDs that contain a comma.
     * The GFF loader should allow it
     */
    // BEGIN NEW FILE: Perform import on gff_tag_unescaped_character
    $gff3_importer = $importer_manager->createInstance('chado_gff3_loader');
    $run_args = [
      'files' => [
        0 => [
          'file_path' => __DIR__ . '/../../../fixtures/gff3_loader/gff_tag_unescaped_character.gff'
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
      'landmark_type' => 'supercontig',
      'alt_id_attr' => NULL,
      'skip_protein' => NULL,
    ];

    $file_details = [
      'file_local' => __DIR__ . '/../../../fixtures/gff3_loader/gff_tag_unescaped_character.gff',
    ];

    $has_exception = false;
    try {
      $gff3_importer->createImportJob($run_args, $file_details);
      $gff3_importer->prepareFiles();
      $gff3_importer->run();
      $gff3_importer->postRun();
    }
    catch (\Exception $ex) {
      $message = $ex->getMessage();
      $has_exception = true;
    }
    $this->assertEquals($has_exception, false, 'Unescaped tag should not throw an error but did.');
  // @TODO check that the feature with the comma in the ID was inserted properly.
    /**
     * Run the GFF loader on gff_invalidstartend.gff for testing.
     *
     * This tests whether the GFF loader fixes start end values
     */
    $gff3_importer = $importer_manager->createInstance('chado_gff3_loader');
    $run_args = [
      'files' => [
        0 => [
          'file_path' => __DIR__ . '/../../../fixtures/gff3_loader/gff_invalidstartend.gff'
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
      'landmark_type' => 'supercontig',
      'alt_id_attr' => NULL,
      'skip_protein' => NULL,
    ];

    $file_details = [
      'file_local' => __DIR__ . '/../../../fixtures/gff3_loader/gff_invalidstartend.gff',
    ];

    $has_exception = false;
    try {
      $gff3_importer->createImportJob($run_args, $file_details);
      $gff3_importer->prepareFiles();
      $gff3_importer->run();
      $gff3_importer->postRun();
    }
    catch (\Exception $ex) {
      $message = $ex->getMessage();
      $has_exception = true;
    }
    $this->assertEquals($has_exception, false, "The GFF3 loader should reverse the values automatically but somehow produced an exception which is an error");
    // @TODO Add additional assertions for start and end to ensure the loader does the reverse correctly



    /**
     * Run the GFF loader on gff_phase_invalid_character.gff for testing.
     *
     * This tests whether the GFF loader interprets the phase values correctly
     * for CDS rows when a character outside of the range 0,1,2 is specified.
     */
    $gff3_importer = $importer_manager->createInstance('chado_gff3_loader');
    $run_args = [
      'files' => [
        0 => [
          'file_path' => __DIR__ . '/../../../fixtures/gff3_loader/gff_phase_invalid_character.gff'
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
      'landmark_type' => 'supercontig',
      'alt_id_attr' => NULL,
      'skip_protein' => NULL,
    ];

    $file_details = [
      'file_local' => __DIR__ . '/../../../fixtures/gff3_loader/gff_phase_invalid_character.gff',
    ];

    $has_exception = false;
    try {
      $gff3_importer->createImportJob($run_args, $file_details);
      $gff3_importer->prepareFiles();
      $gff3_importer->run();
      $gff3_importer->postRun();
    }
    catch (\Exception $ex) {
      $message = $ex->getMessage();
      $has_exception = true;
    }
    $this->assertEquals($has_exception, true, "Should not complete when there
      is invalid phase value (in this case character a) but did throw error.");

    /**
     * Run the GFF loader on gff_phase_invalid_number.gff for testing.
     *
     * This tests whether the GFF loader interprets the phase values correctly
     * for CDS rows when a number outside of the range 0,1,2 is specified.
     */
    $gff3_importer = $importer_manager->createInstance('chado_gff3_loader');
    $run_args = [
      'files' => [
        0 => [
          'file_path' => __DIR__ . '/../../../fixtures/gff3_loader/gff_phase_invalid_number.gff'
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
      'landmark_type' => 'supercontig',
      'alt_id_attr' => NULL,
      'skip_protein' => NULL,
    ];

    $file_details = [
      'file_local' => __DIR__ . '/../../../fixtures/gff3_loader/gff_phase_invalid_number.gff',
    ];

    $has_exception = false;
    try {
      $gff3_importer->createImportJob($run_args, $file_details);
      $gff3_importer->prepareFiles();
      $gff3_importer->run();
      $gff3_importer->postRun();
    }
    catch (\Exception $ex) {
      $message = $ex->getMessage();
      $has_exception = true;
    }
    $this->assertEquals($has_exception, true, "Should not complete when there
      is invalid phase value (in this case a number > 2) but did not throw
      error which should have happened.");


    /**
     * Test that when checked, when phase is specified for CDS
     */
    $gff3_importer = $importer_manager->createInstance('chado_gff3_loader');
    $run_args = [
      'files' => [
        0 => [
          'file_path' => __DIR__ . '/../../../fixtures/gff3_loader/gff_phase.gff'
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
      'landmark_type' => 'supercontig',
      'alt_id_attr' => NULL,
      'skip_protein' => NULL,
    ];

    $file_details = [
      'file_local' => __DIR__ . '/../../../fixtures/gff3_loader/gff_phase.gff',
    ];

    $gff3_importer->createImportJob($run_args, $file_details);
    $gff3_importer->prepareFiles();
    $gff3_importer->run();
    $gff3_importer->postRun();

    $results = $chado->query("SELECT * FROM {1:feature} WHERE uniquename = :uniquename", [
      ':uniquename' => 'FRAEX38873_v2_000000010.1.cds1'
    ]);
    $results_object = $results->fetchObject();
    $feature_id = $results_object->feature_id;

    $results = $chado->query("SELECT * FROM {1:featureloc} WHERE feature_id = :feature_id AND
      strand = 1", [
        ':feature_id' => $feature_id
    ]);
    $results_object = $results->fetchObject();
    $strand_value = $results_object->strand;
    $this->assertEquals($strand_value, 1, "Strand value should have been 1 but another value
      was found.");

    /**
     * @TODO
     * Add a skip protein option.  Test that when checked, implicit proteins are
     * not created, but that they are created when unchecked.
     */
    $gff3_importer = $importer_manager->createInstance('chado_gff3_loader');
    $run_args = [
      'files' => [
        0 => [
          'file_path' => __DIR__ . '/../../../fixtures/gff3_loader/gff_protein_generation.gff'
        ]
      ],
      //Skip protein feature generation
      'skip_protein' => 1,
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
      'landmark_type' => 'supercontig',
      'alt_id_attr' => NULL,
      'skip_protein' => NULL,
    ];

    $file_details = [
      'file_local' => __DIR__ . '/../../../fixtures/gff3_loader/gff_protein_generation.gff',
    ];

    $has_exception = false;
    try {
      $gff3_importer->createImportJob($run_args, $file_details);
      $gff3_importer->prepareFiles();
      $gff3_importer->run();
      $gff3_importer->postRun();
    }
    catch (\Exception $ex) {
      $message = $ex->getMessage();
      // print_r($ex->__toString());
      $has_exception = true;
    }

    /**
     * Run the GFF loader on gff_rightarrow_ids.gff for testing.
     *
     * This tests whether the GFF loader fails if ID contains
     * arrow >. It should not fail.
     */
    $gff3_importer = $importer_manager->createInstance('chado_gff3_loader');
    $run_args = [
      'files' => [
        0 => [
          'file_path' => __DIR__ . '/../../../fixtures/gff3_loader/gff_rightarrow_id.gff'
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
      'landmark_type' => 'supercontig',
      'alt_id_attr' => NULL,
      'skip_protein' => NULL,
    ];

    $file_details = [
      'file_local' => __DIR__ . '/../../../fixtures/gff3_loader/gff_rightarrow_id.gff',
    ];

    $has_exception = false;
    try {
      $gff3_importer->createImportJob($run_args, $file_details);
      $gff3_importer->prepareFiles();
      $gff3_importer->run();
      $gff3_importer->postRun();

      $results = $chado->query("SELECT count(*) as c1 FROM {1:feature}
      WHERE uniquename = '>FRAEX38873_v2_000000010';");

      foreach($results as $row) {
        $this->assertEquals($row->c1, 1, 'A feature with uniquename
          >FRAEX38873_v2_000000010 should have been added but was not found.');
      }
    }
    catch (\Exception $ex) {
      $message = $ex->getMessage();
      // echo $message . "\n";
      // echo $ex->getTraceAsString();
      $has_exception = true;
    }

    $this->assertEquals($has_exception, false, "This should not fail and the
    right arrow should be added.");

    /**
     * Run the GFF loader on gff_score.gff for testing.
     *
     * This tests whether the GFF loader interprets the score values
     */
    $gff3_importer = $importer_manager->createInstance('chado_gff3_loader');
    $run_args = [
      'files' => [
        0 => [
          'file_path' => __DIR__ . '/../../../fixtures/gff3_loader/gff_score.gff'
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
      'landmark_type' => 'supercontig',
      'alt_id_attr' => NULL,
      'skip_protein' => NULL,
    ];

    $file_details = [
      'file_local' => __DIR__ . '/../../../fixtures/gff3_loader/gff_score.gff',
    ];

    $gff3_importer->createImportJob($run_args, $file_details);
    $gff3_importer->prepareFiles();
    $gff3_importer->run();
    $gff3_importer->postRun();

    $results = $chado->query("SELECT * FROM {1:analysisfeature}
      WHERE significance = :significance LIMIT 1", [
      ':significance' => 2
    ]);
    foreach ($results as $row) {
      // print_r($row);
      $this->assertEquals($row->significance,2, 'No significance value of 2
        could be found in the db. Import failed.');
    }
    unset($results);

    $results = $chado->query("SELECT * FROM {1:analysisfeature}
      WHERE significance = :significance LIMIT 1", [
      ':significance' => 2.5
    ]);
    foreach ($results as $row) {
      $this->assertEquals($row->significance,2.5, 'No significance value of 2.5
      could be found in the db. Import failed.');
    }
    unset($results);

    $results = $chado->query("SELECT * FROM {1:analysisfeature}
      WHERE significance = :significance LIMIT 1", [
      ':significance' => -2.5
    ]);
    foreach ($results as $row) {
      // print_r($row);
      $this->assertEquals($row->significance,-2.5, 'No significance value of
      -2.5 could be found in the db. Import failed.');
    }
    unset($results);

    /**
     * Run the GFF loader on gff_seqid_invalid_character.gff for testing.
     * Seqids seem to also be called landmarks within GFF loader.
     * This tests whether the GFF loader has any issues with characters like
     * single quotes.
     */
    $gff3_importer = $importer_manager->createInstance('chado_gff3_loader');
    $run_args = [
      'files' => [
        0 => [
          'file_path' => __DIR__ . '/../../../fixtures/gff3_loader/gff_seqid_invalid_character.gff'
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
      'landmark_type' => 'supercontig',
      'alt_id_attr' => NULL,
      'skip_protein' => NULL,
    ];

    $file_details = [
      'file_local' => __DIR__ . '/../../../fixtures/gff3_loader/gff_seqid_invalid_character.gff',
    ];

    $has_exception = false;
    try {
      $gff3_importer->createImportJob($run_args, $file_details);
      $gff3_importer->prepareFiles();
      $gff3_importer->run();
      $gff3_importer->postRun();
    }
    catch (\Exception $ex) {
      $message = $ex->getMessage();
      $has_exception = true;
    }
    $this->assertEquals($has_exception, true, 'An invalid seqid in the
      gff_seqid_invalid_character should have caused an
      exception but did not.');

    /**
     * Run the GFF loader on gff_strand_invalid.gff for testing.
     *
     * This tests whether the GFF loader interprets the strand values
     */
    $gff3_importer = $importer_manager->createInstance('chado_gff3_loader');
    $run_args = [
      'files' => [
        0 => [
          'file_path' => __DIR__ . '/../../../fixtures/gff3_loader/gff_strand_invalid.gff'
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
      'landmark_type' => 'supercontig',
      'alt_id_attr' => NULL,
      'skip_protein' => NULL,
    ];

    $file_details = [
      'file_local' => __DIR__ . '/../../../fixtures/gff3_loader/gff_strand_invalid.gff',
    ];

    $has_exception = false;
    try {
      $gff3_importer->createImportJob($run_args, $file_details);
      $gff3_importer->prepareFiles();
      $gff3_importer->run();
      $gff3_importer->postRun();
    }
    catch (\Exception $ex) {
      $message = $ex->getMessage();
      $has_exception = true;
    }
    $this->assertEquals($has_exception, true, 'An invalid strand in the
      gff_strand_invalid.gff file should have caused an
      exception but did not.');


    /**
     * Run the GFF loader on gff_strand.gff for testing.
     *
     * This tests whether the GFF loader interprets the strand values
     */
    $gff3_importer = $importer_manager->createInstance('chado_gff3_loader');
    $run_args = [
      'files' => [
        0 => [
          'file_path' => __DIR__ . '/../../../fixtures/gff3_loader/gff_strand.gff'
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
      'landmark_type' => 'supercontig',
      'alt_id_attr' => NULL,
      'skip_protein' => NULL,
    ];

    $file_details = [
      'file_local' => __DIR__ . '/../../../fixtures/gff3_loader/gff_strand.gff',
    ];

    $gff3_importer->createImportJob($run_args, $file_details);
    $gff3_importer->prepareFiles();
    $gff3_importer->run();
    $gff3_importer->postRun();

    // Test that integer values for strand that get placed in the db
    // Strand data gets saved in {1:featureloc}
    $results = $chado->query('SELECT * FROM {1:featureloc} fl
      LEFT JOIN {1:feature} f ON (fl.feature_id = f.feature_id)
      WHERE uniquename = :uniquename LIMIT 1',
      array(
        ':uniquename' => 'FRAEX38873_v2_000000010'
      )
    );

    foreach ($results as $row) {
      $this->assertEquals($row->strand, 1); // +
    }

    $results = $chado->query('SELECT * FROM {1:featureloc} fl
      LEFT JOIN {1:feature} f ON (fl.feature_id = f.feature_id)
      WHERE uniquename = :uniquename LIMIT 1',
      array(
        ':uniquename' => 'FRAEX38873_v2_000000010.1'
      )
    );

    foreach ($results as $row) {
      $this->assertEquals($row->strand,-1); // -
    }

    $results = $chado->query('SELECT * FROM {1:featureloc} fl
      LEFT JOIN {1:feature} f ON (fl.feature_id = f.feature_id)
      WHERE uniquename = :uniquename LIMIT 1',
      array(
        ':uniquename' => 'FRAEX38873_v2_000000010.2'
      )
    );

    foreach ($results as $row) {
      $this->assertEquals($row->strand, 0); // ?
    }

    $results = $chado->query('SELECT * FROM {1:featureloc} fl
      LEFT JOIN {1:feature} f ON (fl.feature_id = f.feature_id)
      WHERE uniquename = :uniquename LIMIT 1',
      array(
        ':uniquename' => 'FRAEX38873_v2_000000010.3'
      )
    );

    foreach ($results as $row) {
      $this->assertEquals($row->strand, 0); // .
    }

    /**
     * Run the GFF loader on gff_tag_parent_verification.gff for testing.
     *
     * This tests whether the GFF loader adds Parent attributes
     * The GFF loader should allow it
     */
    $gff3_importer = $importer_manager->createInstance('chado_gff3_loader');
    $run_args = [
      'files' => [
        0 => [
          'file_path' => __DIR__ . '/../../../fixtures/gff3_loader/gff_tag_parent_verification.gff'
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
      'landmark_type' => 'supercontig',
      'alt_id_attr' => NULL,
      'skip_protein' => NULL,
    ];

    $file_details = [
      'file_local' => __DIR__ . '/../../../fixtures/gff3_loader/gff_tag_parent_verification.gff',
    ];

    $gff3_importer->createImportJob($run_args, $file_details);
    $gff3_importer->prepareFiles();
    $gff3_importer->run();
    $gff3_importer->postRun();
    $results = $chado->query("SELECT COUNT(*) as c1 FROM
    (SELECT * FROM {1:feature_relationship} fr
    LEFT JOIN {1:feature} f ON (fr.object_id = f.feature_id)
    WHERE f.uniquename = 'FRAEX38873_v2_000000010' LIMIT 1
    ) as table1;",[]);

    foreach ($results as $row) {
      $this->assertEquals($row->c1, 1);
    }

  /**
   * Run the GFF loader on gff_tagvalue_encoded_character.gff for testing.
   *
   * This tests whether the GFF loader adds IDs that contain encoded character.
   * The GFF loader should allow it
   */
    $gff3_importer = $importer_manager->createInstance('chado_gff3_loader');
    $run_args = [
      'files' => [
        0 => [
          'file_path' => __DIR__ . '/../../../fixtures/gff3_loader/gff_tagvalue_encoded_character.gff'
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
      'landmark_type' => 'supercontig',
      'alt_id_attr' => NULL,
      'skip_protein' => NULL,
    ];

    $file_details = [
      'file_local' => __DIR__ . '/../../../fixtures/gff3_loader/gff_tagvalue_encoded_character.gff',
    ];

    $gff3_importer->createImportJob($run_args, $file_details);
    $gff3_importer->prepareFiles();
    $gff3_importer->run();
    $gff3_importer->postRun();

    $results = $chado->query("SELECT COUNT(*) as c1 FROM {1:feature}
    WHERE uniquename = 'FRAEX38873_v2_000000010,20';",[]);

    foreach ($results as $row) {
      $this->assertEquals($row->c1, 1);
    }

    /**
     * Run the GFF loader on gff_tagvalue_comma_character.gff for testing.
     *
     * This tests whether the GFF loader adds tag values contain comma seperation
     * character.
     * The GFF loader should allow it
     */
    $gff3_importer = $importer_manager->createInstance('chado_gff3_loader');
    $run_args = [
      'files' => [
        0 => [
          'file_path' => __DIR__ . '/../../../fixtures/gff3_loader/gff_tagvalue_comma_character.gff'
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
      'landmark_type' => 'supercontig',
      'alt_id_attr' => NULL,
      'skip_protein' => NULL,
    ];

    $file_details = [
      'file_local' => __DIR__ . '/../../../fixtures/gff3_loader/gff_tagvalue_comma_character.gff',
    ];

    $gff3_importer->createImportJob($run_args, $file_details);
    $gff3_importer->prepareFiles();
    $gff3_importer->run();
    $gff3_importer->postRun();

    $results = $chado->query("SELECT COUNT(*) as c1 FROM {1:featureprop}
      WHERE value ILIKE :value",[
      ':value' => 'T'
    ]);
    foreach ($results as $row) {
      $this->assertEquals($row->c1, 1);
    }

    $results = $chado->query("SELECT COUNT(*) as c1 FROM {1:featureprop}
      WHERE value ILIKE :value",[
      ':value' => 'EST'
    ]);
    foreach ($results as $row) {
      $this->assertEquals($row->c1, 1);
    }

    /**
     * Run the GFF loader on gff_tagvalue_comma_character.gff for testing.
     *
     * This tests whether the GFF loader adds tag values containing encoded comma
     * character.
     * The GFF loader should allow it
     */
    $gff3_importer = $importer_manager->createInstance('chado_gff3_loader');
    $run_args = [
      'files' => [
        0 => [
          'file_path' => __DIR__ . '/../../../fixtures/gff3_loader/gff_tagvalue_encoded_comma.gff'
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
      'landmark_type' => 'supercontig',
      'alt_id_attr' => NULL,
      'skip_protein' => NULL,
    ];

    $file_details = [
      'file_local' => __DIR__ . '/../../../fixtures/gff3_loader/gff_tagvalue_encoded_comma.gff',
    ];


    $gff3_importer->createImportJob($run_args, $file_details);
    $gff3_importer->prepareFiles();
    $gff3_importer->run();
    $gff3_importer->postRun();

    $results = $chado->query("SELECT COUNT(*) as c1 FROM {1:featureprop}
      WHERE value ILIKE :value",[
      ':value' => 'T,EST'
    ]);
    foreach ($results as $row) {
      $this->assertEquals($row->c1, 1);
    }


    /**
     * Run the GFF loader on gff_1380_landmark_test.gff for testing.
     *
     * This tests whether the GFF loader adds landmarks directly from the GFF file
     * character.
     * The GFF loader should allow it
     */
    $gff3_importer = $importer_manager->createInstance('chado_gff3_loader');
    $run_args = [
      'files' => [
        0 => [
          'file_path' => __DIR__ . '/../../../fixtures/gff3_loader/gff_1380_landmark_test.gff'
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
      'file_local' => __DIR__ . '/../../../fixtures/gff3_loader/gff_1380_landmark_test.gff',
    ];


    $gff3_importer->createImportJob($run_args, $file_details);
    $gff3_importer->prepareFiles();
    $gff3_importer->run();
    $gff3_importer->postRun();

    // Check that the chr1_h1 feature (which is a landmark was added to feature table)
    $results = $chado->query("SELECT count(*) as c1 FROM {1:feature}
      WHERE uniquename ILIKE :value",[
      ':value' => 'chr1_h1'
    ]);
    foreach ($results as $row) {
      $this->assertEquals($row->c1, 1);
    }

    // Get the type_id for chromosome
    $chromosome_type_id = NULL;
    $results = $chado->query("SELECT * FROM {1:cvterm} WHERE name = 'chromosome';");
    foreach ($results as $row) {
      $chromosome_type_id = $row->cvterm_id;
    }

    // Check if the same chr1_h1 has type_id of chromosome_type_id
    $results = $chado->query("SELECT count(*) as c1 FROM {1:feature}
      WHERE uniquename ILIKE :value AND type_id = :type_id",[
      ':value' => 'chr1_h1',
      ':type_id' => $chromosome_type_id
    ]);

    foreach ($results as $row) {
      $this->assertEquals($row->c1, 1);
    }

    // Check to make sure landmark exists in featureloc table
    $results = $chado->query("SELECT count(*) as c1 FROM {1:featureloc} fl
      LEFT JOIN {1:feature} f
      ON fl.feature_id = f.feature_id
      WHERE uniquename = :landmark_name",
      [':landmark_name' => 'chr1_h1']);
    foreach ($results as $row) {
      $this->assertEquals($row->c1, 1);
    }

    // Check to make sure landmark exists in featureloc table
    $results = $chado->query("SELECT count(*) as c1 FROM {1:featureloc} fl
      LEFT JOIN {1:feature} f
      ON fl.feature_id = f.feature_id
      WHERE uniquename = :landmark_name",
      [':landmark_name' => 'chr2_h1']);
    foreach ($results as $row) {
      $this->assertEquals($row->c1, 1);
    }

    // Check to make sure landmark exists in featureloc table
    $results = $chado->query("SELECT count(*) as c1 FROM {1:featureloc} fl
      LEFT JOIN {1:feature} f
      ON fl.feature_id = f.feature_id
      WHERE uniquename = :landmark_name",
      [':landmark_name' => 'chr3_h1']);
    foreach ($results as $row) {
      $this->assertEquals($row->c1, 1);
    }

    // Check to make sure landmark exists in featureloc table
    $results = $chado->query("SELECT count(*) as c1 FROM {1:featureloc} fl
      LEFT JOIN {1:feature} f
      ON fl.feature_id = f.feature_id
      WHERE uniquename = :landmark_name",
      [':landmark_name' => 'chr4_h1']);
    foreach ($results as $row) {
      $this->assertEquals($row->c1, 1);
    }



    /**
     * Run the GFF loader on Citrus GFF3 for testing.
     *
     * This tests whether the GFF loader adds Citrus data
     * character.
     * The GFF loader should allow it
     */
    $gff3_importer = $importer_manager->createInstance('chado_gff3_loader');
    $run_args = [
      'files' => [
        0 => [
          'file_path' => __DIR__ . '/../../../fixtures/gff3_loader/gff_Citrus_sinensis-orange1.1g015632m.g.gff3'
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
      'landmark_type' => 'supercontig',
      'alt_id_attr' => NULL,
      'skip_protein' => NULL,
    ];

    $file_details = [
      'file_local' => __DIR__ . '/../../../fixtures/gff3_loader/gff_Citrus_sinensis-orange1.1g015632m.g.gff3',
    ];


    $gff3_importer->createImportJob($run_args, $file_details);
    $gff3_importer->prepareFiles();
    $gff3_importer->run();
    $gff3_importer->postRun();

    // Check to make sure landmark (scaffold0001) exists in featureloc table
    $results = $chado->query("SELECT count(*) as c1 FROM {1:featureloc} fl
      LEFT JOIN {1:feature} f
      ON fl.feature_id = f.feature_id
      LEFT JOIN {1:cvterm} c
      ON c.cvterm_id = f.type_id
      WHERE uniquename = :landmark_name",
      [':landmark_name' => 'scaffold00001']);
    foreach ($results as $row) {
      $this->assertEquals($row->c1, 1);
    }

    // Check to make sure feature orange1.1g015632m.g of type name gene
    $results = $chado->query("SELECT count(*) as c1 FROM
      (SELECT * FROM {1:feature} f
        LEFT JOIN {1:cvterm} c
        ON c.cvterm_id = f.type_id
        WHERE uniquename = :landmark_name
        AND c.name = :name
      ) as table1",
      [
      ':landmark_name' => 'orange1.1g015632m.g',
      ':name' => 'gene'
      ]);
    foreach ($results as $row) {
      $this->assertEquals($row->c1, 1);
    }

    // Check to make sure feature orange1.1g015632m.g of type name mRNA
    $results = $chado->query("SELECT count(*) as c1 FROM
      (SELECT * FROM {1:feature} f
        LEFT JOIN {1:cvterm} c
        ON c.cvterm_id = f.type_id
        WHERE uniquename = :unique_name
        AND c.name = :name
      ) as table1",
      [
      ':unique_name' => 'PAC:18136217',
      ':name' => 'mRNA'
      ]);
    foreach ($results as $row) {
      $this->assertEquals($row->c1, 1);
      // print_r($row);
    }

    // Check to make sure feature PAC:18136217-cds of type name CDS
    $results = $chado->query("SELECT count(*) as c1 FROM
      (SELECT * FROM {1:feature} f
        LEFT JOIN {1:cvterm} c
        ON c.cvterm_id = f.type_id
        WHERE f.name = :feature_name
        AND c.name = :name
      ) as table1",
      [
      ':feature_name' => 'PAC:18136217-cds',
      ':name' => 'CDS'
      ]);
    foreach ($results as $row) {
      $this->assertEquals($row->c1, 12);
    }
  }
}
