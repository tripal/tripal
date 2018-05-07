<?php
namespace Tests\tripal_chado\api;


use StatonLab\TripalTestSuite\DBTransaction;
use StatonLab\TripalTestSuite\TripalTestCase;

class TripalChadoAPITest extends TripalTestCase {

  use DBTransaction;

  /**
   * Test the ability to publish Chado organism records as entities.
   *
   * @group api
   * @group failing
   */
  public function test_tripal_chado_publish_records() {
    $genus_string = 'a_genius_genus';
    //create an organism, publish it
    $organism = factory('chado.organism')->create([
      'genus' => $genus_string,
      'species' => 'fake_species',
    ]);
    //get bundle ID for organism
    $bundle = db_select('public.chado_bundle', 'CB')
      ->fields('CB', ['bundle_id'])
      ->condition('data_table', 'organism')
      ->execute()->fetchField();

    var_dump($bundle);
    $values = ['bundle_name' => 'bio_data_' . $bundle];

 //   ob_start();//dont display the job message
    $bool = chado_publish_records($values);
   // ob_end_clean();

    $this->assertTrue($bool, 'Publishing a fake organism record failed');

    //ensure that our entity was created
    $query = db_select('chado.organism', 'O')
      ->fields('O', ['organism_id']);
    $query->join('public.chado_bio_data_' . $bundle, 'CBD', 'O.organism_id = CBD.record_id');
    $query->condition('O.genus', $genus_string);
    $organism_id = $query->execute()->fetchField();
    $this->assertNotNull($organism_id, 'Organism with record ID not found in chado_bio_data table.');
  }

  /**
   * Test chado_publish_records returns false given bad bundle.
   *
   * @group api
   */
  public function test_tripal_chado_publish_records_false_with_bad_bundle() {
    putenv("TRIPAL_SUPPRESS_ERRORS=TRUE");//this will fail, so we suppress the tripal error reporter
    $bool = chado_publish_records(['bundle_name' => 'never_in_a_million_years']);
    $this->assertFalse($bool);
    putenv("TRIPAL_SUPPRESS_ERRORS");//unset
  }

  /**
   * calls chado_get_tokens.
   *
   * @group api
   */
  public function test_chado_get_tokens() {
    $tokens = chado_get_tokens('organism');
    $this->assertNotEmpty($tokens);
    $this->assertArrayHasKey('[organism.organism_id]', $tokens);
  }
}
