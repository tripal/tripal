<?php

namespace Tests\tripal_chado;

use StatonLab\TripalTestSuite\DBTransaction;
use StatonLab\TripalTestSuite\TripalTestCase;

//require_once(__DIR__ . '/../../tripal_chado/includes/TripalImporter/TaxonomyImporter.inc');

module_load_include('inc', 'tripal_chado', 'TripalImporter/TaxonomyImporter.inc');


class TaxonomyImporterTest extends TripalTestCase {
  use DBTransaction;


  /*
   * Adds an organism and checks that the importer runs and adds some properties to it.
   *
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

  /**
   * the importer can also load an array of pubmed ids.  We use the pillbug again.
   *
   * https://www.ncbi.nlm.nih.gov/Taxonomy/Browser/wwwtax.cgi?mode=Info&id=96821
   *
   * @throws \Exception
   */
  public function testImportOrganismFromTaxID() {

    $file = [];
    $run_args = ['taxonomy_ids' => '96821']; //its the pillbug again!
    $importer = new \TaxonomyImporter();

    ob_start();
    $importer->create($run_args, $file);
    $importer->run();
    ob_end_clean();

    $query = db_select('chado.organism', 'o');
    $query->fields('o', ['genus'])
      ->condition('o.species', 'officinalis');
    $result = $query->execute()->fetchField();
    $this->assertEquals('Armadillo', $result);

  }

}