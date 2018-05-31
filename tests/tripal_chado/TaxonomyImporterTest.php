<?php

namespace Tests\tripal_chado;

use StatonLab\TripalTestSuite\DBTransaction;
use StatonLab\TripalTestSuite\TripalTestCase;

require_once(__DIR__ . '/../../tripal_chado/includes/TripalImporter/TaxonomyImporter.inc');


class TaxonomyImporterTest extends TripalTestCase {
  use DBTransaction;


  /*
   * Adds an organism and checks that the importer runs and adds some properties to it.
   */
  public function testImportExistingTaxonomyLoader() {
    $org = [
      'genus' => 'Armadillo',
      'species' => 'officinalis',
      'abbreviation' => 'A. officinalis',
      'common_name' => 'pillbug',
      'type_id' => null
    ];

    $organism = factory('chado.organism')->create($org);
  //  $this->publish('organism');
    $file = [];
    $run_args = ['import_existing' => TRUE];
    $importer = new \TaxonomyImporter();
    ob_start();
    $importer->create($run_args, $file);
    $importer->run();
    ob_end_clean();


    $query = db_select('chado.organism', 'o');
    $query->join('chado.organismprop', 'op', 'o.organism_id = op.organism_id');
    $query->fields('op', ['value'])
      ->condition('o.organism_id', $organism->organism_id);
    $result = $query->execute()->fetchAll();
    $this->assertNotEmpty($result);

  }
//
//  public function testImportOrganismFromTaxID() {
//
//  }
}