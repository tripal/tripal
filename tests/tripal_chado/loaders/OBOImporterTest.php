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
      $this->load_pto_mini();

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
     $this->load_goslim_plant();

     $exists = db_select('chado.cv', 'c')
       ->fields('c', ['cv_id'])
       ->condition('name', 'core_test_goslim_plant')
       ->execute()
       ->fetchField();
     $this->assertNotFalse($exists);
   }

  private function load_pto_mini() {

    $name = 'core_test_PTO_mini';

    $path = __DIR__ . '/../example_files/pto_colon.obo';

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
