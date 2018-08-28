<?php

namespace Tests;

use StatonLab\TripalTestSuite\DBTransaction;
use StatonLab\TripalTestSuite\TripalTestCase;

class OBOImporterTest extends TripalTestCase {

  // Uncomment to auto start and rollback db transactions per test method.
  use DBTransaction;


  //  /**
  //   * @group obo
  //   */
  //  public function test_PTO_loads() {
  //    $this->load_pto_full();
  //
  //    $exists = db_select('chado.cv', 'c')
  //      ->fields('c', ['cv_id'])
  //      ->condition('name', 'plaint_trait_ontology');
  //    $this->assertNotNull($exists);
  //
  //  }

  /**
   * @group obo
   */

  public function testGO_SLIM_loads() {
    $this->load_goslim_plant();

    $exists = db_select('chado.cv', 'c')
      ->fields('c', ['cv_id'])
      ->condition('name', 'core_test_goslim_plant')
      ->execute()
      ->fetchField();
    $this->assertNotNull($exists);


  }

  private function load_pto_full() {

    $name = 'core_test_PTO_mini';
    $path = 'http://purl.obolibrary.org/obo/to.obo';

    $obo_id = db_select('public.tripal_cv_obo', 't')
      ->fields('t', ['obo_id'])
      ->condition('t.name', $name)->execute()->fetchField();

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

  private function load_goslim_plant() {

    $name = 'core_test_goslim_plant';
    $path = 'http://www.geneontology.org/ontology/subsets/goslim_plant.obo';

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

}
