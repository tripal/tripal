<?php

namespace Tests\tripal_chado\api;

use StatonLab\TripalTestSuite\DBTransaction;
use StatonLab\TripalTestSuite\TripalTestCase;

class TripalChadoOrganismAPITest extends TripalTestCase {

  use DBTransaction;

  /**
   * Test tripal_get_organism.
   *
   * @group api
   */
  public function test_tripal_get_organism() {
    $genus_string = 'a_genius_genus';
    $species_string = 'fake_species';

    $organism = factory('chado.organism')->create([
      'genus' => $genus_string,
      'species' => $species_string,
    ]);

    $results = [];

    $results[] = chado_get_organism(['organism_id' => $organism->organism_id]);
    $results[] = chado_get_organism([
      'genus' => $genus_string,
      'species' => $species_string,
    ]);

    foreach ($results as $result) {
      $this->assertNotFalse($result);
      $this->assertNotNull($result);
      $this->assertObjectHasAttribute('genus', $result);
      $this->assertEquals($genus_string, $result->genus);
    }
  }

  /**
   * Test tripal_get_organism doesn't return anything
   * when the organism doesn't exist.
   */
  public function test_tripal_get_organism_fails_gracefully() {
    $result = chado_get_organism([
      'genus' => uniqid(),
      'species' => uniqid(),
    ]);

    $this->assertNull($result);
  }

  /**
   * Test tripal_get_organism_scientific_name
   *
   * @group  api
   */
  function test_tripal_get_organism_scientific_name() {
    $genus_string = 'a_genius_genus';
    $species_string = 'fake_species';
    $infraspecific_name = "infrawhat?";
    $term = factory('chado.cvterm')->create();

    $organism = factory('chado.organism')->create([
      'genus' => $genus_string,
      'species' => $species_string,
      'infraspecific_name' => $infraspecific_name,
      'type_id' => $term->cvterm_id,
    ]);

    $sci_name = chado_get_organism_scientific_name($organism);
    $this->assertEquals(implode(" ", [
      $genus_string,
      $species_string,
      $term->name,
      $infraspecific_name,
    ]), $sci_name);
  }

  //TODO: Can't test because it uses drupal_json_output.
  //Need HTTP testing.
  //
  //  function test_tripal_autocomplete_organism(){
  //
  //    $genus_string = 'a_genius_genus';
  //    $species_string = 'fake_species';
  //
  //    $organism = factory('chado.organism')->create([
  //      'genus' => $genus_string,
  //      'species' => $species_string,
  //    ]);
  //
  //    tripal_autocomplete_organism(substr($genus_string, 0, 4));
  //
  //   //$this->assertEquals($genus_string, $auto_complete);
  //  }

  //This function is Tripal 2, and needs to be updated or deprecated

  //  function test_tripal_get_organism_select_options_sycned_only_false(){
  //
  //    db_truncate('chado.organism');
  //    factory('chado.organism', 20)->create();
  //
  //    $options = tripal_get_organism_select_options(FALSE);
  //
  //    $this->assertNotEmpty($options);
  //    $this->assertGreaterThan(20, count($options));
  //
  //  }
}
