<?php

namespace Tests;

use StatonLab\TripalTestSuite\DBTransaction;
use StatonLab\TripalTestSuite\TripalTestCase;

class OBOImporterTest extends TripalTestCase {

  // Uncomment to auto start and rollback db transactions per test method.
  use DBTransaction;


  /**
   * Tests that the Goslim ontology loads using a remote URL.  Ensure subgroups
   * load.
   *
   * @group obo
   */

  public function testGO_SLIM_load() {

    $name = 'core_test_goslim_plant';
    $path = 'http://www.geneontology.org/ontology/subsets/goslim_plant.obo';

    $this->load_obo($name, $path);

    $exists = db_select('chado.cv', 'c')
      ->fields('c', ['cv_id'])
      ->condition('name', 'biological_process')
      ->execute()
      ->fetchField();
    $this->assertNotFalse($exists);

    $exists = db_select('chado.cv', 'c')
      ->fields('c', ['cv_id'])
      ->condition('name', 'cellular_component')
      ->execute()
      ->fetchField();
    $this->assertNotFalse($exists);

    $exists = db_select('chado.cv', 'c')
      ->fields('c', ['cv_id'])
      ->condition('name', 'molecular_function')
      ->execute()
      ->fetchField();
    $this->assertNotFalse($exists);


    $sql = "
      SELECT DISTINCT CVTP.value
      FROM {cvtermprop} CVTP
        INNER JOIN {cvterm} CVTPT on CVTPT.cvterm_id = CVTP.type_id
        INNER JOIN {cvterm} CVT on CVT.cvterm_id = CVTP.cvterm_id
        INNER JOIN {dbxref} DBX on CVT.dbxref_id = DBX.dbxref_id
        INNER JOIN {db} DB on DB.db_id = DBX.db_id
      WHERE CVTPT.name = 'Subgroup' and DB.name = 'GO' and CVTP.value = 'goslim_plant'
    ";
    $exists = chado_query($sql)->fetchField();
    $this->assertNotFalse($exists);
  }


  /**
   * @param $name - ontology name.  This goes in the tripal_cv_obo table.
   * @param $path - path to the OBO.  this can be a file path or a URL.
   *
   * @throws \Exception
   */

  private function load_obo($name, $path) {

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
   * Ensure SO can be loaded.  Ensure that synonyms are set properly.
   *
   * @group obo
   * @ticket 525
   */
  public function test_relationships_in_SO_exist() {

    // step 1: drop the SO CV and CASCADE.

    $result = chado_query("SET search_path to public, chado;
    DELETE FROM {cv} WHERE name = 'sequence'");
    $result = chado_query("SET search_path to public, chado;
    DELETE FROM {db} WHERE name = 'SO'");

    // step 2: re-add SO.
    $name = 'Sequence Ontology';
    $path = 'http://purl.obolibrary.org/obo/so.obo';

    $this->load_obo($name, $path);

    $sql = "SELECT CVT.name, CVTSYN.synonym
FROM {cvterm} CVT
  INNER JOIN {dbxref} DBX on DBX.dbxref_id = CVT.dbxref_id
  INNER JOIN {db} on DB.db_id = DBX.db_id
  LEFT JOIN {cvtermsynonym} CVTSYN on CVTSYN.cvterm_id = CVT.cvterm_id
WHERE DB.name = 'SO' and CVT.name = 'supercontig'
ORDER BY DBX.accession";


    $results = chado_query($sql)->fetchAll();
    $result = $results[0];

    $this->assertNotNull($result);
    $this->assertNotEmpty($result);
    $this->assertEquals("supercontig", $result->name);
    $this->assertEquals("scaffold", $result->synonym);
  }


  /**
   * Test simply that nodes are inserted.
   ** @group obo
   *
   */

  public function test_cvtermpath_cv_nodes_inserted() {

    $name = 'path_test_mini';
    $path = __DIR__ . '/../example_files/cvtermpath_test.obo';
    $this->load_obo($name, $path);

    //Check cvtermpath is sane.

    $cv_name = 'cvtermpath_test';


    $nodes = [
      'node01',
      'node02',
      'node03',
      'node04',
      'node05',
      'node06',
      'node07',
      'node08',
      'node09',
      'node10',
      'node11',
      'node12',
      'node13',
      'node14',
    ];

    //check nodes are inserted.
    foreach ($nodes as $node) {

      $query = db_select('chado.cvterm', 'cvt')
        ->fields('cvt', ['cvterm_id', 'cv_id'])
        ->condition('cvt.name', $node);
      $query->join('chado.cv', 'cv', 'cvt.cv_id = cv.cv_id');
      $query->condition('cv.name', $cv_name);
      $result = $query->execute()
        ->fetchObject();
      $this->assertNotFalse($result);

    }
  }


  /**
   * This data provider currently returns an array of data in the following manner:
   * item[0] - the object node
   * item[1] - an array containing a list of subject nodes, ie, nodes that claim they have a "is_a" relationship with this node.
   *
   * @return array
   */
  public function node_data_provider() {

    $data = [
      [
        'node01',// object
        [ //subjects
          'node02',
          'node03',
          'node04',
          'node05',
          'node06',
          'node07',
          'node08',
          'node09',
          'node10',
          'node11',
          'node12',
          'node13',
          //   'node14',
          //Node 14 is not connected!
        ],
      ],

      [
        'node04',
        ['node09','node11'],
      ],
      [
        'node11',
        ['node13',],
      ],
    ];
    return $data;

  }

  /**
   * @group obo
   * @dataProvider node_data_provider
   */
  public function test_cvtermpath_correct($object, $subjects) {

    $name = 'path_test_mini';
    $path = __DIR__ . '/../example_files/cvtermpath_test.obo';
    $this->load_obo($name, $path);


    $cv_name = 'cvtermpath_test';

    $cv_id = chado_get_cv(['name' => $cv_name])->cv_id;

    //populate cvtermpath
    chado_update_cvtermpath($cv_id);


    $query = db_select('chado.cvtermpath', 'cp');
    $query->fields('cp', ['pathdistance']);
    $query->condition('cp.cv_id', $cv_id);
    $query->join('chado.cvterm', 'subject', 'cp.subject_id = subject.cvterm_id');
    $query->join('chado.cvterm', 'object', 'cp.object_id = object.cvterm_id');
    $query->condition('object.name', $object);
    $query->fields('subject', ['name']);

    //First, ensure that the number of relationships is correct

    $countquery = clone $query;

    $results = $countquery->execute()->fetchAll();


    $this->assertEquals(count($subjects), count($results));



    foreach ($subjects as $subject) {

      $query_copy = clone $query;

      $query_copy->condition('subject.name', $subject);
      $results = $query_copy->execute()->fetchObject();

      $this->assertNotFalse($results, "failed for {$object} as object and {$subject} as subject.");

    }
  }

  /**
   * ONLY node 1 should be root for test OBO.
   *
   * @group obo
   */
  public function test_cvtermpath_mview_root_terms_correct() {

    $name = 'path_test_mini';
    $path = __DIR__ . '/../example_files/cvtermpath_test.obo';
    $this->load_obo($name, $path);

    //populate mview
    chado_populate_mview(chado_get_mview_id('cv_root_mview'));


    $cv_name = 'cvtermpath_test';

    $roots = db_select('chado.cv_root_mview', 't')
      ->fields('t', ['name'])
      ->condition('cv_name', $cv_name)
      ->execute()
      ->fetchAll();

    $this->assertNotEmpty($roots);

    $this->assertLessThan(2, count($roots));
    $this->assertEquals("node01", $roots[0]->name);

  }

  /**
   * Test OBO has a synonym.  check its inserted properly.
   * @group obo
   * @group wip
   */
  public function test_synonyms_are_loaded(){
    //node11 has a synonym:"crazy node" EXACT []


    $name = 'path_test_mini';
    $path = __DIR__ . '/../example_files/cvtermpath_test.obo';
    $this->load_obo($name, $path);

    $cv_name = 'cvtermpath_test';

    $query = db_select('chado.cvtermsynonym', 't')
      ->fields('t', ['synonym']);
    $query->join('chado.cvterm', 'cvt', 't.cvterm_id = cvt.cvterm_id');
    $query->condition('cvt.name', 'node11');

    $result = $query->execute()->fetchField();

    $this->assertNotFalse($result);

    $this->assertEquals("crazy node", $result);




  }



  /**
   * Test OBO has a xref.  check its inserted properly.
   * @group obo
   * @group wip
   */
  public function test_dbxref_loaded(){
    //node11 has a synonym:"crazy node" EXACT []


    $name = 'path_test_mini';
    $path = __DIR__ . '/../example_files/cvtermpath_test.obo';
    $this->load_obo($name, $path);

    $cv_name = 'cvtermpath_test';

    $query = db_select('chado.cvterm_dbxref', 't');
    $query->join('chado.cvterm', 'cvt', 't.cvterm_id = cvt.cvterm_id');
    $query->join('chado.dbxref', 'dbx', 'dbx.dbxref_id = t.dbxref_id');
    $query->fields('dbx', ['accession']);
    $query->condition('cvt.name', 'node11');

    $result = $query->execute()->fetchField();

    $this->assertNotFalse($result);

    $this->assertEquals("0043226", $result);

  }

  /**
   * ensure that new CV's aren't accidentally created when term names have
   * colons in them.
   *
   * @group obo
   * @ticket 525
   */
  public function test_PTO_loads_colon_issue() {

    $name = 'core_test_PTO_mini';
    $path = __DIR__ . '/../example_files/pto_colon.obo';

    $this->load_obo($name, $path);

    $exists = db_select('chado.cv', 'c')
      ->fields('c', ['cv_id'])
      ->condition('name', 'core_test_PTO_mini')
      ->execute()
      ->fetchField();
    $this->assertNotFalse($exists);

    //hte colon splitting issue: a new CV will created named fatty acid 18
    $exists = db_select('chado.cv', 'c')
      ->fields('c', ['cv_id'])
      ->condition('name', 'fatty acid 18')
      ->execute()
      ->fetchField();
    $this->assertFalse($exists);

  }


}