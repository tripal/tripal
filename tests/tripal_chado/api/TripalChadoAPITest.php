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

    // Create an organism, publish it
    $organism = factory('chado.organism')->create([
      'genus' => $genus_string,
      'species' => 'fake_species',
    ]);

    // Get bundle ID for organism
    $bundle = db_select('public.chado_bundle', 'CB')
      ->fields('CB', ['bundle_id'])
      ->condition('data_table', 'organism')
      ->execute()
      ->fetchField();

    $values = ['bundle_name' => 'bio_data_' . $bundle];

    // Don't display the job message
    $bool = silent(function () use ($values) {
      return chado_publish_records($values);
    });

    $this->assertTrue($bool->getReturnValue(),
      'Publishing a fake organism record failed');

    // Ensure that our entity was created
    $query = db_select('chado.organism', 'O')->fields('O', ['organism_id']);
    $query->join('public.chado_bio_data_' . $bundle, 'CBD',
      'O.organism_id = CBD.record_id');
    $query->condition('O.genus', $genus_string);
    $organism_id = $query->execute()->fetchField();

    $this->assertNotNull($organism_id,
      'Organism with record ID not found in chado_bio_data table.');
  }

  /**
   * Test chado_publish_records returns false given bad bundle.
   *
   * @group api
   */
  public function test_tripal_chado_publish_records_false_with_bad_bundle() {
    $bool = silent(function () {
      return chado_publish_records(['bundle_name' => 'never_in_a_million_years']);
    });

    $this->assertFalse($bool->getReturnValue());
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
