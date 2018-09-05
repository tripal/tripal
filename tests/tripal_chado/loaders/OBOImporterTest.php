<?php

namespace Tests;

use StatonLab\TripalTestSuite\DBTransaction;
use StatonLab\TripalTestSuite\TripalTestCase;

class OBOImporterTest extends TripalTestCase {

  // Uncomment to auto start and rollback db transactions per test method.
  use DBTransaction;


  /**
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

  /**
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


  private function load_obo($name,$path){

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
   * @throws \Exception
   * @group obo
   * @ticket 525
   */
  public function test_relationships_in_SO_exist() {

    // step 1: drop the SO CV and CASCADE.

    $result = chado_query("DELETE FROM {cv} WHERE name = 'sequence'");
    $result = chado_query("DELETE FROM {db} WHERE name = 'SO'");

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
   $this->assertEquals("scaffold", $result->synonym);

  }

}