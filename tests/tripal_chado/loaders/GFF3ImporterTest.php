<?php

namespace Tests;

use StatonLab\TripalTestSuite\DBTransaction;
use StatonLab\TripalTestSuite\TripalTestCase;

class GFF3ImporterTest extends TripalTestCase {

  use DBTransaction;

  /**
   * Confirm basic GFF importer functionality.
   *
   * @group gff
   */
  public function testGFFImporter() {
    $gff_file = ['file_local' => __DIR__ . '/../data/small_gene.gff'];
    $fasta = ['file_local' => __DIR__ . '/../data/short_scaffold.fasta'];    
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
      // regexps for mRNA and protein.
      're_mrna' => NULL,
      're_protein' => NULL,
      // optional
      'target_organism_id' => NULL,
      'target_type' => NULL,
      'start_line' => NULL,
      'landmark_type' => NULL,
      'alt_id_attr' => NULL,
    ];
    $this->loadLandmarks($analysis, $organism, $fasta);
    $this->runGFFLoader($run_args, $gff_file);

    // This protein is an explicit protein / polypeptide imported from the GFF
    // file. 
    $name = 'test_protein_001.1';
    $query = db_select('chado.feature', 'f')
      ->fields('f', ['uniquename'])
      ->condition('f.uniquename', $name)
      ->execute()
      ->fetchField();
    $this->assertEquals($name, $query);
  }

  /**
   * Run the GFF loader on gff_tag_unescaped_character.gff for testing.
   *
   * This tests whether the GFF loader adds IDs that contain a comma. 
   * The GFF loader should allow it
   */  
  public function testGFFImporterUnescapedTagWithComma() {
    $gff_file = ['file_local' => __DIR__ . '/../data/gff_tag_unescaped_character.gff'];
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
      // regexps for mRNA and protein.
      're_mrna' => NULL,
      're_protein' => NULL,
      // optional
      'target_organism_id' => NULL,
      'target_type' => NULL,
      'start_line' => NULL,
      'landmark_type' => NULL,
      'alt_id_attr' => NULL,
    ];

  
    $this->loadLandmarks($analysis, $organism);
    // This should throw an error based on the tag name having the comma
    $hasException = false;
    try {
      $this->runGFFLoader($run_args, $gff_file);
    }
    catch (\Exception $ex) {
      $hasException = true;
    }

    $this->assertEquals($hasException, false);
  }

  /**
   * Run the GFF loader on small_gene.gff for testing.
   *
   * This tests whether the GFF loader adds Alias attributes
   * The GFF loader should allow it
   */  
  public function testGFFImporterTagAliasVerification() {
    $gff_file = ['file_local' =>
      __DIR__ . '/../data/small_gene.gff'];
    $fasta = ['file_local' => __DIR__ . '/../data/short_scaffold.fasta'];      
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
      // regexps for mRNA and protein.
      're_mrna' => NULL,
      're_protein' => NULL,
      // optional
      'target_organism_id' => NULL,
      'target_type' => NULL,
      'start_line' => NULL,
      'landmark_type' => NULL,
      'alt_id_attr' => NULL,
    ];

  
    $this->loadLandmarks($analysis, $organism, $fasta);
    $this->runGFFLoader($run_args, $gff_file);

    $results = db_query("SELECT * FROM chado.feature_synonym fs
      LEFT JOIN chado.synonym s ON (fs.feature_synonym_id = s.synonym_id)
      WHERE name = 'first_test_gene'
    ;",array());

    $this->assertEquals($results->rowCount(), 1);
  }


  /**
   * Run the GFF loader on gff_tag_parent_verification.gff for testing.
   *
   * This tests whether the GFF loader adds Parent attributes
   * The GFF loader should allow it
   */  
  public function testGFFImporterTagParentVerification() {
    $gff_file = ['file_local' =>
      __DIR__ . '/../data/gff_tag_parent_verification.gff'];
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
      // regexps for mRNA and protein.
      're_mrna' => NULL,
      're_protein' => NULL,
      // optional
      'target_organism_id' => NULL,
      'target_type' => NULL,
      'start_line' => NULL,
      'landmark_type' => NULL,
      'alt_id_attr' => NULL,
    ];

  
    $this->loadLandmarks($analysis, $organism);
    $this->runGFFLoader($run_args, $gff_file);

    $results = db_query("SELECT * FROM chado.feature_relationship fr
      LEFT JOIN chado.feature f ON (fr.object_id = f.feature_id)
      WHERE f.uniquename = 'FRAEX38873_v2_000000010'
    ;",array());

    // Found parent via object_id FRAEX38873_v2_000000010
    $this->assertEquals($results->rowCount(), 1);
  }

  /**
   * Run the GFF loader on gff_tagvalue_unescaped_character.gff for testing.
   *
   * This tests whether the GFF loader adds IDs that contain a comma. 
   * The GFF loader should allow it
   */  
  public function testGFFImporterEscapedTagValueWithEncodedCharacter() {
    $gff_file = ['file_local' => 
      __DIR__ . '/../data/gff_tagvalue_encoded_character.gff'];
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
      // regexps for mRNA and protein.
      're_mrna' => NULL,
      're_protein' => NULL,
      // optional
      'target_organism_id' => NULL,
      'target_type' => NULL,
      'start_line' => NULL,
      'landmark_type' => NULL,
      'alt_id_attr' => NULL,
    ];

  
    $this->loadLandmarks($analysis, $organism);
    $this->runGFFLoader($run_args, $gff_file);

    $results = db_query("SELECT * FROM chado.feature 
      WHERE uniquename = 'FRAEX38873_v2_000000010,20';",array());

    $this->assertEquals($results->rowCount(), 1);
  }


  /**
   * Run the GFF loader on gff_tagvalue_unescaped_character.gff for testing.
   *
   * This tests whether the GFF loader adds IDs that contain a comma. 
   * The GFF loader should allow it
   */  
  public function testGFFImporterUnescapedTagValueWithComma() {
    $gff_file = ['file_local' => __DIR__ . '/../data/gff_tagvalue_unescaped_character.gff'];
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
      // regexps for mRNA and protein.
      're_mrna' => NULL,
      're_protein' => NULL,
      // optional
      'target_organism_id' => NULL,
      'target_type' => NULL,
      'start_line' => NULL,
      'landmark_type' => NULL,
      'alt_id_attr' => NULL,
    ];

  
    $this->loadLandmarks($analysis, $organism);
    // This should throw an error based on the tag name having the comma
    $hasException = false;
    try {
      $this->runGFFLoader($run_args, $gff_file);
    }
    catch (\Exception $ex) {
      $hasException = true;
    }

    $this->assertEquals($hasException, false);
  }

  /**
   * Run the GFF loader on gff_seqid_invalid_character.gff for testing.
   * Seqids seem to also be called landmarks within GFF loader.
   * This tests whether the GFF loader has any issues with characters like  
   * single quotes.
   */  
  public function testGFFImporterSeqidWithInvalidCharacter() {
    $gff_file = ['file_local' => 
      __DIR__ . '/../data/gff_seqid_invalid_character.gff'];
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
      // regexps for mRNA and protein.
      're_mrna' => NULL,
      're_protein' => NULL,
      // optional
      'target_organism_id' => NULL,
      'target_type' => NULL,
      'start_line' => NULL,
      'landmark_type' => NULL,
      'alt_id_attr' => NULL,
    ];

  
    $this->loadLandmarks($analysis, $organism);
    // This will produce an exception due to quote character in Seqid
    $hasException = false;
    try {
      $this->runGFFLoader($run_args, $gff_file);
    }
    catch (\Exception $ex) {
      $hasException = true;
    }

    $this->assertEquals($hasException, true);

  }

  /**
   * Run the GFF loader on gff_unescaped_ids.gff for testing.
   *
   * This tests whether the GFF loader adds IDs that contain whitespaces. 
   * The GFF loader should allow it
   */  
  public function testGFFImporterUnescapedWhitespaceID() {
    $gff_file = ['file_local' => __DIR__ . '/../data/gff_unescaped_ids.gff'];
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
      // regexps for mRNA and protein.
      're_mrna' => NULL,
      're_protein' => NULL,
      // optional
      'target_organism_id' => NULL,
      'target_type' => NULL,
      'start_line' => NULL,
      'landmark_type' => NULL,
      'alt_id_attr' => NULL,
    ];

  
    $this->loadLandmarks($analysis, $organism);
    // This should go through just fine
    $this->runGFFLoader($run_args, $gff_file);

    $results = db_query("SELECT * FROM chado.feature WHERE uniquename = 
      'FRAEX38873_v2_000000010 SPACED';");
    $this->assertEquals($results->rowCount(), 1);
  }

  /**
   * Run the GFF loader on gff_rightarrow_ids.gff for testing.
   *
   * This tests whether the GFF loader fails if ID contains  
   * arrow >. It should not fail.
   */  
  public function testGFFImporterRightArrowID() {
    $gff_file = ['file_local' => __DIR__ . '/../data/gff_rightarrow_id.gff'];
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
      // regexps for mRNA and protein.
      're_mrna' => NULL,
      're_protein' => NULL,
      // optional
      'target_organism_id' => NULL,
      'target_type' => NULL,
      'start_line' => NULL,
      'landmark_type' => NULL,
      'alt_id_attr' => NULL,
    ];

 
    $this->loadLandmarks($analysis, $organism);
    // This will produce an exception due to right arrow in ID
    $this->runGFFLoader($run_args, $gff_file);

    $results = db_query("SELECT * FROM chado.feature 
      WHERE uniquename = '>FRAEX38873_v2_000000010';");

    // We expect this record to get inserted.
    $this->assertEquals($results->rowCount(), 1);
  }


  /**
   * Run the GFF loader on gff_duplicate_ids.gff for testing.
   *
   * This tests whether the GFF loader detects duplicate IDs which makes a 
   * GFF file invalid since IDs should be unique. The GFF loader should throw 
   * and exception which this test checks for
   */  
  public function testGFFImporterDuplicateIDsExceptionCheck() {
    $gff_file = ['file_local' => __DIR__ . '/../data/gff_duplicate_ids.gff'];
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
      // regexps for mRNA and protein.
      're_mrna' => NULL,
      're_protein' => NULL,
      // optional
      'target_organism_id' => NULL,
      'target_type' => NULL,
      'start_line' => NULL,
      'landmark_type' => NULL,
      'alt_id_attr' => NULL,
    ];

    $hasException = false;
    try {    
      $this->loadLandmarks($analysis, $organism);
      // This will produce an exception of duplicate feature ID
      $this->runGFFLoader($run_args, $gff_file);
    }
    catch(\Exception $ex) {
      $hasException = true;
    }

    // We expect an exception to happen so we are looking for a return of true
    $this->assertEquals($hasException, true);
  }

  /**
   * Run the GFF loader on gff_invalidstartend.gff for testing.
   *
   * This tests whether the GFF loader fixes start end values 
   */  
  public function testGFFImporterInvalidStartEnd() {
    $gff_file = ['file_local' => __DIR__ . '/../data/gff_invalidstartend.gff'];
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
      // regexps for mRNA and protein.
      're_mrna' => NULL,
      're_protein' => NULL,
      // optional
      'target_organism_id' => NULL,
      'target_type' => NULL,
      'start_line' => NULL,
      'landmark_type' => NULL,
      'alt_id_attr' => NULL,
    ];

   
    $this->loadLandmarks($analysis, $organism);
    // This will produce an exception of duplicate feature ID
    $this->runGFFLoader($run_args, $gff_file);

    $results = db_select('chado.feature', 'f')
      ->fields('f', ['uniquename'])
      ->condition('f.uniquename', 'FRAEX38873_v2_000000010')
      ->execute()
      ->fetchAll();    

    // We expect the feature to still be added to the database
    // since the GFF Loader caters for reversing backward numbers
    $this->assertEquals(count($results), 1);
  }

  /**
   * Run the GFF loader on gff_score.gff for testing.
   *
   * This tests whether the GFF loader interprets the score values
   */  
  public function testGFFImporterScoreTest() {
    $gff_file = ['file_local' => __DIR__ . '/../data/gff_score.gff'];
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
      // regexps for mRNA and protein.
      're_mrna' => NULL,
      're_protein' => NULL,
      // optional
      'target_organism_id' => NULL,
      'target_type' => NULL,
      'start_line' => NULL,
      'landmark_type' => NULL,
      'alt_id_attr' => NULL,
    ];

   
    $this->loadLandmarks($analysis, $organism);
    $this->runGFFLoader($run_args, $gff_file);

    // Test that integer values get placed in the db
    $results = db_query('SELECT * FROM chado.analysisfeature WHERE significance = 2 LIMIT 1', array(
    ));
    foreach ($results as $row){
      $this->assertEquals($row->significance,2);
    }

    // Test that decimal/float values get placed in the db
    $results = db_query('SELECT * FROM chado.analysisfeature WHERE significance = 2.5 LIMIT 1', array(
    ));
    foreach ($results as $row){
      $this->assertEquals($row->significance,2.5);
    } 
    
    // Test that negative score values get placed in the db
    $results = db_query('SELECT * FROM chado.analysisfeature WHERE significance = -2.5 LIMIT 1', array(
    ));
    foreach ($results as $row){
      $this->assertEquals($row->significance,-2.5);
    }     

  }

    /**
   * Run the GFF loader on gff_strand.gff for testing.
   *
   * This tests whether the GFF loader interprets the strand values
   */  
  public function testGFFImporterInvalidStrandTest() {
    $gff_file = ['file_local' => __DIR__ . '/../data/gff_strand_invalid.gff'];
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
      // regexps for mRNA and protein.
      're_mrna' => NULL,
      're_protein' => NULL,
      // optional
      'target_organism_id' => NULL,
      'target_type' => NULL,
      'start_line' => NULL,
      'landmark_type' => NULL,
      'alt_id_attr' => NULL,
    ];

   
    $this->loadLandmarks($analysis, $organism);
    
    $isException = false;
    try {
      $this->runGFFLoader($run_args, $gff_file);
    }
    catch(\Exception $ex) {
      $isException = true;
    }

    $this->assertEquals($isException, true);

  }

  /**
   * Run the GFF loader on gff_strand.gff for testing.
   *
   * This tests whether the GFF loader interprets the strand values
   */  
  public function testGFFImporterStrandTest() {
    $gff_file = ['file_local' => __DIR__ . '/../data/gff_strand.gff'];
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
      // regexps for mRNA and protein.
      're_mrna' => NULL,
      're_protein' => NULL,
      // optional
      'target_organism_id' => NULL,
      'target_type' => NULL,
      'start_line' => NULL,
      'landmark_type' => NULL,
      'alt_id_attr' => NULL,
    ];

   
    $this->loadLandmarks($analysis, $organism);
    $this->runGFFLoader($run_args, $gff_file);

    // Test that integer values for strand that get placed in the db
    // Strand data gets saved in chado.featureloc
    $results = db_query('SELECT * FROM chado.featureloc fl 
      LEFT JOIN chado.feature f ON (fl.feature_id = f.feature_id)
      WHERE uniquename = :uniquename LIMIT 1', 
      array(
        ':uniquename' => 'FRAEX38873_v2_000000010'
      )
    );

    foreach ($results as $row) {
      $this->assertEquals($row->strand, 1); // +
    }

    $results = db_query('SELECT * FROM chado.featureloc fl 
      LEFT JOIN chado.feature f ON (fl.feature_id = f.feature_id)
      WHERE uniquename = :uniquename LIMIT 1', 
      array(
        ':uniquename' => 'FRAEX38873_v2_000000010.1'
      )
    );

    foreach ($results as $row) {
      $this->assertEquals($row->strand,-1); // -
    } 
    
    $results = db_query('SELECT * FROM chado.featureloc fl 
      LEFT JOIN chado.feature f ON (fl.feature_id = f.feature_id)
      WHERE uniquename = :uniquename LIMIT 1', 
      array(
        ':uniquename' => 'FRAEX38873_v2_000000010.2'
      )
    );

    foreach ($results as $row) {
      $this->assertEquals($row->strand, 0); // ?
    }
    
    $results = db_query('SELECT * FROM chado.featureloc fl 
      LEFT JOIN chado.feature f ON (fl.feature_id = f.feature_id)
      WHERE uniquename = :uniquename LIMIT 1', 
      array(
        ':uniquename' => 'FRAEX38873_v2_000000010.3'
      )
    );

    foreach ($results as $row) {
      $this->assertEquals($row->strand, 0); // .
    }     

  }

  /**
   * Run the GFF loader on gff_phase.gff for testing.
   *
   * This tests whether the GFF loader interprets the phase values correctly
   * for CDS rows.
   */  
  public function testGFFImporterPhaseTest() {
    $gff_file = ['file_local' => __DIR__ . '/../data/gff_phase.gff'];
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
      // regexps for mRNA and protein.
      're_mrna' => NULL,
      're_protein' => NULL,
      // optional
      'target_organism_id' => NULL,
      'target_type' => NULL,
      'start_line' => NULL,
      'landmark_type' => NULL,
      'alt_id_attr' => NULL,
    ];

   
    $this->loadLandmarks($analysis, $organism);
    $this->runGFFLoader($run_args, $gff_file);

    $results = db_query("SELECT * FROM chado.feature 
      WHERE uniquename = :uniquename LIMIT 1", array(
      ':uniquename' => 'FRAEX38873_v2_000000010.1.cds1'
      )
    );

    // Check to make sure it returns a single row (implying a match)
    // by the uniquename specified
    $this->assertEquals($results->rowCount(), 1);

    $results = db_query("SELECT * FROM chado.featureloc 
      WHERE phase = 1 LIMIT 1", array(
      )
    );

    // Check to make sure it returns a single row (implying a match)
    // by phase value 1
    $this->assertEquals($results->rowCount(), 1);    
  }

  /**
   * Run the GFF loader on gff_phase_invalid_number.gff for testing.
   *
   * This tests whether the GFF loader interprets the phase values correctly
   * for CDS rows when a number outside of the range 0,1,2 is specified.
   */  
  public function testGFFImporterInvalidPhaseNumberTest() {
    $gff_file = ['file_local' => __DIR__ . '/../data/gff_phase_invalid_number.gff'];
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
      // regexps for mRNA and protein.
      're_mrna' => NULL,
      're_protein' => NULL,
      // optional
      'target_organism_id' => NULL,
      'target_type' => NULL,
      'start_line' => NULL,
      'landmark_type' => NULL,
      'alt_id_attr' => NULL,
    ];

    $this->loadLandmarks($analysis, $organism);
    $hasException = false;
    try {
      $this->runGFFLoader($run_args, $gff_file);
    }
    catch (\Exception $ex) {
      $hasException = true;
    }

    // An exception should have been thrown since the phase number is invalid
    $this->assertEquals($hasException, true);
  }

  /**
   * Run the GFF loader on gff_phase_invalid_character.gff for testing.
   *
   * This tests whether the GFF loader interprets the phase values correctly
   * for CDS rows when a character outside of the range 0,1,2 is specified.
   */  
  public function testGFFImporterInvalidPhaseCharacterTest() {
    $gff_file = ['file_local' => __DIR__ . '/../data/gff_phase_invalid_character.gff'];
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
      // regexps for mRNA and protein.
      're_mrna' => NULL,
      're_protein' => NULL,
      // optional
      'target_organism_id' => NULL,
      'target_type' => NULL,
      'start_line' => NULL,
      'landmark_type' => NULL,
      'alt_id_attr' => NULL,
    ];

    $this->loadLandmarks($analysis, $organism);
    $hasException = false;
    try {
      $this->runGFFLoader($run_args, $gff_file);
    }
    catch (\Exception $ex) {
      $hasException = true;
    }

    // An exception should have been thrown since the phase number is invalid
    $this->assertEquals($hasException, true);
  }



  /**
   * Run the GFF loader on small_gene.gff for testing.
   *
   * This gff has many attributes that we would like to test in the
   * testGFFImporterAttribute*() methods.
   */
  private function initGFFImporterAttributes() {
    $gff = ['file_local' => __DIR__ . '/../data/small_gene.gff'];
    $fasta = ['file_local' => __DIR__ . '/../data/short_scaffold.fasta'];
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
      'target_organism_id' => $organism->organism_id,
      'target_type' => NULL,
      'start_line' => NULL,
      'landmark_type' => NULL,
      'alt_id_attr' => NULL,
    ];
    $this->loadLandmarks($analysis, $organism, $fasta);
    $this->runGFFLoader($run_args, $gff);
    $this->organism = $organism;
    $this->analysis = $analysis;
    $this->gene_cvt = chado_get_cvterm(array(
      'name' => 'gene',
      'cv_id' => array(
        'name' => 'sequence',
      ),
    ))->cvterm_id;
    $this->mrna_cvt = chado_get_cvterm(array(
      'name' => 'mRNA',
      'cv_id' => array(
        'name' => 'sequence',
      ),
    ))->cvterm_id;
    $this->supercontig_cvt = chado_get_cvterm(array(
      'name' => 'supercontig',
      'cv_id' => array(
        'name' => 'sequence',
      ),
    ))->cvterm_id;
    $this->gene_1_uname = 'test_gene_001';
    $this->gene_2_uname = 'test_gene_002';
    $this->scaffold_1_uname = 'scaffold1';
  }

  /**
   * Ensures that the feature record is loaded correctly into chado.
   *
   * @group gff
   */
  public function testGFFImporterAttributeFeature() {
    $this->initGFFImporterAttributes();
    $organism = $this->organism;

    $query = db_select('chado.feature', 'f')
      ->fields('f')
      ->condition('uniquename', $this->gene_1_uname)
      ->condition('type_id', $this->gene_cvt)
      ->execute();

    $gene_1 = $query->fetchObject();
    $this->assertEquals('test_gene_001', $gene_1->uniquename);
    $this->assertEquals('test_gene_001', $gene_1->name);
    $this->assertEquals($organism->organism_id, $gene_1->organism_id);
    $this->assertEquals($this->gene_cvt, $gene_1->type_id);
  }

  /**
   * Ensures the feature alias is loaded correctly into chado.
   *
   * @group gff
   */
  public function testGFFImporterAttributeAlias() {
    $this->initGFFImporterAttributes();
    $alias = 'first_test_gene';

    $gene_1 = db_select('chado.feature', 'f')
      ->fields('f')
      ->condition('uniquename', $this->gene_1_uname)
      ->condition('type_id', $this->gene_cvt)
      ->execute()->fetchObject();

    $query = db_select('chado.feature_synonym', 'fs');
    $query->join('chado.synonym', 's', 's.synonym_id = fs.synonym_id');
    $query->fields('s');
    $query->condition('fs.feature_id', $gene_1->feature_id);
    $query = $query->execute();
    $result = $query->fetchObject();
    $this->assertEquals($alias, $result->name);
  }

  /**
   * Ensures that the dbxref records are loaded correctly into chado.
   *
   * @group gff
   */
  public function testGFFImporterAttributeDbxref() {
    $this->initGFFImporterAttributes();
    $test_db_name = 'TEST_DB';
    $dbx_accession = 'test_gene_dbx_001';
    $test_db = chado_get_db(array('name' => $test_db_name));
    $gff_db = chado_get_db(array('name' => 'GFF_source'));

    $gene_1 = db_select('chado.feature', 'f')
      ->fields('f')
      ->condition('uniquename', $this->gene_1_uname)
      ->condition('type_id', $this->gene_cvt)
      ->execute()->fetchObject();

    $dbx_query = db_select('chado.feature_dbxref', 'fdbx');
    $dbx_query->join('chado.dbxref', 'dbx', 'dbx.dbxref_id = fdbx.dbxref_id');
    $dbx_query->fields('dbx');
    $dbx_query->condition('fdbx.feature_id', $gene_1->feature_id);
    $gff_query = clone $dbx_query;

    $dbx_query->condition('dbx.db_id', $test_db->db_id);
    $dbx_query = $dbx_query->execute();

    $gff_query->condition('dbx.db_id', $gff_db->db_id);
    $gff_query = $gff_query->execute();

    $dbxref = $dbx_query->fetchObject();
    $gff_dbxref = $gff_query->fetchObject();
    $this->assertEquals($dbx_accession, $dbxref->accession);
    $this->assertEquals($this->gene_1_uname, $gff_dbxref->accession);
  }

  /**
   * Ensures ontology term records loaded correctly into chado.
   *
   * @group gff
   */
  public function testGFFImporterAttributeOntology() {
    $this->initGFFImporterAttributes();
    $ontology_db = 'SO';
    $ontology_accession = '0000704';

    $gene_1 = db_select('chado.feature', 'f')
      ->fields('f')
      ->condition('uniquename', $this->gene_1_uname)
      ->condition('type_id', $this->gene_cvt)
      ->execute()->fetchObject();

    $term = chado_get_cvterm(array(
      'dbxref_id' => array(
        'accession' => $ontology_accession,
        'db_id' => array(
          'name' => $ontology_db,
        ),
      ),
    ));

    $feature_cvt = db_select('chado.feature_cvterm', 'fcvt')
      ->fields('fcvt')
      ->condition('cvterm_id', $term->cvterm_id)
      ->condition('feature_id', $gene_1->feature_id)
      ->execute();
    $this->assertEquals(1, $feature_cvt->rowCount());
  }

  /**
   * Ensures feature parent record loaded correctly into chado.
   *
   * @group gff
   */
  public function testGFFImporterAttributeParent() {
    $this->initGFFImporterAttributes();
    $mrna_uname = 'test_mrna_001.1';

    $rel_cvt = chado_get_cvterm(array(
      'name' => 'part_of',
      'cv_id' => array(
        'name' => 'sequence',
      ),
    ))->cvterm_id;

    $mrna = db_select('chado.feature', 'f')
      ->fields('f')
      ->condition('uniquename', $mrna_uname)
      ->condition('type_id', $this->mrna_cvt)
      ->execute()->fetchObject();

    $query = db_select('chado.feature_relationship', 'fr');
    $query->join('chado.feature', 'f', 'f.feature_id = fr.object_id');
    $query->fields('f');
    $query->condition('fr.subject_id', $mrna->feature_id);
    $query->condition('fr.type_id', $rel_cvt);
    $query = $query->execute();
    $parent = $query->fetchObject();

    $this->assertEquals('test_gene_001', $parent->uniquename);
    $this->assertEquals('test_gene_001', $parent->name);
    $this->assertEquals($this->gene_cvt, $parent->type_id);
    $this->assertEquals($this->organism->organism_id, $parent->organism_id);
  }

  /**
   * Ensure target record loaded correctly into chado.
   *
   * @group gff
   */
  public function testGFFImporterAttributeTarget() {
    $this->initGFFImporterAttributes();
    $target_feature = 'scaffold1';
    $start = 99;
    $end = 200;
    $target_type = 'supercontig';
    $target_cvt = chado_get_cvterm(array(
      'name' => $target_type,
      'cv_id' => array(
        'name' => 'sequence',
      ),
    ))->cvterm_id;

    $source_feature = db_select('chado.feature', 'f')
      ->fields('f')
      ->condition('uniquename', $target_feature)
      ->condition('type_id', $target_cvt)
      ->execute()->fetchObject();

    $gene_1 = db_select('chado.feature', 'f')
      ->fields('f')
      ->condition('uniquename', $this->gene_1_uname)
      ->condition('type_id', $this->gene_cvt)
      ->execute()->fetchObject();

    $featureloc = db_select('chado.featureloc', 'fl')
      ->fields('fl')
      ->condition('fl.feature_id', $gene_1->feature_id)
      ->condition('fl.srcfeature_id', $source_feature->feature_id)
      ->execute()->fetchObject();

    $this->assertEquals($start, $featureloc->fmin);
    $this->assertEquals($end, $featureloc->fmax);
  }

  /**
   * Ensure properties loaded correctly into chado.
   *
   * @group gff
   */
  public function testGFFImporterAttributeProperty() {
    $this->initGFFImporterAttributes();
    $gap_1 = 'test_gap_1';
    $gap_2 = 'test_gap_2';
    $note_val = 'test_gene_001_note';

    $gene_1 = db_select('chado.feature', 'f')
      ->fields('f')
      ->condition('uniquename', $this->gene_1_uname)
      ->condition('type_id', $this->gene_cvt)
      ->execute()->fetchObject();

    $gap_cvt = chado_get_cvterm(array(
      'name' => 'Gap',
      'cv_id' => array(
        'name' => 'feature_property',
      ),
    ))->cvterm_id;

    $note_cvt = chado_get_cvterm(array(
      'name' => 'Note',
      'cv_id' => array(
        'name' => 'feature_property',
      ),
    ))->cvterm_id;

    // Assert gaps loaded correctly
    $gaps_query = db_select('chado.featureprop', 'fp')
      ->fields('fp')
      ->condition('feature_id', $gene_1->feature_id)
      ->condition('type_id', $gap_cvt)
      ->execute();

    while (($gap = $gaps_query->fetchObject())) {
      $gaps[$gap->value] = $gap;
    }

    $this->assertEquals($gap_1, $gaps[$gap_1]->value);
    $this->assertEquals(0, $gaps[$gap_1]->rank);

    // Assert note loaded correctly
    $note = db_select('chado.featureprop', 'fp')
      ->fields('fp')
      ->condition('feature_id', $gene_1->feature_id)
      ->condition('type_id', $note_cvt)
      ->execute()->fetchObject();

    $this->assertEquals($note_val, $note->value);
    $this->assertEquals(0, $note->rank);
  }

  /**
   * Ensure derives from information loaded correctly into chado.
   *
   * @group gff
   */
  public function testGFFImporterAttributeDerivesFrom() {
    $this->initGFFImporterAttributes();

    $gene_2 = db_select('chado.feature', 'f')
      ->fields('f')
      ->condition('uniquename', $this->gene_2_uname)
      ->condition('type_id', $this->gene_cvt)
      ->execute()->fetchObject();

    $derivesfrom_cvt = chado_get_cvterm(array(
      'name' => 'derives_from',
      'cv_id' => array(
        'name' => 'sequence',
      ),
    ))->cvterm_id;

    $query = db_select('chado.feature', 'f');
    $query->join('chado.feature_relationship', 'fr', 'f.feature_id = fr.object_id');
    $query->fields('f');
    $query->condition('fr.subject_id', $gene_2->feature_id);
    $query->condition('fr.type_id', $derivesfrom_cvt);
    $query = $query->execute();
    $derivesfrom_feature = $query->fetchObject();

    $this->assertEquals($this->gene_1_uname, $derivesfrom_feature->uniquename);
    $this->assertEquals($this->gene_1_uname, $derivesfrom_feature->name);
    $this->assertEquals($this->gene_cvt, $derivesfrom_feature->type_id);
  }

  /**
   * Ensure FASTA information loaded correctly into chado.
   *
   * @group gff
   */
  public function testGFFImporterAttributeFastas() {
    $this->initGFFImporterAttributes();

    $scaffold = db_select('chado.feature', 'f')
      ->fields('f')
      ->condition('uniquename', $this->scaffold_1_uname)
      ->condition('type_id', $this->supercontig_cvt)
      ->execute()->fetchObject();

    $this->assertEquals(720, $scaffold->seqlen);
    $this->assertEquals(720, strlen($scaffold->residues));
    $this->assertEquals('83578d8afdaec399c682aa6c0ddd29c9', $scaffold->md5checksum);
  }
  
  /**
   * Test that when checked, explicit proteins are created when specified within
   * the GFF file. Explicit proteins will not respect the skip_protein argument
   * and will therefore be added to the database.
   *
   * @group gff
   * @ticket 77
   *
   */
  public function testGFFPolypeptide() {
    $gff_file = ['file_local' => __DIR__ . '/../data/simpleGFF.gff'];
    $analysis = factory('chado.analysis')->create();
    $organism = factory('chado.organism')->create();
    $run_args = [
      // The new argument
      'skip_protein' => 1,
      'analysis_id' => $analysis->analysis_id,
      'organism_id' => $organism->organism_id,
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

    $name = 'FRAEX38873_v2_000000010.1.3_test_protein';
    $query = db_select('chado.feature', 'f')
      ->fields('f', ['uniquename'])
      ->condition('f.uniquename', $name)
      ->condition('f.type_id', $protein_type_id->cvterm_id)
      ->execute()
      ->fetchAll();
    $this->assertEquals(1, count($query));
  }


  /**
   * Add a skip protein option.  Test that when checked, implicit proteins are
   * not created, but that they are created when unchecked.
   *
   * @group gff
   * @ticket 77
   *
   */
  public function testGFFNoProteinOption() {
    $gff_file = ['file_local' => __DIR__ . '/../data/gff_protein_generation.gff'];
    $analysis = factory('chado.analysis')->create();
    $organism = factory('chado.organism')->create();
    $run_args = [
      //Skip protein feature generation
      'skip_protein' => 1,
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

    $name = "FRAEX38873_v2_000000190.1-protein";
    $results = db_select('chado.feature', 'f')
      ->fields('f', ['uniquename'])
      ->condition('f.uniquename', $name)
      ->condition('f.type_id', $protein_type_id->cvterm_id)
      ->execute()
      ->fetchAll();

    // There should be no proteins since we used the skip_proteins flag and no
    // explicit proteins were specified in the test file
    $this->assertEquals(0, count($results));

    // Now perform a unit test where we do not skip proteins generation
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


  /**
   * The GFF importer should still create explicitly defined proteins if
   * skip_protein is true.
   *
   * @group gff
   * @ticket 77
   */
  public function testGFFImporterLoadsExplicitProteins() {

    $gff_file = ['file_local' => __DIR__ . '/../data/simpleGFF.gff'];
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

    $name = 'FRAEX38873_v2_000000010.1.3_test_protein';
    $query = db_select('chado.feature', 'f')
      ->fields('f', ['uniquename'])
      ->condition('f.uniquename', $name)
      ->execute()
      ->fetchField();
    $this->assertEquals($name, $query);
  }

  private function runGFFLoader($run_args, $file) {
    // silent(function ($run_args, $file) {
    module_load_include('inc', 'tripal_chado', 'includes/TripalImporter/GFF3Importer');
    $importer = new \GFF3Importer();
    $importer->create($run_args, $file);
    $importer->prepareFiles();
    $importer->run();
    //  });
  }

  private function loadLandmarks($analysis, $organism, $landmark_file = array()) {
    if (empty($landmark_file)) {
      $landmark_file = ['file_local' => __DIR__ . '/../data/empty_landmarks.fasta'];
    }

    $run_args = [
      'organism_id' => $organism->organism_id,
      'analysis_id' => $analysis->analysis_id,
      'seqtype' => 'supercontig',
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
    //silent(function ($run_args, $landmark_file) {
    $importer = new \FASTAImporter();
    $importer->create($run_args, $landmark_file);
    $importer->prepareFiles();
    $importer->run();
    // });

  }

}
