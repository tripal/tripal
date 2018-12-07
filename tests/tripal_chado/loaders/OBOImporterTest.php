<?php

namespace Tests;

use StatonLab\TripalTestSuite\DBTransaction;
use StatonLab\TripalTestSuite\TripalTestCase;

class OBOImporterTest extends TripalTestCase {

  // Uncomment to auto start and rollback db transactions per test method.
  use DBTransaction;

  /**
   * A helper function for loading any OBO.
   *
   * @param $name - ontology name.  This goes in the tripal_cv_obo table.
   * @param $path - path to the OBO.  this can be a file path or a URL.
   *
   * @throws \Exception
   */
  private function loadOBO($name, $path) {

    $obo_id = db_select('public.tripal_cv_obo', 't')
      ->fields('t', ['obo_id'])
      ->condition('t.name', $name)
      ->execute()
      ->fetchField();

    if (!$obo_id) {

      $obo_id = db_insert('public.tripal_cv_obo')
        ->fields(['name' => $name, 'path' => $path])
        ->execute();
    }

    $run_args = ['obo_id' => $obo_id];

    module_load_include('inc', 'tripal_chado', 'includes/TripalImporter/OBOImporter');
    $importer = new \OBOImporter();
    $importer->create($run_args);
    $importer->prepareFiles();
    $importer->run();
  }


  /**
   * Tests that an OBO from a remote URL can be loaded.
   *
   * For this test we will use the GO Plant Slim.
   *
   * @group obo
   */
  public function testRemoteOBO() {

    $name = 'core_test_goslim_plant';
    $path = 'http://www.geneontology.org/ontology/subsets/goslim_plant.obo';

    $this->loadOBO($name, $path);

    // Test that we get all three vocabularies added:  biological_process,
    // cellular_component and molecular_function.
    $bp_cv_id = db_select('chado.cv', 'c')
      ->fields('c', ['cv_id'])
      ->condition('name', 'biological_process')
      ->execute()
      ->fetchField();
    $this->assertNotFalse($bp_cv_id,
      "Missing the 'biological_process' cv record after loading the GO plant slim.");

    $cc_cv_id = db_select('chado.cv', 'c')
      ->fields('c', ['cv_id'])
      ->condition('name', 'cellular_component')
      ->execute()
      ->fetchField();
    $this->assertNotFalse($cc_cv_id,
      "Missing the 'cellular_component' cv record after loading the GO plant slim.");

    $mf_cv_id = db_select('chado.cv', 'c')
      ->fields('c', ['cv_id'])
      ->condition('name', 'molecular_function')
      ->execute()
      ->fetchField();
    $this->assertNotFalse($mf_cv_id,
      "Missing the 'molecular_function' cv record after loading the GO plant slim.");

    // Make sure we have a proper database record.
    $go_db_id = db_select('chado.db', 'd')
      ->fields('d', ['db_id'])
      ->condition('name', 'GO')
      ->execute()
      ->fetchField();
    $this->assertNotFalse($go_db_id,
      "Missing the 'GO' database record after loading the GO plant slim.");
  }

  /**
   * Tests that an OBO from a local path can be loaded.
   *
   * For this test we will use a test ontology.
   *
   * @group obo
   */
  public function testLocalOBO() {
    $name = 'tripal_obo_test';
    $path = __DIR__ . '/../example_files/test.obo';

    $this->loadOBO($name, $path);

    // Make sure we have a proper vocabulary record.
    $tot_cv_id = db_select('chado.cv', 'c')
      ->fields('c', ['cv_id'])
      ->condition('name', 'tripal_obo_test')
      ->execute()
      ->fetchField();
    $this->assertNotFalse($tot_cv_id,
      "Missing the 'tripal_obo_test' cv record after loading the test.obo file");

    // Make sure we have a proper database record.
    $tot_db_id = db_select('chado.db', 'd')
      ->fields('d', ['db_id'])
      ->condition('name', 'TOT')
      ->execute()
      ->fetchField();
    $this->assertNotFalse($tot_db_id,
      "Missing the 'TOT' db record after loading the test.obo file");

    return [[$tot_cv_id, $tot_db_id]];
  }

  /**
   * Test that all nodes in our test OBO are loaded.
   *
   * @group obo
   * @dataProvider testLocalOBO
   */

  public function testCVterms($cv_id, $db_id) {

    // Our test OBO has 14 nodes.
    $nodes = [
      ['TOT:001' => 'node01'],
      ['TOT:002' => 'node02'],
      ['TOT:003' => 'node03'],
      ['TOT:004' => 'node04'],
      ['TOT:005' => 'node05'],
      ['TOT:006' => 'node06'],
      ['TOT:007' => 'node07'],
      ['TOT:008' => 'node08'],
      ['TOT:009' => 'node09'],
      ['TOT:010' => 'node10'],
      ['TOT:011' => 'node11'],
      ['TOT:012' => 'node12'],
      ['TOT:013' => 'node13'],
      ['TOT:014' => 'node14'],
    ];

    // Test that the proper records were added to identify the term.    
    foreach ($nodes as $id => $node_name) {

      // Check that cvterm record is inserted.
      $cvterm_id = db_select('chado.cvterm', 'cvt')
        ->fields('cvt', ['cvterm_id'])
        ->condition('cvt.name', $node_name)
        ->condition('cvt.cv_id', $cv_id)
        ->execute()
        ->fetchField();
      $this->assertNotFalse($cvterm_id,
        "Missing the cvterm record with name, '$node' after loading the test.obo file");

      // Check that the dbxref record is inserted.
      $accession = preg_replace('/TOT:/', '', $id);
      $dbxref_id = db_select('chado.dbxref', 'dbx')
        ->fields('dbx', ['dbxref_id'])
        ->condition('accession', $accession)
        ->condition('db_id', $db_id);
      $this->assertNotFalse($cvterm_id,
        "Missing the dbxref record forid, '$id' after loading the test.obo file");
    }

    // Test node 11 to make sure the definition was inserted correctly.
    // The definition for node11 has an extra colon and a comment.  The colon
    // should not throw off the insertion of the full definition and
    // the comment should be excluded.
    $def = db_select('chado.cvterm', 'cvt')
      ->fields('cvt', ['definition'])
      ->condition('cvt.name', 'node11')
      ->condition('cvt.cv_id', $cv_id)
      ->execute()
      ->fetchField();
    $this->assertNotFalse($def,
      "The definition for node11 was not added.");
    $this->assertEquals('This is node 11 : Yo', $def,
      "The definition for node11 is incorrect. it was stored as \"$def\" but should be \"def: This is node 11 : Yo\".");

    // Make sure that colons in term names don't screw up the term. This test
    // corresponds to the term with id CHEBI:132502 in the test.obo file.
    $exists = db_select('chado.cv', 'c')
      ->fields('c', ['cv_id'])
      ->condition('name', 'fatty acid 18')
      ->execute()
      ->fetchField();
    $this->assertFalse($exists);


    // Node14 should be marked as obsolete.
    $sql = "
      SELECT CVT.is_obsolete
      FROM {cvterm} CVT
         INNER JOIN {dbxref} DBX on DBX.dbxref_id = CVT.dbxref_id
         INNER JOIN {db} DB on DB.db_id = DBX.db_id
       WHERE DB.name = 'TOT' and DBX.accession = '014'
    ";
    $is_obsolete = chado_query($sql)->fetchField();
    $this->assertEquals(1, $is_obsolete,
      "The term, node14, should be marked as obsolete after loading of the test.obo file.");

    // Every vocabulary should have an is_a term added to support the is_a
    // relationships.
    $sql = "
      SELECT CVT.is_relationshiptype
      FROM {cvterm} CVT
         INNER JOIN {dbxref} DBX on DBX.dbxref_id = CVT.dbxref_id
         INNER JOIN {db} DB on DB.db_id = DBX.db_id
       WHERE CVT.name = 'is_a' and DB.name = 'TOT'
    ";
    $is_reltype = chado_query($sql)->fetchField();
    $this->assertNotFalse($is_reltype,
      "The cvterm record for, is_a, should have been added during loading of the test.obo file.");
    $this->assertEquals(1, $is_reltype,
      "The cvterm record, is_a, should be marked as a relationship type.");

  }

  /**
   * Test that insertion of synonyms works.
   *
   * The term 'node11' has a synonym:"crazy node" EXACT []
   *
   * @group obo
   * @dataProvider testLocalOBO
   */
  public function testSynonyms($cv_id, $db_id) {

    $query = db_select('chado.cvtermsynonym', 'cvts');
    $query->fields('cvts', ['synonym']);
    $query->join('chado.cvterm', 'cvt', 'cvts.cvterm_id = cvt.cvterm_id');
    $query->condition('cvt.name', 'node11');
    $synonym = $query->execute()->fetchField();
    $this->assertNotFalse($synonym,
      "Failed to find the 'crazy node' synonym record for node 11 after loading the test.obo file.");

    $this->assertEquals("crazy node", $synonym,
      "Failed to properly add the 'crazy node' synonym for node 11 instead the following was loaded: $synonym");
  }

  /**
   * Test that insertion of subset works.
   *
   * The term 'node11' belongs to the test_crazy subset. Everything else belongs
   * to the test_normal subset.
   *
   *
   * @group obo
   * @dataProvider testLocalOBO
   */
  public function testSubset($cv_id, $db_id) {

    $sql = "
      SELECT CVT.name
      FROM {cvtermprop} CVTP
        INNER JOIN {cvterm} CVTPT on CVTPT.cvterm_id = CVTP.type_id
        INNER JOIN {cvterm} CVT on CVT.cvterm_id = CVTP.cvterm_id
        INNER JOIN {dbxref} DBX on CVT.dbxref_id = DBX.dbxref_id
        INNER JOIN {db} DB on DB.db_id = DBX.db_id
      WHERE CVTPT.name = 'Subgroup' and DB.name = 'TOT' and CVTP.value = 'test_crazy'
    ";
    $term_name = chado_query($sql)->fetchField();
    $this->assertNotFalse($term_name,
      "This cvtermprop record for the subset 'test_crazy' is missing.");

    $this->assertEquals('node11', $term_name,
      "This cvtermprop record for the subset 'test_crazy' is assigned to term, $term_name, instead of node11.");

    $sql = "
      SELECT count(CVT.cvterm_id)
      FROM {cvtermprop} CVTP
        INNER JOIN {cvterm} CVTPT on CVTPT.cvterm_id = CVTP.type_id
        INNER JOIN {cvterm} CVT on CVT.cvterm_id = CVTP.cvterm_id
        INNER JOIN {dbxref} DBX on CVT.dbxref_id = DBX.dbxref_id
        INNER JOIN {db} DB on DB.db_id = DBX.db_id
      WHERE CVTPT.name = 'Subgroup' and DB.name = 'TOT' and CVTP.value = 'test_normal'
    ";
    $subset_count = chado_query($sql)->fetchField();

    $this->assertNotFalse($subset_count,
      "This cvtermprop record for the subset 'test_normal' are missing.");

    // There should be 12 terms that belong to subset 'test_normal' as node14
    // does not belong to a subset.
    $this->assertEquals(12, $subset_count,
      "There are $subset_count cvtermprop record for the subset 'test_normal' but there should be 13.");
  }

  /**
   * Test that the insertion of xref works.
   *
   * The term 'node11' belongs to the test_crazy subset. Everything else belongs
   * to the test_normal subset.
   *
   *
   * @group obo
   * @dataProvider testLocalOBO
   */
  public function testXref($cv_id, $db_id) {

    $sql = "
      SELECT concat(DB2.name, ':', DBX2.accession)
      FROM {cvterm} CVT
        INNER JOIN {dbxref} DBX on DBX.dbxref_id = CVT.dbxref_id
        INNER JOIN {db} on DB.db_id = DBX.db_id
        INNER JOIN {cvterm_dbxref} CVTDBX on CVTDBX.cvterm_id = CVT.cvterm_id
        INNER JOIN {dbxref} DBX2 on DBX2.dbxref_id = CVTDBX.dbxref_id
        INNER JOIN {db} DB2 on DB2.db_id = DBX2.db_id
      WHERE DB.name = 'TOT' and CVT.name = 'node11'
      ORDER BY DBX.accession
    ";
    $xref_id = chado_query($sql)->fetchField();
    $this->assertNotFalse($xref_id,
      "This cvterm_dbxref record for the xref 'GO:0043226' is missing for node11.");

    $this->assertEquals('GO:0043226', $xref_id,
      "This cvterm_dbxref record for node 11 is, $xref_id, instead of GO:0043226.");
  }

  /**
   * Test that the insertion of comments works.
   *
   * The term 'node11' contains a comment.
   *
   * @group obo
   * @dataProvider testLocalOBO
   */
  public function testComment($cv_id, $db_id) {

    $sql = "
      SELECT CVTP.value
      FROM {cvterm} CVT
        INNER JOIN {dbxref} DBX on DBX.dbxref_id = CVT.dbxref_id
        INNER JOIN {db} on DB.db_id = DBX.db_id
        INNER JOIN {cvtermprop} CVTP on CVTP.cvterm_id = CVT.cvterm_id
        INNER JOIN {cvterm} CVTPT on CVTPT.cvterm_id = CVTP.type_id
      WHERE DB.name = 'TOT' and CVTPT.name = 'comment' and CVT.name = 'node11'
      ORDER BY DBX.accession
    ";
    $comment = chado_query($sql)->fetchField();
    $this->assertNotFalse($xref_id,
      "This cvterm_dbxref record for the xref 'This is a crazy node' is missing for node11.");

    $this->assertEquals('This is a crazy node', $comment,
      "This cvterm_dbxref record for node11 is, \"$comment\", instead of \"This is a crazy node\".");
  }

  /**
   * Tests that the cvtermpath is properly loaded.
   *
   * @group obo
   * @dataProvider testLocalOBO
   */
  public function testRelationships($cv_id, $db_id) {
    $relationships = [
      ['node02', 'is_a', 'node01'],
      ['node03', 'is_a', 'node01'],
      ['node04', 'is_a', 'node01'],
      ['node04', 'has_part', 'node11'],
      ['node05', 'is_a', 'node01'],
      ['node06', 'is_a', 'node03'],
      ['node07', 'is_a', 'node03'],
      ['node08', 'is_a', 'node07'],
      ['node09', 'is_a', 'node04'],
      ['node09', 'is_a', 'node07'],
      ['node10', 'is_a', 'node05'],
      ['node11', 'is_a', 'node09'],
      ['node11', 'is_a', 'node10'],
      ['node12', 'is_a', 'node10'],
      ['node13', 'is_a', 'node11'],
    ];
    foreach ($relationships as $relationship) {
      $subject = $relationship[0];
      $type = $relationship[1];
      $object = $relationship[2];
      $sql = "
        SELECT CVTR.cvterm_relationship_id
        FROM {cvterm} CVT
          INNER JOIN {dbxref} DBX on DBX.dbxref_id = CVT.dbxref_id
          INNER JOIN {db} on DB.db_id = DBX.db_id
          INNER JOIN {cvterm_relationship} CVTR on CVTR.subject_id = CVT.cvterm_id
          INNER JOIN {cvterm} CVT2 on CVT2.cvterm_id = CVTR.object_id
          INNER JOIN {cvterm} CVT3 on CVT3.cvterm_id = CVTR.type_id
        WHERE DB.name = 'TOT' AND CVT2.name = :object AND 
          CVT3.name = :type AND CVT.name = :subject
      ";
      $args = [':object' => $object, ':type' => $type, ':subject' => $subject];
      $rel_id = chado_query($sql, $args)->fetchField();
      $this->assertNotFalse($rel_id,
        "The following relationship could not be found: $subect $type $object.");
    }

    // Now make sure we have no more relationships than what we are supposed
    // to have.
    $sql = "
      SELECT count(CVTR.cvterm_relationship_id)
      FROM {cvterm} CVT
        INNER JOIN {dbxref} DBX on DBX.dbxref_id = CVT.dbxref_id
        INNER JOIN {db} on DB.db_id = DBX.db_id
        INNER JOIN {cvterm_relationship} CVTR on CVTR.object_id = CVT.cvterm_id
      WHERE DB.name = 'TOT' AND CVT.is_relationshiptype = 0
    ";
    $rel_count = chado_query($sql)->fetchField();
    $expected = count($relationships);
    $this->assertEquals($expected, $rel_count,
      "There are an incorrect number of relationships. There were $rel_count found but there should be $expected.");
  }

  /**
   * Tests that the cvtermpath is properly loaded.
   *
   * @group obo
   * @dataProvider testLocalOBO
   */
  public function testCVtermPath($cv_id, $db_id) {

    // For now we won't include distance or type in the check because depending
    // how the tree was loaded and if there are multiple paths to a node
    // then there's no guarantee we'll always get the same path. Therefore the
    // type and pathdistance may be different (althoug not incorrect).
    $relationships = [
      // Node01 as root: note that the root term always has a link to itself
      // in the cvtermpath table.
      ['node01', 'node01'],
      ['node01', 'node02'],
      ['node01', 'node03'],
      ['node01', 'node04'],
      ['node01', 'node05'],
      ['node01', 'node06'],
      ['node01', 'node07'],
      ['node01', 'node08'],
      ['node01', 'node09'],
      ['node01', 'node10'],
      ['node01', 'node11'],
      ['node01', 'node12'],
      ['node01', 'node13'],
      // Node03 as root.
      ['node03', 'node04'],
      ['node03', 'node06'],
      ['node03', 'node07'],
      ['node03', 'node08'],
      ['node03', 'node09'],
      ['node03', 'node11'],
      ['node03', 'node13'],
      // Node04 as root.
      ['node04', 'node09'],
      ['node04', 'node11'],
      ['node04', 'node13'],
      // Node05 as root.
      ['node05', 'node04'],
      ['node05', 'node09'],
      ['node05', 'node10'],
      ['node05', 'node11'],
      ['node05', 'node12'],
      ['node05', 'node13'],
      // Node07 as root.
      ['node07', 'node04'],
      ['node07', 'node08'],
      ['node07', 'node09'],
      ['node07', 'node11'],
      ['node07', 'node13'],
      // Node09 as root.
      ['node09', 'node04'],
      ['node09', 'node11'],
      ['node09', 'node13'],
      // Node10 as root.
      ['node10', 'node04'],
      ['node10', 'node09'],
      ['node10', 'node11'],
      ['node10', 'node12'],
      ['node10', 'node13'],
      // Node11 as root.
      ['node11', 'node04'],
      ['node11', 'node09'],
      ['node11', 'node13'],
    ];

    // Populate the cvtermpath for our test OBO.
    chado_update_cvtermpath($cv_id);

    foreach ($relationships as $relationship) {
      $object = $relationship[0];
      $subject = $relationship[1];
      $sql = "
        SELECT cvtermpath_id
        FROM {cvtermpath} CVTP 
          INNER JOIN {cvterm} CVTO on CVTO.cvterm_id = CVTP.object_id
          INNER JOIN {cvterm} CVTS on CVTS.cvterm_id = CVTP.subject_id
          INNER JOIN {cvterm} CVTT on CVTT.cvterm_id = CVTP.type_id
        WHERE CVTP.cv_id = :cv_id and CVTO.name = :object and 
          CVTS.name = :subject
      ";
      $args = [
        ':cv_id' => $cv_id,
        ':object' => $object,
        ':subject' => $subject,
      ];
      $cvtermpath_id = chado_query($sql, $args)->fetchField();
      $this->assertNotFalse($cvtermpath_id,
        "Cound not find the cvtermpath record for the relationship: $subject => $object.");
    }

    // Now make sure we have no additional entries.
    $sql = "
          SELECT count(cvtermpath_id)
          FROM {cvtermpath} CVTP
            INNER JOIN {cvterm} CVTO on CVTO.cvterm_id = CVTP.object_id
            INNER JOIN {cvterm} CVTS on CVTS.cvterm_id = CVTP.subject_id
            INNER JOIN {cvterm} CVTT on CVTT.cvterm_id = CVTP.type_id
          WHERE CVTP.cv_id = :cv_id
        ";
    $args = [':cv_id' => $cv_id];
    $rel_count = chado_query($sql, $args)->fetchField();
    $expected = count($relationships);
    $this->assertEquals($expected, $rel_count,
      "There are an incorrect number of paths. There were $rel_count found but there should be $expected.");
  }

  /**
   * Tests that the EBI Lookup is properly working.
   *
   * The term CHEBI:132502 should have been loaded via EBI.
   *
   * @group obo
   * @dataProvider testLocalOBO
   */
  public function testEBILookup($cv_id, $db_id) {
    $sql = "
       SELECT CVT.cvterm_id
       FROM  {cvterm} CVT
         INNER JOIN {dbxref} DBX on DBX.dbxref_id = CVT.dbxref_id
         INNER JOIN {db} DB on DB.db_id = DBX.db_id
       WHERE DB.name = 'CHEBI' and DBX.accession = '132502'
    ";
    $cvterm_id = chado_query($sql)->fetchField();
    $this->assertNotFalse($cvterm_id,
      "The term, CHEBI:132502, is not present the EBI OLS lookup must not have succeeded.");
  }

  /**
   * Tests when changes are made between OBO loads.
   *
   * Sometimes an ontology can change the names of it's terms, or set some
   * as obsolete, etc. We need to makes sure that when changes are made and
   * the OBO is reloaded that the terms are properly update.
   *
   * @group obo
   * @dataProvider testLocalOBO
   */
  public function testOBOChanges($cv_id, $db_id) {
    $name = 'tripal_obo_test_update';
    $path = __DIR__ . '/../example_files/test.update.obo';

    $this->loadOBO($name, $path);

    // Did the name of term 13 change?
    $sql = "
      SELECT CVT.name
      FROM {cvterm} CVT
         INNER JOIN {dbxref} DBX on DBX.dbxref_id = CVT.dbxref_id
         INNER JOIN {db} DB on DB.db_id = DBX.db_id
       WHERE DB.name = 'TOT' and DBX.accession = '013'
    ";
    $name = chado_query($sql)->fetchField();
    $this->assertEquals('New name 13.', $name,
      "The name for node13 (TOT:013) failed to update to 'New name 13'.");

    // Node15 is new, and node02 got removed. Node15 now uses node02's name and
    // has TOT:002 as an alt_id. So, node02 should be marked as obsolete
    $sql = "
      SELECT CVT.is_obsolete
      FROM {cvterm} CVT
         INNER JOIN {dbxref} DBX on DBX.dbxref_id = CVT.dbxref_id
         INNER JOIN {db} DB on DB.db_id = DBX.db_id
       WHERE DB.name = 'TOT' and DBX.accession = '002'
    ";
    $is_obsolete = chado_query($sql)->fetchField();
    $this->assertEquals(1, $is_obsolete,
      "The node02 (TOT:002) should be marked as obsolete after update.");

    // Node16 is new, and node08 is now obsolete. Node16 now uses node08's name,
    // so, node08 should be marked as obsolete and have the word '(obsolete)'
    // added to prevent future conflicts.
    $sql = "
      SELECT CVT.name
      FROM {cvterm} CVT
         INNER JOIN {dbxref} DBX on DBX.dbxref_id = CVT.dbxref_id
         INNER JOIN {db} DB on DB.db_id = DBX.db_id
       WHERE DB.name = 'TOT' and DBX.accession = '008'
    ";
    $name = chado_query($sql)->fetchField();
    $this->assertEquals("node08 (obsolete)", $name,
      "The node08 (TOT:008) should be marked as obsolete after update.");
  }

  /**
   * @group obo
   * @group chado
   */
  public function testfindEBITerm_finder_retrieves_term() {

    module_load_include('inc', 'tripal_chado', 'includes/TripalImporter/OBOImporter');
    $importer_private = new \OBOImporter();
    $importer = reflect($importer_private);
    $id = 'PECO:0007085';
    $result = $importer->findEBITerm($id);
    $this->assertNotEmpty($result);
    $this->assertEquals('fertilizer exposure', $result['name'][0]);
    }

}