<?php

namespace Tests;

use StatonLab\TripalTestSuite\DBTransaction;
use StatonLab\TripalTestSuite\TripalTestCase;

class FASTAImporterTest extends TripalTestCase {

  // Uncomment to auto start and rollback db transactions per test method.
  use DBTransaction;

  /**
   * Basic test example.
   * Tests must begin with the word "test".
   * See https://phpunit.readthedocs.io/en/latest/ for more information.
   */

  /**
   * @group fasta
   * @group chado
   */
  public function testImporterAssociatesParentWithoutRegexp() {
    module_load_include('inc', 'tripal_chado', 'includes/TripalImporter/FASTAImporter');
    $importer = new \FASTAImporter();

    //this test will first create an organism and mrna features with the same name as devseed proteins.
    //It will then load the devseed protein fasta via the importer.
    //Finally it will ensure feature_relationships exist with the test mrna.

    $organism = factory('chado.organism')->create();
    $mrna_term = chado_get_cvterm(['id' => 'SO:0000234']);
    $analysis = factory('chado.analysis')->create();
    $mrna_1 = factory('chado.feature')->create([
      'type_id' => $mrna_term->cvterm_id,
      'organism_id' => $organism->organism_id,
      'name' => 'FRAEX38873_v2_000000010.1',
      'uniquename' => 'FRAEX38873_v2_000000010.1',
    ]);
    $mrna_2 = factory('chado.feature')->create([
      'type_id' => $mrna_term->cvterm_id,
      'organism_id' => $organism->organism_id,
      'name' => 'FRAEX38873_v2_000000010.2',
      'uniquename' => 'FRAEX38873_v2_000000010.2',
    ]);
    $file = ['file_local' => __DIR__ . '/../data/two_prots.fasta'];

    $run_args = [
      'analysis_id' => $analysis->analysis_id,
      'organism_id' => $organism->organism_id,
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

    $importer->create($run_args, $file);
    $importer->prepareFiles();
    $importer->run();


    $result = db_select('chado.feature', 'f')
      ->fields('f')
      ->condition('f.organism_id', $organism->organism_id)
      ->execute()
      ->fetchAll();

    $this->assertNotEquals(2, count($result), 'The child features were not loaded when a regexp was not provided.');


    $query = db_select('chado.feature_relationship', 'fr')
      ->fields('fr')
      ->condition('fr.object_id', $mrna_1->feature_id);
    $query->join('chado.feature', 'f', 'f.feature_id = fr.subject_id');
    $result = $query->execute()
      ->fetchObject();

    $this->assertNotFalse($result, 'relationship was not added to parente feature when regexp not provided (same parent/child name).');
  }
}
