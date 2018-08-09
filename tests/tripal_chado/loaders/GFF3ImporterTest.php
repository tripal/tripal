<?php

namespace Tests;

use StatonLab\TripalTestSuite\DBTransaction;
use StatonLab\TripalTestSuite\TripalTestCase;

class GFF3ImporterTest extends TripalTestCase {

  // Uncomment to auto start and rollback db transactions per test method.
  use DBTransaction;

  /**
   * Confirm GFF loads.
   *
   * @group gff
   */
  public function testGFFImporter() {
    $gff_file = ['file_remote' => 'https://raw.githubusercontent.com/statonlab/tripal_dev_seed/master/Fexcel_mini/gff/filtered.gff'];
    $analysis = factory('chado.analysis')->create();
    $organism = factory('chado.organism')->create();
    $run_args = [
      'analysis_id' => $analysis->analysis_id,
      'organism_id' => $organism->organism_id,
      'use_transaction' => 1,
      'add_only' => 0,
      'update' => 1,
      'create_organism' => 0,
      'create_target' => 0,
      ///regexps for mRNA and protein.
      're_mrna' => NULL,
      're_protein' => NULL,
      //optional
      'target_organism_id' => NULL,
      'target_type' => NULL,
      'start_line' => NULL,
      'landmark_type' => NULL,
      'alt_id_attr' => NULL,
    ];
    $this->loadLandmarks($analysis, $organism);
    $this->runGFFLoader($run_args, $gff_file);

    $name = 'FRAEX38873_v2_000000110.2.exon4';
    $query = db_select('chado.feature', 'f')
      ->fields('f', ['uniquename'])
      ->condition('f.uniquename', $name)
      ->execute()
      ->fetchField();
    $this->assertEquals($name, $query);
  }


  /**
   * @group gff
   * @group failing
   * @ticket 77
   *
   */
  public function testGFFNoProteinOption() {

    $gff_file = ['file_remote' => 'https://raw.githubusercontent.com/statonlab/tripal_dev_seed/master/Fexcel_mini/gff/filtered.gff'];
    $analysis = factory('chado.analysis')->create();
    $organism = factory('chado.organism')->create();
    $run_args = [
      //The new argument
      'skip_protein' => 1,
      ///
      'analysis_id' => $analysis->analysis_id,
      'organism_id' => $organism->organism_id,
      'use_transaction' => 1,
      'add_only' => 0,
      'update' => 1,
      'create_organism' => 0,
      'create_target' => 0,
      ///regexps for mRNA and protein.
      're_mrna' => NULL,
      're_protein' => NULL,
      //optional
      'target_organism_id' => NULL,
      'target_type' => NULL,
      'start_line' => NULL,
      'landmark_type' => NULL,
      'alt_id_attr' => NULL,
    ];
    $this->loadLandmarks($analysis, $organism);

    $this->runGFFLoader($run_args, $gff_file);


    $identifier = [
      'cv_id' => ['name' => 'sequence'],
      'name' => 'polypeptide',
    ];
    $protein_type_id = tripal_get_cvterm($identifier);

    //This works i think i just dont have proteins described in the GFF.

    $name = 'FRAEX38873_v2_000000110.1-protein';
    $query = db_select('chado.feature', 'f')
      ->fields('f', ['uniquename'])
      ->condition('f.uniquename', $name)
      ->condition('f.type_id', $protein_type_id->cvterm_id)
      ->execute()
      ->fetchField();
    $this->assertFalse($query);

    $run_args['skip_protein'] = 0;

    $this->runGFFLoader($run_args, $gff_file);

    $query = db_select('chado.feature', 'f')
      ->fields('f', ['uniquename'])
      ->condition('f.uniquename', $name)
      ->condition('f.type_id', $protein_type_id->cvterm_id)
      ->execute()
      ->fetchObject();
    $this->assertEquals($name, $query->uniquename);

  }

  private function runGFFLoader($run_args, $file) {
    module_load_include('inc', 'tripal_chado', 'includes/TripalImporter/GFF3Importer');
    $importer = new \GFF3Importer();
    $importer->create($run_args, $file);
    $importer->prepareFiles();
    $importer->run();

  }

  private function loadLandmarks($analysis, $organism) {
    $landmark_file = ['file_remote' => 'https://raw.githubusercontent.com/statonlab/tripal_dev_seed/master/Fexcel_mini/sequences/empty_landmarks.fasta'];

    $run_args = [
      'organism_id' => $organism->organism_id,
      'analysis_id' => $analysis->analysis_id,
      'seqtype' => 'scaffold',
      'method' => 2, //default insert and update
      'match_type' => 1, //unique name default
      //optional
      're_name' => NULL,
      're_uname' => NULL,
      're_accession' => NULL,
      'db_id' => NULL,
      'rel_type' => NULL,
      're_subject' => NULL,
      'parent_type' => NULL,
    ];
    module_load_include('inc', 'tripal_chado', 'includes/TripalImporter/FASTAImporter');
    $importer = new \FASTAImporter();
    $importer->create($run_args, $landmark_file);
    $importer->prepareFiles();
    $importer->run();
  }

}
