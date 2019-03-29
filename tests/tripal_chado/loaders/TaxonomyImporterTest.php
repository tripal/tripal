<?php

namespace Tests\tripal_chado;

use StatonLab\TripalTestSuite\DBTransaction;
use StatonLab\TripalTestSuite\TripalTestCase;

/**
 *
 */
class TaxonomyImporterTest extends TripalTestCase {

  use DBTransaction;

  /**
   * Adds an organism and checks that the importer runs and adds some
   * properties to it.
   *
   */
  public function testImportExistingTaxonomyLoader() {
    module_load_include('inc', 'tripal_chado', 'includes/TripalImporter/TaxonomyImporter');

    $org = [
      'genus' => 'Armadillo',
      'species' => 'officinalis',
      'abbreviation' => 'A. officinalis',
      'common_name' => 'pillbug',
      'type_id' => NULL,
    ];

    $organism = factory('chado.organism')->create($org);
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
   * The importer can also load an array of pubmed ids.  We use the pillbug
   * again.
   *
   * Https://www.ncbi.nlm.nih.gov/Taxonomy/Browser/wwwtax.cgi?mode=Info&id=96821
   *
   * @throws \Exception
   */
  public function testImportOrganismFromTaxID() {

    module_load_include('inc', 'tripal_chado', 'includes/TripalImporter/TaxonomyImporter');

    $file = [];
    // Its the pillbug again!
    $run_args = ['taxonomy_ids' => '96821'];
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
